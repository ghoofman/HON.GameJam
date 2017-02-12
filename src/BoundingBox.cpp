#pragma once

#include "./OPengine.h"
#include "BoundingBox.h"
#include "Globals.h"

void BoundingBox::Init(OPscene* scene) {
	physX = OPphysXSceneCreateStatic(PHYSX_SCENE, OPvec3(0));
	OPphysXMaterial* material = OPphysXCreateMaterial(0.8, 0.8, 0.0);
	shapes = OPphysXAddBoxShape(physX, material, OPvec3(1.0));
	OPphysXSceneAddActor(PHYSX_SCENE, physX);
}

void BoundingBox::Update(OPtimer* timer) {

}

void BoundingBox::PrepRender(OPfloat delta) {

}

JS_HELPER_SELF_WRAPPER(_BoundingBoxUpdatePhysics) {
	SCOPE_AND_ISOLATE;

	BoundingBox* ptr = JS_NEXT_ARG_AS(BoundingBox);

	ptr->physX->setGlobalPose(
		PxTransform(
			PxVec3(
				ptr->position.x,
				ptr->position.y,
				ptr->position.z),
			PxQuat(ptr->rotate.y,
				PxVec3(0, 1, 0))));

	if (ptr->rendererEntity != NULL) {
		OPphysXGetTransform((OPphysXRigidActor*)ptr->physX, &ptr->rendererEntity->world);
		ptr->rendererEntity->world.Scl(ptr->scale);
	}

	JS_RETURN_NULL;
}

JS_HELPER_SELF_WRAPPER(_BoundingBoxSetSize) {
	SCOPE_AND_ISOLATE;

	BoundingBox* ptr = JS_NEXT_ARG_AS(BoundingBox);
	double x = JS_NEXT_ARG_AS_NUMBER;
	double y = JS_NEXT_ARG_AS_NUMBER;
	double z = JS_NEXT_ARG_AS_NUMBER;


	PxBoxGeometry geo = PxBoxGeometry(ptr->scale.x * x / 2.0f, ptr->scale.y * y / 2.0f, ptr->scale.z * z / 2.0f);
	ptr->shapes->setGeometry(geo);

	//OPvec3 centerPoint =
	//	ptr->rendererEntity->model->meshes[0].boundingBox.max +
	//	ptr->rendererEntity->model->meshes[0].boundingBox.min;
	//ptr->shapes->setLocalPose(PxTransform(PxVec3(centerPoint.x / 2.0, centerPoint.y / 2.0, centerPoint.z / 2.0)));

	JS_RETURN_NULL;
}

Handle<Object> BoundingBox::Wrap(Handle<Object> result) {
	GameEntity::Wrap(result);
	JS_SET_METHOD(result, "SetSize", _BoundingBoxSetSizeSelf);
	JS_SET_METHOD(result, "UpdatePhysics", _BoundingBoxUpdatePhysicsSelf);
	return result;
}