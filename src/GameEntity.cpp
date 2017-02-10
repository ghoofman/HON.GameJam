#pragma once

#include "GameEntity.h"

void UpdateWorldMatrix(GameEntity* entity) {
	if (entity->rendererEntity == NULL) return;

	entity->rendererEntity->world
		.SetTranslate(entity->position)
		->Scl(entity->scale)
		->RotY(entity->rotate.y)
		->RotZ(entity->rotate.z)
		->RotX(entity->rotate.x);
}

void SetTransform(GameEntity* entity, OPvec3 pos, OPvec3 rot, OPvec3 scl) {
	entity->position = pos;
	entity->scale = scl;
	entity->rotate = rot;

	UpdateWorldMatrix(entity);
}

JS_HELPER_SELF_WRAPPER(_GameEntitySetTransform) {
	SCOPE_AND_ISOLATE;

	GameEntity* ptr = JS_NEXT_ARG_AS(GameEntity);
	OPvec3 pos = JS_NEXT_ARG_AS_VEC3;
	OPvec3 rot = JS_NEXT_ARG_AS_VEC3;
	OPvec3 scl = JS_NEXT_ARG_AS_VEC3;

	SetTransform(ptr, pos, rot, scl);
	
	JS_RETURN_NULL;
}

Handle<Object> GameEntity::Wrap(Handle<Object> result) {

	JS_SET_PTR(result, this);
	JS_SET_METHOD(result, "SetTransform", _GameEntitySetTransformSelf);

	return result;
}