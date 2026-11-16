#include "pch-il2cpp.h"
#include "_rpc.h"
#include "game.h"

static PlayerControl* GetLocalPlayerForCosmeticRpc() {
	if (Game::pLocalPlayer == nullptr) return nullptr;
	return *Game::pLocalPlayer;
}

RpcSetPet::RpcSetPet(String* petId)
{
	this->PetId = petId ? petId : convert_to_string("0");
}

void RpcSetPet::Process()
{
	PlayerControl* localPlayer = GetLocalPlayerForCosmeticRpc();
	if (localPlayer == nullptr) return;
	PlayerControl_RpcSetPet(localPlayer, this->PetId, NULL);
}

RpcForcePet::RpcForcePet(PlayerControl* Player, String* petId)
{
	this->Player = Player;
	this->PetId = petId ? petId : convert_to_string("0");
}

void RpcForcePet::Process()
{
	if (!PlayerSelection(Player).has_value()) return;
	
	PlayerControl_RpcSetPet(Player, this->PetId, NULL);
}

RpcSetSkin::RpcSetSkin(String* skinId)
{
	this->SkinId = skinId ? skinId : convert_to_string("0");
}

void RpcSetSkin::Process()
{
	PlayerControl* localPlayer = GetLocalPlayerForCosmeticRpc();
	if (localPlayer == nullptr) return;
	PlayerControl_RpcSetSkin(localPlayer, this->SkinId, NULL);
}

RpcForceSkin::RpcForceSkin(PlayerControl* Player, String* skinId)
{
	this->Player = Player;
	this->SkinId = skinId ? skinId : convert_to_string("0");
}

void RpcForceSkin::Process()
{
	if (!PlayerSelection(Player).has_value()) return;

	PlayerControl_RpcSetSkin(Player, this->SkinId, NULL);
}

RpcSetHat::RpcSetHat(String* hatId)
{
	this->HatId = hatId ? hatId : convert_to_string("0");
}

void RpcSetHat::Process()
{
	PlayerControl* localPlayer = GetLocalPlayerForCosmeticRpc();
	if (localPlayer == nullptr) return;
	PlayerControl_RpcSetHat(localPlayer, this->HatId, NULL);
}

RpcForceHat::RpcForceHat(PlayerControl* Player, String* hatId)
{
	this->Player = Player;
	this->HatId = hatId ? hatId : convert_to_string("0");
}

void RpcForceHat::Process()
{
	if (!PlayerSelection(Player).has_value()) return;

	PlayerControl_RpcSetHat(Player, this->HatId, NULL);
}

RpcSetVisor::RpcSetVisor(String* visorId)
{
	this->VisorId = visorId ? visorId : convert_to_string("0");
}

void RpcSetVisor::Process()
{
	PlayerControl* localPlayer = GetLocalPlayerForCosmeticRpc();
	if (localPlayer == nullptr) return;
	PlayerControl_RpcSetVisor(localPlayer, this->VisorId, NULL);
}

RpcForceVisor::RpcForceVisor(PlayerControl* Player, String* visorId)
{
	this->Player = Player;
	this->VisorId = visorId ? visorId : convert_to_string("0");
}

void RpcForceVisor::Process()
{
	if (!PlayerSelection(Player).has_value()) return;

	PlayerControl_RpcSetVisor(Player, this->VisorId, NULL);
}

RpcSetNamePlate::RpcSetNamePlate(String* namePlateId)
{
	this->NamePlateId = namePlateId ? namePlateId : convert_to_string("0");
}

void RpcSetNamePlate::Process()
{
	PlayerControl* localPlayer = GetLocalPlayerForCosmeticRpc();
	if (localPlayer == nullptr) return;
	PlayerControl_RpcSetNamePlate(localPlayer, this->NamePlateId, NULL);
}

RpcForceNamePlate::RpcForceNamePlate(PlayerControl* Player, String* namePlateId)
{
	this->Player = Player;
	this->NamePlateId = namePlateId ? namePlateId : convert_to_string("0");
}

void RpcForceNamePlate::Process()
{
	if (!PlayerSelection(Player).has_value()) return;

	PlayerControl_RpcSetNamePlate(Player, this->NamePlateId, NULL);
}