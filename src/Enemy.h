#pragma once

#include "./OPengine.h"
#include "GameEntity.h"

class Enemy : public GameEntity {
private:
	PxShape* shapes = NULL;

public:
	i32 health = 10;
	OPphysXController* physX = NULL;
	OPvec3 target;
	bool hasTarget = false;
	ui32 attacked = 0;
	ui32 damage = 1;

	void Init(OPscene* scene, const OPchar* model, const OPchar* texture);
	void Update(OPtimer* timer);
	void DealDamage(ui32 amount);
	void PrepRender(OPfloat delta);
	bool CanAttack();
	void RenderDebug();
	Handle<Object> Wrap(Handle<Object> result);
};