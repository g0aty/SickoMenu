#include "pch-il2cpp.h"
#include "game.h"
#include "SignatureScan.hpp"

namespace Game {
	AmongUsClient** pAmongUsClient = nullptr;
	GameData** pGameData = nullptr;
	List_1_PlayerControl_** pAllPlayerControls = nullptr;
	PlayerControl** pLocalPlayer = nullptr;
	ShipStatus** pShipStatus = nullptr;
	LobbyBehaviour** pLobbyBehaviour = nullptr;
	DestroyableSingleton<app::RoleManager*> RoleManager { "Assembly-CSharp, RoleManager" };
	DestroyableSingleton<app::HudManager*> HudManager { "Assembly-CSharp, HudManager" };
	DestroyableSingleton<app::AccountManager*> AccountManager { "Assembly-CSharp, AccountManager" };
}