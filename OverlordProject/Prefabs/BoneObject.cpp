#include "stdafx.h"
#include "BoneObject.h"

BoneObject::BoneObject(BaseMaterial* pMaterial, float length)
	: m_pMaterial{ pMaterial }
	, m_Length{ length }
{
}

void BoneObject::Initialize(const SceneContext&)
{
	// Create empty gameObject
	GameObject* pEmpty{ new GameObject{} };
	this->AddChild(pEmpty);

	// Add modelComponent
	ModelComponent* pModelComponent{ new ModelComponent{L"Meshes/Bone.ovm"} };
	pModelComponent->SetMaterial(m_pMaterial);

	pEmpty->AddComponent(pModelComponent);

	// Transform gameObject
	pEmpty->GetTransform()->Scale(m_Length);
	pEmpty->GetTransform()->Rotate(0.f, -90.f, 0.f, true);
}

void BoneObject::AddBone(BoneObject* pBone)
{
	// Transform bone
	pBone->GetTransform()->Translate(m_Length, 0.f, 0.f);

	// Add bone to this
	this->AddChild(pBone);
}

void BoneObject::CalculateBindPose()
{
	// Calculate own bindPose
	// ----------------------
	const XMFLOAT4X4 worldMatrix{ GetTransform()->GetWorld() };
	const XMMATRIX inversedWorldMatrix{ XMMatrixInverse(nullptr, XMLoadFloat4x4(&worldMatrix)) };

	XMStoreFloat4x4(&m_BindPose, inversedWorldMatrix);

	// Calculate children's bindPose
	// -----------------------------
	for (const auto& currentChild : GetChildren<BoneObject>())
	{
		currentChild->CalculateBindPose();
	}
}