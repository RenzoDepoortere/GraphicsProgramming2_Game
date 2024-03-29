float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION;
float4x4 gWorldViewProj_Light;
float3 gLightDirection = float3(-0.577f, -0.577f, 0.577f);
float gShadowMapBias = 0.01f;
float4x4 gBones[70];

Texture2D gDiffuseMap;
Texture2D gShadowMap;

SamplerComparisonState cmpSampler
{
	// sampler state
	Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
	AddressU = MIRROR;
	AddressV = MIRROR;

	// sampler comparison state
	ComparisonFunc = LESS_EQUAL;
};

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;// or Mirror or Clamp or Border
	AddressV = Wrap;// or Mirror or Clamp or Border
};

struct VS_INPUT
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
	float4 blendIndices : BLENDINDICES;
	float4 blendWeights : BLENDWEIGHTS;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
	float4 lPos : TEXCOORD1;
};

BlendState gBS_EnableBlending
{
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
};

DepthStencilState EnableDepth
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
};

RasterizerState NoCulling
{
	CullMode = NONE;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	//TODO: complete Vertex Shader 
	//Hint: use the previously made shaders PosNormTex3D_Shadow and PosNormTex3D_Skinned as a guide

	// Bone transformation
	// -------------------

	float4 transformedPosition = float4(input.pos, 1.f);
	float3 transformedNormal = input.normal;

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

	// First bone
	float currentBoneIdx = input.blendIndices[0];
	if (currentBoneIdx >= 0)
	{
		bonePositions[0] = mul(float4(input.pos, 1.0f), gBones[currentBoneIdx]);
		boneNormals[0] = normalize(mul(input.normal, (float3x3)gBones[currentBoneIdx]));
	}

	// Second bone
	currentBoneIdx = input.blendIndices[1];
	if (currentBoneIdx >= 0)
	{
		bonePositions[1] = mul(float4(input.pos, 1.0f), gBones[currentBoneIdx]);
		boneNormals[1] = normalize(mul(input.normal, (float3x3)gBones[currentBoneIdx]));
	}

	// Third bone
	currentBoneIdx = input.blendIndices[2];
	if (currentBoneIdx >= 0)
	{
		bonePositions[2] = mul(float4(input.pos, 1.0f), gBones[currentBoneIdx]);
		boneNormals[2] = normalize(mul(input.normal, (float3x3)gBones[currentBoneIdx]));
	}

	// Fourth bone
	currentBoneIdx = input.blendIndices[3];
	if (currentBoneIdx >= 0)
	{
		bonePositions[3] = mul(float4(input.pos, 1.0f), gBones[currentBoneIdx]);
		boneNormals[3] = normalize(mul(input.normal, (float3x3)gBones[currentBoneIdx]));
	}



	transformedPosition = bonePositions[0] * input.blendWeights[0]
		+ bonePositions[1] * input.blendWeights[1]
		+ bonePositions[2] * input.blendWeights[2]
		+ bonePositions[3] * input.blendWeights[3];
	transformedPosition.w = 1;

	transformedNormal = boneNormals[0] * input.blendWeights[0]
		+ boneNormals[1] * input.blendWeights[1]
		+ boneNormals[2] * input.blendWeights[2]
		+ boneNormals[3] * input.blendWeights[3];
	transformedNormal = normalize(transformedNormal);

	transformedPosition = mul(transformedPosition, gWorldViewProj);
	transformedNormal = normalize(mul(transformedNormal, (float3x3)gWorld));

	// Normal transformation
	// ---------------------
	output.pos = transformedPosition;
	output.normal = transformedNormal;
	output.texCoord = input.texCoord;
	output.lPos = mul(transformedPosition, gWorldViewProj_Light);

	// Return
	return output;
}

float2 texOffset(int u, int v)
{
	//TODO: return offseted value (our shadow map has the following dimensions: 1280 * 720)
	return float2(u * 1.0f / 1280, v * 1.0f / 720);
}

float EvaluateShadowMap(float4 lpos)
{
	//TODO: complete

	//re-homogenize position after interpolation
	lpos /= lpos.w;

	//if position is not visible to the light - dont illuminate it
	//results in hard light frustum
	if (lpos.x < -1.0f || 1.0f < lpos.x ||
		lpos.y < -1.0f || 1.0f < lpos.y ||
		lpos.z < 0.0f || 1.0f < lpos.z)
		return 1.0f;

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

	return sum / 8.0f;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET
{
	float shadowValue = EvaluateShadowMap(input.lPos);
	if (shadowValue < 0.5f) shadowValue = 0.5f;
	float ndotl = dot(normalize(input.normal), -gLightDirection);

	float4 diffuseColor = gDiffuseMap.Sample(samLinear,input.texCoord);
	float3 color_rgb = diffuseColor.rgb;
	float color_a = diffuseColor.a;

	//HalfLambert Diffuse :)
	float diffuseStrength = dot(input.normal, -gLightDirection);
	diffuseStrength = diffuseStrength * 0.5 + 0.5;
	diffuseStrength = saturate(diffuseStrength);
	color_rgb = color_rgb * diffuseStrength;

	// Don't write to depthBuffer if is fully opaque
	clip(color_a - 1.0f);

	return float4(color_rgb * shadowValue, color_a);
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

		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));
	}
}

