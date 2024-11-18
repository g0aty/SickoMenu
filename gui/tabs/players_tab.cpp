				if (!State.SafeMode || IsHost() && IsInGame() && ImGui::Button("Shield Destruction")) {
					if (IsInGame()) {
						for (PlayerSelection p : selectedPlayers) {
							auto validPlayer = p.validate();

							{
								app::NetworkedPlayerInfo_PlayerOutfit* outfit = GetPlayerOutfit(validPlayer.get_PlayerData());
								auto colorId = outfit->fields.ColorId;
								if (IsInGame()) {
									State.rpcQueue.push(new RpcProtectPlayer(*Game::pLocalPlayer, validPlayer, colorId));
								}
								else if (IsInLobby()) {
									State.lobbyRpcQueue.push(new RpcProtectPlayer(*Game::pLocalPlayer, validPlayer, colorId));
								}
							}
							{
								bool canBeKilled = validPlayer.get_PlayerControl()->fields.protectedByGuardianId < 0 || State.BypassAngelProt;
								if (IsInGame()) {
									State.rpcQueue.push(new RpcMurderPlayer(*Game::pLocalPlayer, validPlayer.get_PlayerControl(), canBeKilled));
								}
								else if (IsInLobby()) {
									State.lobbyRpcQueue.push(new RpcMurderPlayer(*Game::pLocalPlayer, validPlayer.get_PlayerControl(), canBeKilled));
								}
							}
						}
					}
				}
