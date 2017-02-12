#include "./UpgradeState.h"
#include "./GameState.h"
#include "Globals.h"

void UpgradeState::Init(OPgameState* last) {
	GlobalsInit();

	scene.Init(&rendererForward, 1000, 1000);
	scene.camera = &CAMERA;
	CAMERA.target = OPvec3(0);
	CAMERA.pos = CAMERA.target + OPvec3(-5, 20, 30);
	OPfloat shadowCameraSize = 40.0f;
	scene.shadowCamera->SetOrtho(OPvec3(-50, 50, 10), OPVEC3_ZERO, OPVEC3_UP, 25.0f, 150.0f, -shadowCameraSize, shadowCameraSize, -shadowCameraSize, shadowCameraSize);

	ARMY->AddToScene(&scene);
	ARMY->SetSpawn(OPvec3(0));
}

OPint UpgradeState::Update(OPtimer* timer) {
	ARMY->Update(timer);

	if (OPGAMEPADS[0]->LeftThumbNowDown() || OPKEYBOARD.WasPressed(OPkeyboardKey::S)) {
		active++;
		active = active % 4;
	}
	if (OPGAMEPADS[0]->LeftThumbNowUp() || OPKEYBOARD.WasPressed(OPkeyboardKey::W)) {
		active--;
		if (active < 0) {
			active = 3;
		}
	}

	if ((OPGAMEPADS[0]->WasPressed(OPgamePadButton::A) || OPKEYBOARD.WasPressed(OPkeyboardKey::ENTER)) && active == 0 && ARMY->honor >= 5) {
		ARMY->honor -= 5;
		OPfmodPlay(SND_PUSH);
		ARMY->Spawn(&scene);
	}
	if ((OPGAMEPADS[0]->WasPressed(OPgamePadButton::A) || OPKEYBOARD.WasPressed(OPkeyboardKey::ENTER)) && active == 1 && ARMY->honor >= 5) {
		ARMY->honor -= 5;
		OPfmodPlay(SND_PUSH);
		for (ui32 i = 0; i < ARMY->index; i++) {
			ARMY->warriors[i]->damage += 1;
		}
	}
	if ((OPGAMEPADS[0]->WasPressed(OPgamePadButton::A) || OPKEYBOARD.WasPressed(OPkeyboardKey::ENTER)) && active == 2 && ARMY->honor >= 1) {
		ARMY->honor -= 1;
		OPfmodPlay(SND_PUSH);
		for (ui32 i = 0; i < ARMY->index; i++) {
			ARMY->warriors[i]->health = 10;
		}
	}

	if ((OPGAMEPADS[0]->WasPressed(OPgamePadButton::A) || OPKEYBOARD.WasPressed(OPkeyboardKey::ENTER)) && active == 3) {
		OPfmodPlay(SND_PUSH); 
		OPgameState::Change(&GS_EXAMPLE);
	}

	scene.Update(timer);

	OPvec3 camera_target = ARMY->centerPoint;

	OPvec3 inbetweenPos = CAMERA.target - camera_target - OPvec3(10, 0, 0);
	if (OPvec3Len(inbetweenPos) < 0.05) {

	}
	else {
		OPfloat len = OPvec3Len(inbetweenPos);
		inbetweenPos.Norm();
		inbetweenPos = inbetweenPos * (0.01 * timer->Elapsed * (OPMIN(1.0, len)));

		CAMERA.target -= inbetweenPos;
		CAMERA.pos = CAMERA.target + OPvec3(-5, 20, 30);
		scene.shadowCamera->target = CAMERA.target;
		scene.shadowCamera->pos = CAMERA.target + OPvec3(-50, 50, 5);
	}

	return false;
}

void UpgradeState::Render(OPfloat delta) {
	CAMERA.UpdateView();
	scene.shadowCamera->UpdateView();
	OPrenderClear(0.0, 0.0, 0.0, 1);

	ARMY->PrepRender(delta);
	scene.Render(delta);

	OPrenderCull(false);
	for (ui32 i = 0; i < ARMY->index; i++) {
		if (ARMY->warriors[i]->dead || ARMY->warriors[i]->suiciding) continue;

		OPsprite3DRender(ARMY->warriors[i]->healthTex, &CAMERA);
	}

	FONT_MANAGER->_color = OPvec4(1, 1, 1, 1);
	OPfontRenderBegin(FONT_MANAGER);
	char buffer[32];
	sprintf(buffer, "HONOR: %d", ARMY->honor);
	OPfontRender(buffer, OPvec2(OPRENDERER_ACTIVE->OPWINDOW_ACTIVE->WindowWidth / 2.0, 50));

	FONT_MANAGER->_color = OPvec4(1, 1, 1, 1);
	sprintf(buffer, "Attack Damage: %d", ARMY->warriors[0]->damage);
	OPfontRender(buffer, OPvec2(OPRENDERER_ACTIVE->OPWINDOW_ACTIVE->WindowWidth / 3.0, OPRENDERER_ACTIVE->OPWINDOW_ACTIVE->WindowHeight - 100));

	OPfontRenderEnd();

	FONT_MANAGER->_color = OPvec4(0, 0, 0, 1);
	RenderButton(0, 0, "New Samurai (5)", (active == 0) + (active == 0 && (OPGAMEPADS[0]->WasPressed(OPgamePadButton::A) || OPKEYBOARD.IsDown(OPkeyboardKey::ENTER))));
	RenderButton(0, 100, "Upgrade Swords (5)", (active == 1) + (active == 1 && (OPGAMEPADS[0]->WasPressed(OPgamePadButton::A) || OPKEYBOARD.IsDown(OPkeyboardKey::ENTER))));
	RenderButton(0, 200, "Restore Health (1)", (active == 2) + (active == 2 && (OPGAMEPADS[0]->WasPressed(OPgamePadButton::A) || OPKEYBOARD.IsDown(OPkeyboardKey::ENTER))));
	RenderButton(0, 350, "Done", (active == 3) + (active == 3 && (OPGAMEPADS[0]->WasPressed(OPgamePadButton::A) || OPKEYBOARD.IsDown(OPkeyboardKey::ENTER))));


	OPrenderPresent();
}

OPint UpgradeState::Exit(OPgameState* next) {
	return 0;
}

UpgradeState GS_UPGRADE;
