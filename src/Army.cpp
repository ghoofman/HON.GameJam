#include "Army.h"
#include "Globals.h"

Army::Army(ui32 size) {
	ASSERT(size < MAX_ARMY_SIZE + 1, "CANT HAVE THAT MANY ARMY PEOPLE");

	index = size;
	for (ui32 i = 0; i < size; i++) {
		warriors[i] = OPNEW(Player());
		warriors[i]->position = OPvec3(-5 + OPrandom() * 10, 1, -5 + OPrandom() * 10);
	}
}

void Army::AddToScene(OPscene* scene) {
	for (ui32 i = 0; i < index; i++) {
		warriors[i]->AddToScene(scene);
	}
}

void Army::RemoveFromScene() {
	for (ui32 i = 0; i < index; i++) {
		warriors[i]->RemoveFromScene();
	}
}

void Army::SetSpawn(OPvec3 pos) {
	for (ui32 i = 0; i < index; i++) {
		warriors[i]->position = pos + OPvec3(-5 + OPrandom() * 10, 1, -5 + OPrandom() * 10);
		if (warriors[i]->physX != NULL) {
			OPphysXControllerSetPos(warriors[i]->physX, warriors[i]->position);
		}
	}
}

void Army::Update(OPtimer* timer) {

	if (index > 1) {
		if (OPGAMEPADS[0]->WasPressed(OPgamePadButton::B) || OPKEYBOARD.WasPressed(OPkeyboardKey::C)) {
			// Sacrifice a warrior
			warriors[index - 1]->rendererEntity->material->visible = false;
			warriors[index - 1]->rendererEntity->shadowMaterial->visible = false;
			warriors[index - 1]->physX->release();
			index--;
			honor += 2;
		}
	}
	

	centerPoint = warriors[0]->position;
	for (ui32 i = 1; i < index; i++) {
		centerPoint.x += warriors[i]->position.x;
		centerPoint.y += warriors[i]->position.y;
		centerPoint.z += warriors[i]->position.z;
	}
	centerPoint.x /= index;
	centerPoint.y /= index;
	centerPoint.z /= index;

	if (OPGAMEPADS[0]->WasPressed(OPgamePadButton::Y) || OPKEYBOARD.WasPressed(OPkeyboardKey::E)) {

		for (ui32 i = 0; i < index; i++) {
			warriors[i]->target = centerPoint;
			warriors[i]->hasTarget = true;
		}
	}
	for (ui32 i = 0; i < index; i++) {
		warriors[i]->Update(timer);
	}
}

void Army::Attack(Enemy* enemy) {
	if (enemy->health <= 0) return;

	for (ui32 i = 0; i < index; i++) {
		if (!warriors[i]->attacking && !enemy->CanAttack()) continue;

		// Check for player->enemy collision
		
		if (OPvec3Dist(enemy->position, warriors[i]->position) < 1.5) {
			if (warriors[i]->attacking) {
				enemy->DealDamage(warriors[i]->damage);
				OPfmodPlay(SND_HIT);
				if (enemy->health <= 0) {
					honor++;
					return;
				}
			}

			if (enemy->CanAttack()) {
				enemy->attacked = 0;
				warriors[i]->DealDamage(enemy->damage);
				OPfmodPlay(SND_HIT);
				if (warriors[i]->health <= 0) {
					if (i < index - 1) {
						warriors[i] = warriors[index - 1];
					}
					index--;
				}
				return;
			}
		}
	}
}
bool Army::Within(Enemy* enemy) {
	if (enemy->health <= 0) return false;

	for (ui32 i = 0; i < index; i++) {
		if (OPvec3Dist(enemy->position, warriors[i]->position) < (OPMAX(index, 5))) {
			return true;
		}
	}
	return false;
}

void Army::PrepRender(OPfloat delta) {
	for (ui32 i = 0; i < index; i++) {
		warriors[i]->PrepRender(delta);
	}
}

Handle<Object> Army::Wrap(Handle<Object> result) {
	return result;
}