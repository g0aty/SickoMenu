#pragma once

#include "RenderCmd.hpp"

#include <utility>
#include <assert.h>

class ImGuiRenderer
{
public:
	using RenderCommandFunc = void(*)(void*);

	template<typename Function>
	static void Submit(Function&& function)
	{
		auto renderCmd = [](void* ptr) {
			(*static_cast<Function*>(ptr))();
		};
		auto destroyCmd = [](void* ptr) {
			static_cast<Function*>(ptr)->~Function();
		};

		auto& commandQueue = GetCmdQueue();
		auto storageBuffer = commandQueue.Allocate(sizeof(function), alignof(Function));
		if (storageBuffer == nullptr)
			return;

		new (storageBuffer) Function(std::forward<Function>(function));
		if (!commandQueue.Enqueue(renderCmd, destroyCmd, storageBuffer))
			destroyCmd(storageBuffer);
	}

	static void ExecuteQueue();
private:
	static RenderCmdQueue& GetCmdQueue();
};