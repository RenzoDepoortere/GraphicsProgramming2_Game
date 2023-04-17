#include "stdafx.h"
#include "UberMaterial.h"

UberMaterial::UberMaterial()
	: Material<UberMaterial>(L"Effects/UberShader.fx")
{

}

void UberMaterial::SetDiffuseTexture(const std::wstring& assetFile)
{
	// Load texture
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);

	// Set variables
	SetVariable_Texture(L"gTextureDiffuse", m_pDiffuseTexture);
}
void UberMaterial::SetSpecularLevelTexture(const std::wstring& assetFile)
{
	// Load texture
	m_pSpecularLevelTexture = ContentManager::Load<TextureData>(assetFile);

	// Set variables
	SetVariable_Texture(L"gTextureSpecularIntensity", m_pSpecularLevelTexture);
}
void UberMaterial::SetNormalTexture(const std::wstring& assetFile)
{
	// Load texture
	m_pNormalTexture = ContentManager::Load<TextureData>(assetFile);

	// Set variables
	SetVariable_Texture(L"gTextureNormal", m_pNormalTexture);
}
void UberMaterial::SetEnvironmentCubeTexture(const std::wstring& assetFile)
{
	// Load texture
	m_pEnvironmentCubeTexture = ContentManager::Load<TextureData>(assetFile);

	// Set variables
	SetVariable_Texture(L"gCubeEnvironment", m_pEnvironmentCubeTexture);
}
void UberMaterial::SetOpacityMapTexture(const std::wstring& assetFile)
{
	// Load texture
	m_pOpacityMapTexture = ContentManager::Load<TextureData>(assetFile);

	// Set variables
	SetVariable_Texture(L"gTextureOpacity", m_pOpacityMapTexture);
}

void UberMaterial::InitializeEffectVariables()
{
	// Diffuse
	SetVariable_Vector(L"gColorDiffuse", XMFLOAT4{ 1.f,0.f,0.f,1.f });
	SetDiffuseTexture(L"Textures/Skulls_Diffusemap.tga");

	// Normal
	SetVariable_Scalar(L"gUseTextureNormal", true);
	SetNormalTexture(L"Textures/Skulls_Normalmap.tga");

	// Specular
	SetVariable_Vector(L"gColorSpecular", XMFLOAT4{ 1.f,1.f,0.f,1.f });
	SetVariable_Scalar(L"gUseSpecularBlinn", true);

	// Reflection
	SetVariable_Scalar(L"gReflectionStrength", 1.f);

	// Fresnel
	SetVariable_Scalar(L"gUseFresnelFalloff", true);
	SetVariable_Vector(L"gColorFresnel", XMFLOAT4{ 255 / 255.f,250 / 255.f,205 / 255.f, 1.f });
	SetVariable_Scalar(L"gFresnelPower", 7.f);
	SetVariable_Scalar(L"gFresnelMultiplier", 0.6f);
	SetVariable_Scalar(L"gFresnelHardness", 5.8f);

	// Environment
	SetEnvironmentCubeTexture(L"Textures/Sunol_Cubemap.dds");
}