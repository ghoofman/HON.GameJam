#include "./MenuState.h"
#include "./GameState.h"
#include "Globals.h"

void MenuState::Init(OPgameState* last) {
	GlobalsInit();

}


OPint MenuState::Update(OPtimer* timer) {
	if (OPGAMEPADS[0]->LeftThumbNowDown() || OPKEYBOARD.WasPressed(OPkeyboardKey::S)) {
		active++;
		active = active % 2;
	}
	if (OPGAMEPADS[0]->LeftThumbNowUp() || OPKEYBOARD.WasPressed(OPkeyboardKey::W)) {
		active--;
		if (active < 0) {
			active = 1;
		}
	}

	if ((OPGAMEPADS[0]->WasPressed(OPgamePadButton::A) || OPKEYBOARD.WasPressed(OPkeyboardKey::ENTER)) && active == 0) {
		OPfmodPlay(SND_PUSH); 
		OPgameState::Change(&GS_EXAMPLE);
	}

	if ((OPGAMEPADS[0]->WasPressed(OPgamePadButton::A) || OPKEYBOARD.WasPressed(OPkeyboardKey::ENTER)) && active == 1) {
		OPfmodPlay(SND_PUSH); 
		return true;
	}

	return false;
}

void MenuState::Render(OPfloat delta) {
	OPrenderClear(0.0, 0.0, 0.0, 1);

	FONT_MANAGER->_color = OPvec4(1, 1, 1, 1);
	FONT_MANAGER->scale = 1.5;// = OPvec4(1, 1, 1, 1);
	OPfontRenderBegin(FONT_MANAGER);
	OPfontRender("Samurai Showdown", OPvec2(OPRENDERER_ACTIVE->OPWINDOW_ACTIVE->WindowWidth / 3.0, 100));

	FONT_MANAGER->scale = 0.5;
	OPfontRender("Plays best with a gamepad", OPvec2(OPRENDERER_ACTIVE->OPWINDOW_ACTIVE->WindowWidth / 3.0, OPRENDERER_ACTIVE->OPWINDOW_ACTIVE->WindowHeight - 100));
	OPfontRenderEnd();
	FONT_MANAGER->scale = 0.5;

	RenderButton(0, 100, "Start Game", (active == 0) + (active == 0 && (OPGAMEPADS[0]->WasPressed(OPgamePadButton::A) || OPKEYBOARD.IsDown(OPkeyboardKey::ENTER))));
	RenderButton(0, 250, "Exit", (active == 1) + (active == 1 && (OPGAMEPADS[0]->WasPressed(OPgamePadButton::A) || OPKEYBOARD.IsDown(OPkeyboardKey::ENTER))));

	OPrenderPresent();
}

OPint MenuState::Exit(OPgameState* next) {
	return 0;
}

MenuState GS_MENU;
