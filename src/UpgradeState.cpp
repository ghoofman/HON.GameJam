#include "./UpgradeState.h"
#include "./GameState.h"
#include "Globals.h"

void UpgradeState::Init(OPgameState* last) {
	GlobalsInit();

}

OPint UpgradeState::Update(OPtimer* timer) {
	if (OPKEYBOARD.WasPressed(OPkeyboardKey::S)) {
		active++;
		active = active % 4;
	}
	if (OPKEYBOARD.WasPressed(OPkeyboardKey::W)) {
		active--;
		if (active < 0) {
			active = 3;
		}
	}

	if (OPKEYBOARD.WasPressed(OPkeyboardKey::ENTER) && active == 1) {
		ARMY->honor -= 5;
		for (ui32 i = 0; i < ARMY->index; i++) {
			ARMY->warriors[i]->damage += 10;
		}
	}

	if (OPKEYBOARD.WasPressed(OPkeyboardKey::ENTER) && active == 3) {
		OPgameState::Change(&GS_EXAMPLE);
	}

	return false;
}

void UpgradeState::Render(OPfloat delta) {
	OPrenderClear(0.0, 0.0, 0.0, 1);

	FONT_MANAGER->_color = OPvec4(1, 1, 1, 1);
	OPfontRenderBegin(FONT_MANAGER);
	char buffer[32];
	sprintf(buffer, "HONOR: %d", ARMY->honor);
	OPfontRender(buffer, OPvec2(OPRENDERER_ACTIVE->OPWINDOW_ACTIVE->WindowWidth / 2.0, 50));
	OPfontRenderEnd();

	FONT_MANAGER->_color = OPvec4(0, 0, 0, 1);
	RenderButton(0, 0, "Upgrade Armor (5)", (active == 0) + (active == 0 && OPKEYBOARD.IsDown(OPkeyboardKey::ENTER)));
	RenderButton(0, 100, "Upgrade Swords (5)", (active == 1) + (active == 1 && OPKEYBOARD.IsDown(OPkeyboardKey::ENTER)));
	RenderButton(0, 200, "Upgrade Health (5)", (active == 2) + (active == 2 && OPKEYBOARD.IsDown(OPkeyboardKey::ENTER)));
	RenderButton(0, 350, "Done", (active == 3) + (active == 3 && OPKEYBOARD.IsDown(OPkeyboardKey::ENTER)));

	OPrenderPresent();
}

OPint UpgradeState::Exit(OPgameState* next) {
	return 0;
}

UpgradeState GS_UPGRADE;
