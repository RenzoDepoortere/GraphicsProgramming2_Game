float4x4 gWorld;
float4x4 gLightViewProj;
float4x4 gBones[70];
 
DepthStencilState depthStencilState
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
};

RasterizerState rasterizerState
{
	FillMode = SOLID;
	CullMode = NONE;
};

//--------------------------------------------------------------------------------------
// Vertex Shader [STATIC]
//--------------------------------------------------------------------------------------
float4 ShadowMapVS(float3 position:POSITION):SV_POSITION
{
	//TODO: return the position of the vertex in correct space (hint: seen from the view of the light)
	float4 lightViewPosition = mul(float4(position, 1.0f), mul(gWorld, gLightViewProj));
	return lightViewPosition;
}

//--------------------------------------------------------------------------------------
// Vertex Shader [SKINNED]
//--------------------------------------------------------------------------------------
float4 ShadowMapVS_Skinned(float3 position:POSITION, float4 BoneIndices : BLENDINDICES, float4 BoneWeights : BLENDWEIGHTS) : SV_POSITION
{
	//TODO: return the position of the ANIMATED vertex in correct space (hint: seen from the view of the light)
	
	// Bone transformation
	// -------------------
	
	float4 transformedPosition = float4(position, 1.f);
	
	float4x4 bonePositions;
	bonePositions[0] = float4(0,0,0,0);
	bonePositions[1] = float4(0,0,0,0);
	bonePositions[2] = float4(0,0,0,0);
	bonePositions[3] = float4(0,0,0,0);
	
	// First bone
	float currentBoneIdx = BoneIndices[0];
	if(currentBoneIdx >= 0)
	{
		bonePositions[0] = mul(float4(position,1.0f), gBones[currentBoneIdx]);
	}
	
	// Second bone
	currentBoneIdx = BoneIndices[1];
	if(currentBoneIdx >= 0)
	{
		bonePositions[1] = mul(float4(position,1.0f), gBones[currentBoneIdx]);
	}
	
	// Third bone
	currentBoneIdx = BoneIndices[2];
	if(currentBoneIdx >= 0)
	{
		bonePositions[2] = mul(float4(position,1.0f), gBones[currentBoneIdx]);
	}
	
	// Fourth bone
	currentBoneIdx = BoneIndices[3];
	if(currentBoneIdx >= 0)
	{
		bonePositions[3] = mul(float4(position,1.0f), gBones[currentBoneIdx]);
	}
	
	
	
	transformedPosition = bonePositions[0] * BoneWeights[0]
						+ bonePositions[1] * BoneWeights[1]
						+ bonePositions[2] * BoneWeights[2]
						+ bonePositions[3] * BoneWeights[3];
	transformedPosition.w = 1;
						
	float4 lightViewPosition = mul(transformedPosition, mul(gWorld, gLightViewProj));
	return lightViewPosition;
}
 
//--------------------------------------------------------------------------------------
// Pixel Shaders
//--------------------------------------------------------------------------------------
void ShadowMapPS_VOID(float4 position:SV_POSITION){}

technique10 GenerateShadows
{
	pass P0
	{
		SetRasterizerState(rasterizerState);
	    SetDepthStencilState(depthStencilState, 0);
		SetVertexShader(CompileShader(vs_4_0, ShadowMapVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, ShadowMapPS_VOID()));
	}
}

technique10 GenerateShadows_Skinned
{
	pass P0
	{
		SetRasterizerState(rasterizerState);
		SetDepthStencilState(depthStencilState, 0);
		SetVertexShader(CompileShader(vs_4_0, ShadowMapVS_Skinned()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, ShadowMapPS_VOID()));
	}
}