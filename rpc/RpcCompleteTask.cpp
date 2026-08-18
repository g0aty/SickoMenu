#include "pch-il2cpp.h"
#include "_rpc.h"
#include "game.h"
#include "state.hpp"

RpcCompleteTask::RpcCompleteTask(uint32_t taskId)
{
	this->taskId = taskId;
}

void RpcCompleteTask::Process()
{
	PlayerControl_RpcCompleteTask(*Game::pLocalPlayer, taskId, NULL);
}

RpcDrainHideTimer::RpcDrainHideTimer(float timeToSubtract)
{
	this->timeToSubtract = timeToSubtract;
}

void RpcDrainHideTimer::Process()
{
	auto gameMgr = GameManager_get_Instance(NULL);
	auto gameFlowHns = (LogicGameFlowHnS*)gameMgr->fields._LogicFlow_k__BackingField;

	if (gameFlowHns == NULL) return;

	LogicGameFlowHnS_AdjustEscapeTimer(gameFlowHns, timeToSubtract, true, NULL);
}

RpcForceCompleteTask::RpcForceCompleteTask(PlayerControl* Player, uint32_t taskId)
{
	this->Player = Player;
	this->taskId = taskId;
}

void RpcForceCompleteTask::Process()
{
	if (Player == nullptr) return;
	
	auto tasks = GetNormalPlayerTasks(Player);

	for (auto task : tasks) {
		if (task->fields._._Id_k__BackingField == taskId && !NormalPlayerTask_get_IsComplete(task, NULL)) {
			PlayerControl_RpcCompleteTask(Player, task->fields._._Id_k__BackingField, NULL);
		}
	}
}