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

void dChatController_AddChat(ChatController* __this, PlayerControl* sourcePlayer, String* chatText, bool censor, MethodInfo* method) {
	if (!State.PanicMode) {
		bool wasDead = false;
		auto player = GetPlayerData(sourcePlayer);
		auto local = GetPlayerData(*Game::pLocalPlayer);

		if (player != NULL && player->fields.IsDead && local != NULL && !local->fields.IsDead) {
			local->fields.IsDead = true;
			wasDead = true;
		}
		ChatController_AddChat(__this, sourcePlayer, chatText, censor, method);
		
		std::string playerName = convert_from_string(NetworkedPlayerInfo_get_PlayerName(GetPlayerData(sourcePlayer), nullptr));
		auto outfit = GetPlayerOutfit(GetPlayerData(sourcePlayer));
		uint32_t colorId = outfit->fields.ColorId;
		std::string message = RemoveHtmlTags(convert_from_string(chatText));
		if (wasDead) {
			local->fields.IsDead = false;
		}

		auto playerFc = convert_from_string(player->fields.FriendCode);
		if (!PlayerIsImpostor(player) && IsHost() && State.TournamentMode && message.substr(0, 9) == "/callout " &&
			std::find(State.tournamentCallers.begin(), State.tournamentCallers.end(), playerFc) == State.tournamentCallers.end()) {
			try {
				if (!player->fields.IsDead) {
					std::map<std::string, std::string> playerNames = {};
					for (auto p : GetAllPlayerData()) {
						if (!p->fields.IsDead) {
							auto outfit = GetPlayerOutfit(p);
							auto friendCode = convert_from_string(p->fields.FriendCode);
							playerNames[friendCode] = strToLower(convert_from_string(outfit->fields.PlayerName));
						}
					}
					State.tournamentCallers.push_back(playerFc);
					std::string calledOutPlayer = strToLower(message.substr(9));
					std::vector<std::string> calloutResult = {};
					for (auto i : playerNames) {
						std::string playerName = i.second;
						if (playerName.find(calledOutPlayer) != std::string::npos)
							calloutResult.push_back(i.first);
					}
					if (calloutResult.size() == 1 && 
						std::find(State.tournamentCalledOut.begin(), State.tournamentCalledOut.end(), calloutResult[0]) == State.tournamentCalledOut.end()) {
						if (std::find(State.tournamentAliveImpostors.begin(), State.tournamentAliveImpostors.end(), calloutResult[0]) != State.tournamentAliveImpostors.end()) {
							//check if called-out player was an impostor
							UpdateTournamentPoints(player, 8); //CorrectCallout
							auto friendCode = convert_from_string(player->fields.FriendCode);
							State.tournamentCalloutPoints[friendCode] += 1;
						}
						else {
							UpdateTournamentPoints(player, 9); //IncorrectCallout
						}
						State.tournamentCalledOut.push_back(calloutResult[0]);
					}
				}
			}
			catch (...) {
				LOG_ERROR("Exception occurred while checking callout (Chat)");
			}
		}
	}
	else {
		ChatController_AddChat(__this, sourcePlayer, chatText, censor, method);
	}
}

void dChatController_SetVisible(ChatController* __this, bool visible, MethodInfo* method) {
	if (State.ChatAlwaysActive && !State.PanicMode)
		ChatController_SetVisible(__this, true, method);
	else
	{
		State.ChatActiveOriginalState = visible;
		ChatController_SetVisible(__this, visible, method);
	}
}

void dChatBubble_SetName(ChatBubble* __this, String* playerName, bool isDead, bool voted, Color color, MethodInfo* method) {	
	if (!State.PanicMode && (IsInGame() || IsInLobby())) {
		for (auto playerData : GetAllPlayerData()) {
			auto outfit = GetPlayerOutfit(playerData);
			if (outfit == NULL) continue;
			if (playerName == NetworkedPlayerInfo_get_PlayerName(playerData, nullptr)) {
				auto localData = GetPlayerData(*Game::pLocalPlayer);
				color = State.RevealRoles ? GetRoleColor(playerData->fields.Role) :
					(PlayerIsImpostor(localData) && PlayerIsImpostor(playerData) ? Palette__TypeInfo->static_fields->ImpostorRed : Palette__TypeInfo->static_fields->White);
				if (State.RevealRoles && IsInGame()) {
					playerName = convert_to_string("<size=50%>" + GetRoleName(playerData->fields.Role, State.AbbreviatedRoleNames) + "</size> " + convert_from_string(playerName));
				}
				if (State.CustomName && !State.ServerSideCustomName && playerData == GetPlayerData(*Game::pLocalPlayer)) {
					if (State.ColoredName && !State.RgbName) {
						playerName = convert_to_string(GetGradientUsername(convert_from_string(playerName)));
					}
					//we don't want to hide our own chat messages
					/*if (State.ResizeName)
						playerName = convert_to_string(std::format("<size={}>", State.NameSize) + convert_from_string(playerName) + "</size>");*/
					if (State.ItalicName)
						playerName = convert_to_string("<i>" + convert_from_string(playerName) + "</i>");
					if (State.UnderlineName && (!State.ColoredName || State.RgbName))
						playerName = convert_to_string("<u>" + convert_from_string(playerName) + "</u>");
					if (State.StrikethroughName && (!State.ColoredName || State.RgbName))
						playerName = convert_to_string("<s>" + convert_from_string(playerName) + "</s>");
					//rgb color doesn't change
					/*if (State.RgbName) {
						playerName = convert_to_string(State.rgbCode + convert_from_string(playerName) + "</color>");
					}*/
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
	__this->fields.freeChatField->fields.textArea->fields.characterLimit = State.SafeMode ? 120 : 2147483647;
	__this->fields.freeChatField->fields.textArea->fields.allowAllCharacters = true;
	__this->fields.freeChatField->fields.textArea->fields.AllowEmail = true;
	__this->fields.freeChatField->fields.textArea->fields.AllowSymbols = true;
	if (!State.SafeMode)
		__this->fields.timeSinceLastMessage = 420.69f; //we can set this to anything more than or equal to 3 and it'll work

	if (State.DarkMode) {
		auto gray32 = Color32();
		gray32.r = 34; gray32.g = 34; gray32.b = 34; gray32.a = 255;
		auto gray = Color32_op_Implicit_1(gray32, NULL);
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
			TMP_Text_set_color((app::TMP_Text*)text, Palette__TypeInfo->static_fields->White, NULL);
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

	auto chatText = (String*)(__this->fields.freeChatField->fields.textArea->fields.text);
	bool isCtrl = ImGui::IsKeyDown(0x11) || ImGui::IsKeyDown(0xA2) || ImGui::IsKeyDown(0xA3);
	bool isCpressed = ImGui::IsKeyPressed(0x43) || ImGui::IsKeyDown(0x63);
	bool isXpressed = ImGui::IsKeyPressed(0x58) || ImGui::IsKeyDown(0x78);
	if (State.ChatPaste && isCtrl && (isCpressed || isXpressed) && convert_from_string(chatText) != "") {
		ClipboardHelper_PutClipboardString((String*)(__this->fields.freeChatField->fields.textArea->fields.text), NULL); //ctrl+c
	}
	if (State.ChatPaste && isCtrl && isXpressed && convert_from_string(chatText) != "") {
		auto text = (String*)(__this->fields.freeChatField->fields.textArea->fields.text);
		text = convert_to_string("");
	}

	if (State.MessageSent && State.SafeMode) {
		__this->fields.timeSinceLastMessage = 0.f;
		State.MessageSent = false;
	}
	State.ChatCooldown = __this->fields.timeSinceLastMessage;
	State.ChatFocused = __this->fields.freeChatField->fields.textArea->fields.hasFocus;

	/*if (State.FollowerCam != nullptr && !State.PanicMode && State.EnableZoom &&
		(__this->fields.state == ChatControllerState__Enum::Closed || (__this->fields.state == ChatControllerState__Enum::Closing && State.EnableZoom))) {
		Camera_set_orthographicSize(State.FollowerCam, 3.f, NULL);
		int32_t width = Screen_get_width(NULL);
		int32_t height = Screen_get_height(NULL);
		bool fullscreen = Screen_get_fullScreen(NULL);
		ChatController_OnResolutionChanged(__this, (float)(width / height), width, height, fullscreen, NULL);
		if (__this->fields.state == ChatControllerState__Enum::Closing && State.EnableZoom) ChatController_ForceClosed(__this, NULL); //force close the chat as it stays open otherwise
		Camera_set_orthographicSize(State.FollowerCam, 3.f * (State.EnableZoom ? State.CameraHeight : 1.f), NULL);
	}*/

	if (!State.PanicMode && State.SafeMode && State.ChatSpam && (IsInGame() || IsInLobby()) && __this->fields.timeSinceLastMessage >= 3.5f) {
		PlayerControl_RpcSendChat(*Game::pLocalPlayer, convert_to_string(State.chatMessage), NULL);
		//remove rpc queue stuff cuz of delay and anticheat kick
		State.MessageSent = true;
	}


	ChatController_Update(__this, method);
}

bool dTextBoxTMP_IsCharAllowed(TextBoxTMP* __this, uint16_t unicode_char, MethodInfo* method)
{
	//0x08 is backspace, 0x0D is carriage return, 0x7F is delete character, 0x3C is <, 0x3E is >
	//lobby codes force uppercase, and we don't change that to fix joining a lobby with code not working
	if (!__this->fields.ForceUppercase) return (unicode_char != 0x08 && unicode_char != 0x0D && unicode_char != 0x7F && ((State.SafeMode && unicode_char != 0x3C && unicode_char != 0x3E) || !State.SafeMode));
	return TextBoxTMP_IsCharAllowed(__this, unicode_char, method);
}

void dTextBoxTMP_SetText(TextBoxTMP* __this, String* input, String* inputCompo, MethodInfo* method)
{
	if (!State.PanicMode) {
		if (!State.SafeMode)
			__this->fields.characterLimit = 2147483647;
		else
			__this->fields.characterLimit = 120;
	}
	else __this->fields.characterLimit = 100;

	TextBoxTMP_SetText(__this, input, inputCompo, method);
	
}

void dPlayerControl_RpcSendChat(PlayerControl* __this, String* chatText, MethodInfo* method)
{
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
			ChatController_AddChat(Game::HudManager.GetInstance()->fields.Chat, playerToChatAs, convert_to_string(whisperMsgSelf), false, NULL);
		}
		else if (__this == *Game::pLocalPlayer && !State.SafeMode && State.activeChatSpoof && State.playerToChatAs.has_value()) {
			auto writer = InnerNetClient_StartRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), GetPlayerControlById(State.playerToChatAs.get_PlayerId())->fields._.NetId,
				uint8_t(RpcCalls__Enum::SendChat), SendOption__Enum::None, -1, NULL);
			MessageWriter_WriteString(writer, chatText, NULL);
			InnerNetClient_FinishRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), writer, NULL);
			ChatController_AddChat(Game::HudManager.GetInstance()->fields.Chat, GetPlayerControlById(State.playerToChatAs.get_PlayerId()), chatText, false, NULL);
		}
		else {
			auto writer = InnerNetClient_StartRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), __this->fields._.NetId,
				uint8_t(RpcCalls__Enum::SendChat), SendOption__Enum::None, -1, NULL);
			MessageWriter_WriteString(writer, chatText, NULL);
			InnerNetClient_FinishRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), writer, NULL);
			ChatController_AddChat(Game::HudManager.GetInstance()->fields.Chat, __this, chatText, false, NULL);
		}
	}
	else {
		PlayerControl_RpcSendChat(__this, chatText, NULL);
	}
}

void dChatBubble_SetText(ChatBubble* __this, String* chatText, MethodInfo* method) {
	if (State.DarkMode) {
		auto black = Palette__TypeInfo->static_fields->Black;
		if (__this->fields.playerInfo->fields.IsDead) black.a *= 0.75f;
		SpriteRenderer_set_color(__this->fields.Background, black, NULL);
		auto textArea = __this->fields.TextArea;
		TMP_Text_set_color((app::TMP_Text*)textArea, Palette__TypeInfo->static_fields->White, NULL);
	}
	ChatBubble_SetText(__this, chatText, NULL);
}