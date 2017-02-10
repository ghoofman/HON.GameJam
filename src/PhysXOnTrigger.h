#pragma once

#include "./OPengine.h"
#include "GameEntity.h"

typedef void(*TriggerFn)(GameEntity*, GameEntity*, bool);

struct TriggerPair {
	GameEntityType t0;
	GameEntityType t1;
	TriggerFn fn;

	bool Process(GameEntity* one, GameEntity* two, bool status);
};

void PhysXOnTrigger(OPphysXTrigger trigger);