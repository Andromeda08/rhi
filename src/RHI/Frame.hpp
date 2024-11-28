#pragma once

#include <cstdint>
#include <initializer_list>
#include <vector>

#include "RHICommandList.hpp"

struct Frame
{
    const uint32_t               mCurrentFrame;
    const uint32_t               mAcquiredFrameIndex;
    std::vector<RHICommandList*> mCommandLists;

    uint32_t getCurrentFrame() const { return mCurrentFrame; }

    uint32_t getAcquiredFrameIndex() const { return mAcquiredFrameIndex; }

    Frame& addCommandLists(const std::initializer_list<RHICommandList*> commandLists)
    {
        mCommandLists.insert(std::end(mCommandLists), std::begin(commandLists), std::end(commandLists));
        return *this;
    }
};