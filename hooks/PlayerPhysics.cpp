#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"
#include "game.h"

void dPlayerPhysics_FixedUpdate(PlayerPhysics* __this, MethodInfo* method)
{
	if (State.ShowHookLogs) Log.HookDebug("Hook dPlayerPhysics_FixedUpdate executed", false);
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
		auto playerData = GetPlayerData(player);
		auto localData = GetPlayerData(*Game::pLocalPlayer);
		if (player == NULL || playerData == NULL || localData == NULL) return;
		if (Object_1_IsNull((Object_1*)player->fields.cosmetics)) return;
		if (!State.TempPanicMode && !State.PanicMode) {
			bool shouldSeePhantom = __this->fields.myPlayer == *Game::pLocalPlayer || PlayerIsImpostor(localData) || localData->fields.IsDead || State.ShowPhantoms;
			bool shouldSeeGhost = localData->fields.IsDead || State.ShowGhosts;
			auto roleType = playerData->fields.RoleType;

			bool isFullyVanished = std::find(State.vanishedPlayers.begin(), State.vanishedPlayers.end(), playerData->fields.PlayerId) != State.vanishedPlayers.end();
			
			auto playerTransform = Component_get_transform((Component_1*)player, NULL);
			if (playerTransform != NULL) {
				auto vanishEffect = (Component_1*)Transform_FindChild(playerTransform, convert_to_string("VanishChargeEffect(Clone)"), NULL);
				if (roleType == RoleTypes__Enum::Phantom && vanishEffect != NULL) {
					isFullyVanished = !isFullyVanished;
					// this fixes https://github.com/g0aty/SickoMenu/issues/601
				}
			}

			bool isDead = playerData->fields.IsDead;
			auto nameText = Component_get_gameObject((Component_1*)player->fields.cosmetics->fields.nameText, NULL);
			bool isSeekerBody = player->fields.cosmetics->fields.bodyType == PlayerBodyTypes__Enum::Seeker || player->fields.cosmetics->fields.bodyType == PlayerBodyTypes__Enum::LongSeeker;
			if (player->fields.inVent) {
				if (!PlayerControl_get_Visible(player, NULL) && State.ShowPlayersInVents && (!isFullyVanished || shouldSeePhantom) && !State.PanicMode) {
					PlayerControl_set_Visible(player, true, NULL);
					player->fields.invisibilityAlpha = 0.5f;
					CosmeticsLayer_SetPhantomRoleAlpha(player->fields.cosmetics, player->fields.invisibilityAlpha, NULL);
					if (isSeekerBody) {
						SpriteRenderer_set_color(player->fields.cosmetics->fields.skin->fields.layer, Palette__TypeInfo->static_fields->ClearWhite, NULL);
					}
				}
				else if (player->fields.invisibilityAlpha == 0.5f && (!(State.ShowPlayersInVents && (!isFullyVanished || shouldSeePhantom)) || State.PanicMode)) {
					PlayerControl_set_Visible(player, false, NULL);
					player->fields.invisibilityAlpha = 0.f;
					CosmeticsLayer_SetPhantomRoleAlpha(player->fields.cosmetics, player->fields.invisibilityAlpha, NULL);
					if (isSeekerBody) {
						SpriteRenderer_set_color(player->fields.cosmetics->fields.skin->fields.layer, Palette__TypeInfo->static_fields->ClearWhite, NULL);
					}
				}
				GameObject_SetActive(nameText, player->fields.invisibilityAlpha > 0.f, NULL);
			}
			else if (!isDead) {
				player->fields.invisibilityAlpha = isFullyVanished ? (shouldSeePhantom ? 0.5f : 0.f) : 1.f;
				CosmeticsLayer_SetPhantomRoleAlpha(player->fields.cosmetics, player->fields.invisibilityAlpha, NULL);
				PlayerControl_set_Visible(player, player->fields.invisibilityAlpha > 0.f, NULL);
				if (isSeekerBody) {
					SpriteRenderer_set_color(player->fields.cosmetics->fields.skin->fields.layer, Palette__TypeInfo->static_fields->ClearWhite, NULL);
				}
				GameObject_SetActive(nameText, player->fields.invisibilityAlpha > 0.f, NULL);
			}
			else if (isDead) {
				PlayerControl_set_Visible(player, shouldSeeGhost, NULL);
			}
		}

		if (__this->fields.myPlayer == *Game::pLocalPlayer && !(*Game::pLocalPlayer)->fields.shapeshifting) {
			// "fix" a vanilla bug that turns the local player tiny if you shift into yourself with the animation
			// https://github.com/scp222thj/MalumMenu/commit/c6b9717d1e08b281eaff95f9433af5cb896093e3

			auto local = (*Game::pLocalPlayer);
			auto localTransform = Component_get_transform((Component_1*)local, NULL);
			if (Transform_get_localScale(localTransform, NULL).x != local->fields.defaultCosmeticsScale.x) {
				Vector3 defaultPlayerScale = PlayerAnimations_get_DefaultPlayerScale((PlayerAnimations*)__this->fields.Animations, NULL);
				Vector3 defaultCosmeticsScale = local->fields.defaultCosmeticsScale;
				CosmeticsLayer_SetScale(local->fields.cosmetics, defaultPlayerScale, defaultCosmeticsScale, NULL);
			}
		}

		app::PlayerPhysics_FixedUpdate(__this, method);
	}
	catch (...) {
		app::PlayerPhysics_FixedUpdate(__this, method);
	}
}

void dPlayerPhysics_RpcExitVent(PlayerPhysics* __this, int32_t id, MethodInfo* method) {
	if (State.ShowHookLogs) Log.HookDebug("Hook dPlayerPhysics_RpcExitVent executed", false);
	PlayerPhysics_RpcExitVent(__this, id, method);
}

void dPlayerPhysics_BootFromVent(PlayerPhysics* __this, int32_t ventId, MethodInfo* method) {
	if (State.ShowHookLogs) Log.HookDebug("Hook dPlayerPhysics_FixedUpdate executed", false);

	bool isLocal = __this->fields.myPlayer == *Game::pLocalPlayer;

	if (isLocal && State.AntiExploit_IsTeleportingSelf) {
		PlayerPhysics_BootFromVent(__this, ventId, method);
		State.AntiExploit_IsTeleportingSelf = false;
		return;
	}

	if (isLocal && !State.PanicMode && State.AntiExploit_UnauthorizedTeleports && !(*Game::pLocalPlayer)->fields.inVent)
		return;
	// we allowed to be booted out only while we are in a vent
	// this is to prevent desyncing when someone cleans a vent that we are in

	PlayerPhysics_BootFromVent(__this, ventId, method);
}

void dPlayerPhysics_HandleRpc(PlayerPhysics* __this, uint8_t callId, MessageReader* reader, MethodInfo* method) {
	if (State.ShowHookLogs) Log.HookDebug("Hook dPlayerPhysics_HandleRpc executed", false);
	PlayerPhysics_HandleRpc(__this, callId, reader, method);
}