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
// Light matrix
float4x4 gWorldViewProj_Light;

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

SamplerComparisonState cmpSampler
{
	// sampler state
	Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
	AddressU = MIRROR;
	AddressV = MIRROR;

	// sampler comparison state
	ComparisonFunc = LESS_EQUAL;
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

// SHADOW
// ******
float gShadowMapBias = 0.01f;
Texture2D gShadowMap;


//VS IN & OUT
//***********
struct VS_Input
{
	float3 Position: POSITION;
	float3 Normal: NORMAL;
	float3 Tangent: TANGENT;
	float3 Binormal: BINORMAL;
	float2 TexCoord: TEXCOORD0;
};

struct VS_Output
{
	float4 Position: SV_POSITION;
	float3 Normal: NORMAL;
	float3 Tangent: TANGENT;
	float3 Binormal: BINORMAL;
	float2 TexCoord: TEXCOORD0;
	float4 LPos : TEXCOORD1;
};

struct PS_Output
{
	float4 LightAccumulation : SV_TARGET0;
	float4 Diffuse : SV_TARGET1;
	float4 Specular : SV_TARGET2;
	float4 Normal : SV_TARGET3;
	float Shadow : SV_TARGET4;
};

// The main vertex shader
VS_Output MainVS(VS_Input input) {

	VS_Output output = (VS_Output)0;

	output.Position = mul(float4(input.Position, 1.0), gWorldViewProj);

	output.Normal = normalize(mul(input.Normal, (float3x3)gWorld));
	output.Tangent = normalize(mul(input.Tangent, (float3x3)gWorld));
	output.Binormal = normalize(mul(input.Binormal, (float3x3)gWorld));

	output.TexCoord = input.TexCoord;
	output.LPos = mul(float4(input.Position, 1.0f), gWorldViewProj_Light);

	return output;
}

float2 texOffset(int u, int v)
{
	return float2(u * 1.0f / 1280, v * 1.0f / 720);
}

float EvaluateShadowMap(float4 lpos)
{
	//re-homogenize position after interpolation
	lpos /= lpos.w;

	//if position is not visible to the light - dont illuminate it
	//results in hard light frustum
	if (lpos.x < -1.0f || 1.0f < lpos.x ||
		lpos.y < -1.0f || 1.0f < lpos.y ||
		lpos.z < 0.0f || 1.0f < lpos.z)
		return 0.0f;

	//transform clip space coords to texture space coords (-1:1 to 0:1)
	lpos.x = lpos.x / 2 + 0.5;
	lpos.y = lpos.y / -2 + 0.5;

	//apply shadow map bias
	lpos.z -= gShadowMapBias;

	//PCF sampling for shadow map
	float sum = 0;
	float x, y;

	//perform PCF filtering on a 4 x 4 texel neighborhood
	for (y = -1.5f; y < 1.5f; y += 1.0f)
	{
		for (x = -1.5f; x < 1.5f; x += 1.0f)
		{
			sum += gShadowMap.SampleCmpLevelZero(cmpSampler, lpos.xy + texOffset(x, y), lpos.z);
		}
	}

	return sum / 16.0f;
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

	// Shadow
	// ******
	float shadowValue = EvaluateShadowMap(input.LPos);

	// Set shadow
	output.Shadow = shadowValue;

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