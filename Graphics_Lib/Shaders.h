#ifndef SHADERS_H
#define SHADERS_H
#include <string>

namespace Graphics{

	namespace Shader_Defines{
		const std::string FullScreenQuadDefinitions = std::string(
		"struct VSQuadOut{"
			"float4 position : SV_Position;"
			"float2 texcoord: TexCoord;"
		"};\n"
		"struct VSQuadOutMax{"
			"float4 position : SV_Position;"
			"float2 texcoord: TexCoord0;"
			"float4 vWorldPos: TexCoord1;"
		"};"
		);


		const std::string FullScreenQuadVS = std::string(
			FullScreenQuadDefinitions +
			"VSQuadOut VS(uint VertexID: SV_VertexID){"
				"VSQuadOut Out;"
				"Out.texcoord = float2( (VertexID << 1) & 2, VertexID & 2 );"
				"Out.position = float4( Out.texcoord * float2( 2.0f, -2.0f ) + float2( -1.0f, 1.0f), 0.0f, 1.0f );"
				"return Out;"
			"}"
		);
		const std::string FullScreenQuadWOneVS = std::string(
			FullScreenQuadDefinitions +
			"VSQuadOut VS(uint VertexID: SV_VertexID){"
			"VSQuadOut Out;"
			"Out.texcoord = float2( (VertexID << 1) & 2, VertexID & 2 );"
			"Out.position = float4( Out.texcoord * float2( 2.0f, -2.0f ) + float2( -1.0f, 1.0f), 0.0f, 1.0f );"
			"Out.texcoord = Out.position.xy*1.1f;"
			"return Out;"
			"}"
			);

		
		

		const std::string NormalBumpConverterPS = std::string(
			FullScreenQuadDefinitions +
			"Texture2D tex0;"
			"Texture2D tex1;"
			"SamplerState sampler0;"
			"float4 PS(VSQuadOut In) : SV_Target0 {"
				"return float4(tex0.Sample(sampler0, In.texcoord).xyz, tex1.Sample(sampler0, In.texcoord).x);"
			"}"
		);


		const std::string Debug_ShadowPS = std::string(
			FullScreenQuadDefinitions +
			"cbuffer perminent{"
			   " static const int SplitPowLookup[8] = {0, 1, 1, 2, 2, 2, 2, 3};"
			"};"
			"Texture2D tex0;"
			"Texture2DArray tex1;"
			"SamplerState sampler0;"
			"cbuffer buffer{"
			"	float4   SunPosition;"
			"	float4x4 CameraViewProjection;"
			"	float4x4 InvCameraViewProjectionFromScreen;"
			"	float4   SplitPositions;"
			"	float4x4 ShadowMapSplits[4];"
			"	float4x4 ShadowMapSplitsTex[4];"
			"};"
			"float4 PS(VSQuadOut In ) : SV_Target {"
			"	int3 texCoord = int3(int2(In.position.xy), 0);"
			"	float depth = tex0.Load(texCoord).r;"
			"	float4 cPos = float4(In.position.xy, depth, 1);"
				// World-space position
			"	cPos = mul(cPos, InvCameraViewProjectionFromScreen);"
			"	float3 pos = cPos.xyz / cPos.w;"// pos is now the real world position

			"	float receiver = length( SunPosition- pos);"
	
			"	float3 otherp = mul(float4(pos, 1.0f), CameraViewProjection).xyz;"
			"	int Split = dot(1, otherp.z > SplitPositions);"
    
				// Ensure that every fragment in the quad choses the same split so that derivatives
				// will be meaningful for proper texture filtering and LOD selection.
			"    int SplitPow = 1 << Split;"
			"    int SplitX = abs(ddx(SplitPow));"
			"    int SplitY = abs(ddy(SplitPow));"
			"    int SplitXY = abs(ddx(SplitY));"
			"    int SplitMax = max(SplitXY, max(SplitX, SplitY));"
			"    Split = SplitMax > 0 ? SplitPowLookup[SplitMax-1] : Split;"
			"	float4 PosLight = mul(float4(pos, 1), ShadowMapSplitsTex[Split]);"// no need for a divide by w, I use an orthog proj
			"	float occluder;"
			"	occluder= tex1.Sample( sampler0, float3(PosLight.xy, Split) ).x;"
			"	float	lit = exp(OverDark* ( occluder - receiver));"
			"	return float4(1, 1, 1, 4)*saturate(lit);"
			"}"
		);
		const std::string Debug_DiffusePS = std::string(
			FullScreenQuadDefinitions +
			"Texture2D tex0;"
			"float3 PS(VSQuadOut In ) : SV_Target {"
			"	int3 texCoord = int3(int2(In.position.xy), 0);"//rebuild World position.....
			"	return tex0.Load(texCoord).rgb;"
			"}"
		);
		const std::string Debug_NormalsPS = std::string(
			FullScreenQuadDefinitions +
			"Texture2D tex0;"
			"float3 PS(VSQuadOut In ) : SV_Target {"
			"	int3 texCoord = int3(int2(In.position.xy), 0);"//rebuild World position.....
			"	return tex0.Load(texCoord).rgb;"
			"}"
		);
		const std::string Debug_SpecularPS = std::string(
			FullScreenQuadDefinitions +
			"Texture2D tex0;"
			"float3 PS(VSQuadOut In ) : SV_Target {"
				"int3 texCoord = int3(int2(In.position.xy), 0);"//rebuild World position.....
				"return float3(1, 1, 1) * (tex0.Load(texCoord).a/5.0f);"
			"}"
		);
		const std::string Debug_StencilPS = std::string(
			FullScreenQuadDefinitions +
			"float3 PS(VSQuadOut In ) : SV_Target {"
			"	int3 texCoord = int3(int2(In.position.xy), 0);"//rebuild World position.....
			"	return float3(1, 1, 1);"
			"}"
		);

		const std::string Debug_DepthPS = std::string(
			FullScreenQuadDefinitions +
			"Texture2D tex0;"
			"cbuffer buffer{"
				"float4x4 InvCameraViewProjectionFromScreen;"
				"float4   CameraPosition;"
				"float FarPlane;"
			"};"
			"float4 PS_Depth(VSQuadOut In ) : SV_Target {"
			"	int3 texCoord = int3(int2(In.position.xy), 0);"//rebuild World position.....
			"	float depth = tex0.Load(texCoord).x;"
			"	float4 cPos = float4(In.position.xy, depth, 1);"// World-space position
			"	cPos = mul(cPos, InvCameraViewProjectionFromScreen);"
			"	float3 pos = cPos.xyz / cPos.w;"// pos is now the real world position
			"	return float4(1, 1, 1, 1) *(length(pos-CameraPosition)/FarPlane.y);"// Planes.y is the far plane
			"}"
		);
		struct Blur_PS_struct{
			float Settings[4];
			bool UDLR;
			float pad[3];
		};
		const std::string Blur_PS = std::string(
			FullScreenQuadDefinitions +
			"Texture2D tex0;"
			"SamplerState sampler0;"
			"cbuffer buffer0{"
				"float4 Settings;"
				"bool UDLR;"
			"};"
			"float BoxFilterStart( float fWidth ){"
				"return ( ( fWidth - 1.0f ) / 2.0f );"
			"}"

			"float4 PS( VSQuadOut input ) : SV_Target{"
				"float fStartOffset = BoxFilterStart( Settings.z);"
				"float2 fTexelOffset = float2(UDLR * (Settings.y ), !UDLR * (Settings.y ) );"
				"float2 fTexStart = input.texcoord - ( fStartOffset * fTexelOffset );"
				"float4 output = float4(0.0f, 0.0f, 0.0f, 0.0f);"
    
				"for( int i = 0; i < Settings.z ; ++i )"
				"	output += tex0.Sample( sampler0, float2( fTexStart + fTexelOffset * i));"
				"return output * Settings.x;"
			"}"
			);	



		const std::string PreHSPassThrough_VS =  std::string(	
		"struct HSIn_Heightfield"
		"{"
			"float2 origin   : ORIGIN;"
			"float2 size     : SIZE;"
		"};"
		"HSIn_Heightfield PassThroughVS(float4 PatchParams : Position)"
		"{"
			"HSIn_Heightfield output;"
			"output.origin = PatchParams.xy;"
			"output.size = PatchParams.zw;"
			"return output;"
		"}"
		);
	};
};
#endif