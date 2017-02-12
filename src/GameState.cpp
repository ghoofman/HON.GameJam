#include "./GameState.h"
#include "./UpgradeState.h"
#include "Globals.h"

void GameState::Init(OPgameState* last) {
	GlobalsInit();

	scene.Init(&rendererForward, 1000, 1000);
	scene.camera = &CAMERA;
	CAMERA.target = OPvec3(0);
	CAMERA.pos = CAMERA.target + OPvec3(-5, 20, 30);
	camera_target = scene.camera->pos;
	OPfloat shadowCameraSize = 40.0f;
	scene.shadowCamera->SetOrtho(OPvec3(-50, 50, 10), OPVEC3_ZERO, OPVEC3_UP, 25.0f, 150.0f, -shadowCameraSize, shadowCameraSize, -shadowCameraSize, shadowCameraSize);

	endScene = false;
	canExit = false;

	//dialog = "Samurai!";

	ARMY->AddToScene(&scene);

	staticEntities = OPNEW(DynamicAllocator<StaticEntity>(128));
	enemies = OPNEW(DynamicAllocator<Enemy>(128));
	boundingBoxes = OPNEW(DynamicAllocator<BoundingBox>(128));
	
	SCOPE_AND_ISOLATE;
	
	Handle<Context> localContext = Local<Context>::New(isolate, V8CONTEXT);
	v8::Context::Scope context_scope(localContext);

	Handle<Object> gameLevelObject = JS_NEW_OBJECT();
	Wrap(gameLevelObject);
	Handle<Value> gameLevelValue = gameLevelObject;
	SCRIPT.Function("GameInit", 1, &gameLevelValue);
}

OPint GameState::Update(OPtimer* timer) {

	if (dialogCount != 0) {
		if (OPKEYBOARD.WasPressed(OPkeyboardKey::ENTER) || OPGAMEPADS[0]->WasPressed(OPgamePadButton::A)) {
			for (ui32 i = 0; i < dialogCount; i++) {
				OPfree(dialog[i]);
			}
			dialogCount = 0;
			if (hasOnDialogFinish) {
				hasOnDialogFinish = false;
				SCOPE_AND_ISOLATE;

				Handle<Context> context = Local<Context>::New(isolate, SCRIPT.Context);
				v8::Context::Scope context_scope(context);

				Handle<Object> obj = JS_NEW_OBJECT();
				Local<Function> func;
				Handle<Value> values;
				func = Local<Function>::New(isolate, _onDialogFinish);
				func->Call(obj, 0, &values);
				return false;
			}
		}
		else {
			return false;
		}
	}

	if (hasOnFirstGather && ARMY->gathered) {
		gatherTime += timer->Elapsed;
		if (gatherTime > 500) {
			hasOnFirstGather = false;
			SCOPE_AND_ISOLATE;

			Handle<Context> context = Local<Context>::New(isolate, SCRIPT.Context);
			v8::Context::Scope context_scope(context);

			Handle<Object> obj = JS_NEW_OBJECT();
			Local<Function> func;
			Handle<Value> values;
			func = Local<Function>::New(isolate, _onFirstGather);
			func->Call(obj, 0, &values);
		}
	}

	if (hasOnFirstSacrifice && ARMY->sacrificed) {
		sacrificeTime += timer->Elapsed;
		if (sacrificeTime > 1500) {
			hasOnFirstSacrifice = false;
			SCOPE_AND_ISOLATE;

			Handle<Context> context = Local<Context>::New(isolate, SCRIPT.Context);
			v8::Context::Scope context_scope(context);

			Handle<Object> obj = JS_NEW_OBJECT();
			Local<Function> func;
			Handle<Value> values;
			func = Local<Function>::New(isolate, _onFirstSacrifice);
			func->Call(obj, 0, &values);
		}
	}

	if (hasOnFirstAttack && ARMY->attacked) {
		attackedTime += timer->Elapsed;
		if (attackedTime > 1500) {
			hasOnFirstAttack = false;
			SCOPE_AND_ISOLATE;

			Handle<Context> context = Local<Context>::New(isolate, SCRIPT.Context);
			v8::Context::Scope context_scope(context);

			Handle<Object> obj = JS_NEW_OBJECT();
			Local<Function> func;
			Handle<Value> values;
			func = Local<Function>::New(isolate, _onFirstAttack);
			func->Call(obj, 0, &values);
		}
	}

	ARMY->Update(timer);
	enemiesAlive = 0;
	for (ui32 i = 0; i < enemies->Count(); i++) {
		Enemy* enemy = enemies->Get(i);
		if (ARMY->Within(enemy)) {
			enemy->target = ARMY->centerPoint;
			enemy->hasTarget = true;
		}
		enemy->Update(timer);
		ARMY->Attack(enemy);
		if (enemy->health > 0) {
			enemiesAlive++;
		}
	}

	OPphysXSceneUpdate(PHYSX_SCENE, 16);
	scene.Update(timer);


	camera_target = ARMY->centerPoint;

	OPvec3 inbetweenPos = CAMERA.target - camera_target;
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

	if (enemiesAlive == 0 && hasOnFinish) {
		finishedTime += timer->Elapsed;
		if (OPGAMEPADS[0]->WasPressed(OPgamePadButton::A) || OPKEYBOARD.WasPressed(OPkeyboardKey::ENTER)) {
			canExit = true;
		}
		if (canExit && finishedTime > waitFinishTime) {

			hasOnFinish = false;

			SCOPE_AND_ISOLATE;

			Handle<Context> context = Local<Context>::New(isolate, SCRIPT.Context);
			v8::Context::Scope context_scope(context);

			Handle<Object> obj = JS_NEW_OBJECT();
			Local<Function> func;
			Handle<Value> values;
			func = Local<Function>::New(isolate, _onFinish);
			func->Call(obj, 0, &values);

			if (endScene) {
				OPgameState::Change(&GS_UPGRADE);
			}
		}
	}
	return false;
}

void GameState::Render(OPfloat delta) {
	CAMERA.UpdateView();
	scene.shadowCamera->UpdateView();

	ARMY->PrepRender(delta);
	for (ui32 i = 0; i < enemies->Count(); i++) {
		Enemy* enemy = enemies->Get(i);
		enemy->PrepRender(delta);
	}

	OPrenderClear(0.0, 0.0, 0.0, 1);
	scene.Render(delta);

	OPrenderCull(false);
	for (ui32 i = 0; i < ARMY->index; i++) {
		if (ARMY->warriors[i]->dead || ARMY->warriors[i]->suiciding) continue;

		OPsprite3DRender(ARMY->warriors[i]->healthTex, &CAMERA);
	}

	char buffer[32];
	sprintf(buffer, "HONOR: %d", ARMY->honor);

	OPfontRenderBegin(FONT_MANAGER);
	OPfontRender(buffer, OPvec2(OPRENDERER_ACTIVE->OPWINDOW_ACTIVE->WindowWidth / 2.0, 50));
	if (enemiesAlive == 0 && !hasOnFirstAttack && !hasOnFirstGather && !hasOnFirstSacrifice) {
		FONT_MANAGER->_color = OPvec4(1, 1, 1, 1);
		OPfontRender("A or Enter to continue", OPvec2(OPRENDERER_ACTIVE->OPWINDOW_ACTIVE->WindowWidth / 2.0, OPRENDERER_ACTIVE->OPWINDOW_ACTIVE->Height - 100));
	}
	OPfontRenderEnd();

	if (dialogCount != 0) {
		RenderDialog(dialog, dialogCount);
	}

	OPrenderPresent();
}

OPint GameState::Exit(OPgameState* next) {
	// Cleanup time
	// Remove all of the physx stuff
	for (ui32 i = 0; i < enemies->Count(); i++) {
		if (enemies->Get(i)->physX == NULL) continue;
		enemies->Get(i)->physX->release();
	}
	for (ui32 i = 0; i < boundingBoxes->Count(); i++) {
		if (boundingBoxes->Get(i)->physX == NULL) continue;
		boundingBoxes->Get(i)->physX->release();
	}
	for (ui32 i = 0; i < staticEntities->Count(); i++) {
		staticEntities->Get(i)->physX->release();
	}

	boundingBoxes->Destroy();
	enemies->Destroy();
	staticEntities->Destroy();

	ARMY->RemoveFromScene();
	scene.Destroy();

	return 0;
}


JS_HELPER_SELF_WRAPPER(_SceneEntitySetTransform) {
	SCOPE_AND_ISOLATE;

	OPrendererEntity* ptr = JS_NEXT_ARG_AS(OPrendererEntity);
	OPvec3 pos = JS_NEXT_ARG_AS_VEC3;
	OPvec3 rot = JS_NEXT_ARG_AS_VEC3;
	OPvec3 scl = JS_NEXT_ARG_AS_VEC3;

	ptr->world.
		SetRotY(rot.y)->
		RotZ(rot.z)->
		RotX(rot.x)->
		Translate(pos)->
		Scl(scl);

	JS_RETURN_NULL;
}


JS_HELPER_SELF_WRAPPER(_GameStateAddDrawable) {
	SCOPE_AND_ISOLATE;

	GameState* ptr = JS_NEXT_ARG_AS(GameState);
	JS_NEXT_ARG_AS_STRING(model);
	JS_NEXT_ARG_AS_STRING(texture);

	OPrendererEntity* sceneEntity = ptr->scene.Add((OPmodel*)OPCMAN.LoadGet(model), OPrendererEntityDesc(false));
	sceneEntity->world = OPMAT4_IDENTITY;
	sceneEntity->SetAlbedoMap(texture);


	Handle<Object> result = JS_NEW_OBJECT();
	JS_SET_PTR(result, sceneEntity);
	Handle<Object> entity = JS_NEW_OBJECT();
	JS_SET_PTR(entity, sceneEntity);
	JS_SET_METHOD(entity, "SetTransform", _SceneEntitySetTransformSelf);
	JS_SET_OBJECT(result, "Entity", entity);
	JS_RETURN(result);
}

JS_HELPER_SELF_WRAPPER(_GameStateAddStatic) {
	SCOPE_AND_ISOLATE;

	GameState* ptr = JS_NEXT_ARG_AS(GameState);
	JS_NEXT_ARG_AS_STRING(model);
	JS_NEXT_ARG_AS_STRING(texture);

	StaticEntity* entity = ptr->staticEntities->Next();
	entity->Init(&ptr->scene, model, texture);
	Handle<Object> obj = JS_NEW_OBJECT();
	JS_RETURN(entity->Wrap(obj));
}

JS_HELPER_SELF_WRAPPER(_GameStateAddEnemy) {
	SCOPE_AND_ISOLATE;

	GameState* ptr = JS_NEXT_ARG_AS(GameState);
	JS_NEXT_ARG_AS_STRING(model);
	JS_NEXT_ARG_AS_STRING(texture);

	Enemy* entity = ptr->enemies->Next();
	entity->Init(&ptr->scene, model, texture);
	Handle<Object> obj = JS_NEW_OBJECT();
	JS_RETURN(entity->Wrap(obj));
}

JS_HELPER_SELF_WRAPPER(_GameStateSetSpawn) {
	SCOPE_AND_ISOLATE;

	GameState* ptr = JS_NEXT_ARG_AS(GameState);
	f32 x = JS_NEXT_ARG_AS_NUMBER;
	f32 y = JS_NEXT_ARG_AS_NUMBER;
	f32 z = JS_NEXT_ARG_AS_NUMBER;

	ARMY->SetSpawn(OPvec3(x, y, z));

	JS_RETURN_NULL;
}

JS_RETURN_VAL _ExampleStateCreate(const JS_ARGS& args) {
	SCOPE_AND_ISOLATE;

	GameState* state = OPNEW(GameState());
	Handle<Object> obj = JS_NEW_OBJECT();
	state->Wrap(obj);

	JS_RETURN(obj);
}

JS_HELPER_SELF_WRAPPER(_ExampleStateInit) {
	SCOPE_AND_ISOLATE;

	GameState* ptr = JS_NEXT_ARG_AS(GameState);

	//ptr->Init();

	JS_RETURN_NULL;
}

JS_HELPER_SELF_WRAPPER(_GameStateOnFinish) {
	SCOPE_AND_ISOLATE;

	GameState* ptr = JS_NEXT_ARG_AS(GameState);

	Handle<Function> funcLocal = Handle<Function>::Cast(args[_JS_ARGC++]);

	ptr->_onFinish = Persistent<Function, CopyablePersistentTraits<Function>>(isolate, funcLocal);
	ptr->hasOnFinish = true;

	if (args.Length() != _JS_ARGC) {
		ui32 t = JS_NEXT_ARG_AS_INTEGER;
		ptr->waitFinishTime = t;
	}
	else {
		ptr->waitFinishTime = 2000;
	}

	JS_RETURN_NULL;
}

JS_HELPER_SELF_WRAPPER(_GameStateAddBoundingBox) {
	SCOPE_AND_ISOLATE;

	GameState* ptr = JS_NEXT_ARG_AS(GameState);

	BoundingBox* entity = ptr->boundingBoxes->Next();
	entity->Init(&ptr->scene);
	Handle<Object> obj = JS_NEW_OBJECT();
	JS_RETURN(entity->Wrap(obj));
}

JS_HELPER_SELF_WRAPPER(_GameStateSetDialog) {
	SCOPE_AND_ISOLATE;

	GameState* ptr = JS_NEXT_ARG_AS(GameState);
	ui32 count = JS_NEXT_ARG_AS_INTEGER;
	ptr->dialogCount = count;
	for (ui32 i = 0; i < count; i++) {
		JS_NEXT_ARG_AS_STRING(msg);
		ptr->dialog[i] = OPstringCopy(msg);
	}

	if (args.Length() != _JS_ARGC) {
		Handle<Function> funcLocal = Handle<Function>::Cast(args[_JS_ARGC++]);
		ptr->_onDialogFinish = Persistent<Function, CopyablePersistentTraits<Function>>(isolate, funcLocal);
		ptr->hasOnDialogFinish = true;
	}

	JS_RETURN_NULL;
}

JS_HELPER_SELF_WRAPPER(_GameStateSetOnGather) {
	SCOPE_AND_ISOLATE;

	GameState* ptr = JS_NEXT_ARG_AS(GameState);
	Handle<Function> funcLocal = Handle<Function>::Cast(args[_JS_ARGC++]);
	ptr->_onFirstGather = Persistent<Function, CopyablePersistentTraits<Function>>(isolate, funcLocal);
	ptr->hasOnFirstGather = true;

	JS_RETURN_NULL;
}

JS_HELPER_SELF_WRAPPER(_GameStateSetOnSacrifice) {
	SCOPE_AND_ISOLATE;

	GameState* ptr = JS_NEXT_ARG_AS(GameState);
	Handle<Function> funcLocal = Handle<Function>::Cast(args[_JS_ARGC++]);
	ptr->_onFirstSacrifice = Persistent<Function, CopyablePersistentTraits<Function>>(isolate, funcLocal);
	ptr->hasOnFirstSacrifice = true;

	JS_RETURN_NULL;
}

JS_HELPER_SELF_WRAPPER(_GameStateSetOnAttack) {
	SCOPE_AND_ISOLATE;

	GameState* ptr = JS_NEXT_ARG_AS(GameState);
	Handle<Function> funcLocal = Handle<Function>::Cast(args[_JS_ARGC++]);
	ptr->_onFirstAttack = Persistent<Function, CopyablePersistentTraits<Function>>(isolate, funcLocal);
	ptr->hasOnFirstAttack = true;

	JS_RETURN_NULL;
}

JS_HELPER_SELF_WRAPPER(_GameStateEndScene) {
	SCOPE_AND_ISOLATE;

	GameState* ptr = JS_NEXT_ARG_AS(GameState);
	ptr->endScene = true;

	JS_RETURN_NULL;
}

void GameState::Wrap(Handle<Object> result) {
	JS_SET_PTR(result, this);
	JS_SET_METHOD(result, "Init", _ExampleStateInitSelf);
	JS_SET_METHOD(result, "AddDrawable", _GameStateAddDrawableSelf);
	JS_SET_METHOD(result, "AddStatic", _GameStateAddStaticSelf);
	JS_SET_METHOD(result, "AddEnemy", _GameStateAddEnemySelf);
	JS_SET_METHOD(result, "SetSpawn", _GameStateSetSpawnSelf);
	JS_SET_METHOD(result, "OnFinish", _GameStateOnFinishSelf);
	JS_SET_METHOD(result, "SetDialog", _GameStateSetDialogSelf);
	JS_SET_METHOD(result, "SetOnGather", _GameStateSetOnGatherSelf);
	JS_SET_METHOD(result, "SetOnSacrifice", _GameStateSetOnSacrificeSelf);
	JS_SET_METHOD(result, "SetOnAttack", _GameStateSetOnAttackSelf);
	JS_SET_METHOD(result, "EndScene", _GameStateEndSceneSelf);
	JS_SET_METHOD(result, "AddBoundingBox", _GameStateAddBoundingBoxSelf);
}

void GameState::Wrapper(Handle<Object> exports) {
	SCOPE_AND_ISOLATE;

	Local<FunctionTemplate> tpl = JS_NEW_FUNCTION_TEMPLATE(_ExampleStateCreate);
	Handle<Object> result = tpl->GetFunction();

	JS_SET_NUMBER(result, "size", sizeof(GameState));
	JS_SET_OBJECT(exports, "GameState", result);
}


GameState GS_EXAMPLE;
