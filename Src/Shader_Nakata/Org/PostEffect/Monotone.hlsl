#include "../../Common/Pixel/PixelShader2DHeader.hlsli"

cbuffer ColorBuf : register(b4)
{
	float4 color;
}

float4 main(PS_INPUT PSInput) : SV_TARGET
{
	float4 srcCol = tex.Sample(texSampler, PSInput.uv);
	srcCol.rgb = (srcCol.r + srcCol.g + srcCol.b) / 3;
	srcCol.rgb = srcCol.rgb * color.rgb;
	return srcCol;
}

	//	制作者：中田
	//	他プロジェクトに持ち出し禁止！
