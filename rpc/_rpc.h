#pragma once
#include "utility.h"

void HandleRpc(PlayerControl* player, uint8_t callId, MessageReader* reader);
void SMAC_HandleRpc(PlayerControl* player, uint8_t callId, MessageReader* reader);

class RPCInterface {
public:
	virtual ~RPCInterface() {}
	virtual void Process() = 0;
};

class RpcCloseDoorsOfType : public RPCInterface {
	SystemTypes__Enum selectedSystem;
	bool pinDoor;
public:
	RpcCloseDoorsOfType(SystemTypes__Enum selectedSystem, bool pinDoor);
	virtual void Process() override;
};

class RpcOpenDoorsOfType : public RPCInterface {
	SystemTypes__Enum selectedSystem;
public:
	RpcOpenDoorsOfType(SystemTypes__Enum selectedSystem);
	virtual void Process() override;
};

class RpcSnapTo : public RPCInterface {
	Vector2 targetVector;
public:
	RpcSnapTo(Vector2 targetVector);
	virtual void Process() override;
};

class RpcForceSnapTo : public RPCInterface {
	PlayerControl* Player;
	Vector2 targetVector;
public:
	RpcForceSnapTo(PlayerControl* Player, Vector2 targetVector);
	virtual void Process() override;
};

class RpcUpdateSystem : public RPCInterface {
	SystemTypes__Enum selectedSystem;
	int32_t amount;
public:
	RpcUpdateSystem(SystemTypes__Enum selectedSystem, SystemTypes__Enum amount);
	RpcUpdateSystem(SystemTypes__Enum selectedSystem, uint32_t amount);
	virtual void Process() override;
};

class RpcRevive : public RPCInterface {
	PlayerControl* Player;
public:
	RpcRevive(PlayerControl* Player);
	virtual void Process() override;
};

class RpcVent : public RPCInterface {
	PlayerControl* Player;
	int32_t ventId;
	bool exit;
public:
	RpcVent(PlayerControl* Player, int32_t ventId, bool exit);
	virtual void Process() override;
};

class RpcBootAllVents : public RPCInterface {
public:
	RpcBootAllVents();
	virtual void Process() override;
};

class RpcSetLevel : public RPCInterface {
	PlayerControl* Player;
	int level;
public:
	RpcSetLevel(PlayerControl* Player, int level);
	virtual void Process() override;
};

class RpcEndGame : public RPCInterface {
	GameOverReason__Enum reason;
public:
	RpcEndGame(GameOverReason__Enum reason);
	virtual void Process() override;
};

class RpcProtectPlayer : public RPCInterface {
	PlayerControl* Player;
	PlayerSelection target;
	uint8_t color;
public:
	RpcProtectPlayer(PlayerControl* Player, PlayerSelection target, uint8_t color);
	virtual void Process() override;
};

class CmdCheckProtect : public RPCInterface {
	PlayerControl* Player;
	PlayerSelection target;
public:
	CmdCheckProtect(PlayerControl* Player, PlayerSelection target);
	virtual void Process() override;
};

class RpcCompleteTask : public RPCInterface {
	uint32_t taskId;
public:
	RpcCompleteTask(uint32_t taskId);
	virtual void Process() override;
};

class RpcForceCompleteTask : public RPCInterface {
	PlayerControl* Player;
	uint32_t taskId;
public:
	RpcForceCompleteTask(PlayerControl* Player, uint32_t taskId);
	virtual void Process() override;
};

class RpcPlayAnimation : public RPCInterface {
	uint8_t animId;
public:
	RpcPlayAnimation(uint8_t taskId);
	virtual void Process() override;
};

class RpcSetScanner : public RPCInterface {
	bool playAnimation;
public:
	RpcSetScanner(bool playAnimation);
	virtual void Process() override;
};

class RpcForceScanner : public RPCInterface {
	PlayerControl* Player;
	bool playAnimation;
public:
	RpcForceScanner(PlayerControl* Player, bool playAnimation);
	virtual void Process() override;
};

class RpcReportBody : public RPCInterface {
	PlayerSelection reportedPlayer;
public:
	RpcReportBody(const PlayerSelection& target);
	virtual void Process() override;
};

class RpcForceReportBody : public RPCInterface {
	PlayerControl* Player;
	PlayerSelection reportedPlayer;
public:
	RpcForceReportBody(PlayerControl* Player, const PlayerSelection& target);
	virtual void Process() override;
};

class RpcForceMeeting : public RPCInterface {
	PlayerControl* Player;
	PlayerSelection reportedPlayer;
public:
	RpcForceMeeting(PlayerControl* Player, const PlayerSelection& target);
	virtual void Process() override;
};

class RpcSpamMeeting : public RPCInterface {
	PlayerControl* Player;
	PlayerControl* target;
	bool inMeeting;
public:
	RpcSpamMeeting(PlayerControl* Player, PlayerControl* target, bool inMeeting);
	virtual void Process() override;
};

class RpcSpamChatNote : public RPCInterface {
	PlayerControl* exploitedPlayer;
public:
	RpcSpamChatNote(PlayerControl* exploitedPlayer);
	virtual void Process() override;
};

class RpcSendChatNote : public RPCInterface {
	PlayerControl* player;
	int32_t type;
public:
	RpcSendChatNote(PlayerControl* player, int32_t type);
	virtual void Process() override;
};

class CmdCheckMurder : public RPCInterface {
	PlayerSelection target;
public:
	CmdCheckMurder(const PlayerSelection& target);
	virtual void Process() override;
};

class RpcMurderPlayer : public RPCInterface {
	PlayerControl* Player;
	PlayerControl* target;
	bool success;
public:
	RpcMurderPlayer(PlayerControl* Player, PlayerControl* target, bool success = true);
	virtual void Process() override;
};

class RpcMurderLoop : public RPCInterface {
	PlayerControl* Player;
	PlayerControl* target;
	int count;
	bool onlyOnTarget;
public:
	RpcMurderLoop(PlayerControl* Player, PlayerControl* target, int count = 30, bool onlyOnTarget = true);
	virtual void Process() override;
};

class RpcExiled : public RPCInterface {
	PlayerControl* target;
	bool onlyOnTarget;
public:
	RpcExiled(PlayerControl* target, bool onlyOnTarget = true);
	virtual void Process() override;
};

class RpcShapeshift : public RPCInterface {
	PlayerControl* Player;
	PlayerSelection target;
	bool animate;
public:
	RpcShapeshift(PlayerControl* Player, const PlayerSelection& target, bool animate);
	virtual void Process() override;
};

class CmdCheckShapeshift : public RPCInterface {
	PlayerControl* Player;
	PlayerSelection target;
	bool animate;
public:
	CmdCheckShapeshift(PlayerControl* Player, const PlayerSelection& target, bool animate);
	virtual void Process() override;
};

class RpcVanish : public RPCInterface {
	PlayerControl* Player;
	bool appear;
public:
	RpcVanish(PlayerControl* Player, bool appear = false);
	virtual void Process() override;
};

class RpcSendChat : public RPCInterface {
	PlayerControl* Player;
	std::string msg;
public:
	RpcSendChat(PlayerControl* Player, std::string_view msg);
	virtual void Process() override;
};

class RpcVotePlayer : public RPCInterface {
	PlayerControl* Player;
	PlayerControl* target;
	bool skip;
public:
	RpcVotePlayer(PlayerControl* Player, PlayerControl* target, bool skip = false);
	virtual void Process() override;
};

class RpcVoteKick : public RPCInterface {
	PlayerControl* target;
	bool exploit;
public:
	RpcVoteKick(PlayerControl* target, bool exploit = false);
	virtual void Process() override;
};

class ReportPlayer : public RPCInterface {
	PlayerControl* target;
	ReportReasons__Enum reason;
public:
	ReportPlayer(PlayerControl* target, ReportReasons__Enum reason);
	virtual void Process() override;
};

class RpcClearVote : public RPCInterface {
	PlayerControl* Player;
public:
	RpcClearVote(PlayerControl* Player);
	virtual void Process() override;
};

class RpcEndMeeting : public RPCInterface {
public:
	RpcEndMeeting();
	virtual void Process() override;
};

class EndMeeting : public RPCInterface {
public:
	EndMeeting();
	virtual void Process() override;
};

class DestroyMap : public RPCInterface {
public:
	DestroyMap();
	virtual void Process() override;
};

class RpcSetColor : public RPCInterface {
	uint8_t bodyColor;
	bool forceColor; //Uses RpcSetColor, only can use as host
public:
	RpcSetColor(uint8_t colorId, bool force = false);
	virtual void Process() override;
};

class RpcForceColor : public RPCInterface {
	PlayerControl* Player;
	uint8_t bodyColor;
public:
	RpcForceColor(PlayerControl* player, uint8_t colorId);
	virtual void Process() override;
};

class RpcSetName : public RPCInterface {
	std::string name;
public:
	RpcSetName(std::string_view name);
	virtual void Process() override;
};

class RpcForceName : public RPCInterface {
	PlayerControl* Player;
	std::string name;
public:
	RpcForceName(PlayerControl* player, std::string_view name);
	virtual void Process() override;
};

class RpcUsePlatform : public RPCInterface {
public:
	RpcUsePlatform();
	virtual void Process() override;
};

class RpcSetPet : public RPCInterface {
	String* PetId;
public:
	RpcSetPet(String* petId);
	virtual void Process() override;
};

class RpcForcePet : public RPCInterface {
	PlayerControl* Player;
	String* PetId;
public:
	RpcForcePet(PlayerControl* Player, String* petId);
	virtual void Process() override;
};

class RpcSetSkin : public RPCInterface {
	String* SkinId;
public:
	RpcSetSkin(String* skinId);
	virtual void Process() override;
};

class RpcForceSkin : public RPCInterface {
	PlayerControl* Player;
	String* SkinId;
public:
	RpcForceSkin(PlayerControl* Player, String* skinId);
	virtual void Process() override;
};

class RpcSetHat : public RPCInterface {
	String* HatId;
public:
	RpcSetHat(String* hatId);
	virtual void Process() override;
};

class RpcForceHat : public RPCInterface {
	PlayerControl* Player;
	String* HatId;
public:
	RpcForceHat(PlayerControl* Player, String* hatId);
	virtual void Process() override;
};

class RpcSetVisor : public RPCInterface {
	String* VisorId;
public:
	RpcSetVisor(String* visorId);
	virtual void Process() override;
};

class RpcForceVisor : public RPCInterface {
	PlayerControl* Player;
	String* VisorId;
public:
	RpcForceVisor(PlayerControl* Player, String* visorId);
	virtual void Process() override;
};

class RpcSetNamePlate : public RPCInterface {
	String* NamePlateId;
public:
	RpcSetNamePlate(String* namePlateId);
	virtual void Process() override;
};

class RpcForceNamePlate : public RPCInterface {
	PlayerControl* Player;
	String* NamePlateId;
public:
	RpcForceNamePlate(PlayerControl* Player, String* namePlateId);
	virtual void Process() override;
};

class RpcSetRole : public RPCInterface {
	PlayerControl* Player;
	RoleTypes__Enum Role;
public:
	RpcSetRole(PlayerControl* player, RoleTypes__Enum role);
	virtual void Process() override;
};

class SetRole : public RPCInterface {
	RoleTypes__Enum Role;
public:
	SetRole(RoleTypes__Enum role);
	virtual void Process() override;
};

class RpcForceDetectAum : public RPCInterface {
	PlayerSelection target;
	bool completeForce;
public:
	RpcForceDetectAum(const PlayerSelection& target, bool completeForce = false);
	virtual void Process() override;
};

class RpcForceAumChat : public RPCInterface {
	PlayerSelection target;
	std::string msg;
	bool completeForce;
public:
	RpcForceAumChat(const PlayerSelection& target, std::string_view msg, bool completeForce = false);
	virtual void Process() override;
};

class RpcSyncSettings : public RPCInterface {
public:
	RpcSyncSettings();
	virtual void Process() override;
};

class RpcSpawnDummy : public RPCInterface {
	uint8_t colorId;
	std::string name;
public:
	RpcSpawnDummy(uint8_t colorId = -1, std::string_view name = "");
	virtual void Process() override;
};

class RpcBootFromVent : public RPCInterface {
	PlayerControl* Player;
	int ventId;
public:
	RpcBootFromVent(PlayerControl* Player, int ventId);
	virtual void Process() override;
};