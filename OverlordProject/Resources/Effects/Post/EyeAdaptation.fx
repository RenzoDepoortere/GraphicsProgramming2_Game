//=============================================================================
//// Shader uses position and texture
//=============================================================================
SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Mirror;
    AddressV = Mirror;
};

Texture2D gTexture;

/// Create Depth Stencil State (ENABLE DEPTH WRITING)
DepthStencilState EnableDepth
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
};

/// Create Rasterizer State (Backface culling) 
RasterizerState BackfaceCulling
{
	CullMode = BACK;
};


//IN/OUT STRUCTS
//--------------
struct VS_INPUT
{
    float3 Position : POSITION;
	float2 TexCoord : TEXCOORD0;

};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD1;
};


//VERTEX SHADER
//-------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;

	output.Position = float4(input.Position, 1.0f);
	output.TexCoord = input.TexCoord;
	
	return output;
}


//PIXEL SHADER
//------------
float4 PS(PS_INPUT input): SV_Target
{
    // Get pixel color
	// ---------------
	float4 sampledColor = gTexture.Sample(samPoint, input.TexCoord);
	
	// Eye Adaptation
	// --------------
	
	// Exposure based on this value, usually is 0.18
	float middleGray = 0.18f;	
	
	// Min/Max amount pixel color can change
	float minExposure = -6.f;
	float maxExposure = 6.f;	
	
	// Relative sensitivity of the eye to different colors
	float3 luminanceWeights = float3(0.299f, 0.587f, 0.114f);
	
	// Convert pixel to luminance
	float luminance = dot(sampledColor.rgb, luminanceWeights);
	
	// Calculate desired exposure adjustment
	float exposureAdjustement = log2(luminance / middleGray);
	exposureAdjustement = clamp(exposureAdjustement, minExposure, maxExposure);
	
	// Apply exposure adjustement to pixel
	sampledColor.rgb *= pow(2.0f, exposureAdjustement);
	
	// Return
	// ------
    return sampledColor;
}


//TECHNIQUE
//---------
technique10 Grayscale
{
    pass P0
    {          
		SetRasterizerState(BackfaceCulling);
		SetDepthStencilState(EnableDepth, 0);
		
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

