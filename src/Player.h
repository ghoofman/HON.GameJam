#pragma once

#include "./OPengine.h"
#include "GameEntity.h"
#include "ControlsManager.h"

class Player : GameEntity {
private:
	OPphysXController* physX;
	PxShape* shapes;
	ControlsManager controls;

	i32 health;

public:
	Player(OPscene* scene);
	void Update(OPtimer* timer);
	void PrepRender(OPfloat delta);
	void Player::RenderDebug();
	Handle<Object> Wrap(Handle<Object> result);
};