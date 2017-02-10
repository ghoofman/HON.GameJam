#include "Globals.h"

OPcam CAMERA;
OPphysXScene* PHYSX_SCENE;
OPphysXControllerManager* PHYSX_CONTROLLERMANAGER;
bool _GLOBALS_INITIALIZED = false;

void GlobalsInit() {
	if (_GLOBALS_INITIALIZED) return;
	_GLOBALS_INITIALIZED = true;

	CAMERA.SetPerspective(OPvec3Create(-25, 25, 40), OPVEC3_UP);
	PHYSX_SCENE = OPphysXSceneCreate(OPvec3Create(0, -9.8, 0), PhysXOnTrigger, PhysXOnContact, PhysXFilterShader);
	PHYSX_CONTROLLERMANAGER = OPphysXControllerCreateManager(PHYSX_SCENE);

	OPphysXMaterial* material = OPphysXCreateMaterial(0.8, 0.8, 0.1);
	OPphysXSceneAddWalls(PHYSX_SCENE, -25, 25, 25, -25, material, true);
	OPphysXSceneUpdate(PHYSX_SCENE, 16);
	
	OPphysXDebugger("127.0.0.1");
}