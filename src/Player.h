#pragma once

#include "./OPengine.h"
#include "GameEntity.h"
#include "ControlsManager.h"

class Player : public GameEntity {
private:
	PxShape* shapes = NULL;
	ControlsManager controls;


public:
	OPphysXController* physX = NULL;
	OPvec3 target;
	bool hasTarget = false;
	bool attacking = false;
	i32 health = 10;
	i32 damage = 1;

	Player();
	void AddToScene(OPscene* scene);
	void RemoveFromScene();
	void Update(OPtimer* timer);
	void PrepRender(OPfloat delta);
	void DealDamage(ui32 amount);
	void RenderDebug(); 
	Handle<Object> Wrap(Handle<Object> result);
};