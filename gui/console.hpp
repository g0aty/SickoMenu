#pragma once
#include <vector>
#include "_events.h"

namespace ConsoleGui {
	bool IsEventFiltered(EVENT_TYPES eventType);
	bool IsPlayerFiltered(Game::PlayerId playerId);
	void Init();
	void Render();
};