#include "pch-il2cpp.h"
#include "_rpc.h"
#include "game.h"
#include "utility.h"
#include "state.hpp"

RpcMurderPlayer::RpcMurderPlayer(PlayerControl* Player, PlayerControl* target, bool success)
{
	this->Player = Player;
	this->target = target;
	this->success = success;
}

void RpcMurderPlayer::Process()
{
	if (!PlayerSelection(Player).has_value() || !PlayerSelection(target).has_value()) return;
	if (IsInGame() && !IsInMultiplayerGame()) PlayerControl_RpcMurderPlayer(Player, target, success, NULL);
	else if (target != *Game::pLocalPlayer || IsInGame()) {
		PlayerControl_RpcMurderPlayer(Player, target, success, NULL);
		/*for (auto p : GetAllPlayerControl()) {
			auto writer = InnerNetClient_StartRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), Player->fields._.NetId,
				uint8_t(RpcCalls__Enum::MurderPlayer), SendOption__Enum::None, p->fields._.OwnerId, NULL);
			MessageExtensions_WriteNetObject(writer, (InnerNetObject*)target, NULL);
			MessageWriter_WriteInt32(writer, int32_t(MurderResultFlags__Enum::Succeeded), NULL);
			InnerNetClient_FinishRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), writer, NULL);
		}*/
	}
	else {
		for (auto p : GetAllPlayerControl()) {
			if (p != *Game::pLocalPlayer) {
				auto writer = InnerNetClient_StartRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), Player->fields._.NetId,
					uint8_t(RpcCalls__Enum::MurderPlayer), SendOption__Enum::None, p->fields._.OwnerId, NULL);
				MessageExtensions_WriteNetObject(writer, (InnerNetObject*)target, NULL);
				MessageWriter_WriteInt32(writer, int32_t(success ? MurderResultFlags__Enum::Succeeded : MurderResultFlags__Enum::FailedProtected), NULL);
				InnerNetClient_FinishRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), writer, NULL);
			}
			if (success) GetPlayerData(target)->fields.IsDead = true;
		}
	}
	State.LevelFarm = false;
}

RpcMurderLoop::RpcMurderLoop(PlayerControl* Player, PlayerControl* target, int count, bool onlyOnTarget)
{
	this->Player = Player;
	this->target = target;
	this->count = count;
	this->onlyOnTarget = onlyOnTarget;
}

void RpcMurderLoop::Process()
{
	if (onlyOnTarget && (IsInMultiplayerGame() || IsInLobby())) {
		for (size_t i = 1; i <= (size_t)count; ++i) {
			if (!PlayerSelection(Player).has_value() || !PlayerSelection(target).has_value()) break;
			auto writer = InnerNetClient_StartRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), Player->fields._.NetId,
				uint8_t(RpcCalls__Enum::MurderPlayer), SendOption__Enum::None, target->fields._.OwnerId, NULL);
			MessageExtensions_WriteNetObject(writer, (InnerNetObject*)target, NULL);
			MessageWriter_WriteInt32(writer, int32_t(MurderResultFlags__Enum::Succeeded), NULL);
			InnerNetClient_FinishRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), writer, NULL);
		}
	}
	else if ((IsInLobby() && State.KillInLobbies) && target == *Game::pLocalPlayer) {
		for (size_t i = 1; i <= (size_t)count; ++i) {
			if (!PlayerSelection(Player).has_value() || !PlayerSelection(target).has_value()) break;
			for (auto p : GetAllPlayerControl()) {
				if (p != *Game::pLocalPlayer) {
					auto writer = InnerNetClient_StartRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), Player->fields._.NetId,
						uint8_t(RpcCalls__Enum::MurderPlayer), SendOption__Enum::None, p->fields._.OwnerId, NULL);
					MessageWriter_WriteInt32(writer, int32_t(MurderResultFlags__Enum::Succeeded), NULL);
					MessageExtensions_WriteNetObject(writer, (InnerNetObject*)target, NULL);
					InnerNetClient_FinishRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), writer, NULL);
				}
			}
		}
	}
	else {
		if (IsInLobby() && !State.KillInLobbies) return;
		for (size_t i = 1; i <= (size_t)count; ++i) {
			if (!PlayerSelection(Player).has_value() || !PlayerSelection(target).has_value()) break;
			PlayerControl_RpcMurderPlayer(Player, target, true, NULL);
		}
	}
}

RpcExiled::RpcExiled(PlayerControl* target, bool onlyOnTarget)
{
	this->target = target;
	this->onlyOnTarget = onlyOnTarget;
}

void RpcExiled::Process()
{
	for (auto p : GetAllPlayerControl()) {
		if (!PlayerSelection(target).has_value() || (onlyOnTarget && target != p)) break;
		auto writer = InnerNetClient_StartRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), target->fields._.NetId,
			uint8_t(RpcCalls__Enum::Exiled), SendOption__Enum::None, p->fields._.OwnerId, NULL);
		InnerNetClient_FinishRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), writer, NULL);
	}
}

//damn im too lazy to add new files

RpcShapeshift::RpcShapeshift(PlayerControl* Player, const PlayerSelection& target, bool animate)
{
	this->Player = Player;
	this->target = target;
	this->animate = animate;
}

void RpcShapeshift::Process()
{
	if (!PlayerSelection(Player).has_value() || !target.has_value()) return;

	PlayerControl_RpcShapeshift(Player, target.get_PlayerControl().value_or(nullptr), animate, NULL);
}

CmdCheckShapeshift::CmdCheckShapeshift(PlayerControl* Player, const PlayerSelection& target, bool animate)
{
	this->Player = Player;
	this->target = target;
	this->animate = animate;
}

void CmdCheckShapeshift::Process()
{
	if (!PlayerSelection(Player).has_value() || !target.has_value()) return;

	PlayerControl_CmdCheckShapeshift(Player, target.get_PlayerControl().value_or(nullptr), animate, NULL);
}

RpcVanish::RpcVanish(PlayerControl* Player, bool appear)
{
	this->Player = Player;
	this->appear = appear;
}

void RpcVanish::Process()
{
	if (!PlayerSelection(Player).has_value()) return;

	if (appear) PlayerControl_CmdCheckAppear(Player, true, NULL);
	else PlayerControl_CmdCheckVanish(Player, 1.f, NULL);
}

RpcSendChat::RpcSendChat(PlayerControl* Player, std::string_view msg, PlayerControl* target)
{
	this->Player = Player;
	this->msg = msg;
	this->target = target;
}

void RpcSendChat::Process()
{
	if (!PlayerSelection(Player).has_value()) return;
	if (State.ChatCooldown < 3.f) return;
	//PlayerControl_RpcSendChat(Player, convert_to_string(msg), NULL);
	//this allows us to do formatting in chat message
	if (target == NULL) {
		auto writer = InnerNetClient_StartRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), Player->fields._.NetId,
			uint8_t(RpcCalls__Enum::SendChat), SendOption__Enum::None, -1, NULL);
		MessageWriter_WriteString(writer, convert_to_string(msg), NULL);
		InnerNetClient_FinishRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), writer, NULL);
	}
	else {
		auto writer = InnerNetClient_StartRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), Player->fields._.NetId,
			uint8_t(RpcCalls__Enum::SendChat), SendOption__Enum::None, target->fields._.OwnerId, NULL);
		MessageWriter_WriteString(writer, convert_to_string(msg), NULL);
		InnerNetClient_FinishRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), writer, NULL);
	}
	ChatController_AddChat(Game::HudManager.GetInstance()->fields.Chat, Player, convert_to_string(msg), false, NULL);
}

RpcSendChatNote::RpcSendChatNote(PlayerControl* player, int32_t type)
{
	this->player = player;
	this->type = type;
}

void RpcSendChatNote::Process()
{
	if (!PlayerSelection(player).has_value()) return;

	PlayerControl_RpcSendChatNote(*Game::pLocalPlayer, player->fields.PlayerId, (ChatNoteTypes__Enum)type, NULL);
}

RpcVotePlayer::RpcVotePlayer(PlayerControl* Player, PlayerControl* target, bool skip)
{
	this->Player = Player;
	this->target = target;
	this->skip = skip;
}

void RpcVotePlayer::Process()
{
	if (!PlayerSelection(Player).has_value() || !PlayerSelection(target).has_value()) return;

	if (skip)
		MeetingHud_CmdCastVote(MeetingHud__TypeInfo->static_fields->Instance, Player->fields.PlayerId, 253, NULL);
	else
		MeetingHud_CmdCastVote(MeetingHud__TypeInfo->static_fields->Instance, Player->fields.PlayerId, target->fields.PlayerId, NULL);
}

RpcVoteKick::RpcVoteKick(PlayerControl* target, bool exploit)
{
	this->target = target;
	this->exploit = exploit;
}

void RpcVoteKick::Process()
{
	if (!PlayerSelection(target).has_value())
		return;

	if (!exploit) VoteBanSystem_CmdAddVote(VoteBanSystem__TypeInfo->static_fields->Instance, target->fields._.OwnerId, NULL);
	else {
		for (auto p : GetAllPlayerControl()) {
			auto writer = InnerNetClient_StartRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient),
				VoteBanSystem__TypeInfo->static_fields->Instance->fields._.NetId, uint8_t(RpcCalls__Enum::AddVote),
				SendOption__Enum::None, InnerNetClient_GetHost((InnerNetClient*)(*Game::pAmongUsClient), NULL)->fields.Id, NULL);
			MessageWriter_WriteInt32(writer, p->fields._.OwnerId, NULL);
			MessageWriter_WriteInt32(writer, target->fields._.OwnerId, NULL);
			InnerNetClient_FinishRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), writer, NULL);
		}
	}
}

ReportPlayer::ReportPlayer(PlayerControl* target, ReportReasons__Enum reason)
{
	this->target = target;
	this->reason = reason;
}

void ReportPlayer::Process()
{
	if (!PlayerSelection(target).has_value())
		return;

	InnerNetClient_ReportPlayer((InnerNetClient*)(*Game::pAmongUsClient), target->fields._.OwnerId, reason, NULL);
}

RpcClearVote::RpcClearVote(PlayerControl* Player)
{
	this->Player = Player;
}

void RpcClearVote::Process()
{
	if (!PlayerSelection(Player).has_value()) return;

	MeetingHud_RpcClearVote(MeetingHud__TypeInfo->static_fields->Instance, Player->fields._.OwnerId, NULL);
}

RpcEndMeeting::RpcEndMeeting() {

}

void RpcEndMeeting::Process()
{
	MeetingHud_RpcClose(MeetingHud__TypeInfo->static_fields->Instance, NULL);
}

EndMeeting::EndMeeting() {

}

void EndMeeting::Process()
{
	MeetingHud_Close(MeetingHud__TypeInfo->static_fields->Instance, NULL);
}

DestroyMap::DestroyMap() {

}

void DestroyMap::Process()
{
	InnerNetObject* netObj = NULL;
	if (IsInLobby() && LobbyBehaviour__TypeInfo->static_fields->Instance) netObj = (InnerNetObject*)(LobbyBehaviour__TypeInfo->static_fields->Instance);
	else if (IsInGame() && ShipStatus__TypeInfo->static_fields->Instance)netObj = (InnerNetObject*)(ShipStatus__TypeInfo->static_fields->Instance);
	if (netObj == NULL) return;
	InnerNetObject_Despawn(netObj, NULL);
}

RpcRevive::RpcRevive(PlayerControl* Player)
{
	this->Player = Player;
}

void RpcRevive::Process()
{
	if (!PlayerSelection(Player).has_value()) return;

	PlayerControl_Revive(Player, NULL);
	//PlayerControl_RpcSetColor(Player, GetPlayerOutfit(GetPlayerData(Player))->fields.ColorId, NULL);
	if (PlayerIsImpostor(GetPlayerData(Player)))
		PlayerControl_RpcSetRole(Player, RoleTypes__Enum::Impostor, true, NULL);
	else
		PlayerControl_RpcSetRole(Player, RoleTypes__Enum::Crewmate, true, NULL);
}

RpcVent::RpcVent(PlayerControl* Player, int32_t ventId, bool exit)
{
	this->Player = Player;
	this->ventId = ventId;
	this->exit = exit;
}

void RpcVent::Process()
{
	if (!PlayerSelection(Player).has_value()) return;

	if (exit)
		PlayerPhysics_RpcExitVent(Player->fields.MyPhysics, ventId, NULL);
	else
		PlayerPhysics_RpcEnterVent(Player->fields.MyPhysics, ventId, NULL);
}

RpcBootAllVents::RpcBootAllVents()
{

}

void RpcBootAllVents::Process()
{
	il2cpp::Array<Vent__Array> allVents = (*Game::pShipStatus)->fields._AllVents_k__BackingField;
	for (auto vent : allVents) {
		//PlayerPhysics_RpcBootFromVent((*Game::pLocalPlayer)->fields.MyPhysics, vent->fields.Id, NULL);
		//anticheat will kick you if you do this
		VentilationSystem_Update(VentilationSystem_Operation__Enum::BootImpostors, vent->fields.Id, NULL);
	}
}

RpcSetLevel::RpcSetLevel(PlayerControl* Player, int level)
{
	this->Player = Player;
	this->level = level;
}

void RpcSetLevel::Process()
{
	if (!PlayerSelection(Player).has_value()) return;

	PlayerControl_RpcSetLevel(Player, level - 1, NULL);
}

RpcEndGame::RpcEndGame(GameOverReason__Enum reason)
{
	this->reason = reason;
}

void RpcEndGame::Process()
{
	bool impostorWin = false;
	switch (reason) {
	case GameOverReason__Enum::HideAndSeek_ByKills:
	case GameOverReason__Enum::ImpostorByKill:
	case GameOverReason__Enum::ImpostorBySabotage:
	case GameOverReason__Enum::ImpostorByVote:
	case GameOverReason__Enum::HumansDisconnect:
		impostorWin = true;
		break;
	}
	std::string winnersText = "Game Winners: ";
	int count = 0;
	for (auto p : GetAllPlayerData()) {
		if (State.TournamentMode) {
			auto friendCode = convert_from_string(p->fields.FriendCode);
			if (impostorWin) {
				if (State.tournamentAliveImpostors == State.tournamentAssignedImpostors && PlayerIsImpostor(p)) {
					State.tournamentPoints[friendCode] += 2; //AllImpsWin
					LOG_DEBUG(std::format("Added 2 points to {} for all impostors win", ToString(p)).c_str());
					State.tournamentWinPoints[friendCode] += 1;
				}
				else if (PlayerIsImpostor(p)) {
					if (State.tournamentAliveImpostors.size() == 1 && !p->fields.IsDead) {
						State.tournamentPoints[friendCode] += 2; //ImpWin
						LOG_DEBUG(std::format("Added 2 points to {} for solo win", ToString(p)).c_str());
						State.tournamentWinPoints[friendCode] += 2;
					}
					else {
						State.tournamentPoints[friendCode] += 1; //ImpWin
						LOG_DEBUG(std::format("Added 1 point to {} for impostor win", ToString(p)).c_str());
						State.tournamentWinPoints[friendCode] += 1;
					}
				}
			}
			else {
				if (PlayerIsImpostor(p)) {
					State.tournamentPoints[friendCode] -= 1; //ImpLose
					LOG_DEBUG(std::format("Deducted -1 point from {} for impostor loss", ToString(p)).c_str());
				}
				else {
					State.tournamentPoints[friendCode] += 2; //CrewWin
					LOG_DEBUG(std::format("Added 2 points to {} for crewmate win", ToString(p)).c_str());
					State.tournamentWinPoints[friendCode] += 1;
				}
			}
		}
		auto name = convert_from_string(GetPlayerOutfit(p)->fields.PlayerName);
		if ((impostorWin && PlayerIsImpostor(p)) || (!impostorWin && !PlayerIsImpostor(p))) {
			winnersText += name + ", ";
			count++;
		}
	}
	if (count == 0) LOG_DEBUG("No one was a winner in the game.");
	else LOG_DEBUG(winnersText.substr(0, (size_t)winnersText.size() - 2));
	State.tournamentKillCaps.clear();
	State.tournamentAssignedImpostors.clear();
	State.tournamentAliveImpostors.clear();
	State.tournamentCallers.clear();
	State.tournamentCalledOut.clear();
	State.tournamentFirstMeetingOver = false;
	GameManager_RpcEndGame(GameManager__TypeInfo->static_fields->_Instance_k__BackingField, reason, false, NULL);
}

RpcProtectPlayer::RpcProtectPlayer(PlayerControl* Player, PlayerSelection target, uint8_t color)
{
	this->Player = Player;
	this->target = target;
	this->color = color;
}

void RpcProtectPlayer::Process()
{
	if (!PlayerSelection(Player).has_value() || !target.has_value()) return;

	PlayerControl_RpcProtectPlayer(Player, target.get_PlayerControl().value_or(nullptr), color, NULL);
}

CmdCheckProtect::CmdCheckProtect(PlayerControl* Player, PlayerSelection target)
{
	this->Player = Player;
	this->target = target;
}

void CmdCheckProtect::Process()
{
	if (!PlayerSelection(Player).has_value() || !target.has_value()) return;

	PlayerControl_CmdCheckProtect(Player, target.get_PlayerControl().value_or(nullptr), NULL);
}

RpcForceDetectAum::RpcForceDetectAum(const PlayerSelection& target, bool completeForce)
{
	this->target = target;
	this->completeForce = completeForce;
}

void RpcForceDetectAum::Process()
{
	if (!target.has_value()) return;
	PlayerControl* player = target.validate().get_PlayerControl();
	MessageWriter* rpcMessage = InnerNetClient_StartRpc((InnerNetClient*)(*Game::pAmongUsClient), (completeForce ? player : *Game::pLocalPlayer)->fields._.NetId, (uint8_t)42069, SendOption__Enum::Reliable, NULL);
	MessageWriter_WriteByte(rpcMessage, player->fields.PlayerId, NULL);
	//we do a little trolling >:)
	//aum only checks for the player id thus making it, so we can send whoever we want to (even as ourselves)
	MessageWriter_EndMessage(rpcMessage, NULL);
}

RpcForceAumChat::RpcForceAumChat(const PlayerSelection& target, std::string_view msg, bool completeForce)
{
	this->target = target;
	this->msg = msg;
	this->completeForce = completeForce;
}

void RpcForceAumChat::Process()
{
	PlayerControl* player = target.validate().get_PlayerControl();
	String* playerName = NetworkedPlayerInfo_get_PlayerName(target.validate().get_PlayerData(), NULL);
	MessageWriter* rpcMessage = InnerNetClient_StartRpc((InnerNetClient*)(*Game::pAmongUsClient), (completeForce ? player : *Game::pLocalPlayer)->fields._.NetId, 101, SendOption__Enum::Reliable, NULL);
	auto outfit = GetPlayerOutfit(GetPlayerData(player));
	//aum only checks for the player name and color, so we can send in anything we want (even as ourselves)
	MessageWriter_WriteString(rpcMessage, playerName, NULL);
	MessageWriter_WriteString(rpcMessage, convert_to_string(msg), NULL);
	MessageWriter_WriteInt32(rpcMessage, outfit->fields.ColorId, NULL);
	MessageWriter_EndMessage(rpcMessage, NULL);
	std::string chatVisual = "<#f55><b>[AUM Chat]</b></color>\n" + msg;
	ChatController_AddChat(Game::HudManager.GetInstance()->fields.Chat, player, convert_to_string(chatVisual), false, NULL);
}

RpcSyncSettings::RpcSyncSettings() {
	//thanks Rabek009! https://github.com/Rabek009/MoreGamemodes/ <3
}

void RpcSyncSettings::Process()
{
	auto gameOptionsManager = GameOptionsManager_get_Instance(NULL);
	GameManager* gameManager = GameManager_get_Instance(NULL);
	GameOptionsManager_set_GameHostOptions(gameOptionsManager, GameOptionsManager_get_CurrentGameOptions(gameOptionsManager, NULL), NULL);
	LogicOptions_SyncOptions(GameManager_get_LogicOptions(gameManager, NULL), NULL);
}

RpcSpawnDummy::RpcSpawnDummy(uint8_t colorId, std::string_view name) {
	this->colorId = colorId;
	this->name = name;
}

void RpcSpawnDummy::Process()
{
	return; // Disable temporarily
	PlayerControl* dummyPc = (PlayerControl*)Object_1_Instantiate((Object*)(*Game::pAmongUsClient)->fields.PlayerPrefab, NULL);
	if (dummyPc != NULL) {
		try {
			GameData_AddDummy(*Game::pGameData, dummyPc, NULL);
			InnerNetClient_Spawn((InnerNetClient*)(*Game::pAmongUsClient), (InnerNetObject*)dummyPc, -2, SpawnFlags__Enum::None, NULL);
			if (colorId != -1) PlayerControl_RpcSetColor(dummyPc, colorId, NULL);
			if (name != "") PlayerControl_RpcSetName(dummyPc, convert_to_string(name), NULL);
		}
		catch (...) {
			LOG_DEBUG("Something wrong happened when spawning dummy");
		}
	}
	else LOG_DEBUG("dummyPc was null");
}

RpcBootFromVent::RpcBootFromVent(PlayerControl* Player, int ventId)
{
	this->Player = Player;
	this->ventId = ventId;
}

void RpcBootFromVent::Process()
{
	if (!PlayerSelection(Player).has_value()) return;

	PlayerPhysics_RpcBootFromVent(Player->fields.MyPhysics, ventId, NULL);
}