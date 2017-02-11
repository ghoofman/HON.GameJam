#include "Globals.h"

OPcam CAMERA;
OPphysXScene* PHYSX_SCENE;
OPphysXControllerManager* PHYSX_CONTROLLERMANAGER;
bool _GLOBALS_INITIALIZED = false;
OPfmodSound* SND_HIT;
OPfontManager* FONT_MANAGER;
OPtexture2DOLD* TEX_BUTTON;
OPtexture2DOLD* TEX_BUTTON2;
OPtexture2DOLD* TEX_BUTTON3;
OPjavaScriptV8Compiled SCRIPT;

Army* ARMY = NULL;

void GlobalsInit() {
	if (_GLOBALS_INITIALIZED) return;
	_GLOBALS_INITIALIZED = true;

	ARMY = OPNEW(Army(50));

	SCRIPT = OPjavaScriptV8Compiled("main.js");


	CAMERA.SetPerspective(OPvec3Create(-5, 25, 40), OPVEC3_UP);
	PHYSX_SCENE = OPphysXSceneCreate(OPvec3Create(0, -9.8, 0), PhysXOnTrigger, PhysXOnContact, PhysXFilterShader);
	PHYSX_CONTROLLERMANAGER = OPphysXControllerCreateManager(PHYSX_SCENE);

	OPphysXMaterial* material = OPphysXCreateMaterial(0.8, 0.8, 0.1);
	OPphysXSceneAddWalls(PHYSX_SCENE, -50, 250, 50, -50, material, true);
	OPphysXSceneUpdate(PHYSX_SCENE, 16);

	OPphysXDebugger("127.0.0.1");

	SND_HIT = OPfmodLoad("Audio/hit.wav");

	FONT_MANAGER = OPNEW(OPfontManager);
	FONT_MANAGER->Init((OPfont*)OPCMAN.LoadGet("Ubuntu.opf"));
	FONT_MANAGER->_align = OPfontAlign::CENTER;
	FONT_MANAGER->scale = 0.5f;


	TEX_BUTTON = OPtexture2DCreate((OPtexture*)OPCMAN.LoadGet("Button_0.png"));
	TEX_BUTTON->Position.x = TEX_BUTTON->Texture->textureDesc.width / 4.0;
	TEX_BUTTON->Position.y = TEX_BUTTON->Texture->textureDesc.height / 4.0;
	TEX_BUTTON->Scale = OPvec2(0.5, 0.5);

	TEX_BUTTON2 = OPtexture2DCreate((OPtexture*)OPCMAN.LoadGet("Button_1.png"));
	TEX_BUTTON2->Position.x = TEX_BUTTON2->Texture->textureDesc.width / 4.0;
	TEX_BUTTON2->Position.y = 150;
	TEX_BUTTON2->Scale = OPvec2(0.5, 0.5);

	TEX_BUTTON3 = OPtexture2DCreate((OPtexture*)OPCMAN.LoadGet("Button_2.png"));
	TEX_BUTTON3->Position.x = TEX_BUTTON3->Texture->textureDesc.width / 4.0;
	TEX_BUTTON3->Position.y = 150;
	TEX_BUTTON3->Scale = OPvec2(0.5, 0.5);
}



void RenderButton(f32 x, f32 y, const OPchar* text, i8 state) {
	f32 offsetX = 2 * OPRENDERER_ACTIVE->OPWINDOW_ACTIVE->WindowWidth / 3.0;
	f32 offsetY = OPRENDERER_ACTIVE->OPWINDOW_ACTIVE->WindowHeight / 4.0;

	OPtexture2DOLD* btn = TEX_BUTTON;
	if (state == 1) {
		btn = TEX_BUTTON2;
	}
	else if (state == 2) {
		btn = TEX_BUTTON3;
	}

	btn->Position.x = offsetX;
	btn->Position.y = offsetY + y + TEX_BUTTON2->Texture->textureDesc.height / 4.0;
	OPtexture2DRender(btn);

	OPfontRenderBegin(FONT_MANAGER);
	OPfontRender(text, OPvec2(offsetX, offsetY + y + btn->Texture->textureDesc.height / 4.0 - 25 + (state == 2 ? 20 : 0)));
	OPfontRenderEnd();
}