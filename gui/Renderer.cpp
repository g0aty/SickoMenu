#include "pch-il2cpp.h"
#include "il2cpp-helpers.h"
#include "Renderer.hpp"
#include "RenderCmd.hpp"
#include "DirectX.h"
#include "logger.h"

static RenderCmdQueue s_CmdQueue;

void ImGuiRenderer::ExecuteQueue()
{
	WaitForSingleObject(DirectX::hRenderSemaphore, INFINITE);
	try
	{
		ScopedThreadAttacher managedThreadAttached;
		s_CmdQueue.Execute();
	}
	catch (...)
	{
		LOG_ERROR("Exception occurred while executing the ImGui render queue");
	}
	ReleaseSemaphore(DirectX::hRenderSemaphore, 1, NULL);
}

RenderCmdQueue& ImGuiRenderer::GetCmdQueue()
{
	return s_CmdQueue;
}