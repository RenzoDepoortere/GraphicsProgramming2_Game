//*************************//
// BASIC EFFECT [DEFERRED] //
//*************************//

/*
	- Diffuse Color/Texture
	- Specular Color
	- SpecularLevel Texture (Blinn)
	- Specular Intensity (Shininess)
	- NormalMap Texture
	- Ambient Color
	- Ambient Intensity [0-1]
	- Opacity Texture/Value [0-1]
*/

//GLOBAL MATRICES
//***************
// The World View Projection Matrix
float4x4 gWorldViewProj : WORLDVIEWPROJECTION;
// The ViewInverse Matrix - the third row contains the camera position!
float4x4 gViewInverse : VIEWINVERSE;
// The World Matrix
float4x4 gWorld : WORLD;
// Bones
float4x4 gBones[70];

//STATES
//******
RasterizerState gRasterizerState
{
	FillMode = SOLID;
	CullMode = FRONT;
};

BlendState gBlendState
{
	BlendEnable[0] = FALSE;
};

DepthStencilState gDepthState
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
};

//SAMPLER STATES
//**************
SamplerState gTextureSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	//Filter = ANISOTROPIC;
	AddressU = WRAP;
	AddressV = WRAP;
	AddressW = WRAP;
};

//LIGHT
//*****
float3 gLightDirection:DIRECTION
<
	string UIName = "Light Direction";
	string Object = "TargetLight";
> = float3(0.577f, 0.577f, 0.577f);

//DIFFUSE
//*******
bool gUseDiffuseMap
<
	string UIName = "Diffuse Texture";
	string UIWidget = "Bool";
> = false;

float4 gDiffuseColor
<
	string UIName = "Diffuse Color";
	string UIWidget = "Color";
> = float4(1, 1, 1, 1);

Texture2D gDiffuseMap
<
	string UIName = "Diffuse Texture";
	string UIWidget = "Texture";
> ;

//SPECULAR
//********
float4 gSpecularColor
<
	string UIName = "Specular Color";
	string UIWidget = "Color";
> = float4(1, 1, 1, 1);

Texture2D gSpecularMap
<
	string UIName = "Specular Level Texture";
	string UIWidget = "Texture";
> ;

bool gUseSpecularMap
<
	string UIName = "Specular Level Texture";
	string UIWidget = "Bool";
> = false;

int gShininess <
	string UIName = "Shininess";
	string UIWidget = "Slider";
	float UIMin = 1;
	float UIMax = 100;
	float UIStep = 0.1f;
> = 15;

//AMBIENT
//*******
float4 gAmbientColor
<
	string UIName = "Ambient Color";
	string UIWidget = "Color";
> = float4(1.0, 1.0, 1.0, 1.0);

float gAmbientIntensity
<
	string UIName = "Ambient Intensity";
	string UIWidget = "slider";
	float UIMin = 0;
	float UIMax = 1;
> = 0.3f;

//NORMAL MAPPING
//**************
bool gFlipGreenChannel
<
	string UIName = "Flip Green Channel";
	string UIWidget = "Bool";
> = false;

bool gUseNormalMap
<
	string UIName = "Normal Mapping";
	string UIWidget = "Bool";
> = false;

Texture2D gNormalMap
<
	string UIName = "Normal Texture";
	string UIWidget = "Texture";
> ;

//OPACITY
//***************
float gOpacityLevel <
	string UIName = "Opacity";
	string UIWidget = "slider";
	float UIMin = 0;
	float UIMax = 1;
> = 1.0f;


//VS IN & OUT
//***********
struct VS_Input
{
	float3 Position: POSITION;
	float3 Normal: NORMAL;
	float3 Tangent: TANGENT;
	float3 Binormal: BINORMAL;
	float2 TexCoord: TEXCOORD0;

	float4 BlendIndices : BLENDINDICES;
	float4 BlendWeights : BLENDWEIGHTS;
};

struct VS_Output
{
	float4 Position: SV_POSITION;
	float3 Normal: NORMAL;
	float3 Tangent: TANGENT;
	float3 Binormal: BINORMAL;
	float2 TexCoord: TEXCOORD0;
};

struct PS_Output
{
	float4 LightAccumulation : SV_TARGET0;
	float4 Diffuse : SV_TARGET1;
	float4 Specular : SV_TARGET2;
	float4 Normal : SV_TARGET3;
};

// The main vertex shader
VS_Output MainVS(VS_Input input) {

	VS_Output output;

	// Bone transformation
	// -------------------

	float4 transformedPosition = float4(input.Position, 1.f);
	float3 transformedNormal = input.Normal;
	float3 transformedTangent = input.Tangent;
	float3 transformedBinormal = input.Binormal;

	float4x4 bonePositions;
	bonePositions[0] = float4(0, 0, 0, 0);
	bonePositions[1] = float4(0, 0, 0, 0);
	bonePositions[2] = float4(0, 0, 0, 0);
	bonePositions[3] = float4(0, 0, 0, 0);

	float4x3 boneNormals;
	boneNormals[0] = float3(0, 0, 0);
	boneNormals[1] = float3(0, 0, 0);
	boneNormals[2] = float3(0, 0, 0);
	boneNormals[3] = float3(0, 0, 0);

	float4x3 boneTangents;
	boneTangents[0] = float3(0, 0, 0);
	boneTangents[1] = float3(0, 0, 0);
	boneTangents[2] = float3(0, 0, 0);
	boneTangents[3] = float3(0, 0, 0);

	float4x3 boneBinormals;
	boneBinormals[0] = float3(0, 0, 0);
	boneBinormals[1] = float3(0, 0, 0);
	boneBinormals[2] = float3(0, 0, 0);
	boneBinormals[3] = float3(0, 0, 0);

	// First bone
	float currentBoneIdx = input.BlendIndices[0];
	if (currentBoneIdx >= 0)
	{
		bonePositions[0] = mul(float4(input.Position, 1.0f), gBones[currentBoneIdx]);
		boneNormals[0] = normalize(mul(input.Normal, (float3x3)gBones[currentBoneIdx]));
		
		boneTangents[0] = normalize(mul(input.Tangent, (float3x3)gBones[currentBoneIdx]));
		boneBinormals[0] = normalize(mul(input.Binormal, (float3x3)gBones[currentBoneIdx]));
	}

	// Second bone
	currentBoneIdx = input.BlendIndices[1];
	if (currentBoneIdx >= 0)
	{
		bonePositions[1] = mul(float4(input.Position, 1.0f), gBones[currentBoneIdx]);
		boneNormals[1] = normalize(mul(input.Normal, (float3x3)gBones[currentBoneIdx]));

		boneTangents[1] = normalize(mul(input.Tangent, (float3x3)gBones[currentBoneIdx]));
		boneBinormals[1] = normalize(mul(input.Binormal, (float3x3)gBones[currentBoneIdx]));
	}

	// Third bone
	currentBoneIdx = input.BlendIndices[2];
	if (currentBoneIdx >= 0)
	{
		bonePositions[2] = mul(float4(input.Position, 1.0f), gBones[currentBoneIdx]);
		boneNormals[2] = normalize(mul(input.Normal, (float3x3)gBones[currentBoneIdx]));

		boneTangents[2] = normalize(mul(input.Tangent, (float3x3)gBones[currentBoneIdx]));
		boneBinormals[2] = normalize(mul(input.Binormal, (float3x3)gBones[currentBoneIdx]));
	}

	// Fourth bone
	currentBoneIdx = input.BlendIndices[3];
	if (currentBoneIdx >= 0)
	{
		bonePositions[3] = mul(float4(input.Position, 1.0f), gBones[currentBoneIdx]);
		boneNormals[3] = normalize(mul(input.Normal, (float3x3)gBones[currentBoneIdx]));

		boneTangents[3] = normalize(mul(input.Tangent, (float3x3)gBones[currentBoneIdx]));
		boneBinormals[3] = normalize(mul(input.Binormal, (float3x3)gBones[currentBoneIdx]));
	}



	transformedPosition = bonePositions[0] * input.BlendWeights[0]
		+ bonePositions[1] * input.BlendWeights[1]
		+ bonePositions[2] * input.BlendWeights[2]
		+ bonePositions[3] * input.BlendWeights[3];
	transformedPosition.w = 1;

	transformedNormal = boneNormals[0] * input.BlendWeights[0]
		+ boneNormals[1] * input.BlendWeights[1]
		+ boneNormals[2] * input.BlendWeights[2]
		+ boneNormals[3] * input.BlendWeights[3];
	transformedNormal = normalize(transformedNormal);

	transformedTangent = boneTangents[0] * input.BlendWeights[0]
		+ boneTangents[1] * input.BlendWeights[1]
		+ boneTangents[2] * input.BlendWeights[2]
		+ boneTangents[3] * input.BlendWeights[3];
	transformedTangent = normalize(transformedTangent);

	transformedBinormal = boneBinormals[0] * input.BlendWeights[0]
		+ boneBinormals[1] * input.BlendWeights[1]
		+ boneBinormals[2] * input.BlendWeights[2]
		+ boneBinormals[3] * input.BlendWeights[3];
	transformedBinormal = normalize(transformedBinormal);

	transformedPosition = mul(transformedPosition, gWorldViewProj);
	transformedNormal = normalize(mul(transformedNormal, (float3x3)gWorld));

	transformedTangent = normalize(mul(transformedTangent, (float3x3)gWorld));
	transformedBinormal = normalize(mul(transformedBinormal, (float3x3)gWorld));

	// Normal transformation
	// ---------------------
	output.Position = transformedPosition;
	output.Normal = transformedNormal;

	output.Tangent = transformedTangent;
	output.Binormal = transformedBinormal;

	output.TexCoord = input.TexCoord;

	return output;
}

// The main pixel shader
PS_Output MainPS(VS_Output input){

	PS_Output output = (PS_Output)0;

	// Fill GBuffer
	// -----------

	// Diffuse
	// *******
	float4 diffuse = gDiffuseColor;
	if (gUseDiffuseMap)
	{
		diffuse *= gDiffuseMap.Sample(gTextureSampler, input.TexCoord);
	}

	// Set diffuse
	output.Diffuse = diffuse;

	// Alpha
	float alpha = diffuse.a * gOpacityLevel;
	clip(alpha - 0.1f);


	// Ambient
	// *******
	float4 ambient = gAmbientColor;
	ambient *= diffuse;
	ambient *= gAmbientIntensity;

	// Set ambient (light_accumulation)
	output.LightAccumulation = ambient;


	// Normal
	// ******
	float3 normal = input.Normal;
	if (gUseNormalMap)
	{
		float3x3 TBN = float3x3
		(
			normalize(input.Tangent),
			normalize(input.Binormal),
			normalize(input.Normal)
		);

		normal = gNormalMap.Sample(gTextureSampler, input.TexCoord).xyz;
		normal = normal * 2.0f - 1.0f;
		normal = mul(normal, TBN);
	}

	// Set normal
	output.Normal = float4(normal, 0);

	// Specular
	// ********
	float3 specular = gSpecularColor.rgb;
	if (gUseSpecularMap)
	{
		specular *= gSpecularMap.Sample(gTextureSampler, input.TexCoord).rgb;
	}

	float shininess = log2(gShininess) / 10.5f;

	// Set specular
	output.Specular = float4(specular, shininess);

	return output;
}

// Default Technique
technique10 Default {
	pass p0 {
		SetDepthStencilState(gDepthState, 0);
		SetRasterizerState(gRasterizerState);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);

		SetVertexShader(CompileShader(vs_4_0, MainVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, MainPS()));
	}
}