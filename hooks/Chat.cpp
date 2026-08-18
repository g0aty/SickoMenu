#include "pch-il2cpp.h"
#include "_hooks.h"
#include "_rpc.h"
#include "utility.h"
#include "game.h"
#include "state.hpp"
#include <regex>

static float copyNotificationTimer = 0.f;
static bool isTextCut = false;

std::string trim(const std::string& str) {
	const auto start = str.find_first_not_of(" \t\n\r");
	if (start == std::string::npos) return "";
	const auto end = str.find_last_not_of(" \t\n\r");
	return str.substr(start, end - start + 1);
}

static std::string strToLower(std::string str) {
	std::string new_str = "";
	for (auto i : str) {
		new_str += char(std::tolower(i));
	}
	return new_str;
}

float calculateChatNewlineSize(std::string playerName) {
	float result = 0.f;
	float maxSize = 100.f;  // track per-line maximum
	size_t i = 0;

	while (i < playerName.size()) {
		// Handle <br>
		if (i + 3 < playerName.size() && playerName.compare(i, 4, "<br>") == 0) {
			result += maxSize;
			maxSize = 100.f;
			i += 4;
			continue;
		}

		// Handle newline
		if (playerName[i] == '\n') {
			result += maxSize;
			maxSize = 100.f;
			i++;
			continue;
		}

		// Handle <size=...>
		if (i + 6 < playerName.size() && playerName.compare(i, 6, "<size=") == 0) {
			i += 6;
			size_t endPos = playerName.find('>', i);
			if (endPos == std::string::npos) break;

			std::string valueStr = playerName.substr(i, endPos - i);
			bool isPercent = false;

			if (!valueStr.empty() && valueStr.back() == '%') {
				isPercent = true;
				valueStr.pop_back();
			}

			try {
				float value = std::stof(valueStr);
				if (isPercent) {
					maxSize = (std::max)(maxSize, value);
				}
				else {
					maxSize = (std::max)(maxSize, value * 40.f);
				}
			}
			catch (...) {
				// ignore invalid values
			}

			i = endPos + 1;
			continue;
		}

		// Handle <voffset=...>
		if (i + 9 < playerName.size() && playerName.compare(i, 9, "<voffset=") == 0) {
			i += 9;
			size_t endPos = playerName.find('>', i);
			if (endPos == std::string::npos) break;

			std::string valueStr = playerName.substr(i, endPos - i);

			try {
				float value = std::stof(valueStr);
				if (value < 0) {
					result += (-value) * 40.f;
				}
			}
			catch (...) {
				// ignore invalid values
			}

			i = endPos + 1;
			continue;
		}

		i++; // normal character
	}

	// Add last line's size
	result += maxSize;

	// Subtract 100%
	result -= 100.f;

	return result;
}

void SendPrivateWarnMessage(PlayerControl* toPlayer, const std::string& reason, int totalWarns) {
	if (!State.NotifyWarned || !toPlayer) return;

	if (!*Game::pLocalPlayer) return;

	if (IsHost() && State.ChatCooldown >= 3.f) /* <- In order not to look ridiculous where we are not host :sob: */ {
		std::string message = std::format("You were warned by Reason: {}\n\nTotal warns: {}", reason, totalWarns);
		if (message.length() > 120) {
			message = message.substr(0, 120);
		}

		MessageWriter* writer = InnerNetClient_StartRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), (*Game::pLocalPlayer)->fields._.NetId, uint8_t(RpcCalls__Enum::SendChat), SendOption__Enum::Reliable, toPlayer->fields._.OwnerId, nullptr);
		MessageWriter_WriteString(writer, convert_to_string(message), nullptr);
		InnerNetClient_FinishRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), writer, nullptr);
	}
}

void updateCharCounterText(FreeChatInputField* freeChatField) {

	int charLimit = freeChatField->fields.textArea->fields.characterLimit;
	int length = freeChatField->fields.textArea->fields.text->fields.m_stringLength;
	std::string chatCooldownText = "";

	bool commandsAllowed = State.ReadAndSendSickoChat || State.ExtraCommands;

	if (!State.PanicMode && State.ShowChatTimer && !(State.CurrentChatMode == QuickChatModes__Enum::QuickChatOnly && commandsAllowed))
		chatCooldownText = State.ChatCooldown >= 3.f ? (length == 0 ? "" : "<#0f0>You can chat now!</color> ") : std::format("<#f00>Wait for {:.1f} seconds!</color> ", 3 - State.ChatCooldown);
	if (!State.PanicMode && copyNotificationTimer > 0.f) chatCooldownText = std::format("<#0f0>{} text to clipboard!</color> ", isTextCut ? "Cut" : "Copied", copyNotificationTimer);
	std::string charCountColor = std::format("<#{}{}0>", length >= 0.75 * charLimit ? "f" : "0", length < charLimit && length >= 0.75 * charLimit ? "f" : "0");

	if (length < 0.75 * charLimit) {
		Color32 chatTextCol = Color32_op_Implicit(TMP_Text_get_color((TMP_Text*)freeChatField->fields.textArea->fields.outputText, NULL), NULL);

		charCountColor = std::format("<#{:02x}{:02x}{:02x}>",
			chatTextCol.r, chatTextCol.g, chatTextCol.b);
	}

	std::string quickChatInfo = !State.PanicMode && State.CurrentChatMode == QuickChatModes__Enum::QuickChatOnly && commandsAllowed ? "<#f00>(Quick Chat Mode, only commands are allowed)</color> " : "";

	std::string updatedText = std::format("{}{}{}{}/{}</color>",
		quickChatInfo, chatCooldownText, charCountColor, length, charLimit);
	TMP_Text_set_text((TMP_Text*)freeChatField->fields.charCountText, convert_to_string(updatedText), NULL);
}

void ChangeChatNotificationBackground(ChatNotification* chatNotif, PlayerControl* sender) {
	if ((!State.PanicMode || State.TempPanicMode) && chatNotif != NULL) {
		auto textArea = chatNotif->fields.chatText;
		auto bgArea = chatNotif->fields.background;
		if (State.CustomGameTheme && sender != NULL) {
			auto bg32 = Color32();
			bg32.r = int(State.GameBgColor.x * 255); bg32.g = int(State.GameBgColor.y * 255); bg32.b = int(State.GameBgColor.z * 255); bg32.a = 255;
			auto bg = Color32_op_Implicit_1(bg32, NULL);
			auto text32 = Color32();
			text32.r = int(State.GameTextColor.x * 255); text32.g = int(State.GameTextColor.y * 255); text32.b = int(State.GameTextColor.z * 255); text32.a = 255;
			auto textCol = Color32_op_Implicit_1(text32, NULL);
			if (GetPlayerData(sender)->fields.IsDead) bg.a *= 0.75f;
			SpriteRenderer_set_color(bgArea, bg, NULL);
			TMP_Text_set_color((app::TMP_Text*)textArea, textCol, NULL);
			/*auto chatText = convert_from_string(TMP_Text_get_text((app::TMP_Text*)textArea, NULL));
			chatText = std::format("<#{:02x}{:02x}{:02x}>", text32.r, text32.g, text32.b) + chatText + "</color>";
			TMP_Text_set_text((app::TMP_Text*)textArea, convert_to_string(chatText), NULL);*/
		}
		else if (State.DarkMode && sender != NULL) {
			auto black = Color(0.133f, 0.133f, 0.133f, 1.f);
			SpriteRenderer_set_color(bgArea, black, NULL);
			TMP_Text_set_color((app::TMP_Text*)textArea, Palette__TypeInfo->static_fields->White, NULL);
			/*auto chatText = convert_from_string(TMP_Text_get_text((app::TMP_Text*)textArea, NULL));
			chatText = "<#fff>" + chatText + "</color>";
			TMP_Text_set_text((app::TMP_Text*)textArea, convert_to_string(chatText), NULL);*/
		}

		if (State.IsProcessingSickoChat) {
			SpriteRenderer_set_color(bgArea, Color(0.6f, 0.4f, 0.f, 1.f), NULL);
		}
	}
}
std::string UncensorLink(std::string text, std::string dotReplacer = ",");

void ChangeOtherChatObjectColors(ChatController* chatController, Color col) {
	// https://github.com/the-real-techiee/DarkModeAU/blob/main/DarkMode/DarkModeMain.cs

	auto openBanMenuIcon = GameObject_Find(convert_to_string("OpenBanMenuIcon"), NULL);
	auto openKeyboardIcon = GameObject_Find(convert_to_string("OpenKeyboardIcon"), NULL);
	auto quickChatIcon = GameObject_Find(convert_to_string("QuickChatIcon"), NULL);

	SpriteRenderer* openBanMenuSpriteRenderer = NULL;
	SpriteRenderer* openKeyboardSpriteRenderer = NULL;
	SpriteRenderer* quickChatSpriteRenderer = NULL;

	static std::string spriteRendererTypeName = translate_type_name("UnityEngine.SpriteRenderer, UnityEngine.CoreModule");
	Type* spriteRendererType = app::Type_GetType(convert_to_string(spriteRendererTypeName), NULL);

	if (openBanMenuIcon != NULL) {
		auto openBanMenuComponent = (Component_1*)GameObject_get_transform(openBanMenuIcon, NULL);
		openBanMenuSpriteRenderer = (SpriteRenderer*)Component_GetComponent(openBanMenuComponent, spriteRendererType, NULL);
	}

	if (openKeyboardIcon != NULL) {
		auto openKeyboardComponent = (Component_1*)GameObject_get_transform(openKeyboardIcon, NULL);
		openKeyboardSpriteRenderer = (SpriteRenderer*)Component_GetComponent(openKeyboardComponent, spriteRendererType, NULL);
	}

	if (quickChatIcon != NULL) {
		auto quickChatComponent = (Component_1*)GameObject_get_transform(quickChatIcon, NULL);
		quickChatSpriteRenderer = (SpriteRenderer*)Component_GetComponent(quickChatComponent, spriteRendererType, NULL);
	}

	float brighteningFactor = 2.36f;
	if (col.r * brighteningFactor <= 1 && col.g * brighteningFactor <= 1 && col.b * brighteningFactor <= 1) {
		col.r *= brighteningFactor;
		col.g *= brighteningFactor;
		col.b *= brighteningFactor;
	}

	if (chatController != NULL) {
		// https://github.com/xChipseq/VanillaEnhancements/blob/main/VanillaEnhancements/Patches/DarkModePatches.cs

		auto chatButton = chatController->fields.chatButton;
		auto chatButtonTransform = chatButton == NULL ? NULL : Component_get_transform((Component_1*)chatButton, NULL);

		auto backgroundChild = chatButtonTransform == NULL ? NULL : (Component_1*)Transform_FindChild(chatButtonTransform, convert_to_string("Background"), NULL);
		auto backgroundSprite = backgroundChild == NULL ? NULL : (SpriteRenderer*)Component_GetComponent(backgroundChild, spriteRendererType, NULL);

		auto inactiveChild = chatButtonTransform == NULL ? NULL : (Component_1*)Transform_FindChild(chatButtonTransform, convert_to_string("Inactive"), NULL);
		auto inactiveSprite = inactiveChild == NULL ? NULL : (SpriteRenderer*)Component_GetComponent(inactiveChild, spriteRendererType, NULL);

		auto activeChild = chatButtonTransform == NULL ? NULL : (Component_1*)Transform_FindChild(chatButtonTransform, convert_to_string("Active"), NULL);
		auto activeSprite = activeChild == NULL ? NULL : (SpriteRenderer*)Component_GetComponent(activeChild, spriteRendererType, NULL);

		auto selectedChild = chatButtonTransform == NULL ? NULL : (Component_1*)Transform_FindChild(chatButtonTransform, convert_to_string("Selected"), NULL);
		auto selectedSprite = selectedChild == NULL ? NULL : (SpriteRenderer*)Component_GetComponent(selectedChild, spriteRendererType, NULL);

		if (backgroundSprite != NULL)
			SpriteRenderer_set_color(backgroundSprite, col, NULL);

		if (inactiveSprite != NULL)
			SpriteRenderer_set_color(inactiveSprite, col, NULL);

		/*if (activeSprite != NULL)
			SpriteRenderer_set_color(activeSprite, col, NULL);

		if (selectedSprite != NULL)
			SpriteRenderer_set_color(selectedSprite, col, NULL);*/
	}

	if (openBanMenuSpriteRenderer != NULL)
		SpriteRenderer_set_color(openBanMenuSpriteRenderer, col, NULL);

	if (openKeyboardSpriteRenderer != NULL)
		SpriteRenderer_set_color(openKeyboardSpriteRenderer, col, NULL);

	if (quickChatSpriteRenderer != NULL)
		SpriteRenderer_set_color(quickChatSpriteRenderer, col, NULL);
}

void ShowChatNotification(ChatNotification* chatNotification, PlayerControl* sender, String* message) {
	// this function replicates the actual function used by the game to display chat notifications
	// this is necessary as ChatNotification_SetUp checks if the ShipStatus is null
	// thankfully, ChatNotification_Update takes care of checking if the PlayerCustomizationMenu is active

	chatNotification->fields.timeOnScreen = 5.f; // default in game duration
	auto gameObj = Component_get_gameObject((Component_1*)chatNotification, NULL);
	GameObject_SetActive(gameObj, true, NULL);

	auto pData = GetPlayerData(sender);
	Color32 playerOutlineColor = GetPlayerTextColor(GetPlayerOutfit(pData)->fields.ColorId, true);
	Color32 playerTextColor = GetPlayerTextColor(GetPlayerOutfit(pData)->fields.ColorId);
	std::string colorCode = std::format("<#{:02x}{:02x}{:02x}{:02x}>",
		playerTextColor.r, playerTextColor.g, playerTextColor.b, playerTextColor.a);
	std::string playerName = convert_from_string(GetPlayerOutfit(pData)->fields.PlayerName);
	std::string colorBlindName = convert_from_string(PoolablePlayer_get_ColorBlindName(chatNotification->fields.player, NULL));
	if (State.IsProcessingSickoChat) colorBlindName += " <b><#fb0>[<#ff006c>SickoChat</color>]</color></b>";

	ChatNotification_SetCosmetics(chatNotification, pData, NULL);
	TMP_Text_set_text((TMP_Text*)chatNotification->fields.playerColorText, convert_to_string(colorBlindName), NULL);
	TMP_Text_set_text((TMP_Text*)chatNotification->fields.playerNameText,
		convert_to_string(colorCode + playerName + "</color>"),
		NULL);
	TMP_Text_set_outlineColor((TMP_Text*)chatNotification->fields.playerNameText, playerOutlineColor, NULL);
	TMP_Text_set_text((TMP_Text*)chatNotification->fields.chatText, message, NULL);

	ChangeChatNotificationBackground(chatNotification, sender);
}

#pragma region CommandHandler

std::vector<std::string> splitCommand(std::string chatCommand, std::string separator = " ") {
	std::vector<std::string> ret;
	size_t begin = 0;

	while (true) {
		size_t end = chatCommand.find_first_of(separator, begin);
		ret.push_back(chatCommand.substr(begin, end - begin));

		if (end == std::string::npos) break;

		begin = end + 1;
	}

	return ret;
}

std::string replaceAll(std::string text, std::string ssToReplace, std::string replaceBy) {
	size_t pos = text.find(ssToReplace);

	while (pos != std::string::npos) {
		text.replace(pos, ssToReplace.size(), replaceBy);
		pos = text.find(ssToReplace, pos + replaceBy.size());
	}

	return text;
}

// return format is in { shouldCancelChat, shouldCancelChatCooldown }
std::vector<bool> handleCommands(PlayerControl* player, std::string chatText, std::string cmdPrefix = "/") {
	if (cmdPrefix.empty()) cmdPrefix = "/";

	if (State.PanicMode || !chatText.starts_with(cmdPrefix)) return { false, false };

	auto cmdWithArgs = splitCommand(chatText);
	if (cmdWithArgs.size() == 0) return { false, false };

	std::string command = cmdWithArgs[0].substr(cmdPrefix.size());
	if (command.empty()) return { false, false };

	if (player == *Game::pLocalPlayer && (command == "help" || command == "h" || command == "cmds")) {
		std::string msg =
			"<#87cefa><font=\"Barlow-Regular Masked\"><b>"
			"<size=120%>Available Commands:</size><size=75%>\n\n"
			"<#ff033e><p>help, <p>h, <p>cmds</color> ~ <#ff033e>Show All Commands</color>\n"
			"<#ff033e><p>add, <p>wl</color> ~ <#ff033e>Add Player's Friend Code to the Whitelist</color>\n"
			"<#ff033e><p>remove, <p>rwl</color> ~ <#ff033e>Remove Player's Friend Code From the Whitelist</color>\n"
			"<#ff033e><p>warn, <p>w</color> ~ <#ff033e>Warn Player by Friend Code</color>\n"
			"<#ff033e><p>unwarn, <p>uw</color> ~ <#ff033e>Unwarn Player by Friend Code</color>\n"
			"<#ff033e><p>checkwarns, <p>cw</color> ~ <#ff033e>Check All Warns of Player by Friend Code</color>\n\n";

		if (State.ReadAndSendSickoChat)
			msg += "<#ff033e><p>sickochat, <p>sc</color> ~ <#ff033e>Send SickoChat</color>\n\n";

		if (IsHost()) msg +=
			"<#ff033e><p>allkick</color> ~ <#ff033e>Kick Everyone</color>\n"
			"<#ff033e><p>allban</color> ~ <#ff033e>Ban Everyone</color>\n"
			"</size></b></font></color>";
		else msg += "</size></b></font></color>";

		msg = replaceAll(msg, "<p>", cmdPrefix);
		// ensure the output is correct with any prefix

		ChatController_AddChatWarning(Game::HudManager.GetInstance()->fields.Chat, convert_to_string(msg), NULL);
		return { true, true };
	}

	if (State.ReadAndSendSickoChat && player == *Game::pLocalPlayer && (command == "sickochat" || command == "sc")) {
		if (cmdWithArgs.size() <= 1) {
			std::string msg = "<#aaaaaa><size=-0.24><font=\"Barlow-Regular Masked\"><b>Usage: " +
				cmdPrefix + command + " <Message></b></font></color>";
			ChatController_AddChatWarning(Game::HudManager.GetInstance()->fields.Chat, convert_to_string(msg), NULL);
			return { true, true };
		}

		std::string scMessage = "";

		for (size_t i = 1; i < cmdWithArgs.size(); ++i) {
			scMessage += cmdWithArgs[i] + " ";
		}
		if (scMessage.ends_with(" ")) scMessage = scMessage.substr(0, scMessage.size() - 1);

		if (scMessage.empty()) {
			std::string msg = "<#ff0000><size=-0.24><font=\"Barlow-Regular Masked\"><b>SickoChat message cannot be empty.</b></font></color>";
			ChatController_AddChatWarning(Game::HudManager.GetInstance()->fields.Chat, convert_to_string(msg), NULL);
			return { true, true };
		}

		auto playerToChatAs = (!State.SafeMode && State.activeChatSpoof && State.playerToChatAs.has_value()) ? State.playerToChatAs.validate().get_PlayerControl() : *Game::pLocalPlayer;

		if (IsInGame()) State.rpcQueue.push(new RpcForceSickoChat(PlayerSelection(playerToChatAs), scMessage, true));
		if (IsInLobby()) State.lobbyRpcQueue.push(new RpcForceSickoChat(PlayerSelection(playerToChatAs), scMessage, true));

		return { true, true };
	}

	if (!State.ExtraCommands) return { false, false };

	if (true && (command == "add" || command == "wl")) {
		if (cmdWithArgs.size() == 1) {
			std::string msg = "<#aaaaaa><size=-0.24><font=\"Barlow-Regular Masked\"><b>Usage: " +
				cmdPrefix + command + " <FriendCode></b></font></color>";
			ChatController_AddChatWarning(Game::HudManager.GetInstance()->fields.Chat, convert_to_string(msg), NULL);
			return { true, true };
		}

		std::string fc = cmdWithArgs[1];
		if (!fc.empty()) {
			if (std::find(State.WhitelistFriendCodes.begin(), State.WhitelistFriendCodes.end(), fc) == State.WhitelistFriendCodes.end()) {
				State.WhitelistFriendCodes.push_back(fc);

				std::string msg = std::format("<#5cff83><size=-0.24><font=\"Barlow-Regular Masked\"><b>\"{}\" Added to Whitelist.</b></font></color>", fc);
				ChatController_AddChatWarning(Game::HudManager.GetInstance()->fields.Chat, convert_to_string(msg), NULL);
			}
			else {
				std::string msg = std::format("<#ffd93d><size=-0.24><font=\"Barlow-Regular Masked\"><b>\"{}\" Already in Whitelist.</b></font></color>", fc);
				ChatController_AddChatWarning(Game::HudManager.GetInstance()->fields.Chat, convert_to_string(msg), NULL);
			}
		}
		return { true, true };
	}

	if (true && (command == "remove" || command == "rwl")) {
		if (cmdWithArgs.size() == 1) {
			std::string msg = "<#aaaaaa><size=-0.24><font=\"Barlow-Regular Masked\"><b>Usage: " +
				cmdPrefix + command + " <FriendCode></b></font></color>";
			ChatController_AddChatWarning(Game::HudManager.GetInstance()->fields.Chat, convert_to_string(msg), NULL);
			return { true, true };
		}

		std::string fc = cmdWithArgs[1];
		auto it = std::find(State.WhitelistFriendCodes.begin(), State.WhitelistFriendCodes.end(), fc);
		if (it != State.WhitelistFriendCodes.end()) {
			State.WhitelistFriendCodes.erase(it);

			std::string msg = std::format("<#ff5c5c><size=-0.24><font=\"Barlow-Regular Masked\"><b>\"{}\" Removed from Whitelist.</b></font></color>", fc);
			ChatController_AddChatWarning(Game::HudManager.GetInstance()->fields.Chat, convert_to_string(msg), NULL);
		}
		else {
			std::string msg = std::format("<#ff0000><size=-0.24><font=\"Barlow-Regular Masked\"><b>\"{}\" Not found in Whitelist.</b></font></color>", fc);
			ChatController_AddChatWarning(Game::HudManager.GetInstance()->fields.Chat, convert_to_string(msg), NULL);
		}
		return { true, true };
	}

	if (true && (command == "warn" || command == "w")) {
		if (cmdWithArgs.size() <= 2) {
			std::string msg = "<#aaaaaa><size=-0.24><font=\"Barlow-Regular Masked\"><b>Usage: " +
				cmdPrefix + command + " <FriendCode> <Reason></b></font></color>";
			ChatController_AddChatWarning(Game::HudManager.GetInstance()->fields.Chat, convert_to_string(msg), NULL);
			return { true, true };
		}

		std::string fc = cmdWithArgs[1];
		std::string warnReason = "";

		for (size_t i = 2; i < cmdWithArgs.size(); ++i) {
			warnReason += cmdWithArgs[i] + " ";
		}
		if (warnReason.ends_with(" ")) warnReason = warnReason.substr(0, warnReason.size() - 1);

		if (warnReason.empty()) {
			std::string msg = "<#ff0000><size=-0.24><font=\"Barlow-Regular Masked\"><b>Warn reason cannot be empty.</b></font></color>";
			ChatController_AddChatWarning(Game::HudManager.GetInstance()->fields.Chat, convert_to_string(msg), NULL);
			return { true, true };
		}

		int& warnCount = State.WarnedFriendCodes[fc];
		warnCount++;
		State.WarnReasons[fc].push_back(warnReason);
		State.Save();

		std::string msg = std::format("<#ff5c5c><size=-0.24><font=\"Barlow-Regular Masked\"><b>\"{}\" Was Warned. Reason: \"{}\". Total warns: {}</b></font></color>", fc, warnReason, warnCount);
		ChatController_AddChatWarning(Game::HudManager.GetInstance()->fields.Chat, convert_to_string(msg), NULL);

		if (State.NotifyWarned && player == *Game::pLocalPlayer) {
			for (auto& player : GetAllPlayerControl()) {
				if (!player) continue;
				auto pdata = GetPlayerDataById(player->fields.PlayerId);
				if (!pdata) continue;

				std::string playerFC = convert_from_string(pdata->fields.FriendCode);
				if (playerFC == fc) {
					SendPrivateWarnMessage(player, warnReason, warnCount);
					break;
				}
			}
			return { true, false }; // ensure chat cooldown exists
		}

		return { true, true };
	}

	if (true && (command == "unwarn" || command == "uw")) {
		if (cmdWithArgs.size() <= 2) {
			std::string msg = "<#aaaaaa><size=-0.24><font=\"Barlow-Regular Masked\"><b>Usage: " +
				cmdPrefix + command + " <FriendCode> <ReasonNumber></b></font></color>";
			ChatController_AddChatWarning(Game::HudManager.GetInstance()->fields.Chat, convert_to_string(msg), NULL);
			return { true, false };
		}

		std::string fc = cmdWithArgs[1];
		std::string numberStr = cmdWithArgs[2];

		int reasonIndex = -1;
		try {
			reasonIndex = std::stoi(numberStr) - 1;
		}
		catch (...) {
			std::string msg = "<#ff0000><size=-0.24><font=\"Barlow-Regular Masked\"><b>Invalid <ReasonNumber>.</b></font></color>";
			ChatController_AddChatWarning(Game::HudManager.GetInstance()->fields.Chat, convert_to_string(msg), NULL);
			return { true, true };
		}

		auto it = State.WarnReasons.find(fc);
		if (it != State.WarnReasons.end() && reasonIndex >= 0 && reasonIndex < (int)it->second.size()) {
			it->second.erase(it->second.begin() + reasonIndex);

			if (--State.WarnedFriendCodes[fc] <= 0) {
				State.WarnedFriendCodes.erase(fc);
				State.WarnReasons.erase(fc);
			}

			State.Save();

			std::string msg = std::format("<#5cff83><size=-0.24><font=\"Barlow-Regular Masked\"><b>Removed Reason [#{}] for \"{}\".</b></font></color>", reasonIndex + 1, fc);
			ChatController_AddChatWarning(Game::HudManager.GetInstance()->fields.Chat, convert_to_string(msg), NULL);
		}
		else {
			std::string msg = std::format("<#ff0000><size=-0.24><font=\"Barlow-Regular Masked\"><b>Invalid <FriendCode> or <ReasonNumber>.</b></font></color>");
			ChatController_AddChatWarning(Game::HudManager.GetInstance()->fields.Chat, convert_to_string(msg), NULL);
		}

		return { true, true };
	}

	if (player == *Game::pLocalPlayer && (command == "checkwarns" || command == "cw")) {
		if (cmdWithArgs.size() == 1) {
			std::string msg = "<#aaaaaa><size=-0.24><font=\"Barlow-Regular Masked\"><b>Usage: " +
				cmdPrefix + command + " <FriendCode></b></font></color>";
			ChatController_AddChatWarning(Game::HudManager.GetInstance()->fields.Chat, convert_to_string(msg), NULL);
			return { true, true };
		}

		std::string fc = cmdWithArgs[1];
		auto it = State.WarnReasons.find(fc);

		if (it != State.WarnReasons.end() && !it->second.empty()) {
			std::string allReasons;
			for (size_t i = 0; i < it->second.size(); ++i) {
				allReasons += std::format("[{}] {}", i + 1, it->second[i]);
				if (i + 1 < it->second.size())
					allReasons += "\n";
			}
			std::string msg = std::format("<#ffff00><size=-0.24><font=\"Barlow-Regular Masked\"><b>All warns for <#FFF>\"{}\":\n\n{}</b></font></color>", fc, allReasons);
			ChatController_AddChatWarning(Game::HudManager.GetInstance()->fields.Chat, convert_to_string(msg), NULL);
		}
		else {
			std::string msg = std::format("<#ff0000><size=-0.24><font=\"Barlow-Regular Masked\"><b>No warns found for \"{}\".</b></font></color>", fc);
			ChatController_AddChatWarning(Game::HudManager.GetInstance()->fields.Chat, convert_to_string(msg), NULL);
		}
		return { true, true };
	}

	if (IsHost()) {
		// banall can fail to work when the host has censor chat turned on,
		// and someone else with correct permissions uses it, as banall has "anal" in it
		// so the commands have been changed to allkick and allban

		if (true && command == "allkick") {
			bool isBan = false;
			if (IsInGame()) {
				State.rpcQueue.push(new PunishEveryone(isBan));
			}
			else if (IsInLobby()) {
				State.lobbyRpcQueue.push(new PunishEveryone(isBan));
			}
			return { true, true };
		}

		if (true && command == "allban") {
			bool isBan = true;
			if (IsInGame()) {
				State.rpcQueue.push(new PunishEveryone(isBan));
			}
			else if (IsInLobby()) {
				State.lobbyRpcQueue.push(new PunishEveryone(isBan));
			}
			return { true, true };
		}
	}

	return { false, false };
}

#pragma endregion

void dChatController_AddChat(ChatController* __this, PlayerControl* sourcePlayer, String* chatText, bool censor, MethodInfo* method) {
	if (State.ShowHookLogs) Log.HookDebug("Hook dChatController_AddChat executed", false);
	censor = IsChatCensored(); // Fix chat not being censored
	if (!State.PanicMode) {
		auto player = GetPlayerData(sourcePlayer);
		auto local = GetPlayerData(*Game::pLocalPlayer);
		std::string message = convert_from_string(chatText);
		std::string newChatText = message;
		if (State.BetterChatNotifications && __this->fields.state == ChatControllerState__Enum::Closed &&
			(State.ReadGhostMessages || local->fields.IsDead || !player->fields.IsDead) &&
			player == ((!State.SafeMode && State.playerToChatAs.has_value()) ?
			State.playerToChatAs.validate().get_PlayerData() : local)) {
			auto chatNotif = Game::HudManager.GetInstance()->fields.Chat->fields.chatNotification;
			ShowChatNotification(chatNotif, sourcePlayer, chatText);
			LOG_DEBUG("smth");
		}
		std::string playerName = convert_from_string(NetworkedPlayerInfo_get_PlayerName(player, nullptr));
		if (State.CustomName && !State.ServerSideCustomName && (sourcePlayer == *Game::pLocalPlayer || State.CustomNameForEveryone)) {
			playerName = GetCustomName(playerName);
		}
		float newlineSize = calculateChatNewlineSize(playerName);
		if (newlineSize > 0.f) {
			newChatText = std::format("<size={}%><#0000>0</color></size>\n", newlineSize) + newChatText;
		}
		if (State.ReadGhostMessages) {
			bool wasDead = false;

			if (player != NULL && player->fields.IsDead && local != NULL && !local->fields.IsDead) {
				local->fields.IsDead = true;
				wasDead = true;
			}
			chatText = convert_to_string(newChatText);
			ChatController_AddChat(__this, sourcePlayer, chatText, censor, method);
			auto outfit = GetPlayerOutfit(player);
			uint32_t colorId = outfit->fields.ColorId;
			if (wasDead) {
				local->fields.IsDead = false;
			}
		}
		else {
			chatText = convert_to_string(newChatText);
			ChatController_AddChat(__this, sourcePlayer, chatText, censor, method);
		}
		if (State.Enable_SMAC) {
			static const std::set<std::string> KNOWN_COMMAND_WORDS = {
				"/help", "/h", "/cmds", "/color", "/colour", "/sicko", "/r", "/rules", "/s", "/start", "/end",
				"/kick", "/kickc", "/ban", "/banc",
				"/warn", "/w", "/warnc", "/unwarn", "/uw", "/unwarnc", "/checkwarns", "/cw",
				"/callmeeting", "/endmeeting",
			};
			std::string firstWordLower = strToLower(message.substr(0, message.find(' ')));
			bool isRecognizedCommand = KNOWN_COMMAND_WORDS.count(firstWordLower) > 0;
			if (State.SMAC_CheckChat && !isRecognizedCommand && ((IsInGame() && !State.InMeeting && !player->fields.IsDead) || chatText->fields.m_stringLength > 120)) {
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
		if (State.BetterMessageSounds && (State.ReadGhostMessages || !player->fields.IsDead) && player != local &&
			(__this->fields.state == ChatControllerState__Enum::Open || __this->fields.state == ChatControllerState__Enum::Opening)) {
			auto audioSource = SoundManager_PlaySound(SoundManager__TypeInfo->static_fields->instance, (AudioClip*)__this->fields.messageSound, false, 1.f, NULL, NULL);
			AudioSource_set_pitch(audioSource, 0.5f + (float)sourcePlayer->fields.PlayerId / 15, NULL);
		}
	}
	else {
		ChatController_AddChat(__this, sourcePlayer, chatText, censor, method);
	}
}

void dChatController_SetVisible(ChatController* __this, bool visible, MethodInfo* method) {
	if (State.ShowHookLogs) Log.HookDebug("Hook dChatController_SetVisible executed", false);
	if (State.ChatAlwaysActive && !State.PanicMode)
		ChatController_SetVisible(__this, true, method);
	else
	{
		State.ChatActiveOriginalState = visible;
		ChatController_SetVisible(__this, visible, method);
	}
}

void dChatBubble_SetName(ChatBubble* __this, String* playerName, bool isDead, bool voted, Color color, MethodInfo* method) {
	if (State.ShowHookLogs) Log.HookDebug("Hook dChatBubble_SetName executed", false);
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
				if (State.IsProcessingSickoChat) {
					std::string prefix = "<size=90%><#fb0>[<#ff006c>SickoChat</color>]</color> ";
					playerName = convert_to_string(prefix + convert_from_string(playerName) + "</size>");
				}
			}
		}
	}
	ChatBubble_SetName(__this, playerName, isDead, voted, color, method);
}

void dChatController_Update(ChatController* __this, MethodInfo* method) {
	if (State.ShowHookLogs) Log.HookDebug("Hook dChatController_Update executed", false);
	auto freeChatField = __this->fields.freeChatField;
	int length = freeChatField->fields.textArea->fields.text->fields.m_stringLength;

	auto pool = __this->fields.chatBubblePool;
	if (pool->fields.poolSize == 20 && State.ExtendChatHistory) {
		pool->fields.poolSize = 127; // Weird glitches happen to the chat if we extend it past 127
		ObjectPoolBehavior_ReclaimOldest(pool, NULL);
	}
	if (pool->fields.poolSize == 127 && !State.ExtendChatHistory) {
		pool->fields.poolSize = 20;
		ObjectPoolBehavior_ReclaimOldest(pool, NULL);
	}

	if (__this->fields.state != ChatControllerState__Enum::Closed) {
		if (State.PanicMode) freeChatField->fields.textArea->fields.characterLimit = 100;
		else freeChatField->fields.textArea->fields.characterLimit = State.SafeMode ? (State.ExtendChatLimit ? 120 : 100) : 2147483647;

		freeChatField->fields.textArea->fields.allowAllCharacters = !State.PanicMode;
		freeChatField->fields.textArea->fields.AllowEmail = !State.PanicMode;
		freeChatField->fields.textArea->fields.AllowSymbols = !State.PanicMode;

		freeChatField->fields.charCountText->fields._.m_enableWordWrapping = State.PanicMode;

		updateCharCounterText(freeChatField);
		if (State.CurrentChatMode == QuickChatModes__Enum::QuickChatOnly) ChatController_UpdateChatMode(__this, NULL);
		// Always update quick chat chatting mode
	}

	if (!State.SafeMode || State.CurrentChatMode == QuickChatModes__Enum::QuickChatOnly || State.WasPreviousMessageCommand) {
		__this->fields.timeSinceLastMessage = 420.69f; //we can set this to anything more than or equal to 3 and it'll work
		if (State.WasPreviousMessageCommand) State.WasPreviousMessageCommand = false;
	}

	if ((!State.PanicMode || State.TempPanicMode) && (State.CustomGameTheme || State.DarkMode)) {
		if (State.CustomGameTheme) {
			auto bg32 = Color32();
			bg32.r = int(State.GameBgColor.x * 255); bg32.g = int(State.GameBgColor.y * 255); bg32.b = int(State.GameBgColor.z * 255); bg32.a = 255;
			auto bg = Color32_op_Implicit_1(bg32, NULL);
			auto text32 = Color32();
			text32.r = int(State.GameTextColor.x * 255); text32.g = int(State.GameTextColor.y * 255); text32.b = int(State.GameTextColor.z * 255); text32.a = 255;
			auto textCol = Color32_op_Implicit_1(text32, NULL);
			if (__this->fields.freeChatField != NULL) {
				auto outputText = __this->fields.freeChatField->fields.textArea->fields.outputText;
				TMP_Text_set_color((app::TMP_Text*)outputText, textCol, NULL);
				auto col = SpriteRenderer_get_color(__this->fields.freeChatField->fields._.background, NULL);
				bool isHighlighted = col.r == 0.f && col.g == 1.f && col.b == 0.f && col.a == 1.f;
				if (!isHighlighted)
					SpriteRenderer_set_color(__this->fields.freeChatField->fields._.background, bg, NULL);
				else {
					auto green32 = Color32();
					green32.r = 34; green32.g = 100; green32.b = 34; green32.a = 255;
					auto green = Color32_op_Implicit_1(green32, NULL);
					SpriteRenderer_set_color(__this->fields.freeChatField->fields._.background, green, NULL);
				}
			}
			if (__this->fields.quickChatField != NULL) {
				auto text = __this->fields.quickChatField->fields.text;
				auto placeholderText = __this->fields.quickChatField->fields.placeholderText;
				TMP_Text_set_color((app::TMP_Text*)text, textCol, NULL);
				TMP_Text_set_color((app::TMP_Text*)placeholderText, textCol, NULL);
				auto col = SpriteRenderer_get_color(__this->fields.quickChatField->fields._.background, NULL);
				bool isHighlighted = col.r == 0.f && col.g == 1.f && col.b == 0.f && col.a == 1.f;
				if (!isHighlighted)
					SpriteRenderer_set_color(__this->fields.quickChatField->fields._.background, bg, NULL);
				else {
					auto green32 = Color32();
					green32.r = 34; green32.g = 100; green32.b = 34; green32.a = 255;
					auto green = Color32_op_Implicit_1(green32, NULL);
					SpriteRenderer_set_color(__this->fields.quickChatField->fields._.background, green, NULL);
				}
			}

			SpriteRenderer* backgroundSpriteRenderer = Game::HudManager.GetInstance()->fields.Chat->fields.backgroundImage;
			if (backgroundSpriteRenderer != NULL)
				SpriteRenderer_set_color(backgroundSpriteRenderer, bg, NULL);

			if (State.DarkMode) {
				auto gray = Color(0.5f, 0.5f, 0.5f, 1.f);
				ChangeOtherChatObjectColors(__this, gray);
			}
			else {
				ChangeOtherChatObjectColors(__this, Palette__TypeInfo->static_fields->White);
			}
		}
		else if (State.DarkMode) {
			auto gray32 = Color32();
			gray32.r = 34; gray32.g = 34; gray32.b = 34; gray32.a = 255;
			auto gray = Color32_op_Implicit_1(gray32, NULL);
			auto green32 = Color32();
			green32.r = 34; green32.g = 100; green32.b = 34; green32.a = 255;
			auto green = Color32_op_Implicit_1(green32, NULL);
			if (__this->fields.freeChatField != NULL) {
				auto outputText = __this->fields.freeChatField->fields.textArea->fields.outputText;
				TMP_Text_set_color((app::TMP_Text*)outputText, Palette__TypeInfo->static_fields->White, NULL);
				auto col = SpriteRenderer_get_color(__this->fields.quickChatField->fields._.background, NULL);
				bool isHighlighted = col.r == 0.f && col.g == 1.f && col.b == 0.f && col.a == 1.f;
				SpriteRenderer_set_color(__this->fields.freeChatField->fields._.background, isHighlighted ? green : gray, NULL);
			}
			if (__this->fields.quickChatField != NULL) {
				auto text = __this->fields.quickChatField->fields.text;
				auto placeholderText = __this->fields.quickChatField->fields.placeholderText;
				TMP_Text_set_color((app::TMP_Text*)text, Palette__TypeInfo->static_fields->White, NULL);
				TMP_Text_set_color((app::TMP_Text*)placeholderText, Palette__TypeInfo->static_fields->White, NULL);
				auto col = SpriteRenderer_get_color(__this->fields.quickChatField->fields._.background, NULL);
				bool isHighlighted = col.r == 0.f && col.g == 1.f && col.b == 0.f && col.a == 1.f;
				SpriteRenderer_set_color(__this->fields.quickChatField->fields._.background, isHighlighted ? green : gray, NULL);
			}

			SpriteRenderer* backgroundSpriteRenderer = Game::HudManager.GetInstance()->fields.Chat->fields.backgroundImage;
			if (backgroundSpriteRenderer != NULL)
				SpriteRenderer_set_color(backgroundSpriteRenderer, gray, NULL);

			ChangeOtherChatObjectColors(__this, Color(0.5f, 0.5f, 0.5f, 1.f));
		}
	}
	else {
		if (__this->fields.freeChatField != NULL) {
			auto outputText = __this->fields.freeChatField->fields.textArea->fields.outputText;
			TMP_Text_set_color((app::TMP_Text*)outputText, Palette__TypeInfo->static_fields->Black, NULL);
			auto col = SpriteRenderer_get_color(__this->fields.freeChatField->fields._.background, NULL);
			bool isHighlighted = col.r == 0.f && col.g == 1.f && col.b == 0.f && col.a == 1.f;
			if (!isHighlighted)
				SpriteRenderer_set_color(__this->fields.freeChatField->fields._.background, Palette__TypeInfo->static_fields->White, NULL);
		}
		if (__this->fields.quickChatField != NULL) {
			auto text = __this->fields.quickChatField->fields.text;
			TMP_Text_set_color((app::TMP_Text*)text, Palette__TypeInfo->static_fields->Black, NULL);
			auto col = SpriteRenderer_get_color(__this->fields.quickChatField->fields._.background, NULL);
			bool isHighlighted = col.r == 0.f && col.g == 1.f && col.b == 0.f && col.a == 1.f;
			if (!isHighlighted)
				SpriteRenderer_set_color(__this->fields.quickChatField->fields._.background, Palette__TypeInfo->static_fields->White, NULL);
		}

		SpriteRenderer* backgroundSpriteRenderer = Game::HudManager.GetInstance()->fields.Chat->fields.backgroundImage;
		if (backgroundSpriteRenderer != NULL)
			SpriteRenderer_set_color(backgroundSpriteRenderer, Palette__TypeInfo->static_fields->White, NULL);

		ChangeOtherChatObjectColors(__this, Palette__TypeInfo->static_fields->White);
	}

	State.MessageSound = (AudioClip*)__this->fields.messageSound;

	auto chatText = __this->fields.freeChatField->fields.textArea->fields.text;
	__this->fields.freeChatField->fields.textArea->fields.AllowPaste = State.ChatPaste && !State.PanicMode && length == 0;
	// Only allow pasting if the field is empty and chat paste is enabled
	bool isCtrl = ImGui::IsKeyDown(0x11) || ImGui::IsKeyDown(0xA2) || ImGui::IsKeyDown(0xA3);
	bool isCpressed = ImGui::IsKeyPressed(0x43) || ImGui::IsKeyDown(0x63);
	bool isXpressed = ImGui::IsKeyPressed(0x58) || ImGui::IsKeyDown(0x78);
	if (State.ChatPaste && isCtrl && (isCpressed || isXpressed) && convert_from_string(chatText) != "") {
		ClipboardHelper_PutClipboardString(chatText, NULL); //ctrl+c
		copyNotificationTimer = 1.5f; // Show copy notification for 1.5 seconds
		if (isXpressed) {
			FreeChatInputField_Clear(__this->fields.freeChatField, NULL); //ctrl+x
			isTextCut = true; // Set flag to indicate text was cut
		}
		else isTextCut = false; // Reset flag if only copied
	}

	if (copyNotificationTimer > 0.f) {
		copyNotificationTimer -= Time_get_deltaTime(NULL);
		if (copyNotificationTimer <= 0.f) copyNotificationTimer = 0.f;
	}

	if (State.MessageSent && State.SafeMode) {
		__this->fields.timeSinceLastMessage = 0.f;
		State.MessageSent = false;
	}
	State.ChatCooldown = __this->fields.timeSinceLastMessage;
	State.ChatFocused = __this->fields.freeChatField->fields.textArea->fields.hasFocus;

	if (!(IsHost() || !State.SafeMode)) State.ChatSpamMode = 0;

	if (IsChatValid(State.chatMessage)) {
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
	}

	if (!State.PanicMode && State.AprilFoolsMode && State.BrainrotEveryone && (IsInGame() || IsInLobby()) && (__this->fields.timeSinceLastMessage >= 3.5f || !State.SafeMode)) {
		std::vector<std::string> brainrotList = { "Crazy? I was crazy once. They locked me in a room. A rubber room with Fucksons, and Fucksons give me rats.",
			"I like my cheese drippy bruh", "Imagine if Ninja got a low taper fade", "I woke up in Ohio, feeling kinda fly", "What trollface are you?",
			"Skibidi dop dop dop yes yes", "From the gyatt to the sus to the rizz to the mew", "Yeah I'm edging in Ohio, fanum taxing as I goon",
			"You gotta give him that Hawk TUAH and spit on that thang", "Sticking out your gyatt for the rizzler", "I'm Baby Gronk from Ohio",
			"19 dollar fortnite card, who wants it?", "Erm, what the sigma?", "I'll take a double triple Grimace Shake on a gyatt",
			"I know I'm a SIGMA but that doesnt mean I can't have a GYATT too", "Just put the fries in the bag bro", "Stay on the sigma grindset",
			"Sigma Sigma on the wall, who is the skibidiest of them all?", "Duke Dennis did you pray today?", "What kinda bomboclat dawg are ya" };
		auto player = !State.SafeMode && State.playerToChatAs.has_value() ? State.playerToChatAs.validate().get_PlayerControl() : *Game::pLocalPlayer;
		PlayerControl_RpcSendChat(player, convert_to_string(brainrotList[randi(0, (int)brainrotList.size() - 1)]), NULL);
		State.MessageSent = true;
	}

	if (!State.PanicMode && State.AprilFoolsMode && State.RizzUpEveryone && (IsInGame() || IsInLobby()) && (__this->fields.timeSinceLastMessage >= 3.5f || !State.SafeMode)) {
		std::vector<std::string> rizzLinesList = { "Do you have some Ohio rizz? Because you just turned my brain into pure jelly!",
			"If beauty were a Skibidi Toilet, you'd be the one everyone’s trying to get next to!", "Is your name Ohio? Because you’re making my heart do the Skibidi!",
			"Is your aura made of coffee? Because you’re brewing up some strong feelings in me!", "I see dat gyatt and I wanna fanum tax some of dat",
			"Am I Baby Gronk? Because you can be my Livvy Dunne", "Sup shawty, are you skibidi, because I could use that to my sigma", "Hey shawty, are you skibidi rizz in ohio?",
			"Yer a rizzard Harry", "Remind me what a work of skibidi rizz looks like" };
		auto player = !State.SafeMode && State.playerToChatAs.has_value() ? State.playerToChatAs.validate().get_PlayerControl() : *Game::pLocalPlayer;
		PlayerControl_RpcSendChat(player, convert_to_string(rizzLinesList[randi(0, (int)rizzLinesList.size() - 1)]), NULL);
		State.MessageSent = true;
	}

	ChatController_Update(__this, method);

	/*if ((!State.PanicMode || State.TempPanicMode) && (State.DarkMode || State.CustomGameTheme) && __this->fields.freeChatField != NULL) {
		//__this->fields.freeChatField->fields.textArea->fields.compoText = convert_to_string(RemoveHtmlTags(convert_from_string(__this->fields.freeChatField->fields.textArea->fields.compoText)));
	}*/
	//nah fuck compoText
}

bool dTextBoxTMP_IsCharAllowed(TextBoxTMP* __this, uint16_t unicode_char, MethodInfo* method)
{
	if (State.ShowHookLogs) Log.HookDebug("Hook dTextBoxTMP_IsCharAllowed executed", false);
	if (__this->fields.ForceUppercase) return TextBoxTMP_IsCharAllowed(__this, unicode_char, method);
	// Patch lobby codes

	//0x08 is backspace, 0x0D is carriage return, 0x7F is delete character, 0x3C is <, 0x3E is >, 0x5B is [
	//lobby codes force uppercase, and we don't change that to fix joining a lobby with code not working
	if (!State.PanicMode) return (unicode_char != 0x08 && unicode_char != 0x0D && unicode_char != 0x7F && ((State.SafeMode && unicode_char != 0x3C && unicode_char != 0x3E && unicode_char != 0x5B) || !State.SafeMode));
	return TextBoxTMP_IsCharAllowed(__this, unicode_char, method);
}

void dTextBoxTMP_SetText(TextBoxTMP* __this, String* input, String* inputCompo, MethodInfo* method)
{
	if (State.ShowHookLogs) Log.HookDebug("Hook dTextBoxTMP_SetText executed", false);
	if (__this->fields.ForceUppercase) {
		__this->fields.SendOnFullChars = State.PanicMode || !State.BetterLobbyCodeInput;
		__this->fields.ClearOnFocus = State.PanicMode || !State.BetterLobbyCodeInput;
		return TextBoxTMP_SetText(__this, input, inputCompo, method);
	}
	// Patch lobby codes

	if (!State.PanicMode) {
		if (!State.SafeMode)
			__this->fields.characterLimit = 2147483647;
		else
			__this->fields.characterLimit = State.ExtendChatLimit ? 120 : 100;
	}
	else __this->fields.characterLimit = 100;
	//inputCompo = convert_to_string(RemoveHtmlTags(convert_from_string(inputCompo))); // Fix #fff/color bug in text input field
	//nah fuck compoText

	TextBoxTMP_SetText(__this, input, inputCompo, method);
}

std::string UncensorLink(std::string text, std::string dotReplacer) {
	std::string pattern = R"((http[s]?://)?([a-zA-Z0-9-]+\.)+[a-zA-Z]{2,6}(/[\w\-./?%&=]*)?|([a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9-.]+))";
	std::regex regex(pattern);

	std::string result;
	std::sregex_iterator begin(text.begin(), text.end(), regex);
	std::sregex_iterator end;

	size_t lastPos = 0;

	for (auto it = begin; it != end; ++it) {
		auto matchPos = it->position();
		auto matchLen = it->length();

		// Append the part before the match
		result += text.substr(lastPos, matchPos - lastPos);

		// Censor the match
		std::string censored = it->str();
		size_t pos = 0;
		while ((pos = censored.find(".", pos)) != std::string::npos) {
			censored.replace(pos, 1, dotReplacer);
			pos += dotReplacer.length(); // move past the inserted text
		}
		result += censored;

		lastPos = matchPos + matchLen;
	}

	// Append the rest of the string after the last match
	result += text.substr(lastPos);

	return result;
}

void dPlayerControl_RpcSendChat(PlayerControl* __this, String* chatText, MethodInfo* method)
{
	if (State.ShowHookLogs) Log.HookDebug("Hook dPlayerControl_RpcSendChat executed", false);
	PlayerControl_RpcSendChat(__this, chatText, NULL); // This hook should be useless since dChatController_SendFreeChat sends rpc directly
}

void dChatBubble_SetText(ChatBubble* __this, String* chatText, MethodInfo* method) {
	if (State.ShowHookLogs) Log.HookDebug("Hook dChatBubble_SetText executed", false);
	if ((!State.PanicMode || State.TempPanicMode)) {
		// std::string colorOpener = "", colorCloser = "";
		SpriteRenderer_set_color(__this->fields.MaskArea, Palette__TypeInfo->static_fields->ClearWhite, NULL);

		if (State.CustomGameTheme) {
			auto bg32 = Color32();
			bg32.r = int(State.GameBgColor.x * 255); bg32.g = int(State.GameBgColor.y * 255); bg32.b = int(State.GameBgColor.z * 255); bg32.a = 255;
			auto bg = Color32_op_Implicit_1(bg32, NULL);
			auto text32 = Color32();
			text32.r = int(State.GameTextColor.x * 255); text32.g = int(State.GameTextColor.y * 255); text32.b = int(State.GameTextColor.z * 255); text32.a = 255;
			auto textCol = Color32_op_Implicit_1(text32, NULL);
			bool isChatWarning = __this->fields.playerInfo == NULL;
			if (!isChatWarning && __this->fields.playerInfo->fields.IsDead) bg.a *= 0.75f;
			SpriteRenderer_set_color(__this->fields.Background, bg, NULL);
			if (!isChatWarning) {
				auto textArea = __this->fields.TextArea;
				TMP_Text_set_color((app::TMP_Text*)textArea, textCol, NULL);
				// colorOpener = std::format("<#{:02x}{:02x}{:02x}>", text32.r, text32.g, text32.b);
				// colorCloser = "</color>";
			}
		}
		else if (State.DarkMode) {
			auto black = Color(0.133f, 0.133f, 0.133f, 1.f);
			bool isChatWarning = __this->fields.playerInfo == NULL;
			if (!isChatWarning && __this->fields.playerInfo->fields.IsDead) black.a *= 0.75f;
			SpriteRenderer_set_color(__this->fields.Background, black, NULL);
			if (!isChatWarning) {
				auto textArea = __this->fields.TextArea;
				TMP_Text_set_color((app::TMP_Text*)textArea, Palette__TypeInfo->static_fields->White, NULL);
				// colorOpener = "<#fff>";
				// colorCloser = "</color>";
			}
		}

		if (State.IsProcessingSickoChat) {
			auto darkGold = Color(0.6f, 0.4f, 0.f, 1.f);
			if (__this->fields.playerInfo->fields.IsDead) darkGold.a *= 0.75f;
			SpriteRenderer_set_color(__this->fields.Background, darkGold, NULL);
		}

		std::string fontOpener = "", fontCloser = "";
		if (State.ChatFont) {
			switch (State.ChatFontType) {
			case 0: {
				fontOpener = "<font=\"Barlow-Italic SDF\">";
				break;
			}
			case 1: {
				fontOpener = "<font=\"Barlow-Medium SDF\">";
				break;
			}
			case 2: {
				fontOpener = "<font=\"Barlow-Bold SDF\">";
				break;
			}
			case 3: {
				fontOpener = "<font=\"Barlow-SemiBold SDF\">";
				break;
			}
			case 4: {
				fontOpener = "<font=\"Barlow-SemiBold Masked\">";
				break;
			}
			case 5: {
				fontOpener = "<font=\"Barlow-ExtraBold SDF\">";
				break;
			}
			case 6: {
				fontOpener = "<font=\"Barlow-BoldItalic SDF\">";
				break;
			}
			case 7: {
				fontOpener = "<font=\"Barlow-BoldItalic Masked\">";
				break;
			}
			case 8: {
				fontOpener = "<font=\"Barlow-Black SDF\">";
				break;
			}
			case 9: {
				fontOpener = "<font=\"Barlow-Light SDF\">";
				break;
			}
			case 10: {
				fontOpener = "<font=\"Barlow-Regular SDF\">";
				break;
			}
			case 11: {
				fontOpener = "<font=\"Barlow-Regular Masked\">";
				break;
			}
			case 12: {
				fontOpener = "<font=\"Barlow-Regular Outline\">";
				break;
			}
			case 13: {
				fontOpener = "<font=\"Brook SDF\">";
				break;
			}
			case 14: {
				fontOpener = "<font=\"LiberationSans SDF\">";
				break;
			}
			case 15: {
				fontOpener = "<font=\"NotoSansJP-Regular SDF\">";
				break;
			}
			case 16: {
				fontOpener = "<font=\"VCR SDF\">";
				break;
			}
			case 17: {
				fontOpener = "<font=\"CONSOLA SDF\">";
				break;
			}
			case 18: {
				fontOpener = "<font=\"digital-7 SDF\">";
				break;
			}
			case 19: {
				fontOpener = "<font=\"OCRAEXT SDF\">";
				break;
			}
			case 20: {
				fontOpener = "<font=\"DIN_Pro_Bold_700 SDF\">";
				break;
			}
			}

			fontCloser = "</font>";
		}

		chatText = convert_to_string(fontOpener + /*colorOpener + */convert_from_string(chatText) + /*colorCloser + */fontCloser);
	}
	ChatBubble_SetText(__this, chatText, method);
}

void dChatController_SendFreeChat(ChatController* __this, MethodInfo* method) {
	if (State.ShowHookLogs) Log.HookDebug("Hook dChatController_SendFreeChat executed", false);
	auto chatText = convert_from_string(__this->fields.freeChatField->fields.textArea->fields.text);
	if (convert_to_string(UncensorLink(chatText, ".­"))->fields.m_stringLength > 120) chatText = UncensorLink(chatText);
	else chatText = UncensorLink(chatText, ".­");
	if (chatText == "") return;
	std::string chatTextLower = strToLower(chatText);
	if (!State.PanicMode) {
		auto cmdResult = handleCommands(*Game::pLocalPlayer, chatText, "/");
		State.WasPreviousMessageCommand = cmdResult[1];
		if (cmdResult[0]) return;

		if (State.CurrentChatMode == QuickChatModes__Enum::QuickChatOnly) {
			ChatController_AddChatWarning(Game::HudManager.GetInstance()->fields.Chat, convert_to_string("Free chat is not allowed!"), NULL);
			return;
		}

		auto playerToChatAs = (!State.SafeMode && State.activeChatSpoof && State.playerToChatAs.has_value()) ? State.playerToChatAs.validate().get_PlayerControl() : *Game::pLocalPlayer;

		if (State.activeWhisper && State.playerToWhisper.has_value()) {
			MessageWriter* writer = InnerNetClient_StartRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient),
				playerToChatAs->fields._.NetId, uint8_t(RpcCalls__Enum::SendChat), SendOption__Enum::Reliable,
				State.playerToWhisper.get_PlayerControl().value_or(nullptr)->fields._.OwnerId, NULL);
			std::string whisperMsg = std::format("{} whispers to you:\n{}",
				RemoveHtmlTags(convert_from_string(NetworkedPlayerInfo_get_PlayerName(GetPlayerData(*Game::pLocalPlayer), NULL))),
				chatText);
			if (whisperMsg.length() <= 100 || !State.SafeMode)
				MessageWriter_WriteString(writer, convert_to_string(whisperMsg), NULL);
			else MessageWriter_WriteString(writer, convert_to_string(chatText), NULL);
			InnerNetClient_FinishRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), writer, NULL);

			std::string whisperMsgSelf = std::format("You whisper to {}:\n{}",
				RemoveHtmlTags(convert_from_string(NetworkedPlayerInfo_get_PlayerName(State.playerToWhisper.get_PlayerData().value_or(nullptr), NULL))),
				chatText);
			dChatController_AddChat(Game::HudManager.GetInstance()->fields.Chat, playerToChatAs, convert_to_string(whisperMsgSelf), false, NULL);
		}
		else if (!State.SafeMode && State.activeChatSpoof && State.playerToChatAs.has_value()) {
			auto writer = InnerNetClient_StartRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), GetPlayerControlById(State.playerToChatAs.get_PlayerId())->fields._.NetId,
				uint8_t(RpcCalls__Enum::SendChat), SendOption__Enum::Reliable, -1, NULL);
			MessageWriter_WriteString(writer, convert_to_string(chatText), NULL);
			InnerNetClient_FinishRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), writer, NULL);
			dChatController_AddChat(Game::HudManager.GetInstance()->fields.Chat, GetPlayerControlById(State.playerToChatAs.get_PlayerId()), convert_to_string(chatText), false, NULL);
		}
		else {
			auto writer = InnerNetClient_StartRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), (*Game::pLocalPlayer)->fields._.NetId,
				uint8_t(RpcCalls__Enum::SendChat), SendOption__Enum::Reliable, -1, NULL);
			MessageWriter_WriteString(writer, convert_to_string(chatText), NULL);
			InnerNetClient_FinishRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), writer, NULL);
			dChatController_AddChat(Game::HudManager.GetInstance()->fields.Chat, *Game::pLocalPlayer, convert_to_string(chatText), false, NULL);
		}
	}
	else {
		if (State.CurrentChatMode == QuickChatModes__Enum::QuickChatOnly) return;
		PlayerControl_RpcSendChat(*Game::pLocalPlayer, convert_to_string(chatText), NULL);
	}
}

void dChatNotification_SetUp(ChatNotification* __this, PlayerControl* sender, String* text, MethodInfo* method) {
	if (!State.PanicMode && State.BetterChatNotifications) {
		ShowChatNotification(__this, sender, text);
		return;
	}
	ChatNotification_SetUp(__this, sender, text, method);
	ChangeChatNotificationBackground(__this, sender);
}

void dFreeChatInputField_UpdateCharCount(FreeChatInputField* __this, MethodInfo* method) {
	if (State.ShowHookLogs) Log.HookDebug("Hook dFreeChatInputField_UpdateCharCount executed", false);
	FreeChatInputField_UpdateCharCount(__this, method);
	__this->fields.charCountText->fields._.m_enableWordWrapping = false;

	updateCharCounterText(__this);
}

void dObjectPoolBehavior_InitPool(ObjectPoolBehavior* __this, PoolableBehavior* prefab, MethodInfo* method) {
	ObjectPoolBehavior_InitPool(__this, prefab, method);
}

AudioSource* dSoundManager_PlaySound(SoundManager* __this, AudioClip* clip, bool loop, float volume, AudioMixerGroup* audioMixer, MethodInfo* method) {
	if (State.ShowHookLogs) Log.HookDebug("Hook dSoundManager_PlaySound executed", false);
	if (State.BetterMessageSounds && State.MessageSound != NULL && clip == State.MessageSound) volume = 0.f;
	return SoundManager_PlaySound(__this, clip, loop, volume, audioMixer, method);
}

void dChatController_Toggle(ChatController* __this, MethodInfo* method) {
	auto hud = Game::HudManager.GetInstance();
	auto chatState = hud->fields.Chat->fields.state;
	bool isOpenOrOpening = chatState == ChatControllerState__Enum::Open ||
		chatState == ChatControllerState__Enum::Opening;

	if (State.FollowerCam != NULL && Camera_get_orthographicSize(State.FollowerCam, NULL) != 3.f &&
		!isOpenOrOpening) {
		Camera_set_orthographicSize(State.FollowerCam, 3.f, NULL);
		Camera_set_orthographicSize(Game::HudManager.GetInstance()->fields.UICamera, 3.f, NULL);
		State.HasRefreshedUI = false;
	}

	if (isOpenOrOpening) {
		auto hudGameObject = Component_get_gameObject((Component_1*)Game::HudManager.GetInstance(), NULL);
		auto fullScreen = hud->fields.FullScreen;
		Color fullScreenCol = fullScreen != NULL ? SpriteRenderer_get_color(fullScreen, NULL) : Color(1.f, 1.f, 1.f, 0.f);
		bool isFullScreenActive = fullScreen != NULL &&
			fullScreenCol.r == 0.f && fullScreenCol.g == 0.f && fullScreenCol.b == 0.f &&
			GameObject_GetActive(Component_get_gameObject((Component_1*)fullScreen, NULL), NULL);
		bool shouldEnableZoom = (!State.InMeeting && !State.InExileUI && !isFullScreenActive && (State.GameLoaded || IsInLobby()) && !State.PanicMode);

		if (hudGameObject != NULL && isOpenOrOpening && shouldEnableZoom) {
			GameObject_SetActive(hudGameObject, false, NULL);
			GameObject_SetActive(hudGameObject, true, NULL);
			// restore the HUD after closing the chat UI
		}
	}
	ChatController_Toggle(__this, method);
}