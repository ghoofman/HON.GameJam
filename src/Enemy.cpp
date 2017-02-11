#include "Enemy.h"
#include "Globals.h"

void Enemy::Init(OPscene* scene, const OPchar* model, const OPchar* texture) {
	entityType = ENEMY;

	position = OPvec3(0);
	rendererEntity = scene->Add((OPmodel*)OPCMAN.LoadGet(model), OPrendererEntityDesc(false, true, true, false));
	rendererEntity->SetAlbedoMap((OPtexture*)OPCMAN.LoadGet(texture));
	rendererEntity->world.SetIdentity();

	OPphysXMaterial* material = OPphysXCreateMaterial(1.0, 1.0, 1.0); // OPphysXCreateMaterial(0.8, 0.8, 0.6);
	physX = OPphysXControllerCreate(PHYSX_CONTROLLERMANAGER, material, 0.5, 0.5, NULL, NULL, NULL, NULL);
	OPphysXControllerSetPos(physX, position);
	physX->setStepOffset(0.1);
}

bool Enemy::CanAttack() {
	return attacked > 1000;
}

void Enemy::Update(OPtimer* timer) {
	if (health <= 0) return;

	OPfloat dt = timer->Elapsed / 1000.0f;
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

	OPphysXControllerMove(physX, entityPhysics.velocity, timer);
	position = OPphysXControllerGetFootPos(physX);

	if (hasTarget && OPvec3Len(position - prevPosition) < 0.002) {
		hasTarget = false;
	}
}

void Enemy::DealDamage(ui32 amount) {
	if (health <= 0) return;

	health -= amount;
	if (health <= 0) {
		rendererEntity->material->visible = false;
		rendererEntity->shadowMaterial->visible = false;
		// Remove controller
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

Handle<Object> Enemy::Wrap(Handle<Object> result) {
	GameEntity::Wrap(result);
	JS_SET_METHOD(result, "UpdatePhysics", _EnemyUpdatePhysicsSelf);
	return result;
}