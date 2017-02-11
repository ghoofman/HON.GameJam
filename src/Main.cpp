//////////////////////////////////////
// Application Entry Point
//////////////////////////////////////
#include "./OPengine.h"
#include "Main.h"
#include "GameState.h"
#include "MenuState.h"
#include "SceneLoader.h"

//////////////////////////////////////
// Application Methods
//////////////////////////////////////

OPwindow mainWindow;


void GameWrapper(Handle<Object> exports) {
	OPlog("Wrapping Game");

	SCOPE_AND_ISOLATE;

	Handle<Object> wrapper = JS_NEW_OBJECT();
	GameState::Wrapper(wrapper);

	JS_SET_OBJECT(exports, "ExampleState", wrapper);
}

void ApplicationInit() {
	OPCMAN.Init(OPIFEX_ASSETS);
	OPloadersAddDefault();
	OPscriptAddLoader();
	SceneAddLoader();

	OPrenderSetup();

	OPwindowSystemInit();
	mainWindow.Init(NULL, OPwindowParameters("Main Window", false, 1280, 720));

	// Sound System initialize
	OPfmodInit();

	// Physics Setup
	OPphysXInit();

	// Scripting Setup
	OPJAVASCRIPTV8_CUSTOMWRAPPER = GameWrapper;
	OPjavaScriptV8Init();

	OPrenderInit(&mainWindow);

	//OPgameState::Change(&GS_EXAMPLE);
	OPgameState::Change(&GS_MENU);
}

OPint ApplicationUpdate(OPtimer* timer) {
	if (mainWindow.Update()) {
		// Window received an exit request
		return 1;
	}

	OPinputSystemUpdate(timer);
	if (OPKEYBOARD.WasPressed(OPkeyboardKey::ESCAPE)) return 1;

	OPfmodUpdate();

	return ActiveState->Update(timer);
}

void ApplicationRender(OPfloat delta) {
	ActiveState->Render(delta);
}

void ApplicationDestroy() {

}

void ApplicationSetup() {
	OPinitialize = ApplicationInit;
	OPupdate = ApplicationUpdate;
	OPrender = ApplicationRender;
	OPdestroy = ApplicationDestroy;
}

//////////////////////////////////////
// Application Entry Point
//////////////////////////////////////
OP_MAIN_START

	OPLOGLEVEL = (ui32)OPlogLevel::VERBOSE;
	OPlog("Starting up OPifex Engine");

	ApplicationSetup();

	OP_MAIN_RUN
	//OP_MAIN_RUN_STEPPED

OP_MAIN_END
