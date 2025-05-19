#include "pch-il2cpp.h"
#include "_hooks.h"
#include "utility.h"
#include "logger.h"
#include <state.hpp>

NetworkedPlayerInfo* exiledInfo = NULL;

void dExileController_ReEnableGameplay(ExileController* __this, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dExileController_ReEnableGameplay executed");
	app::ExileController_ReEnableGameplay(__this, method);

	if (State.Replay_ClearAfterMeeting) {
		State.ClearReplayData();
		State.Replay_IsLive = true;
		State.Replay_IsPlaying = true;
		State.ShowReplay = true;
		State.MatchCurrent = State.MatchLive;
		if (State.ShowHookLogs) LOG_DEBUG("Replay Cleaned After Meeting");
	}

	try {// ESP: Reset Kill Cooldown
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
		if (State.GodMode && ((IsHost() && IsInGame()) || !State.SafeMode)) {
			PlayerControl_RpcProtectPlayer(*Game::pLocalPlayer, *Game::pLocalPlayer, GetPlayerOutfit(GetPlayerData(*Game::pLocalPlayer))->fields.ColorId, NULL);
		}
	}
	catch (...) {
		LOG_ERROR("Exception occurred in ExileController_ReEnableGameplay (ExileController)");
	}
}

void dExileController_BeginForGameplay(ExileController* __this, NetworkedPlayerInfo* exiled, bool voteTie, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dExileController_BeginForGameplay executed");
	ExileController_BeginForGameplay(__this, exiled, voteTie, method);
	exiledInfo = exiled;
	try {
		if (IsHost() && State.TournamentMode && !voteTie && exiled != NULL) {
			if (PlayerIsImpostor(exiled)) {
				UpdatePoints(exiled, -1); //ImpVoteOut
				for (auto i : State.voteMonitor) {
					if (i.second == exiled->fields.PlayerId && !PlayerIsImpostor(GetPlayerDataById(i.first))) {
						UpdatePoints(GetPlayerDataById(i.first), 1); //ImpVoteOutCorrect
						LOG_DEBUG(std::format("Added 1 point to {} for voting out impostor correctly", ToString(i.first)).c_str());
					}
				}
				auto exiledFc = convert_from_string(exiled->fields.FriendCode);
				auto pos = std::find(State.tournamentAliveImpostors.begin(), State.tournamentAliveImpostors.end(), exiledFc);
				if (pos != State.tournamentAliveImpostors.end()) State.tournamentAliveImpostors.erase(pos);
			}
			else {
				for (auto p : GetAllPlayerData()) {
					if (PlayerIsImpostor(p) && !p->fields.IsDead) {
						std::string friendCode = convert_from_string(p->fields.FriendCode);
						if (State.tournamentKillCaps[friendCode] < 3.f) {
							State.tournamentKillCaps[friendCode] += 1.f;
							UpdatePoints(p, 1); //CrewVoteOut
							LOG_DEBUG(std::format("Added 1 point to {} for voting out crewmate", ToString(p)).c_str());
						}
					}
				}
				for (auto i : State.voteMonitor) {
					if (i.second == exiled->fields.PlayerId && !PlayerIsImpostor(GetPlayerDataById(i.first))) {
						UpdatePoints(GetPlayerDataById(i.first), -1); //ImpVoteOutIncorrect
						LOG_DEBUG(std::format("Deducted 1 point from {} for voting out crewmate", ToString(i.first)).c_str());
					}
				}
			}
		}
	}
	catch (...) {
		LOG_ERROR("Exception occurred in ExileController_Begin (ExileController)");
	}
}