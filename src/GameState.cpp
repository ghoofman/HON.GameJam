#include "./GameState.h"
#include "Globals.h"

void GameState::Init(OPgameState* last) {
	scene.Init(&rendererForward, 1000, 1000);
	scene.camera = &CAMERA;

	GlobalsInit();


	player = OPNEW(Player(&scene));
	staticEntities = OPNEW(DynamicAllocator<StaticEntity>(128));


	SCOPE_AND_ISOLATE;

	SCRIPT = OPjavaScriptV8Compiled("main.js");

	Handle<Context> localContext = Local<Context>::New(isolate, V8CONTEXT);
	v8::Context::Scope context_scope(localContext);

	Handle<Object> gameLevelObject = JS_NEW_OBJECT();
	Wrap(gameLevelObject);
	Handle<Value> gameLevelValue = gameLevelObject;
	SCRIPT.Function("GameInit", 1, &gameLevelValue);


}


OPint GameState::Update(OPtimer* timer) {
	player->Update(timer);
	OPphysXSceneUpdate(PHYSX_SCENE, 16);
	scene.Update(timer);
	return false;
}

void GameState::Render(OPfloat delta) {
	player->PrepRender(delta);
	OPrenderClear(0, 0, 0, 1);
	scene.Render(delta);
	OPrenderPresent();
}

OPint GameState::Exit(OPgameState* next) {
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

void GameState::Wrap(Handle<Object> result) {
	JS_SET_PTR(result, this);
	JS_SET_METHOD(result, "Init", _ExampleStateInitSelf);
	JS_SET_METHOD(result, "AddDrawable", _GameStateAddDrawableSelf);
	JS_SET_METHOD(result, "AddStatic", _GameStateAddStaticSelf);
}

void GameState::Wrapper(Handle<Object> exports) {
	SCOPE_AND_ISOLATE;

	Local<FunctionTemplate> tpl = JS_NEW_FUNCTION_TEMPLATE(_ExampleStateCreate);
	Handle<Object> result = tpl->GetFunction();

	JS_SET_NUMBER(result, "size", sizeof(GameState));
	JS_SET_OBJECT(exports, "GameState", result);
}


GameState GS_EXAMPLE;
