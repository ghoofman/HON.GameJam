#pragma once

#include "./OPengine.h"
#include "Army.h"
#include "Enemy.h"
#include "StaticEntity.h"
#include "DynamicAllocator.h"

class UpgradeState : public OPgameState {
	i32 active = 0;
    void Init(OPgameState* last);
    OPint Update(OPtimer* time);
    void Render(OPfloat delta);
    OPint Exit(OPgameState* next);
public:
	OPscene scene;
	OPrendererForward rendererForward;

};

extern UpgradeState GS_UPGRADE;
