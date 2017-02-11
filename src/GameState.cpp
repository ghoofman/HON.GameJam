#include "./GameState.h"
#include "./UpgradeState.h"
#include "Globals.h"

void GameState::Init(OPgameState* last) {
	GlobalsInit();

	scene.Init(&rendererForward, 1000, 1000);
	scene.camera = &CAMERA;
	OPfloat shadowCameraSize = 40.0f;
	scene.shadowCamera->SetOrtho(OPvec3(-50, 50, 10), OPVEC3_ZERO, OPVEC3_UP, 25.0f, 150.0f, -shadowCameraSize, shadowCameraSize, -shadowCameraSize, shadowCameraSize);

	ARMY->AddToScene(&scene);

	staticEntities = OPNEW(DynamicAllocator<StaticEntity>(128));
	enemies = OPNEW(DynamicAllocator<Enemy>(128));
	
	SCOPE_AND_ISOLATE;
	
	Handle<Context> localContext = Local<Context>::New(isolate, V8CONTEXT);
	v8::Context::Scope context_scope(localContext);

	Handle<Object> gameLevelObject = JS_NEW_OBJECT();
	Wrap(gameLevelObject);
	Handle<Value> gameLevelValue = gameLevelObject;
	SCRIPT.Function("GameInit", 1, &gameLevelValue);
}

OPint GameState::Update(OPtimer* timer) {
	ARMY->Update(timer);
	ui32 enemiesAlive = 0;
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

	if (enemiesAlive == 0) {
		if (hasOnFinish) {
			SCOPE_AND_ISOLATE;

			Handle<Context> context = Local<Context>::New(isolate, SCRIPT.Context);
			v8::Context::Scope context_scope(context);

			Handle<Object> obj = JS_NEW_OBJECT();
			Local<Function> func;
			Handle<Value> values;
			func = Local<Function>::New(isolate, _onFinish);
			func->Call(obj, 0, &values);

		}
		OPgameState::Change(&GS_UPGRADE);
	}
	return false;
}

void GameState::Render(OPfloat delta) {
	CAMERA.target = ARMY->centerPoint;
	CAMERA.pos = ARMY->centerPoint + OPvec3(-5, 30, 40);
	scene.shadowCamera->target = ARMY->centerPoint;
	scene.shadowCamera->pos = ARMY->centerPoint + OPvec3(-50, 50, 5);
	CAMERA.UpdateView();
	scene.shadowCamera->UpdateView();

	ARMY->PrepRender(delta);
	for (ui32 i = 0; i < enemies->Count(); i++) {
		Enemy* enemy = enemies->Get(i);
		enemy->PrepRender(delta);
	}

	OPrenderClear(0.0, 0.0, 0.0, 1);
	scene.Render(delta);

	char buffer[32];
	sprintf(buffer, "HONOR: %d", ARMY->honor);

	OPfontRenderBegin(FONT_MANAGER);
	OPfontRender(buffer, OPvec2(OPRENDERER_ACTIVE->OPWINDOW_ACTIVE->WindowWidth / 2.0, 50));
	OPfontRenderEnd();

	OPrenderPresent();
}

OPint GameState::Exit(OPgameState* next) {
	// Cleanup time
	// Remove all of the physx stuff
	for (ui32 i = 0; i < enemies->Count(); i++) {
		if (enemies->Get(i)->physX == NULL) continue;
		enemies->Get(i)->physX->release();
	}
	for (ui32 i = 0; i < staticEntities->Count(); i++) {
		staticEntities->Get(i)->physX->release();
	}

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
}

void GameState::Wrapper(Handle<Object> exports) {
	SCOPE_AND_ISOLATE;

	Local<FunctionTemplate> tpl = JS_NEW_FUNCTION_TEMPLATE(_ExampleStateCreate);
	Handle<Object> result = tpl->GetFunction();

	JS_SET_NUMBER(result, "size", sizeof(GameState));
	JS_SET_OBJECT(exports, "GameState", result);
}


GameState GS_EXAMPLE;
