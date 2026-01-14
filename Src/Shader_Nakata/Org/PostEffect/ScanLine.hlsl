#include "../../Common/Pixel/PixelShader2DHeader.hlsli"

cbuffer timeBuf : register(b4)
{
	float4 time;
}

float4 main(PS_INPUT PSInput) : SV_TARGET
{
	float4 srcCol = tex.Sample(texSampler, PSInput.uv);
	srcCol.rgb -= saturate(sin(PSInput.uv.y * 10.0f - time.x * 2.0f) / 8.0f);
	return srcCol;
}

	//	制作者：中田
	//	他プロジェクトに持ち出し禁止！
