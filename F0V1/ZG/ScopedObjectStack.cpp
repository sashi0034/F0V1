#include "pch.h"
#include "ScopedObjectStack.h"

#include <assert.h>

#include "EngineCore.h"

namespace
{
    struct Impl
    {
        std::vector<ID3D12PipelineState*> m_pipelineStateStack{};
        std::vector<ID3D12RootSignature*> m_rootSignatureStack{};
    } s_impl;
}

namespace ZG
{
    void ScopedObjectStack_impl::PushPipelineState(ID3D12PipelineState* pipelineState) const
    {
        assert(pipelineState);
        s_impl.m_pipelineStateStack.push_back(pipelineState);

        const auto commandList = EngineCore.GetCommandList();
        commandList->SetPipelineState(pipelineState);
    }

    void ScopedObjectStack_impl::PopPipelineState() const
    {
        assert(not s_impl.m_pipelineStateStack.empty());
        s_impl.m_pipelineStateStack.pop_back();

        if (not s_impl.m_pipelineStateStack.empty())
        {
            const auto commandList = EngineCore.GetCommandList();
            commandList->SetPipelineState(s_impl.m_pipelineStateStack.back());
        }
    }

    void ScopedObjectStack_impl::PushRootSignature(ID3D12RootSignature* rootSignature) const
    {
        assert(rootSignature);
        s_impl.m_rootSignatureStack.push_back(rootSignature);

        const auto commandList = EngineCore.GetCommandList();
        commandList->SetGraphicsRootSignature(rootSignature);
    }

    void ScopedObjectStack_impl::PopRootSignature() const
    {
        assert(not s_impl.m_rootSignatureStack.empty());
        s_impl.m_rootSignatureStack.pop_back();

        if (not s_impl.m_rootSignatureStack.empty())
        {
            const auto commandList = EngineCore.GetCommandList();
            commandList->SetGraphicsRootSignature(s_impl.m_rootSignatureStack.back());
        }
    }
}
