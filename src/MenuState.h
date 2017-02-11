#pragma once

#include "./OPengine.h"
#include "Army.h"
#include "Enemy.h"
#include "StaticEntity.h"
#include "DynamicAllocator.h"

class MenuState : public OPgameState {
	i32 active = 0;
    void Init(OPgameState* last);
    OPint Update(OPtimer* time);
    void Render(OPfloat delta);
    OPint Exit(OPgameState* next);
public:

};

extern MenuState GS_MENU;
