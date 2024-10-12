#pragma once

namespace Menu {
	enum Tabs {
		About,
		Settings,
		Game,
		Self,
		Radar,
		Replay,
		Esp,
		Players,
		Tasks,
		Sabotage,
		Doors,
		Host,
		Debug
	};
	void CloseAllOtherTabs(Tabs openTab);
	void Init();
	void Render();
}

