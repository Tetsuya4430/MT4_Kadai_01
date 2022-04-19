#include "Sprite.hlsli"


Texture2D<float4> tex : register(t0);	//0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
	return tex.Sample(smp, input.uv) * color;	//輝度をRGBに代入して出力
}