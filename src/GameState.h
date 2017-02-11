#pragma once

#include "./OPengine.h"
#include "Army.h"
#include "Enemy.h"
#include "StaticEntity.h"
#include "DynamicAllocator.h"

class GameState : public OPgameState {
	OPrendererForward rendererForward;

    void Init(OPgameState* last);
    OPint Update(OPtimer* time);
    void Render(OPfloat delta);
    OPint Exit(OPgameState* next);
public:
	OPscene scene;
	DynamicAllocator<StaticEntity>* staticEntities;
	DynamicAllocator<Enemy>* enemies;

	Persistent<Function, CopyablePersistentTraits<Function>> _onFinish;
	bool hasOnFinish = false;

	void Wrap(Handle<Object> exports);
	static void Wrapper(Handle<Object> exports);
};

extern GameState GS_EXAMPLE;
