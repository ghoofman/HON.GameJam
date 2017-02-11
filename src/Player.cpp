#include "Player.h"
#include "Globals.h"

Player::Player() {
	entityType = PLAYER;

	controls.gamePad = OPGAMEPADS.Get(0);

	entityPhysics.maxSpeed += OPrandom() * 0.025;
	entityPhysics.drag -= OPrandom() * 0.125;
	entityPhysics.accel += OPrandom() * 0.5;
}

void Player::AddToScene(OPscene* scene) {
	rendererEntity = scene->Add((OPmodel*)OPCMAN.LoadGet("Samurai.opm"), OPrendererEntityDesc(false, true, true, false));
	rendererEntity->SetAlbedoMap((OPtexture*)OPCMAN.LoadGet("SamuraiPallete.png"));
	rendererEntity->world.SetIdentity();

	OPphysXMaterial* material = OPphysXCreateMaterial(1.0, 1.0, 1.0); // OPphysXCreateMaterial(0.8, 0.8, 0.6);
	physX = OPphysXControllerCreate(PHYSX_CONTROLLERMANAGER, material, 0.5, 0.5, NULL, NULL, NULL, NULL);
	OPphysXControllerSetPos(physX, position);
	physX->setStepOffset(0.1);
}

void Player::RemoveFromScene() {
	physX->release();
}

void Player::DealDamage(ui32 amount) {
	if (health <= 0) return;

	health -= amount;
	if (health <= 0) {
		rendererEntity->material->visible = false;
		rendererEntity->shadowMaterial->visible = false;
		// Remove controller
		physX->release();
	}
}

void Player::Update(OPtimer* timer) {
	OPfloat dt = timer->Elapsed / 1000.0f;

	prevPosition = position;
	prevRotate = rotate;
	prevScale = scale;
	
	attacking = controls.Attacking();

	OPfloat speed = 1.0f + controls.Running();

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