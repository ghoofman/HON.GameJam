#pragma once

#include "./OPengine.h"
#include "PhysXOnTrigger.h"
#include "PhysXOnContact.h"
#include "PhysXFilterShader.h"
#include "Army.h"

extern OPcam CAMERA;
extern OPphysXScene* PHYSX_SCENE;
extern OPphysXControllerManager* PHYSX_CONTROLLERMANAGER;
extern OPfmodSound* SND_HIT;
extern OPfmodSound* SND_PUSH;
extern OPfontManager* FONT_MANAGER;
extern OPtexture2DOLD* TEX_BUTTON;
extern OPtexture2DOLD* TEX_BUTTON2;
extern OPtexture2DOLD* TEX_BUTTON3;
extern OPtexture2DOLD* TEX_DIALOG;
extern Army* ARMY;
extern OPjavaScriptV8Compiled SCRIPT;

void GlobalsInit();
void GlobalsUpdate();
void RenderButton(f32 x, f32 y, const OPchar* text, i8 state);
void RenderDialog(OPchar** text, i32 count);