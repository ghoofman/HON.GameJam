#include "PhysXOnTrigger.h"

#include "Player.h"
#include "Enemy.h"

/////////
// Triggers
//////////////////////

void ProcessPlayerEnemyTrigger(GameEntity* one, GameEntity* two, bool status) {
	Player* player = (Player*)one;
	Enemy* enemy = (Enemy*)two;

}


/////////
// Trigger Registration
//////////////////////

TriggerPair triggerFns[] = {
	{ GameEntityType::PLAYER, GameEntityType::ENEMY, ProcessPlayerEnemyTrigger }
};


/////////
// Trigger Processing
//////////////////////

bool TriggerPair::Process(GameEntity* one, GameEntity* two, bool status) {
	if (one->entityType == t0 && two->entityType == t1) {
		fn(one, two, status);
		return true;
	}
	else if (one->entityType == t1 && two->entityType == t0) {
		fn(two, one, status);
		return true;
	}
	return false;
}

void PhysXOnTrigger(OPphysXTrigger trigger) {

	for (OPuint i = 0; i < trigger.count; i++) {
		GameEntity* entity = (GameEntity*)trigger.triggerPairs[i].triggerShape->userData;
		if (entity == NULL) continue;

		GameEntity* actorEntity = (GameEntity*)trigger.triggerPairs[i].otherShape->userData;
		if (actorEntity == NULL) continue;

		bool lost = trigger.triggerPairs[i].status == PxPairFlag::eNOTIFY_TOUCH_LOST;

		ui32 fnCount = sizeof(triggerFns) / sizeof(*triggerFns);
		for (OPuint j = 0; j < fnCount; j++) {
			if (triggerFns[j].Process(entity, actorEntity, lost)) {
				break;
			}
		}
	}
}