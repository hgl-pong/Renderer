#pragma once
#include "Common/pch.h"
#include "Engine/RenderSystemInterface.h"
#include <d3d11.h>

class DX11RenderBuffer : virtual public IRenderBuffer, public RenderSystemResource
{
public:
    virtual ~DX11RenderBuffer();
    virtual bool Init() = 0;

protected:
    UniquePtr<ID3D11_Buffer> m_Buffer;
};

template <typename DataBlock>
class DX11VertexBuffer : public DX11RenderBuffer
{
public:
    virtual ~DX11VertexBuffer() override
    {
        DX11RenderBuffer::~DX11RenderBuffer();
    }
    void Init() override
    {
        // Create buffer
        D3D11_BUFFER_DESC bufferDesc = {};
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.ByteWidth = sizeof(DataBlock) * m_Data.size();
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bufferDesc.CPUAccessFlags = 0;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA vertexData = {};
        vertexData.pSysMem = m_Data.data();
        vertexData.SysMemPitch = 0;
        vertexData.SysMemSlicePitch = 0;

        // CALL_DX11_RENDER_SYSTEM(CreateBuffer, &bufferDesc, &vertexData, &m_Buffer);
    };

private:
    size_t m_DataSize;
    std::vector<DataBlock> m_Data;
    D3D11_INPUT_ELEMENT_DESC m_InputElementDesc;
};

template <typename DataBlock>
