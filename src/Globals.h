#pragma once

#include "./OPengine.h"
#include "PhysXOnTrigger.h"
#include "PhysXOnContact.h"
#include "PhysXFilterShader.h"

extern OPcam CAMERA;
extern OPphysXScene* PHYSX_SCENE;
extern OPphysXControllerManager* PHYSX_CONTROLLERMANAGER;

void GlobalsInit();