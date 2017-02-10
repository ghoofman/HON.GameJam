#pragma once

#include "./OPengine.h"
#include "Player.h"
#include "StaticEntity.h"
#include "DynamicAllocator.h"

class GameState : public OPgameState {
	OPjavaScriptV8Compiled SCRIPT;
	OPrendererForward rendererForward;

	Player* player;

    void Init(OPgameState* last);
    OPint Update(OPtimer* time);
    void Render(OPfloat delta);
    OPint Exit(OPgameState* next);
public:
	OPscene scene;
	DynamicAllocator<StaticEntity>* staticEntities;

	void Wrap(Handle<Object> exports);
	static void Wrapper(Handle<Object> exports);
};

extern GameState GS_EXAMPLE;
