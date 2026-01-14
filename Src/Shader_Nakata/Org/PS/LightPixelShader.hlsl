// VS/PS共通
#include "../../Common/VertexToPixelHeader.hlsli"

//PS
#include "../../Common/Pixel/PixelShader3DHeader.hlsli"


// IN
#define PS_INPUT VertexToPixelLit

// 定数バッファ：スロット0番目(b0と書く)
cbuffer cbParam : register(b0)
{
	float4 g_color;
}

Texture2D myTexture : register(t0); // t0 はテクスチャレジスタの番号
SamplerState mySampler : register(s0); // s0 はサンプラーステートのレジスタの番号

float4 main(PS_INPUT PSInput) : SV_TARGET
{
	float2 uv = PSInput.uv;
	float4 TextureDiffuseColor = myTexture.Sample(mySampler, uv);

	if (TextureDiffuseColor.a == 0)
	{
        // テクスチャが正しくサンプルされていない場合のエラーハンドリング
		return float4(1.0, 0.0, 0.0, 1.0); // 赤色を返して確認
	}

    // 以下のコードは正常動作していると仮定
	//float3 lightDir = g_common.light[0].direction;
	float4 srcCol = PSInput.diffuse * TextureDiffuseColor;
	//float dotNormLight = dot(PSInput.normal, lightDir);
	//srcCol.xyz *= (-dotNormLight + 1.0f);
	return srcCol;
}

	//	制作者：中田
	//	他プロジェクトに持ち出し禁止！
