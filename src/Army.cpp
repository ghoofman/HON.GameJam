#include "Army.h"
#include "Globals.h"

Army::Army(ui32 size) {
	ASSERT(size < MAX_ARMY_SIZE + 1, "CANT HAVE THAT MANY ARMY PEOPLE");

	index = size;
	for (ui32 i = 0; i < size; i++) {
		warriors[i] = OPNEW(Player());
		warriors[i]->position = OPvec3(i / 5, 0, i % index);
	}
}

void Army::AddToScene(OPscene* scene) {
	for (ui32 i = 0; i < index; i++) {
		if (warriors[i]->dead || warriors[i]->suiciding) {
			if (warriors[i]->suiciding) {
				sacrifices++;
			}
			if (i < index - 1) {
				warriors[i] = warriors[index - 1];
			}
			index--;
			i--;
		}
	}
	//sacrifices = 0;
	//while (spawn > 0) {
	//	spawn--;

	//	// Each sacrifice gains 2
	//	if (index + 1 < MAX_ARMY_SIZE + 1) {
	//		warriors[index] = OPNEW(Player());
	//		index++;
	//	}
	//}

	//spawn = 0;


	for (ui32 i = 0; i < index; i++) {
		warriors[i]->AddToScene(scene);
	}
}

void Army::Spawn(OPscene* scene) {
	// Each sacrifice gains 2
	if (index + 1 < MAX_ARMY_SIZE + 1) {
		warriors[index] = OPNEW(Player());
		warriors[index]->position = centerPoint + OPvec3(1.5f * (index / 5), 0, 1.5f * (index % 5));
		warriors[index]->AddToScene(scene);
		index++;
	}
}

void Army::RemoveFromScene() {
	for (ui32 i = 0; i < index; i++) {
		warriors[i]->RemoveFromScene();
	}
}

void Army::SetSpawn(OPvec3 pos) {
	for (ui32 i = 0; i < index; i++) {
		warriors[i]->position = pos + OPvec3(1.5f * (i / 5), 0, 1.5f * (i % 5));
		if (warriors[i]->physX != NULL) {
			OPphysXControllerSetPos(warriors[i]->physX, warriors[i]->position);
		}
	}
}

void Army::Update(OPtimer* timer) {

	if (index > 1) {
		if (OPGAMEPADS[0]->IsDown(OPgamePadButton::B) || OPKEYBOARD.IsDown(OPkeyboardKey::C)) {
			sacrificeTime += timer->Elapsed;
			adjustRumble += timer->Elapsed;
			f32 rumbleAmount = sacrificeTime / 2000.0f;
			if (adjustRumble > 100) {
				adjustRumble = 0;
				OPGAMEPADS[0]->SetRumble(rumbleAmount, rumbleAmount, 200);
			}
		}
		else {
			sacrificeTime = 0;
			adjustRumble = 0;
		}
		if (sacrificeTime > 2000) {
			sacrificeTime = 0;
			sacrificed = true;
			adjustRumble = 0;
			// Sacrifice a warrior
			OPGAMEPADS[0]->SetRumble(0, 0, 100);

			// find lowest health warrior
			i32 ind = -1;
			for (ui32 i = 0; i < index; i++) {
				if (warriors[i]->suiciding || warriors[i]->dead) {
					continue;
				}
				if (ind == -1) {
					ind = i;
					continue;
				}
				if (warriors[i]->health < warriors[ind]->health) {
					ind = i;
				}
			}
			if (ind > -1) {
				warriors[ind]->suiciding = true;
				warriors[ind]->physX->release();
				warriors[ind]->physX = NULL;
				honor += warriors[ind]->honor;
			}

		}
	}
	

	ui32 indStart = 0;
	while (indStart < index) {
		if (!warriors[indStart]->dead && !warriors[indStart]->suiciding) {
			break;
		}
		indStart++;
	}

	if (indStart >= index) {
		// shouldn't happen
	}
	else {
		centerPoint = warriors[indStart]->position;

		ui32 totalAdded = 1;
		for (ui32 i = indStart; i < index; i++) {
			if (warriors[i]->suiciding || warriors[i]->dead) continue;
			totalAdded++;
			centerPoint.x += warriors[i]->position.x;
			centerPoint.y += warriors[i]->position.y;
			centerPoint.z += warriors[i]->position.z;
		}
		centerPoint.x /= totalAdded;
		centerPoint.y /= totalAdded;
		centerPoint.z /= totalAdded;

		if (OPGAMEPADS[0]->WasPressed(OPgamePadButton::Y) || OPKEYBOARD.WasPressed(OPkeyboardKey::Y)) {
			gathered = true;
			for (ui32 i = 0; i < index; i++) {
				warriors[i]->target = centerPoint;
				warriors[i]->hasTarget = true;
			}
		}
	}
	for (ui32 i = 0; i < index; i++) {
		warriors[i]->Update(timer);
		if (warriors[i]->attacking) {
			attacked = true;
		}
	}
}

void Army::Attack(Enemy* enemy) {
	if (enemy->health <= 0) return;

	for (ui32 i = 0; i < index; i++) {
		// Check for player->enemy collision
		if (warriors[i]->suiciding || warriors[i]->dead) continue;
		
		if (OPvec3Dist(enemy->position, warriors[i]->position) < 2.5) {
			if (warriors[i]->attacking && warriors[i]->attackingTime < 500 && warriors[i]->attackingTime > 250) {
				warriors[i]->attacking = false;
				enemy->DealDamage(warriors[i]->damage, enemy->position - warriors[i]->position);
				OPfmodPlay(SND_HIT);
				if (enemy->health <= 0) {
					honor++;
					return;
				}
			}

			enemy->Attack();

			if (!enemy->dealtDamage && enemy->attackingTime < 500 && enemy->attackingTime > 250) {
				warriors[i]->DealDamage(enemy->damage);
				enemy->dealtDamage = true;
				OPfmodPlay(SND_HIT);
				if (warriors[i]->health <= 0) {
					warriors[i]->dead = true;
					//if (i < index - 1) {
					//	warriors[i] = warriors[index - 1];
					//}
					//index--;
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