#include "pch-il2cpp.h"
#include "_hooks.h"
#include "utility.h"
#include "game.h"
#include "state.hpp"

static std::string strToLower(std::string str) {
	std::string new_str = "";
	for (auto i : str) {
		new_str += char(std::tolower(i));
	}
	return new_str;
}

void doSabotageFlash() {
	if (State.mapType == Settings::MapType::Ship || State.mapType == Settings::MapType::Hq || State.mapType == Settings::MapType::Fungle)
		ShipStatus_RpcUpdateSystem(*Game::pShipStatus, SystemTypes__Enum::Reactor, 128, NULL);
	else if (State.mapType == Settings::MapType::Pb)
		ShipStatus_RpcUpdateSystem(*Game::pShipStatus, SystemTypes__Enum::Laboratory, 128, NULL);
	else if (State.mapType == Settings::MapType::Airship)
		ShipStatus_RpcUpdateSystem(*Game::pShipStatus, SystemTypes__Enum::HeliSabotage, 128, NULL);

	float timer = 0;
	while (timer <= 1.f) {
		timer += app::Time_get_fixedDeltaTime(nullptr);
	}
	RepairSabotage(*Game::pLocalPlayer);
}

void dChatController_AddChat(ChatController* __this, PlayerControl* sourcePlayer, String* chatText, bool censor, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dChatController_AddChat executed");
	censor = IsChatCensored(); // Fix chat not being censored
	if (!State.PanicMode) {
		auto player = GetPlayerData(sourcePlayer);
		auto local = GetPlayerData(*Game::pLocalPlayer);
		std::string message = RemoveHtmlTags(convert_from_string(chatText));
		if (State.ReadGhostMessages) {
			bool wasDead = false;

			if (player != NULL && player->fields.IsDead && local != NULL && !local->fields.IsDead) {
				local->fields.IsDead = true;
				wasDead = true;
			}
			ChatController_AddChat(__this, sourcePlayer, chatText, censor, method);

			std::string playerName = convert_from_string(NetworkedPlayerInfo_get_PlayerName(player, nullptr));
			auto outfit = GetPlayerOutfit(player);
			uint32_t colorId = outfit->fields.ColorId;
			if (wasDead) {
				local->fields.IsDead = false;
			}
		}
		else ChatController_AddChat(__this, sourcePlayer, chatText, censor, method);
		if (State.Enable_SMAC) {
			if (State.SMAC_CheckChat && ((IsInGame() && !State.InMeeting && !player->fields.IsDead) || chatText->fields.m_stringLength > 120)) {
				SMAC_OnCheatDetected(sourcePlayer, "Abnormal Chat");
			}
			if (State.SMAC_CheckBadWords) {
				std::string lowerMessage = strToLower(message);
				for (auto word : State.SMAC_BadWords) {
					std::string lowerWord = strToLower(word);
					if (lowerMessage.find(lowerWord) != std::string::npos) {
						SMAC_OnCheatDetected(sourcePlayer, "Bad Word: " + word);
						break;
					}
				}
			}
		}
	}
	else {
		ChatController_AddChat(__this, sourcePlayer, chatText, censor, method);
	}
}

void dChatController_SetVisible(ChatController* __this, bool visible, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dChatController_SetVisible executed");
	if (State.ChatAlwaysActive && !State.PanicMode)
		ChatController_SetVisible(__this, true, method);
	else
	{
		State.ChatActiveOriginalState = visible;
		ChatController_SetVisible(__this, visible, method);
	}
}

void dChatBubble_SetName(ChatBubble* __this, String* playerName, bool isDead, bool voted, Color color, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dChatBubble_SetName executed");
	if (!State.PanicMode && (IsInGame() || IsInLobby())) {
		for (auto playerData : GetAllPlayerData()) {
			auto outfit = GetPlayerOutfit(playerData);
			if (outfit == NULL) continue;
			if (playerName == NetworkedPlayerInfo_get_PlayerName(playerData, nullptr)) {
				auto localData = GetPlayerData(*Game::pLocalPlayer);
				color = State.RevealRoles ? GetRoleColor(playerData->fields.Role) :
					(PlayerIsImpostor(localData) && PlayerIsImpostor(playerData) ? Palette__TypeInfo->static_fields->ImpostorRed : Palette__TypeInfo->static_fields->White);
				if (State.CustomName && !State.ServerSideCustomName && (playerData == GetPlayerData(*Game::pLocalPlayer) || State.CustomNameForEveryone)) {
					playerName = convert_to_string(GetCustomName(convert_from_string(playerName)));
				}
				if (State.RevealRoles && IsInGame()) {
					playerName = convert_to_string("<size=50%>" + GetRoleName(playerData->fields.Role, State.AbbreviatedRoleNames) + "</size> " + convert_from_string(playerName));
				}

				if (State.PlayerColoredDots) {
					Color32&& nameColor = GetPlayerColor(outfit->fields.ColorId);
					std::string dot = std::format("<#{:02x}{:02x}{:02x}{:02x}>●</color>",
						nameColor.r, nameColor.g, nameColor.b,
						nameColor.a);

					if (playerData != GetPlayerData(*Game::pLocalPlayer))
						playerName = convert_to_string(convert_from_string(playerName) + " " + dot);
					else
						playerName = convert_to_string(dot + " " + convert_from_string(playerName));
				}
			}
		}
	}
	ChatBubble_SetName(__this, playerName, isDead, voted, color, method);
}

void dChatController_Update(ChatController* __this, MethodInfo* method)
{
	if (State.ShowHookLogs) LOG_DEBUG("Hook dChatController_Update executed");
	__this->fields.freeChatField->fields.textArea->fields.characterLimit = State.SafeMode ? 120 : 2147483647;
	__this->fields.freeChatField->fields.textArea->fields.allowAllCharacters = true;
	__this->fields.freeChatField->fields.textArea->fields.AllowEmail = true;
	__this->fields.freeChatField->fields.textArea->fields.AllowSymbols = true;
	if (!State.SafeMode)
		__this->fields.timeSinceLastMessage = 420.69f; //we can set this to anything more than or equal to 3 and it'll work

	if (!State.PanicMode && State.DarkMode) {
		auto gray32 = Color32();
		gray32.r = 34; gray32.g = 34; gray32.b = 34; gray32.a = 255;
		auto gray = Color32_op_Implicit_1(gray32, NULL);
		auto green32 = Color32();
		green32.r = 0; green32.g = 34; green32.b = 0; green32.a = 255;
		auto green = Color32_op_Implicit_1(green32, NULL);
		if (__this->fields.freeChatField != NULL) {
			auto compoText = convert_from_string(__this->fields.freeChatField->fields.textArea->fields.compoText);
			compoText = "<#fff>" + compoText + "</color>";
			__this->fields.freeChatField->fields.textArea->fields.compoText = convert_to_string(compoText);
			auto outputText = __this->fields.freeChatField->fields.textArea->fields.outputText;
			TMP_Text_set_color((app::TMP_Text*)outputText, Palette__TypeInfo->static_fields->White, NULL);
			SpriteRenderer_set_color(__this->fields.freeChatField->fields._.background, gray, NULL);
		}
		if (__this->fields.quickChatField != NULL) {
			auto text = __this->fields.quickChatField->fields.text;
			auto placeholderText = __this->fields.quickChatField->fields.placeholderText;
			TMP_Text_set_color((app::TMP_Text*)text, Palette__TypeInfo->static_fields->White, NULL);
			TMP_Text_set_color((app::TMP_Text*)placeholderText, Palette__TypeInfo->static_fields->White, NULL);
			SpriteRenderer_set_color(__this->fields.quickChatField->fields._.background, gray, NULL);
		}
	}
	else {
		if (__this->fields.freeChatField != NULL) {
			auto compoText = convert_from_string(__this->fields.freeChatField->fields.textArea->fields.compoText);
			compoText = RemoveHtmlTags(compoText);
			__this->fields.freeChatField->fields.textArea->fields.compoText = convert_to_string(compoText);
			auto outputText = __this->fields.freeChatField->fields.textArea->fields.outputText;
			TMP_Text_set_color((app::TMP_Text*)outputText, Palette__TypeInfo->static_fields->Black, NULL);
			SpriteRenderer_set_color(__this->fields.freeChatField->fields._.background, Palette__TypeInfo->static_fields->White, NULL);
		}
		if (__this->fields.quickChatField != NULL) {
			auto text = __this->fields.quickChatField->fields.text;
			TMP_Text_set_color((app::TMP_Text*)text, Palette__TypeInfo->static_fields->Black, NULL);
			SpriteRenderer_set_color(__this->fields.quickChatField->fields._.background, Palette__TypeInfo->static_fields->White, NULL);
		}
	}

	auto chatText = __this->fields.freeChatField->fields.textArea->fields.text;
	bool isCtrl = ImGui::IsKeyDown(0x11) || ImGui::IsKeyDown(0xA2) || ImGui::IsKeyDown(0xA3);
	bool isCpressed = ImGui::IsKeyPressed(0x43) || ImGui::IsKeyDown(0x63);
	bool isXpressed = ImGui::IsKeyPressed(0x58) || ImGui::IsKeyDown(0x78);
	if (State.ChatPaste && isCtrl && (isCpressed || isXpressed) && convert_from_string(chatText) != "") {
		ClipboardHelper_PutClipboardString(chatText, NULL); //ctrl+c
	}
	if (State.ChatPaste && isCtrl && isXpressed && convert_from_string(chatText) != "") {
		auto text = chatText;
		text = convert_to_string("");
	}

	if (State.MessageSent && State.SafeMode) {
		__this->fields.timeSinceLastMessage = 0.f;
		State.MessageSent = false;
	}
	State.ChatCooldown = __this->fields.timeSinceLastMessage;
	State.ChatFocused = __this->fields.freeChatField->fields.textArea->fields.hasFocus;

	if (!(IsHost() || !State.SafeMode)) State.ChatSpamMode = 0;

	if (!State.PanicMode && State.SafeMode && State.ChatSpam && (State.ChatSpamMode == 0 || State.ChatSpamMode == 2) && (IsInGame() || IsInLobby()) && __this->fields.timeSinceLastMessage >= 3.5f) {
		PlayerControl_RpcSendChat(*Game::pLocalPlayer, convert_to_string(State.chatMessage), NULL);
		//remove rpc queue stuff cuz of delay and anticheat kick
		State.MessageSent = true;
	}
	if (!State.PanicMode && State.SafeMode && State.CrashChatSpam && (State.ChatSpamMode == 0 || State.ChatSpamMode == 2) && (IsInGame() || IsInLobby()) && __this->fields.timeSinceLastMessage >= 3.5f) {
		PlayerControl_RpcSendChat(*Game::pLocalPlayer, convert_to_string(State.chatMessage), NULL);
		//remove rpc queue stuff cuz of delay and anticheat kick
		State.MessageSent = true;
	}

	ChatController_Update(__this, method);

	if (!State.PanicMode && State.DarkMode && __this->fields.freeChatField != NULL) {
		__this->fields.freeChatField->fields.textArea->fields.compoText = convert_to_string(RemoveHtmlTags(convert_from_string(__this->fields.freeChatField->fields.textArea->fields.compoText)));
	}
}

bool dTextBoxTMP_IsCharAllowed(TextBoxTMP* __this, uint16_t unicode_char, MethodInfo* method)
{
	if (State.ShowHookLogs) LOG_DEBUG("Hook dTextBoxTMP_IsCharAllowed executed");
	//0x08 is backspace, 0x0D is carriage return, 0x7F is delete character, 0x3C is <, 0x3E is >, 0x5B is [
	//lobby codes force uppercase, and we don't change that to fix joining a lobby with code not working
	if (!__this->fields.ForceUppercase) return (unicode_char != 0x08 && unicode_char != 0x0D && unicode_char != 0x7F && ((State.SafeMode && unicode_char != 0x3C && unicode_char != 0x3E && unicode_char != 0x5B) || !State.SafeMode));
	return TextBoxTMP_IsCharAllowed(__this, unicode_char, method);
}

void dTextBoxTMP_SetText(TextBoxTMP* __this, String* input, String* inputCompo, MethodInfo* method)
{
	if (State.ShowHookLogs) LOG_DEBUG("Hook dTextBoxTMP_SetText executed");
	if (!State.PanicMode) {
		if (!State.SafeMode)
			__this->fields.characterLimit = 2147483647;
		else
			__this->fields.characterLimit = 120;
	}
	else __this->fields.characterLimit = 100;
	inputCompo = convert_to_string(RemoveHtmlTags(convert_from_string(inputCompo))); // Fix #fff/color bug in text input field

	TextBoxTMP_SetText(__this, input, inputCompo, method);
}

void dPlayerControl_RpcSendChat(PlayerControl* __this, String* chatText, MethodInfo* method)
{
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPlayerControl_RpcSendChat executed");
	if (!State.PanicMode) {
		auto playerToChatAs = (!State.SafeMode && State.activeChatSpoof && State.playerToChatAs.has_value()) ? State.playerToChatAs.validate().get_PlayerControl() : *Game::pLocalPlayer;
		if (State.ReadAndSendAumChat && convert_from_string(chatText).substr(0, 5) == "/aum ") {
			if (IsInGame()) State.rpcQueue.push(new RpcForceAumChat(PlayerSelection(playerToChatAs), convert_from_string(chatText).substr(5), true));
			if (IsInLobby()) State.lobbyRpcQueue.push(new RpcForceAumChat(PlayerSelection(playerToChatAs), convert_from_string(chatText).substr(5), true));
			return; //we don't want the chat to know we're using "aum"
		}
		if (State.activeWhisper && State.playerToWhisper.has_value()) {
			MessageWriter* writer = InnerNetClient_StartRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient),
				playerToChatAs->fields._.NetId, uint8_t(RpcCalls__Enum::SendChat), SendOption__Enum::None,
				State.playerToWhisper.get_PlayerControl().value_or(nullptr)->fields._.OwnerId, NULL);
			std::string whisperMsg = std::format("{} whispers to you:\n{}",
				RemoveHtmlTags(convert_from_string(NetworkedPlayerInfo_get_PlayerName(GetPlayerData(*Game::pLocalPlayer), NULL))),
				convert_from_string(chatText));
			if (whisperMsg.length() <= 100 || !State.SafeMode)
				MessageWriter_WriteString(writer, convert_to_string(whisperMsg), NULL);
			else MessageWriter_WriteString(writer, chatText, NULL);
			InnerNetClient_FinishRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), writer, NULL);

			std::string whisperMsgSelf = std::format("You whisper to {}:\n{}",
				RemoveHtmlTags(convert_from_string(NetworkedPlayerInfo_get_PlayerName(State.playerToWhisper.get_PlayerData().value_or(nullptr), NULL))),
				convert_from_string(chatText));
			dChatController_AddChat(Game::HudManager.GetInstance()->fields.Chat, playerToChatAs, convert_to_string(whisperMsgSelf), false, NULL);
		}
		else if (__this == *Game::pLocalPlayer && !State.SafeMode && State.activeChatSpoof && State.playerToChatAs.has_value()) {
			auto writer = InnerNetClient_StartRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), GetPlayerControlById(State.playerToChatAs.get_PlayerId())->fields._.NetId,
				uint8_t(RpcCalls__Enum::SendChat), SendOption__Enum::None, -1, NULL);
			MessageWriter_WriteString(writer, chatText, NULL);
			InnerNetClient_FinishRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), writer, NULL);
			dChatController_AddChat(Game::HudManager.GetInstance()->fields.Chat, GetPlayerControlById(State.playerToChatAs.get_PlayerId()), chatText, false, NULL);
		}
		else {
			auto writer = InnerNetClient_StartRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), __this->fields._.NetId,
				uint8_t(RpcCalls__Enum::SendChat), SendOption__Enum::None, -1, NULL);
			MessageWriter_WriteString(writer, chatText, NULL);
			InnerNetClient_FinishRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), writer, NULL);
			dChatController_AddChat(Game::HudManager.GetInstance()->fields.Chat, __this, chatText, false, NULL);
		}
	}
	else {
		PlayerControl_RpcSendChat(__this, chatText, NULL);
	}
}

void dChatBubble_SetText(ChatBubble* __this, String* chatText, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dChatBubble_SetText executed");
	if ((!State.PanicMode || State.TempPanicMode) && State.DarkMode) {
		auto black = Palette__TypeInfo->static_fields->Black;
		if (__this->fields.playerInfo->fields.IsDead) black.a *= 0.75f;
		SpriteRenderer_set_color(__this->fields.Background, black, NULL);
		auto textArea = __this->fields.TextArea;
		TMP_Text_set_color((app::TMP_Text*)textArea, Palette__TypeInfo->static_fields->White, NULL);
	}
	ChatBubble_SetText(__this, chatText, NULL);
}
