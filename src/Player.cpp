#include "Player.h"
#include "Globals.h"

Player::Player(OPscene* scene) {
	entityType = PLAYER;

	controls.gamePad = OPGAMEPADS.Get(0);

	rendererEntity = scene->Add((OPmodel*)OPCMAN.LoadGet("box.opm"), OPrendererEntityDesc(false, true, true, false));
	rendererEntity->SetAlbedoMap((OPtexture*)OPCMAN.LoadGet("test4.png"));
	rendererEntity->world.SetIdentity();

	OPphysXMaterial* material = OPphysXCreateMaterial(1.0, 1.0, 1.0); // OPphysXCreateMaterial(0.8, 0.8, 0.6);
	physX = OPphysXControllerCreate(PHYSX_CONTROLLERMANAGER, material, 0.5, 0.25, NULL, NULL, NULL, NULL);
	OPphysXControllerSetPos(physX, position);
	physX->setStepOffset(0.1);
}

void Player::Update(OPtimer* timer) {
	OPfloat dt = timer->Elapsed / 1000.0f;

	prevPosition = position;
	prevRotate = rotate;
	prevScale = scale;
	
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
		entityPhysics.acceleration += forward * (-dir.z * entityPhysics.accel * speed);
		entityPhysics.acceleration += left * (-dir.x * entityPhysics.accel * speed);

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