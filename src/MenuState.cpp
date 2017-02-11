#include "./MenuState.h"
#include "./GameState.h"
#include "Globals.h"

void MenuState::Init(OPgameState* last) {
	GlobalsInit();

}


OPint MenuState::Update(OPtimer* timer) {
	if (OPKEYBOARD.WasPressed(OPkeyboardKey::S)) {
		active++;
		active = active % 2;
	}
	if (OPKEYBOARD.WasPressed(OPkeyboardKey::W)) {
		active--;
		if (active < 0) {
			active = 1;
		}
	}

	if (OPKEYBOARD.WasPressed(OPkeyboardKey::ENTER) && active == 0) {
		OPgameState::Change(&GS_EXAMPLE);
	}

	if (OPKEYBOARD.WasPressed(OPkeyboardKey::ENTER) && active == 1) {
		return true;
	}

	return false;
}

void MenuState::Render(OPfloat delta) {
	OPrenderClear(0.0, 0.0, 0.0, 1);

	RenderButton(0, 0, "Start Game", (active == 0) + (active == 0 && OPKEYBOARD.IsDown(OPkeyboardKey::ENTER)));
	RenderButton(0, 150, "Exit", (active == 1) + (active == 1 && OPKEYBOARD.IsDown(OPkeyboardKey::ENTER)));

	OPrenderPresent();
}

OPint MenuState::Exit(OPgameState* next) {
	return 0;
}

MenuState GS_MENU;
