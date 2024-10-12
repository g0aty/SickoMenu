#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"
#include "game.h"
#include "logger.h"
#include <chrono>

static app::Type* voteSpreaderType = nullptr;

void dMeetingHud_Awake(MeetingHud* __this, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dMeetingHud_Awake executed");
	try {
		State.voteMonitor.clear();
		State.InMeeting = true;

		static std::string strVoteSpreaderType = translate_type_name("VoteSpreader, Assembly-CSharp");
		voteSpreaderType = app::Type_GetType(convert_to_string(strVoteSpreaderType), nullptr);
		if (State.confuser && State.confuseOnMeeting && !State.PanicMode)
			ControlAppearance(true);
	}
	catch (...) {
		LOG_ERROR("Exception occurred in MeetingHud_Awake (MeetingHud)");
	}
	MeetingHud_Awake(__this, method);
}

void dMeetingHud_Close(MeetingHud* __this, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dMeetingHud_Close executed");
	try {
		State.InMeeting = false;

		if (State.Replay_ClearAfterMeeting)
		{
			Replay::Reset(false);
		}
	}
	catch (...) {
		LOG_ERROR("Exception occurred in MeetingHud_Close (MeetingHud)");
	}
	MeetingHud_Close(__this, method);
}

static void Transform_RemoveVotes(app::Transform* transform, size_t count) {
	auto voteSpreader = (VoteSpreader*)app::Component_GetComponent((app::Component_1*)transform, voteSpreaderType, nullptr);
	if (!voteSpreader) return;
	il2cpp::List votes(voteSpreader->fields.Votes);
	const auto length = votes.size();
	if (length == 0) return;
	if (count >= length) {
		for (auto spriteRenderer : votes) {
			app::Object_DestroyImmediate((app::Object_1*)spriteRenderer, nullptr);
		}
		votes.clear();
		return;
	}
	for (size_t pos = length - 1; pos >= length - count; pos--) {
		app::Object_DestroyImmediate((app::Object_1*)votes[pos], nullptr);
		votes.erase(pos);
	}
}

static void Transform_RemoveAllVotes(app::Transform* transform) {
	Transform_RemoveVotes(transform, SIZE_MAX);
}

static void Transform_RevealAnonymousVotes(app::Transform* transform, Game::VotedFor votedFor) {
	if (!transform) return;
	auto voteSpreader = (VoteSpreader*)app::Component_GetComponent((app::Component_1*)transform, voteSpreaderType, nullptr);
	if (!voteSpreader) return;
	auto votes = il2cpp::List(voteSpreader->fields.Votes);
	if (State.RevealAnonymousVotes) {
		size_t idx = 0;
		for (auto& pair : State.voteMonitor) {
			if (pair.second == votedFor) {
				if (idx >= votes.size()) {
					STREAM_ERROR("votedFor " << ToString(votedFor) << ", index " << idx << ", expected less than " << votes.size());
					break;
				}
				auto outfit = GetPlayerOutfit(GetPlayerDataById(pair.first));
				if (!outfit)
					continue;
				auto ColorId = outfit->fields.ColorId;
				auto spriteRenderer = votes[idx++];
				app::PlayerMaterial_SetColors(ColorId, (app::Renderer*)spriteRenderer, nullptr);
			}
		}
	}
	else {
		for (auto spriteRenderer : votes) {
			app::PlayerMaterial_SetColors_1(
				app::Palette__TypeInfo->static_fields->DisabledGrey,
				(app::Renderer*)spriteRenderer, nullptr);
		}
	}
}

void dMeetingHud_PopulateResults(MeetingHud* __this, Il2CppArraySize* states, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dMeetingHud_PopulateResults executed");
	try {// remove all votes before populating results
		for (auto votedForArea : il2cpp::Array(__this->fields.playerStates)) {
			if (!votedForArea) {
				// oops: game bug
				continue;
			}
			auto transform = app::Component_get_transform((app::Component_1*)votedForArea, nullptr);
			Transform_RemoveAllVotes(transform);
		}
		if (__this->fields.SkippedVoting) {
			auto transform = app::GameObject_get_transform(__this->fields.SkippedVoting, nullptr);
			Transform_RemoveAllVotes(transform);
		}

		if (auto exiled = __this->fields.exiledPlayer; exiled != nullptr) {
			synchronized(Replay::replayEventMutex) {
				State.replayDeathTimePerPlayer[exiled->fields.PlayerId] = std::chrono::system_clock::now();
			}
		}

		GameOptions options;
		const auto prevAnonymousVotes = options.GetBool(app::BoolOptionNames__Enum::AnonymousVotes);
		if (prevAnonymousVotes && State.RevealAnonymousVotes)
			options.SetBool(app::BoolOptionNames__Enum::AnonymousVotes, false);
		options.SetBool(app::BoolOptionNames__Enum::AnonymousVotes, prevAnonymousVotes);
	}
	catch (...) {
		LOG_ERROR("Exception occurred in MeetingHud_PopulateResults (MeetingHud)");
	}
	MeetingHud_PopulateResults(__this, states, method);
}

void RevealAnonymousVotes() {
	if (State.PanicMode || (!State.InMeeting
		|| !app::MeetingHud__TypeInfo
		|| !app::MeetingHud__TypeInfo->static_fields->Instance
		|| !GameOptions().GetBool(app::BoolOptionNames__Enum::AnonymousVotes)))
		return;
	auto meetingHud = app::MeetingHud__TypeInfo->static_fields->Instance;
	for (auto votedForArea : il2cpp::Array(meetingHud->fields.playerStates)) {
		if (!votedForArea) continue;
		auto transform = app::Component_get_transform((app::Component_1*)votedForArea, nullptr);
		Transform_RevealAnonymousVotes(transform, votedForArea->fields.TargetPlayerId);
	}
	if (meetingHud->fields.SkippedVoting) {
		auto transform = app::GameObject_get_transform(meetingHud->fields.SkippedVoting, nullptr);
		Transform_RevealAnonymousVotes(transform, Game::SkippedVote);
	}
}

void dMeetingHud_Update(MeetingHud* __this, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dMeetingHud_Update executed");
	try {
		if (!State.PanicMode) {
			const bool isBeforeResultsState = __this->fields.state < app::MeetingHud_VoteStates__Enum::Results;
			il2cpp::Array playerStates(__this->fields.playerStates);
			for (auto playerVoteArea : playerStates) {
				if (!playerVoteArea) {
					// oops: game bug
					continue;
				}
				auto playerData = GetPlayerDataById(playerVoteArea->fields.TargetPlayerId);
				auto localData = GetPlayerData(*Game::pLocalPlayer);
				auto playerControl = GetPlayerControlById(playerVoteArea->fields.TargetPlayerId);
				auto playerNameTMP = playerVoteArea->fields.NameText;
				auto outfit = GetPlayerOutfit(playerData);
				std::string playerName = convert_from_string(outfit->fields.PlayerName);
				if (playerData == GetPlayerData(*Game::pLocalPlayer) && State.CustomName && (!State.ServerSideCustomName || State.ServerSideCustomName && (!IsHost() || State.SafeMode)) && !State.userName.empty()) {
					if (State.CustomName && !State.ServerSideCustomName) {
						if (State.ColoredName && !State.RgbName) {
							playerName = GetGradientUsername(playerName);
						}
						//we don't want a big name hiding everything in the meeting
						/*if (State.ResizeName)
							playerName = std::format("<size={}>", State.NameSize) + playerName + "</size>";*/
						if (State.ItalicName)
							playerName = "<i>" + playerName + "</i>";
						if (State.UnderlineName && (!State.ColoredName || State.RgbName))
							playerName = "<u>" + playerName + "</u>";
						if (State.StrikethroughName && (!State.ColoredName || State.RgbName))
							playerName = "<s>" + playerName + "</s>";
						if (State.BoldName && (!State.ColoredName || State.RgbName))
							playerName = "<b>" + playerName + "</b>";
						if (State.NobrName && (!State.ColoredName || State.RgbName))
							playerName = "<nobr>" + playerName + "</nobr>";
						if (State.RgbName) {
							playerName = State.rgbCode + playerName + "</color>";
						}
					}
				}

				if (playerData && localData && outfit) {
					if (State.PlayerColoredDots)
					{
						Color32&& nameColor = GetPlayerColor(outfit->fields.ColorId);
						std::string dot = std::format("<#{:02x}{:02x}{:02x}{:02x}> ●</color>",
							nameColor.r, nameColor.g, nameColor.b,
							nameColor.a);

						playerName = "<#0000>● </color>" + playerName + dot;
					}
					if (State.RevealRoles)
					{
						std::string roleName = GetRoleName(playerData->fields.Role, State.AbbreviatedRoleNames);
						if (!playerData->fields.Disconnected) {
							int completedTasks = 0;
							int totalTasks = 0;
							auto tasks = GetNormalPlayerTasks(playerControl);
							for (auto task : tasks)
							{
								if (task == nullptr) continue;
								if (task->fields.taskStep == task->fields.MaxStep) {
									completedTasks++;
									totalTasks++;
								}
								else
									totalTasks++;
							}
							std::string tasksText = std::format("({}/{})", completedTasks, totalTasks);
							if (totalTasks == 0 || (PlayerIsImpostor(playerData) && completedTasks == 0))
								playerName = "<size=1.2>" + roleName + "\n</size>" + playerName + "\n<size=1.2><#0000>0";
							else
								playerName = "<size=1.2>" + roleName + " " + tasksText + "\n</size>" + playerName + "\n<size=1.2><#0000>0";
						}
						else
							playerName = "<size=1.2>" + roleName + " (D/C)\n</size>" + playerName + "\n<size=1.2><#0000>0";
						Color32&& roleColor = app::Color32_op_Implicit(GetRoleColor(playerData->fields.Role), NULL);

						playerName = std::format("<color=#{:02x}{:02x}{:02x}{:02x}>{}",
							roleColor.r, roleColor.g, roleColor.b,
							roleColor.a, playerName);
					}

					String* playerNameStr = convert_to_string(playerName);
					TMP_Text_set_text((app::TMP_Text*)playerNameTMP, playerNameStr, NULL);
				}

				if (playerData)
				{
					bool didVote = (playerVoteArea->fields.VotedFor != Game::HasNotVoted);
					// We are goign to check to see if they voted, then we are going to check to see who they voted for, finally we are going to check to see if we already recorded a vote for them
					// votedFor will either contain the id of the person they voted for, 254 if they missed, or 255 if they didn't vote. We don't want to record people who didn't vote
					if (didVote && playerVoteArea->fields.VotedFor != Game::MissedVote
						&& playerVoteArea->fields.VotedFor != Game::DeadVote
						&& State.voteMonitor.find(playerData->fields.PlayerId) == State.voteMonitor.end())
					{
						synchronized(Replay::replayEventMutex) {
							State.liveReplayEvents.emplace_back(std::make_unique<CastVoteEvent>(GetEventPlayer(playerData).value(), GetEventPlayer(GetPlayerDataById(playerVoteArea->fields.VotedFor))));
						}
						State.voteMonitor[playerData->fields.PlayerId] = playerVoteArea->fields.VotedFor;
						STREAM_DEBUG(ToString(playerData) << " voted for " << ToString(playerVoteArea->fields.VotedFor));

						// avoid duplicate votes
						GameOptions options;
						const auto prevAnonymousVotes = options.GetBool(app::BoolOptionNames__Enum::AnonymousVotes);
						if (prevAnonymousVotes && State.RevealAnonymousVotes)
							options.SetBool(app::BoolOptionNames__Enum::AnonymousVotes, false);

						if (isBeforeResultsState) {
							if (playerVoteArea->fields.VotedFor != Game::SkippedVote) {
								for (auto votedForArea : playerStates) {
									if (votedForArea->fields.TargetPlayerId == playerVoteArea->fields.VotedFor) {
										auto transform = app::Component_get_transform((app::Component_1*)votedForArea, nullptr);
										MeetingHud_BloopAVoteIcon(__this, playerData, 0, transform, nullptr);
										break;
									}
								}
							}
							else if (__this->fields.SkippedVoting) {
								auto transform = app::GameObject_get_transform(__this->fields.SkippedVoting, nullptr);
								MeetingHud_BloopAVoteIcon(__this, playerData, 0, transform, nullptr);
							}
							options.SetBool(app::BoolOptionNames__Enum::AnonymousVotes, prevAnonymousVotes);
						}
					}
					else if (!didVote && State.voteMonitor.find(playerData->fields.PlayerId) != State.voteMonitor.end())
					{
						auto it = State.voteMonitor.find(playerData->fields.PlayerId);
						auto dcPlayer = it->second;
						State.voteMonitor.erase(it); //Likely disconnected player

						// Remove all votes for disconnected player 
						for (auto votedForArea : playerStates) {
							if (votedForArea->fields.TargetPlayerId == dcPlayer) {
								auto transform = app::Component_get_transform((app::Component_1*)votedForArea, nullptr);
								Transform_RemoveVotes(transform, 1); // remove a vote
								break;
							}
						}
					}
				}
			}

			if (isBeforeResultsState) {
				for (auto votedForArea : playerStates) {
					if (!votedForArea) {
						// oops: game bug
						continue;
					}
					auto transform = app::Component_get_transform((app::Component_1*)votedForArea, nullptr);
					auto voteSpreader = (VoteSpreader*)app::Component_GetComponent((app::Component_1*)transform, voteSpreaderType, nullptr);
					if (!voteSpreader) continue;
					for (auto spriteRenderer : il2cpp::List(voteSpreader->fields.Votes)) {
						auto gameObject = app::Component_get_gameObject((app::Component_1*)spriteRenderer, nullptr);
						app::GameObject_SetActive(gameObject, State.RevealVotes, nullptr);
					}
				}

				if (__this->fields.SkippedVoting) {
					bool showSkipped = false;
					for (const auto& pair : State.voteMonitor) {
						if (pair.second == Game::SkippedVote) {
							showSkipped = State.RevealVotes;
							break;
						}
					}
					app::GameObject_SetActive(__this->fields.SkippedVoting, showSkipped, nullptr);
				}
			}
		}
	}
	catch (...) {
		LOG_ERROR("Exception occurred in MeetingHud_Update (MeetingHud)");
	}
	app::MeetingHud_Update(__this, method);
}