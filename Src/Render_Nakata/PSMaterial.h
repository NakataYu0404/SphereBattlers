#pragma once
#include <vector>
#include <string>
#include <DxLib.h>

class PSMaterial
{
public:
	//	PixelShaderのパス,定数バッファを何個作るのか
	PSMaterial(std::string PSPath,int bufVolume);
	~PSMaterial(void);

	//	定数バッファに中身を入れるための関数
	void AddConstBuffer(float buf1 = -1.0f, float buf2 = -1.0f, float buf3 = -1.0f, float buf4 = -1.0f);
	void AddConstBuffer(FLOAT4 buf);

	void ChangeConstBuffer(int registerNo = 0, float buf1 = -1.0f, float buf2 = -1.0f, float buf3 = -1.0f, float buf4 = -1.0f);
	void ChangeConstBuffer(int registerNo = 0, FLOAT4 buf = {-1.0f,-1.0f,-1.0f,-1.0f});

	//	ゲッター
	int GetConstBuffer(void);
	std::vector<FLOAT4>& GetConstBuffers(void);

	std::vector<int>& GetTextureImgs(void);

	int GetShaderH(void);

	//	テクスチャの情報を取得しておく(実際にSet処理をするのはRenderer)
	void AddTexture(int texH);

	void Clear(void);
private:
	//	シェーダーH
	int shaderH_;
	
	//	バッファH
	int constBufH_;

	//	定数バッファのポインタの中身に設定する値,vectorは[1]のデータと[2]のデータが連続してメモリ確保しているのがいいところ
	std::vector<FLOAT4> constBufs_;

	std::vector<int> textureImgs_;
};

//	制作者：中田
//	他プロジェクトに持ち出し禁止！
