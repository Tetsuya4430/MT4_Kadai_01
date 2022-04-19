#include "OBJ.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
	VSOutput output;	//ピクセルシェーダに渡す値
	output.svpos = mul(mat, pos);
	output.normal = normal;
	output.uv = uv;

	return output;
}