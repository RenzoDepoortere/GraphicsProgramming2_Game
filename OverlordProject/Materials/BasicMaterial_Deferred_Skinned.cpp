#include "stdafx.h"
#include "BasicMaterial_Deferred_Skinned.h"

BasicMaterial_Deferred_Skinned::BasicMaterial_Deferred_Skinned() :
	Material(L"Effects/Deferred/BasicEffect_Deferred_Skinned.fx")
{
}

void BasicMaterial_Deferred_Skinned::SetDiffuseMap(const std::wstring& assetFile)
{
	SetDiffuseMap(ContentManager::Load<TextureData>(assetFile));
}

void BasicMaterial_Deferred_Skinned::SetDiffuseMap(TextureData* pTextureData)
{
	SetVariable_Scalar(L"gUseDiffuseMap", pTextureData != nullptr);
	SetVariable_Texture(L"gDiffuseMap", pTextureData);
}

void BasicMaterial_Deferred_Skinned::SetNormalMap(const std::wstring& assetFile)
{
	SetNormalMap(ContentManager::Load<TextureData>(assetFile));

}

void BasicMaterial_Deferred_Skinned::SetNormalMap(TextureData* pTextureData)
{
	SetVariable_Scalar(L"gUseNormalMap", pTextureData != nullptr);
	SetVariable_Texture(L"gNormalMap", pTextureData);
}

void BasicMaterial_Deferred_Skinned::SetSpecularMap(const std::wstring& assetFile)
{
	SetSpecularMap(ContentManager::Load<TextureData>(assetFile));

}

void BasicMaterial_Deferred_Skinned::SetSpecularMap(TextureData* pTextureData)
{
	SetVariable_Scalar(L"gUseSpecularMap", pTextureData != nullptr);
	SetVariable_Texture(L"gSpecularMap", pTextureData);
}

void BasicMaterial_Deferred_Skinned::UseTransparency(bool /*enable*/)
{
	//SetTechnique(enable ? L"Default_Alpha" : L"Default");
}

void BasicMaterial_Deferred_Skinned::InitializeEffectVariables()
{
}

void BasicMaterial_Deferred_Skinned::OnUpdateModelVariables(const SceneContext& sceneContext, const ModelComponent* pModel) const
{
	//Retrieve The Animator from the ModelComponent
	const ModelAnimator* pAnimator{ pModel->GetAnimator() };

	//Make sure the animator is not NULL (ASSERT_NULL_)
	ASSERT_NULL_(pAnimator);

	//Retrieve the BoneTransforms from the Animator
	std::vector<XMFLOAT4X4> boneTransforms{ pAnimator->GetBoneTransforms() };

	//Set the 'gBones' variable of the effect (MatrixArray) > BoneTransforms
	SetVariable_MatrixArray(L"gBones", reinterpret_cast<float*>(boneTransforms.data()), static_cast<UINT>(boneTransforms.size()));

	// Set lightDirection
	SetVariable_Vector(L"gLightDirection", sceneContext.pLights->GetDirectionalLight().direction);
}
