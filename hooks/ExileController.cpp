#include "pch-il2cpp.h"
#include "_hooks.h"
#include "utility.h"
#include "logger.h"
#include <state.hpp>

void dExileController_ReEnableGameplay(ExileController* __this, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dExileController_ReEnableGameplay executed");
    app::ExileController_ReEnableGameplay(__this, method);

	try {// ESP: Reset Kill Cooldown
		if (IsHost() && State.TournamentMode && !State.tournamentFirstMeetingOver) State.tournamentFirstMeetingOver = true;
		for (auto pc : GetAllPlayerControl()) {
			if (auto player = PlayerSelection(pc).validate();
				player.has_value() && !player.is_LocalPlayer() && !player.is_Disconnected()) {
				if (auto role = player.get_PlayerData()->fields.Role;
					role != nullptr && role->fields.CanUseKillButton && !player.get_PlayerData()->fields.IsDead) {
					pc->fields.killTimer = (std::max)(GameOptions().GetKillCooldown(), 0.f);
					//STREAM_DEBUG("Player " << ToString(pc) << " KillTimer " << pc->fields.killTimer);
				}
			}
		}
		if (State.GodMode) PlayerControl_RpcProtectPlayer(*Game::pLocalPlayer, *Game::pLocalPlayer, GetPlayerOutfit(GetPlayerData(*Game::pLocalPlayer))->fields.ColorId, NULL);
	}
	catch (...) {
		LOG_ERROR("Exception occurred in ExileController_ReEnableGameplay (ExileController)");
	}
}

void dExileController_BeginForGameplay(ExileController* __this, NetworkedPlayerInfo* exiled, bool voteTie, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dExileController_BeginForGameplay executed");
	ExileController_BeginForGameplay(__this, exiled, voteTie, method);
	try {
		if (IsHost() && State.TournamentMode && !voteTie && exiled != NULL) {
			if (PlayerIsImpostor(exiled)) {
				UpdateTournamentPoints(exiled, 3); //ImpVoteOut
				for (auto i : State.voteMonitor) {
					if (i.second == exiled->fields.PlayerId && !PlayerIsImpostor(GetPlayerDataById(i.first)))
						UpdateTournamentPoints(GetPlayerDataById(i.first), 5); //ImpVoteOutCorrect
				}
				auto exiledFc = convert_from_string(exiled->fields.FriendCode);
				auto pos = std::find(State.tournamentAliveImpostors.begin(), State.tournamentAliveImpostors.end(), exiledFc);
				if (pos != State.tournamentAliveImpostors.end()) State.tournamentAliveImpostors.erase(pos);
			}
			else {
				for (auto p : GetAllPlayerData()) {
					if (PlayerIsImpostor(p))
						UpdateTournamentPoints(exiled, 4); //CrewVoteOut
				}
				for (auto i : State.voteMonitor) {
					if (i.second == exiled->fields.PlayerId && !PlayerIsImpostor(GetPlayerDataById(i.first)))
						UpdateTournamentPoints(GetPlayerDataById(i.first), 6); //ImpVoteOutIncorrect
				}
			}
		}
	}
	catch (...) {
		LOG_ERROR("Exception occurred in ExileController_Begin (ExileController)");
	}
}