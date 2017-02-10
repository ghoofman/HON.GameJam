#pragma once

#include "./OPengine.h"

enum GameEntityType {
	NOTDEFINED,
	STATIC,
	PLAYER,
	ENEMY
};

struct EntityPhysics {
	OPfloat maxSpeed = 0.05;
	OPfloat accel = 0.9;
	OPfloat drag = 0.5;
	OPfloat gravity = -9.8;
	OPfloat jumpSpeed = 1.0;

	OPvec3 velocity = OPvec3(0);
	OPvec3 acceleration = OPvec3(0);
	OPfloat vertical = 0;
};

class GameEntity {
public:
	GameEntityType entityType = NOTDEFINED;
	virtual void Update(OPtimer* timer) = 0;
	virtual void PrepRender(OPfloat delta) = 0;
	virtual Handle<Object> Wrap(Handle<Object> result);

	OPrendererEntity* rendererEntity = NULL;
	OPvec3 position = OPvec3(0), prevPosition = OPvec3(0);
	OPvec3 scale = OPvec3(1), prevScale = OPvec3(1);
	OPvec3 rotate = OPvec3(0), prevRotate = OPvec3(0);
	EntityPhysics entityPhysics;
};