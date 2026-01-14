#pragma once
#include <memory>
#include <vector>
#include <Dxlib.h>
#include "../Manager/SceneManager.h"

class PSMaterial;

class PSMeshRenderer
{
public:
	PSMeshRenderer(std::shared_ptr<PSMaterial> psMaterial);
	~PSMeshRenderer(void);
	
	void Update(void);
	void Draw(int screenH_);
private:
	
	void MakeVertexs(void);


	//	ピクセルシェーダ用オリジナル定数バッファの使用開始スロット
	static constexpr int CONSTANT_BUF_SLOT_BEGIN_PS = 4;

	//	頂点数
	static constexpr int NUM_VERTEX = 4;

	//	頂点インデックス数
	static constexpr int NUM_VERTEX_IDX = 6;

	//	ポリゴン数
	static constexpr int NUM_POLYGON = 2;

	std::shared_ptr<PSMaterial> psMaterial_;

	//	頂点情報
	VERTEX2DSHADER vertexs_[NUM_VERTEX];

	//	頂点インデックス
	WORD indexes_[NUM_VERTEX_IDX];

	int constBuf_;
	int shaderH_;
	std::vector<FLOAT4> constBufs_;
	std::vector<int> textures_;

	int oldScreenH_;
};

//	制作者：中田
//	他プロジェクトに持ち出し禁止！