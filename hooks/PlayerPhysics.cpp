#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"
#include "game.h"

void dPlayerPhysics_FixedUpdate(PlayerPhysics* __this, MethodInfo* method)
{
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPlayerPhysics_FixedUpdate executed");
	/*if (!State.PanicMode && ((*Game::pLocalPlayer) != NULL && __this->fields.myPlayer == *Game::pLocalPlayer && (*Game::pLocalPlayer)->fields.inVent && State.MoveInVentAndShapeshift)) {
		(*Game::pLocalPlayer)->fields.inVent = false;
		app::PlayerPhysics_FixedUpdate(__this, method);
		(*Game::pLocalPlayer)->fields.inVent = true;
	}
	else {
		app::PlayerPhysics_FixedUpdate(__this, method);
	}*/
	try {
		auto player = __this->fields.myPlayer;
		if (!State.TempPanicMode && !State.PanicMode && player != NULL) {
			auto localData = GetPlayerData(*Game::pLocalPlayer);
			bool shouldSeePhantom = __this->fields.myPlayer == *Game::pLocalPlayer || PlayerIsImpostor(localData) || localData->fields.IsDead || State.ShowPhantoms;
			bool shouldSeeGhost = localData->fields.IsDead || State.ShowGhosts;
			auto playerData = GetPlayerData(player);
			auto roleType = playerData->fields.RoleType;
			bool isFullyVanished = std::find(State.vanishedPlayers.begin(), State.vanishedPlayers.end(), GetPlayerData(player)->fields.PlayerId) != State.vanishedPlayers.end();
			bool isDead = playerData->fields.IsDead;
			if (player->fields.inVent) {
				if (!PlayerControl_get_Visible(player, NULL) && State.ShowPlayersInVents && (!isFullyVanished || shouldSeePhantom) && !State.PanicMode) {
					PlayerControl_set_Visible(player, true, NULL);
					player->fields.invisibilityAlpha = 0.5f;
					CosmeticsLayer_SetPhantomRoleAlpha(player->fields.cosmetics, player->fields.invisibilityAlpha, NULL);
				}
				else if (player->fields.invisibilityAlpha == 0.5f && (!(State.ShowPlayersInVents && (!isFullyVanished || shouldSeePhantom)) || State.PanicMode)) {
					PlayerControl_set_Visible(player, false, NULL);
					player->fields.invisibilityAlpha = 0.f;
					CosmeticsLayer_SetPhantomRoleAlpha(player->fields.cosmetics, player->fields.invisibilityAlpha, NULL);
				}
			}
			else if (!isDead) {
				player->fields.invisibilityAlpha = isFullyVanished ? (shouldSeePhantom ? 0.5f : 0.f) : 1.f;
				CosmeticsLayer_SetPhantomRoleAlpha(player->fields.cosmetics, player->fields.invisibilityAlpha, NULL);
				PlayerControl_set_Visible(player, player->fields.invisibilityAlpha > 0.f, NULL);
			}
			else if (playerData->fields.IsDead) {
				PlayerControl_set_Visible(player, shouldSeeGhost && !State.PanicMode, NULL);
			}
		}
		app::PlayerPhysics_FixedUpdate(__this, method);
	}
	catch (...) {
		app::PlayerPhysics_FixedUpdate(__this, method);
	}
}