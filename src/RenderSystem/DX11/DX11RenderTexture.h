#pragma once
#include "Common/pch.h"
#include "Engine/RenderSystemInterface.h"
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11UnorderedAccessView;
struct ID3D11Texture3D;

template<typename DataType>
class DX11RenderTexture : virtual public IRenderTexture, public RenderSystemResource
{
public:
	virtual bool Bind() = 0;
	virtual bool UnBind() = 0;
	virtual bool ExportData(std::vector<DataType> &data) = 0;
	virtual bool ImportData(const std::vector<DataType> &data) = 0;
};

