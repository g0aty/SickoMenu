#include "RenderCmd.hpp"

#include <limits>

namespace
{
    bool AlignOffset(size_t offset, size_t alignment, size_t& alignedOffset)
    {
        if (alignment == 0 || (alignment & (alignment - 1)) != 0)
            return false;

        const size_t mask = alignment - 1;
        if (offset > (std::numeric_limits<size_t>::max)() - mask)
            return false;

        alignedOffset = (offset + mask) & ~mask;
        return true;
    }
}

RenderCmdQueue::RenderCmdQueue()
{
    m_CmdBuffer = new byte[CmdBufferCapacity];
}

RenderCmdQueue::~RenderCmdQueue()
{
    for (auto& command : m_Commands)
    {
        if (command.destroy != nullptr && command.storage != nullptr)
            command.destroy(command.storage);
    }

    delete[] m_CmdBuffer;
}

void* RenderCmdQueue::Allocate(uint32_t size, size_t alignment)
{
    if (alignment == 0 ||
        alignment > alignof(std::max_align_t) ||
        (alignment & (alignment - 1)) != 0)
    {
        return nullptr;
    }

    size_t payloadOffset = 0;
    if (!AlignOffset(m_CmdBufferOffset, alignment, payloadOffset) ||
        payloadOffset > CmdBufferCapacity ||
        static_cast<size_t>(size) > CmdBufferCapacity - payloadOffset)
    {
        return nullptr;
    }

    m_CmdBufferOffset = payloadOffset + size;
    return m_CmdBuffer + payloadOffset;
}

bool RenderCmdQueue::Enqueue(RenderCmdFunc execute, RenderCmdFunc destroy, void* storage)
{
    if (execute == nullptr || destroy == nullptr || storage == nullptr)
        return false;

    try
    {
        m_Commands.push_back({ execute, destroy, storage });
        return true;
    }
    catch (...)
    {
        return false;
    }
}

void RenderCmdQueue::Execute()
{
    size_t commandIndex = 0;

    try
    {
        for (; commandIndex < m_Commands.size(); ++commandIndex)
        {
            auto& command = m_Commands[commandIndex];
            command.execute(command.storage);
            command.destroy(command.storage);
        }
    }
    catch (...)
    {
        // Destroy the throwing command and all commands that can no longer execute.
        for (size_t i = commandIndex; i < m_Commands.size(); ++i)
        {
            auto& command = m_Commands[i];
            if (command.destroy != nullptr && command.storage != nullptr)
                command.destroy(command.storage);
        }

        m_Commands.clear();
        m_CmdBufferOffset = 0;
        throw;
    }

    m_Commands.clear();
    m_CmdBufferOffset = 0;
}