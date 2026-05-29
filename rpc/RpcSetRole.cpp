#include "pch-il2cpp.h"
#include "_rpc.h"
#include "game.h"
#include "utility.h"

RpcSetRole::RpcSetRole(PlayerControl* player, RoleTypes__Enum role)
{
    this->Player = player;
    this->Role = role;
}

void RpcSetRole::Process()
{
    if (Player == nullptr) return;
    if (!Game::RoleManager.IsInstanceExists()) return;

    //bool isDeadRole = Role == RoleTypes__Enum::CrewmateGhost || Role == RoleTypes__Enum::GuardianAngel || Role == RoleTypes__Enum::ImpostorGhost;

    PlayerControl_RpcSetRole(Player, Role, true, NULL);
}

SetRole::SetRole(RoleTypes__Enum role)
{
    this->Role = role;
}

void SetRole::Process()
{
    PlayerControl_CoSetRole(*Game::pLocalPlayer, Role, false, NULL);
    if (!Game::RoleManager.IsInstanceExists()) return;
    RoleManager_SetRole(Game::RoleManager.GetInstance(), *Game::pLocalPlayer, Role, NULL);
}