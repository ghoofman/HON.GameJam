#pragma once

#include "./OPengine.h"
#include "GameEntity.h"
#include "ControlsManager.h"

class Player : public GameEntity {
private:
	PxShape* shapes = NULL;
	ControlsManager controls;

	OPskeleton* skeleton;
	OPskeletonAnimation* animation_idle;
	OPskeletonAnimation* animation_walk;
	OPskeletonAnimation* animation_attack;
	OPskeletonAnimation* animation_suicide;
	OPskeletonAnimation* animation_die;
	OPsprite* sprites[1];
public:
	OPphysXController* physX = NULL;
	OPvec3 target;
	bool hasTarget = false;
	bool attacking = false;
	bool suiciding = false;
	bool dashing = false;
	bool dead = false;
	i32 health = 10;
	i32 damage = 1;
	ui32 honor = 8;
	ui32 attackingTime = 1000;
	ui32 attackTime = 500;
	ui32 dashTime = 1000;
	OPsprite3D* healthTex;

	Player();
	void AddToScene(OPscene* scene);
	void RemoveFromScene();
	void Update(OPtimer* timer);
	void PrepRender(OPfloat delta);
	void DealDamage(ui32 amount);
	void RenderDebug(); 
	Handle<Object> Wrap(Handle<Object> result);
};