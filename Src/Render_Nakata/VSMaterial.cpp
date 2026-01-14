#include "../Utility/AsoUtility.h"
#include "../Application.h"
#include "PSMaterial.h"
#include "VSMaterial.h"

VSMaterial::VSMaterial(std::string VSPath, int bufVolume, int modelId)
{
	//	シェーダ読み込み
	shaderH_ = LoadVertexShader((Application::PATH_SHADER + VSPath).c_str());

	//	バッファ定義
	constBufH_ = CreateShaderConstantBuffer(sizeof(FLOAT4) * bufVolume);

	AddModelId(modelId);
}

VSMaterial::~VSMaterial(void)
{
}

void VSMaterial::AddConstBuffer(float buf1, float buf2, float buf3, float buf4)
{
	FLOAT4 buf;
	buf.x = buf1;
	buf.y = buf2;
	buf.z = buf3;
	buf.w = buf4;
	constBufs_.push_back(buf);
}

void VSMaterial::AddConstBuffer(FLOAT4 buf)
{
	constBufs_.push_back(buf);
}

void VSMaterial::ChangeConstBuffer(int registerNo, float buf1, float buf2, float buf3, float buf4)
{
	FLOAT4 buf;
	buf.x = buf1;
	buf.y = buf2;
	buf.z = buf3;
	buf.w = buf4;
	constBufs_[registerNo] = buf;

}

void VSMaterial::ChangeConstBuffer(int registerNo, FLOAT4 buf)
{
	constBufs_[registerNo] = buf;
}

void VSMaterial::AddModelId(int modelId)
{
	modelId_.push_back(modelId);
}

int VSMaterial::GetConstBuffer(void)
{
	return constBufH_;
}

std::vector<FLOAT4>& VSMaterial::GetConstBuffers(void)
{
	return constBufs_;
}

std::vector<int>& VSMaterial::GetTextureImgs(void)
{
	return textureImgs_;
}

int VSMaterial::GetShaderH(void)
{
	return shaderH_;
}

std::vector<int> VSMaterial::GetModelId(void)
{
	return modelId_;
}

void VSMaterial::AddTexture(int texH)
{
	textureImgs_.push_back(texH);
}

void VSMaterial::Clear(void)
{
	textureImgs_.clear();
	constBufs_.clear();
}

//	制作者：中田
//	他プロジェクトに持ち出し禁止！
