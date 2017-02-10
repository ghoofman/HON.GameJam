#pragma once

#include "./OPengine.h"
#include "GameEntity.h"

class StaticEntity : public GameEntity {
private:
	i32 health;

public:
	OPphysXRigidStatic* physX;
	PxShape* shapes;
	OPrendererEntity* rendererEntity;

	void Init(OPscene* scene, const OPchar* model, const OPchar* texture);
	void Update(OPtimer* timer);
	void PrepRender(OPfloat delta);
	Handle<Object> Wrap(Handle<Object> result);
};