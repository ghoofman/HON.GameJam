#include "Enemy.h"
#include "Globals.h"

void Enemy::Init(OPscene* scene, const OPchar* model, const OPchar* texture) {
	entityType = ENEMY;


	skeleton = ((OPskeleton*)OPCMAN.LoadGet("SamuraiAnim.opm.skel"))->Copy();
	animation_idle = ((OPskeletonAnimation*)OPCMAN.LoadGet("SamuraiAnim.opm.Idle.anim"))->Clone();
	animation_idle->FramesPer = 1000.0 / 15.0;
	animation_walk = ((OPskeletonAnimation*)OPCMAN.LoadGet("SamuraiAnim.opm.Walk.anim"))->Clone();
	animation_walk->FramesPer = 1000.0 / 30.0;
	animation_attack = ((OPskeletonAnimation*)OPCMAN.LoadGet("SamuraiAnim.opm.Attack.anim"))->Clone();
	animation_attack->FramesPer = 1000.0 / 30.0;
	animation_attack->Loop = false;
	animation_die = ((OPskeletonAnimation*)OPCMAN.LoadGet("SamuraiAnim.opm.Die.anim"))->Clone();
	animation_die->FramesPer = 1000.0 / 30.0;
	animation_die->Loop = false;

	position = OPvec3(0);
	rendererEntity = scene->Add((OPmodel*)OPCMAN.LoadGet(model), skeleton, OPrendererEntityDesc(true, true, true, false));
	rendererEntity->SetAlbedoMap((OPtexture*)OPCMAN.LoadGet(texture));
	rendererEntity->world.SetIdentity();

	OPphysXMaterial* material = OPphysXCreateMaterial(1.0, 1.0, 1.0); // OPphysXCreateMaterial(0.8, 0.8, 0.6);
	physX = OPphysXControllerCreate(PHYSX_CONTROLLERMANAGER, material, 0.5, 0.5, NULL, NULL, NULL, NULL);
	OPphysXControllerSetPos(physX, position);
	physX->setStepOffset(0.1);
}

bool Enemy::CanAttack() {
	return attackingTime > 1000;
}

bool Enemy::Attack() {
	if (attackingTime > 1000) {
		attacked = 0;
		attackingTime = 0;
		animation_attack->Reset();
		dealtDamage = false;
		return true;
	}
	return false;
}

void Enemy::Update(OPtimer* timer) {

	OPfloat dt = timer->Elapsed / 1000.0f;

	if (!dead) {
		attacked += timer->Elapsed;

		prevPosition = position;
		prevRotate = rotate;
		prevScale = scale;

		OPfloat speed = 1.0f;// +controls.Running();

		OPvec2 planerVelocity = {
			entityPhysics.velocity.x,
			entityPhysics.velocity.z
		};
		OPvec2 velDragDir = OPvec2Norm(planerVelocity) * -entityPhysics.drag;


		if (knockback.x > 0) {
			knockback.x += velDragDir.x * dt;
			if (knockback.x < 0) knockback.x = 0;
		}
		else if (knockback.x < 0) {
			knockback.x += velDragDir.x * dt;
			if (knockback.x > 0) knockback.x = 0;
		}
		if (knockback.z > 0) {
			knockback.z += velDragDir.y * dt;
			if (knockback.z < 0) knockback.z = 0;
		}
		else if (knockback.z < 0) {
			knockback.z += velDragDir.y * dt;
			if (knockback.z > 0) knockback.z = 0;
		}

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

		if (hasTarget) {
			OPvec3 targetDir = target - position;
			targetDir.y = 0;
			targetDir.Norm();

			OPfloat len = OPvec3Len(targetDir);
			if (len > 0) {

				entityPhysics.acceleration.z += (targetDir.z * entityPhysics.accel * speed);
				entityPhysics.acceleration.x += (targetDir.x * entityPhysics.accel * speed);

				entityPhysics.acceleration.y = 0;
				entityPhysics.velocity += entityPhysics.acceleration * dt;

				rotate = OPvec3(0, OPpi_2 + OPatan2(entityPhysics.acceleration.x, -entityPhysics.acceleration.z), 0);
			}

		}

		entityPhysics.velocity.y = entityPhysics.vertical;

		planerVelocity.x = entityPhysics.velocity.x;
		planerVelocity.y = entityPhysics.velocity.z;
		if (OPvec2Len(planerVelocity) > entityPhysics.maxSpeed * speed) {
			planerVelocity = OPvec2Norm(planerVelocity) * entityPhysics.maxSpeed * speed;
			entityPhysics.velocity.x = planerVelocity.x;
			entityPhysics.velocity.z = planerVelocity.y;
		}

		entityPhysics.velocity.x += knockback.x;
		entityPhysics.velocity.z += knockback.z;

		OPphysXControllerMove(physX, entityPhysics.velocity, timer);
		position = OPphysXControllerGetFootPos(physX);

		if (hasTarget && OPvec3Len(position - prevPosition) < 0.002) {
			hasTarget = false;
		}
	}

	attackingTime += timer->Elapsed; 
	if (dead) {
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
		else if (hasTarget) {
			animation_walk->Update(timer);
			animation_walk->Apply(skeleton);
		}
		else {
			animation_idle->Update(timer);
			animation_idle->Apply(skeleton);
		}
	}
	skeleton->Update();
}

void Enemy::DealDamage(ui32 amount, OPvec3 dir) {
	if (health <= 0) return;

	dir.Norm();
	knockback = dir * (0.2 / weight);

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

void Enemy::PrepRender(OPfloat delta) {
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
		//Scl(0.1)->
		RotY(OPtween(
			prevRotate.y,
			rotate.y,
			delta));
}

void Enemy::RenderDebug() {

#ifdef _DEBUG
#ifdef ADDON_imgui

	//ImGui::InputInt("Health", &health);
	//if (ImGui::InputFloat3("Position", (float*)&position, -20, 20)) {
	//	OPphysXControllerSetPos(physX, position);
	//}

#endif
#endif

}

JS_HELPER_SELF_WRAPPER(_EnemyUpdatePhysics) {
	SCOPE_AND_ISOLATE;

	Enemy* ptr = JS_NEXT_ARG_AS(Enemy);

	OPphysXControllerSetPos(ptr->physX, ptr->position);


	JS_RETURN_NULL;
}

JS_HELPER_SELF_WRAPPER(_EnemySetHealth) {
	SCOPE_AND_ISOLATE;
	Enemy* ptr = JS_NEXT_ARG_AS(Enemy);
	f32 health = JS_NEXT_ARG_AS_NUMBER;
	ptr->health = health;
	JS_RETURN_NULL;
}

JS_HELPER_SELF_WRAPPER(_EnemySetDamage) {
	SCOPE_AND_ISOLATE;
	Enemy* ptr = JS_NEXT_ARG_AS(Enemy);
	f32 damage = JS_NEXT_ARG_AS_NUMBER;
	ptr->damage = damage;
	JS_RETURN_NULL;
}

JS_HELPER_SELF_WRAPPER(_EnemySetArmor) {
	SCOPE_AND_ISOLATE;
	Enemy* ptr = JS_NEXT_ARG_AS(Enemy);
	f32 armor = JS_NEXT_ARG_AS_NUMBER;
	//ptr->armor = armor;
	JS_RETURN_NULL;
}

JS_HELPER_SELF_WRAPPER(_EnemySetSize) {
	SCOPE_AND_ISOLATE;
	Enemy* ptr = JS_NEXT_ARG_AS(Enemy);
	f32 size = JS_NEXT_ARG_AS_NUMBER;
	//ptr->size = size;
	JS_RETURN_NULL;
}

JS_HELPER_SELF_WRAPPER(_EnemySetWeight) {
	SCOPE_AND_ISOLATE;
	Enemy* ptr = JS_NEXT_ARG_AS(Enemy);
	f32 weight = JS_NEXT_ARG_AS_NUMBER;
	ptr->weight = weight;
	JS_RETURN_NULL;
}

Handle<Object> Enemy::Wrap(Handle<Object> result) {
	GameEntity::Wrap(result);
	JS_SET_METHOD(result, "UpdatePhysics", _EnemyUpdatePhysicsSelf);
	JS_SET_METHOD(result, "SetHealth", _EnemySetHealthSelf);
	JS_SET_METHOD(result, "SetDamage", _EnemySetDamageSelf);
	JS_SET_METHOD(result, "SetArmor", _EnemySetArmorSelf);
	JS_SET_METHOD(result, "SetSize", _EnemySetSizeSelf);
	JS_SET_METHOD(result, "SetWeight", _EnemySetWeightSelf);
	return result;
}