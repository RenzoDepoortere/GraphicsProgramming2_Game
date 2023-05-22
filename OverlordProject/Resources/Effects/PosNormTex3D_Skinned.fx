float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 
float3 gLightDirection = float3(-0.577f, -0.577f, 0.577f);

Texture2D gDiffuseMap;
SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;// or Mirror or Clamp or Border
    AddressV = Wrap;// or Mirror or Clamp or Border
};

float4x4 gBones[70];

RasterizerState Solid
{
	FillMode = SOLID;
	CullMode = FRONT;
};

DepthStencilState EnableDepth
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
};

BlendState gBS_EnableBlending 
{     
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
};

struct VS_INPUT{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
	float4 blendIndices : BLENDINDICES;
	float4 blendWeights : BLENDWEIGHTS;
};
struct VS_OUTPUT{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
};

RasterizerState NoCulling
{
	CullMode = NONE;
};

BlendState NoBlending
{
	BlendEnable[0] = FALSE;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input){

	VS_OUTPUT output;
	
	// Bone transformation
	// -------------------
	
	float4 transformedPosition = float4(input.pos, 1.f);
	float3 transformedNormal = input.normal;
	
	float4x4 bonePositions;
	bonePositions[0] = float4(0,0,0,0);
	bonePositions[1] = float4(0,0,0,0);
	bonePositions[2] = float4(0,0,0,0);
	bonePositions[3] = float4(0,0,0,0);
	
	float4x3 boneNormals;
	boneNormals[0] = float3(0,0,0);
	boneNormals[1] = float3(0,0,0);
	boneNormals[2] = float3(0,0,0);
	boneNormals[3] = float3(0,0,0);
	
	// ----------------------------------------
	// This didn't seem to work with a for-loop
	// ----------------------------------------
	
	// First bone
	float currentBoneIdx = input.blendIndices[0];
	if(currentBoneIdx >= 0)
	{
		bonePositions[0] = mul(float4(input.pos,1.0f), gBones[currentBoneIdx]);
		boneNormals[0] = normalize(mul(input.normal, (float3x3)gBones[currentBoneIdx]));
	}
	
	// Second bone
	currentBoneIdx = input.blendIndices[1];
	if(currentBoneIdx >= 0)
	{
		bonePositions[1] = mul(float4(input.pos,1.0f), gBones[currentBoneIdx]);
		boneNormals[1] = normalize(mul(input.normal, (float3x3)gBones[currentBoneIdx]));
	}
	
	// Third bone
	currentBoneIdx = input.blendIndices[2];
	if(currentBoneIdx >= 0)
	{
		bonePositions[2] = mul(float4(input.pos,1.0f), gBones[currentBoneIdx]);
		boneNormals[2] = normalize(mul(input.normal, (float3x3)gBones[currentBoneIdx]));
	}
	
	// Fourth bone
	currentBoneIdx = input.blendIndices[3];
	if(currentBoneIdx >= 0)
	{
		bonePositions[3] = mul(float4(input.pos,1.0f), gBones[currentBoneIdx]);
		boneNormals[3] = normalize(mul(input.normal, (float3x3)gBones[currentBoneIdx]));
	}
	
	
	
	transformedPosition = bonePositions[0] * input.blendWeights[0]
						+ bonePositions[1] * input.blendWeights[1]
						+ bonePositions[2] * input.blendWeights[2]
						+ bonePositions[3] * input.blendWeights[3];
	transformedPosition.w = 1;
	
	transformedNormal 	= boneNormals[0] * input.blendWeights[0]
						+ boneNormals[1] * input.blendWeights[1]
						+ boneNormals[2] * input.blendWeights[2]
						+ boneNormals[3] * input.blendWeights[3];
	transformedNormal = normalize(transformedNormal);
						
	transformedPosition = mul (transformedPosition, gWorldViewProj);
	transformedNormal = normalize(mul(transformedNormal, (float3x3)gWorld));
	
	// Normal transformation
	// ---------------------
	output.pos = transformedPosition;
	output.normal = transformedNormal;
	output.texCoord = input.texCoord;
	
	// Return
	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET{

	float4 diffuseColor = gDiffuseMap.Sample( samLinear,input.texCoord );
	float3 color_rgb= diffuseColor.rgb;
	float color_a = diffuseColor.a;
	
	//HalfLambert Diffuse
	float diffuseStrength = dot(input.normal, -gLightDirection);
	diffuseStrength = diffuseStrength * 0.5 + 0.5;
	diffuseStrength = saturate(diffuseStrength);
	color_rgb = color_rgb * diffuseStrength;

	// Don't write to depthBuffer if is fully opaque
	clip(color_a - 1.0f);

	return float4( color_rgb , color_a );
}

//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------
technique10 Default
{
    pass P0
    {
		SetRasterizerState(NoCulling);
		SetDepthStencilState(EnableDepth, 0);
		SetBlendState(gBS_EnableBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);

		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

