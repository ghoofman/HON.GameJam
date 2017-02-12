#include "Player.h"
#include "Globals.h"

Player::Player() {
	entityType = PLAYER;
	physX = NULL;
	controls.gamePad = OPGAMEPADS.Get(0);

	entityPhysics.maxSpeed += OPrandom() * 0.025;
	entityPhysics.drag -= OPrandom() * 0.125;
	entityPhysics.accel += OPrandom() * 0.5;

	skeleton = ((OPskeleton*)OPCMAN.LoadGet("SamuraiAnim.opm.skel"))->Copy();
	animation_idle = ((OPskeletonAnimation*)OPCMAN.LoadGet("SamuraiAnim.opm.Idle.anim"))->Clone();
	animation_idle->FramesPer = 1000.0 / 15.0;
	animation_walk = ((OPskeletonAnimation*)OPCMAN.LoadGet("SamuraiAnim.opm.Walk.anim"))->Clone();
	animation_walk->FramesPer = 1000.0 / 30.0;
	animation_attack = ((OPskeletonAnimation*)OPCMAN.LoadGet("SamuraiAnim.opm.Attack.anim"))->Clone();
	animation_attack->FramesPer = 1000.0 / 30.0;
	animation_attack->Loop = false;
	animation_suicide = ((OPskeletonAnimation*)OPCMAN.LoadGet("SamuraiAnim.opm.Suicide.anim"))->Clone();
	animation_suicide->FramesPer = 1000.0 / 30.0;
	animation_suicide->Loop = false;
	animation_die = ((OPskeletonAnimation*)OPCMAN.LoadGet("SamuraiAnim.opm.Die.anim"))->Clone();
	animation_die->FramesPer = 1000.0 / 30.0;
	animation_die->Loop = false;

	OPCMAN.Load("health.opss");
	OPsprite* sprite = (OPsprite*)OPCMAN.LoadGet("health/healthbar");
	sprites[0] = sprite;
	OPsprite3DInit(NULL);
	healthTex = OPsprite3DCreate(sprites, NULL);
	healthTex->Scale = OPvec3(1);
	healthTex->Scale.y = 0.5;
}

void Player::AddToScene(OPscene* scene) {
	rendererEntity = scene->Add((OPmodel*)OPCMAN.LoadGet("SamuraiAnim.opm"), skeleton, OPrendererEntityDesc(true, true, true, false));
	rendererEntity->SetAlbedoMap((OPtexture*)OPCMAN.LoadGet("SamuraiPallete.png"));
	rendererEntity->world.SetIdentity();

	OPphysXMaterial* material = OPphysXCreateMaterial(1.0, 1.0, 1.0); // OPphysXCreateMaterial(0.8, 0.8, 0.6);
	physX = OPphysXControllerCreate(PHYSX_CONTROLLERMANAGER, material, 0.5, 0.5, NULL, NULL, NULL, NULL);
	OPphysXControllerSetPos(physX, position);
	physX->setStepOffset(0.1);
}

void Player::RemoveFromScene() {
	if (physX != NULL) {
		physX->release();
		physX = NULL;
	}
}

void Player::DealDamage(ui32 amount) {
	if (health <= 0) return;

	health -= amount;
	if (health <= 0) {
		dead = true;
		//rendererEntity->material->visible = false;
		//rendererEntity->shadowMaterial->visible = false;
		//// Remove controller
		physX->release();
		physX = NULL;
	}
}

void Player::Update(OPtimer* timer) {
	OPfloat dt = timer->Elapsed / 1000.0f;

	OPsprite3DUpdate(healthTex, timer->Elapsed);
	healthTex->Scale.x = (f32)health / 10.0f;

	prevPosition = position;
	prevRotate = rotate;
	prevScale = scale;

	if (!suiciding && !dead) {

		if (attackingTime > attackTime) {
			attacking = controls.Attacking();
			if (attacking) {
				attackingTime = 0;
				animation_attack->Reset();
			}
		}

		dashTime += timer->Elapsed;
		if (dashTime > 3000 && controls.Dashing()) {
			dashTime = 0;
		}
		dashing = dashTime < 1000;
		OPfloat speed = 1.0f + controls.Running() + (dashing * 2);

		OPvec3 dir = OPVEC3_ZERO;

		OPvec3 forward = CAMERA.pos - CAMERA.target;
		forward.y = 0;
		forward = OPvec3Norm(forward);

		OPvec3 left = OPvec3Cross(forward, OPVEC3_UP);

		OPvec2 moveDir = controls.Movement();

		dir.x = moveDir.x;
		dir.z = moveDir.y;
		dir.Norm();

		if (hasTarget) {
			OPvec3 targetDir = target - position;
			dir.x = targetDir.x;
			dir.z = targetDir.z;
			dir.Norm();
		}

		OPvec2 planerVelocity = {
			entityPhysics.velocity.x,
			entityPhysics.velocity.z
		};
		OPvec2 velDragDir = OPvec2Norm(planerVelocity) * -entityPhysics.drag;

		if (entityPhysics.velocity.x > 0) {
			entityPhysics.velocity.x += velDragDir.x * dt;
			if (entityPhysics.velocity.x < 0) entityPhysics.velocity.x = 0;
		}
		else if (entityPhysics.velocity.x < 0) {
			entityPhysics.velocity.x += velDragDir.x * dt;
			if (entityPhysics.velocity.x > 0) entityPhysics.velocity.x = 0;
		}

		if (entityPhysics.velocity.z > 0) {
			entityPhysics.velocity.z += velDragDir.y * dt;
			if (entityPhysics.velocity.z < 0) entityPhysics.velocity.z = 0;
		}
		else if (entityPhysics.velocity.z < 0) {
			entityPhysics.velocity.z += velDragDir.y * dt;
			if (entityPhysics.velocity.z > 0) entityPhysics.velocity.z = 0;
		}

		entityPhysics.vertical += entityPhysics.gravity * dt * 0.05f;
		if (entityPhysics.vertical < entityPhysics.gravity * 0.02f) {
			entityPhysics.vertical = entityPhysics.gravity * 0.02f;
		}

		entityPhysics.acceleration *= entityPhysics.drag;
		entityPhysics.acceleration.y = entityPhysics.vertical;

		OPfloat len = OPvec3Len(dir);
		if (len > 0) {

			// OPvec3 acceleration;
			if (hasTarget) {
				entityPhysics.acceleration.z += (dir.z * entityPhysics.accel * speed);
				entityPhysics.acceleration.x += (dir.x * entityPhysics.accel * speed);
			}
			else {
				entityPhysics.acceleration += forward * (-dir.z * entityPhysics.accel * speed);
				entityPhysics.acceleration += left * (-dir.x * entityPhysics.accel * speed);
			}

			entityPhysics.acceleration.y = 0;
			entityPhysics.velocity += entityPhysics.acceleration * dt;

			rotate = OPvec3(0, OPpi_2 + OPatan2(entityPhysics.acceleration.x, -entityPhysics.acceleration.z), 0);
		}
		entityPhysics.velocity.y = entityPhysics.vertical;

		planerVelocity.x = entityPhysics.velocity.x;
		planerVelocity.y = entityPhysics.velocity.z;
		if (OPvec2Len(planerVelocity) > entityPhysics.maxSpeed * speed) {
			planerVelocity = OPvec2Norm(planerVelocity) * entityPhysics.maxSpeed * speed;
			entityPhysics.velocity.x = planerVelocity.x;
			entityPhysics.velocity.z = planerVelocity.y;
		}

		OPphysXControllerMove(physX, entityPhysics.velocity, timer);
		position = OPphysXControllerGetFootPos(physX);

		if (hasTarget && (OPvec3Len(position - prevPosition) < 0.001 || OPvec3Len(position - target) < 0.1)) {
			hasTarget = false;
		}
	}

	attackingTime += timer->Elapsed;
	if (suiciding) {
		if (animation_suicide->LoopsCompleted >= 1) {
			animation_suicide->Frame = animation_suicide->FrameCount - 1;
		}
		animation_suicide->Update(timer);
		animation_suicide->Apply(skeleton);
	}
	else if (dead) {
		if (animation_die->LoopsCompleted >= 1) {
			animation_die->Frame = animation_die->FrameCount - 1;
		}
		animation_die->Update(timer);
		animation_die->Apply(skeleton);
	}
	else {
		if (attackingTime < attackTime && animation_attack->LoopsCompleted == 0) {
			animation_attack->Update(timer);
			animation_attack->Apply(skeleton);
		}
		else if (controls.Running()) {
			animation_walk->Update(timer);
			animation_walk->Apply(skeleton);
		}
		else {
			animation_idle->Update(timer);
			animation_idle->Apply(skeleton);
		}
	}
	skeleton->Update();

	healthTex->Position = position + OPvec3(0, 2, 0);
}

void Player::PrepRender(OPfloat delta) {
	rendererEntity->world.
		SetTranslate(
			OPvec3Tween(
				prevPosition,
				position,
				delta))->
		Scl(OPvec3Tween(
				prevScale,
				scale,
				delta))->
		Scl(0.25)->
		RotY(OPtween(
				prevRotate.y,
				rotate.y,
				delta));
}

void Player::RenderDebug() {

#ifdef _DEBUG
#ifdef ADDON_imgui

	//ImGui::InputInt("Health", &health);
	//if (ImGui::InputFloat3("Position", (float*)&position, -20, 20)) {
	//	OPphysXControllerSetPos(physX, position);
	//}

#endif
#endif

}

Handle<Object> Player::Wrap(Handle<Object> result) {
	return result;
}