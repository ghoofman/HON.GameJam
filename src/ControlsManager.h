#pragma once

struct ControlsManager;

#include "./OPengine.h"

struct ControlsManager {
	OPgamePad* gamePad;
	OPkeyboardKey
		moveForward = OPkeyboardKey::W,
		moveBackward = OPkeyboardKey::S,
		moveLeft = OPkeyboardKey::A,
		moveRight = OPkeyboardKey::D;

	inline OPvec2 Movement() {
		return gamePad->LeftThumb() +
			OPvec2(
			(OPfloat)(OPKEYBOARD.IsDown(moveRight) - OPKEYBOARD.IsDown(moveLeft)),
				(OPfloat)(OPKEYBOARD.IsDown(moveForward) - OPKEYBOARD.IsDown(moveBackward)));
	}
	
	inline bool Running() {
		return OPvec2Len(Movement()) > 0;
	}
};
