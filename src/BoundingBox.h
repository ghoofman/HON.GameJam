#pragma once

#include "./OPengine.h"
#include "GameEntity.h"

class BoundingBox : public GameEntity {
private:

public:
	OPphysXRigidStatic* physX;
	PxShape* shapes = NULL;

	void Init(OPscene* scene);
	void Update(OPtimer* timer);
	void PrepRender(OPfloat delta);
	Handle<Object> Wrap(Handle<Object> result);
};