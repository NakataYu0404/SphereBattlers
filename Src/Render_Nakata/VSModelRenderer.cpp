#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "VSMaterial.h"
#include "PSMaterial.h"
#include "VSModelRenderer.h"

VSModelRenderer::VSModelRenderer(std::shared_ptr<VSMaterial> vsMate, std::shared_ptr<PSMaterial>psMate)
{
	vsMaterial_ = vsMate;
	psMaterial_ = psMate;
}

VSModelRenderer::~VSModelRenderer(void)
{
}

void VSModelRenderer::Update(void)
{
}

void VSModelRenderer::Draw(int screenH)
{

	modelId_ = vsMaterial_->GetModelId();

	//	AddConstBuf,Textureがコンストラクタの段階で回ってこない
	VSconstBuf_ = vsMaterial_->GetConstBuffer();
	VSconstBufs_ = vsMaterial_->GetConstBuffers();
	VSshaderH_ = vsMaterial_->GetShaderH();

	PSconstBuf_ = psMaterial_->GetConstBuffer();
	PSconstBufs_ = psMaterial_->GetConstBuffers();
	PSshaderH_ = psMaterial_->GetShaderH();

	textures_ = vsMaterial_->GetTextureImgs();

	auto& ins = SceneManager::GetInstance();

	ins.GetCamera()->SetBeforeDraw();

	//	使用シェーダ
	SetUseVertexShader(VSshaderH_);
	SetUsePixelShader(PSshaderH_);

	//	使用テクスチャ
	for (int i = 0; i < textures_.size(); i++)
	{
		SetUseTextureToShader(i, textures_[i]);
	}

	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);

	FLOAT4* constBufsPtr;

	if (VSconstBufs_.size() > 0)
	{
		//	定数バッファ定義
		constBufsPtr = (FLOAT4*)GetBufferShaderConstantBuffer(VSconstBuf_);
		for (int i = 0; i < VSconstBufs_.size(); i++)
		{

			constBufsPtr->x = VSconstBufs_[i].x;
			constBufsPtr->y = VSconstBufs_[i].y;
			constBufsPtr->z = VSconstBufs_[i].z;
			constBufsPtr->w = VSconstBufs_[i].w;
			constBufsPtr++;

		}

		//	定数バッファを更新して書き込んだ内容を反映
		UpdateShaderConstantBuffer(VSconstBuf_);
		//	TODO:このif文の入り方をすると、定数バッファで一つのスロットしか使えない。スロット番号をｲﾝｸﾘできる仕組みと、要素ごとのBufを設定できる仕組みが欲しい。
		SetShaderConstantBuffer(VSconstBuf_, DX_SHADERTYPE_VERTEX, CONSTANT_BUF_SLOT_BEGIN_VS);
	}

	if (PSconstBufs_.size() > 0)
	{
		//	定数バッファ定義
		constBufsPtr = (FLOAT4*)GetBufferShaderConstantBuffer(PSconstBuf_);
		for (int i = 0; i < PSconstBufs_.size(); i++)
		{

			constBufsPtr->x = PSconstBufs_[i].x;
			constBufsPtr->y = PSconstBufs_[i].y;
			constBufsPtr->z = PSconstBufs_[i].z;
			constBufsPtr->w = PSconstBufs_[i].w;
			constBufsPtr++;

		}
		//	定数バッファを更新して書き込んだ内容を反映
		UpdateShaderConstantBuffer(PSconstBuf_);
		SetShaderConstantBuffer(PSconstBuf_, DX_SHADERTYPE_PIXEL, CONSTANT_BUF_SLOT_BEGIN_PS);
	}

	//	シェーダを使用する
	MV1SetUseOrigShader(true);

	for (auto& m : modelId_)
	{
		MV1DrawModel(m);
	}

	SetUseVertexShader(-1); // または元のシェーダハンドル
	SetUsePixelShader(-1); // または元のシェーダハンドル
	MV1SetUseOrigShader(false);

}

//	制作者：中田
//	他プロジェクトに持ち出し禁止！
