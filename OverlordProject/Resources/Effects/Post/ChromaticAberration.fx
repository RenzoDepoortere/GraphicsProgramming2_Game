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
	
	// Chromatic Aberration
	// --------------------
	
	// Get textureDimensions
	float textureWidth;
	float textureHeight;
	gTexture.GetDimensions(textureWidth, textureHeight);
    
	// Calculate color offsets
	float2 offset = float2(0.002, -0.002);
	offset *= float2(textureWidth, textureHeight);

    // Get red and blue with offset
    float red = gTexture.Sample(samPoint, input.TexCoord + offset).r;
    float blue = gTexture.Sample(samPoint, input.TexCoord - offset).b;

    // Combine all colors
    float4 finalColor = float4(red, sampledColor.g, blue, sampledColor.a);
	
	// Return
	// ------
    return finalColor;
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

