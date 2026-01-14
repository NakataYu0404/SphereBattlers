#include "../Utility/AsoUtility.h"
#include "../Application.h"
#include "PSMaterial.h"

PSMaterial::PSMaterial(std::string PSPath, int bufVolume)
{
	//	シェーダ読み込み
	shaderH_ = LoadPixelShader((Application::PATH_SHADER + PSPath).c_str());
	//	バッファ定義
	constBufH_ = CreateShaderConstantBuffer(sizeof(FLOAT4) * bufVolume);
}

PSMaterial::~PSMaterial(void)
{
}

void PSMaterial::AddConstBuffer(float buf1, float buf2, float buf3, float buf4)
{
	FLOAT4 buf;
	buf.x = buf1;
	buf.y = buf2;
	buf.z = buf3;
	buf.w = buf4;
	constBufs_.push_back(buf);
}

void PSMaterial::AddConstBuffer(FLOAT4 buf)
{
	constBufs_.push_back(buf);
}

void PSMaterial::ChangeConstBuffer(int registerNo, float buf1, float buf2, float buf3, float buf4)
{
	FLOAT4 buf;
	buf.x = buf1;
	buf.y = buf2;
	buf.z = buf3;
	buf.w = buf4;
	constBufs_[registerNo] = buf;

}

void PSMaterial::ChangeConstBuffer(int registerNo, FLOAT4 buf)
{
	constBufs_[registerNo] = buf;
}

int PSMaterial::GetConstBuffer(void)
{
	return constBufH_;
}

std::vector<FLOAT4>& PSMaterial::GetConstBuffers(void)
{
	return constBufs_;
}

std::vector<int>& PSMaterial::GetTextureImgs(void)
{
	return textureImgs_;
}

int PSMaterial::GetShaderH(void)
{
	return shaderH_;
}

void PSMaterial::AddTexture(int texH)
{
	textureImgs_.push_back(texH);
}

void PSMaterial::Clear(void)
{
	textureImgs_.clear();
	constBufs_.clear();
}

//	制作者：中田
//	他プロジェクトに持ち出し禁止！
