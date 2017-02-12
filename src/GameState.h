#pragma once

#include "./OPengine.h"
#include "Army.h"
#include "Enemy.h"
#include "StaticEntity.h"
#include "BoundingBox.h"
#include "DynamicAllocator.h"

class GameState : public OPgameState {
	OPrendererForward rendererForward;
	OPvec3 camera_target;
	bool canExit = false;
	ui32 enemiesAlive;

    void Init(OPgameState* last);
    OPint Update(OPtimer* time);
    void Render(OPfloat delta);
    OPint Exit(OPgameState* next);
public:
	OPscene scene;
	DynamicAllocator<StaticEntity>* staticEntities;
	DynamicAllocator<Enemy>* enemies;
	DynamicAllocator<BoundingBox>* boundingBoxes;
	OPchar* dialog[3];
	ui32 dialogCount = 0;

	Persistent<Function, CopyablePersistentTraits<Function>> _onFinish;
	bool hasOnFinish = false;
	Persistent<Function, CopyablePersistentTraits<Function>> _onDialogFinish;
	bool hasOnDialogFinish = false;
	ui32 finishedTime = 0;
	ui32 waitFinishTime = 2000;
	Persistent<Function, CopyablePersistentTraits<Function>> _onFirstGather;
	bool hasOnFirstGather = false;
	ui32 gatherTime = 0;
	Persistent<Function, CopyablePersistentTraits<Function>> _onFirstSacrifice;
	bool hasOnFirstSacrifice = false;
	ui32 sacrificeTime = 0;
	Persistent<Function, CopyablePersistentTraits<Function>> _onFirstAttack;
	bool hasOnFirstAttack = false;
	ui32 attackedTime = 0;

	bool endScene = false;

	void Wrap(Handle<Object> exports);
	static void Wrapper(Handle<Object> exports);
};

extern GameState GS_EXAMPLE;
