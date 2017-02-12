#pragma once

#include "./OPengine.h"
#include "GameEntity.h"

class Enemy : public GameEntity {
private:
	PxShape* shapes = NULL;
	OPskeleton* skeleton;
	OPskeletonAnimation* animation_idle;
	OPskeletonAnimation* animation_walk;
	OPskeletonAnimation* animation_attack;
	OPskeletonAnimation* animation_die;

public:
	i32 health = 10;
	OPphysXController* physX = NULL;
	OPvec3 target;
	bool hasTarget = false;
	bool attacking = false;
	bool dead = false;
	ui32 attacked = 0;
	bool dealtDamage = false;
	ui32 damage = 1;
	f32 weight = 1.0;
	OPvec3 knockback = OPvec3(0);
	ui32 attackingTime = 1000;
	ui32 attackTime = 500;

	void Init(OPscene* scene, const OPchar* model, const OPchar* texture);
	void Update(OPtimer* timer);
	void DealDamage(ui32 amount, OPvec3 dir);
	void PrepRender(OPfloat delta);
	bool CanAttack();
	bool Attack();
	void RenderDebug();
	Handle<Object> Wrap(Handle<Object> result);
};