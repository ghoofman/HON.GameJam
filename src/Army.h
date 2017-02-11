#pragma once

#include "Player.h"
#include "Enemy.h"

#define MAX_ARMY_SIZE 50

class Army : public GameEntity {
public:
	ui32 index;
	Player* warriors[MAX_ARMY_SIZE];
	OPvec3 centerPoint;
	ui32 honor = 0;

	Army(ui32 size);
	void AddToScene(OPscene* scene);
	void SetSpawn(OPvec3 pos);
	void RemoveFromScene();
	void Update(OPtimer* timer);
	void Attack(Enemy* enemy);
	bool Within(Enemy* enemy);
	void PrepRender(OPfloat delta);
	Handle<Object> Wrap(Handle<Object> result);
};