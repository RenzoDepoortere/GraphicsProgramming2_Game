#include "stdafx.h"
#include "DiffuseMaterial_Shadow_Skinned.h"

DiffuseMaterial_Shadow_Skinned::DiffuseMaterial_Shadow_Skinned():
	Material(L"Effects/Shadow/PosNormTex3D_Shadow_Skinned.fx")
{}

void DiffuseMaterial_Shadow_Skinned::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gDiffuseMap", m_pDiffuseTexture);
}
void DiffuseMaterial_Shadow_Skinned::SetDiffuseTexture(TextureData* pTextureData)
{
	// Set texture
	m_pDiffuseTexture = pTextureData;

	// Set variables
	SetVariable_Texture(L"gDiffuseMap", m_pDiffuseTexture);
}

void DiffuseMaterial_Shadow_Skinned::InitializeEffectVariables()
{
}

void DiffuseMaterial_Shadow_Skinned::OnUpdateModelVariables(const SceneContext& sceneContext, const ModelComponent* pModel) const
{
	/*
	 * TODO_W8
	 * Update The Shader Variables
	 * 1. Update the LightWVP > Used to Transform a vertex into Light clipping space
	 * 	LightWVP = model_world * light_viewprojection
	 * 	(light_viewprojection [LightVP] can be acquired from the ShadowMapRenderer)
	 *
	 * 2. Update the ShadowMap texture
	 *
	 * 3. Update the Light Direction (retrieve the direction from the LightManager > sceneContext)
	 *
	 * 4. Update Bones
	*/

	//Update Shadow Variables
	const auto pShadowMapRenderer = ShadowMapRenderer::Get();

	//Update the LightWVP
	const XMFLOAT4X4 lightVP{ pShadowMapRenderer->GetLightVP() };
	const XMFLOAT4X4 modelWorld{ pModel->GetTransform()->GetWorld() };

	const XMMATRIX lightVWPMatrix{ XMMatrixMultiply(XMLoadFloat4x4(&modelWorld), XMLoadFloat4x4(&lightVP)) };
	XMFLOAT4X4 lightVWP{};
	XMStoreFloat4x4(&lightVWP, lightVWPMatrix);

	auto boneTransforms{ pModel->GetAnimator()->GetBoneTransforms() };

	SetVariable_Matrix(L"gWorldViewProj_Light", lightVWP);

	// Update the ShadowMap texture
	SetVariable_Texture(L"gShadowMap", pShadowMapRenderer->GetShadowMap());

	// Update the Light Direction
	SetVariable_Vector(L"gLightDirection", sceneContext.pLights->GetDirectionalLight().direction);

	// Update Bones
	SetVariable_MatrixArray(L"gBones", reinterpret_cast<float*>(boneTransforms.data()), static_cast<UINT>(boneTransforms.size()));
}
