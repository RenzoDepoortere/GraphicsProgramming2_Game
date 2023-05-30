#include "stdafx.h"
#include "BeansProp.h"

#include "Components/BeansCastableComponent.h"

#include "Prefabs/CubePrefab.h"

#include "Materials/BasicMaterial_Deferred.h"

BeansProp::BeansProp(float generalScale, GameObject* pHarry, CastableComponent::Spell spell, const std::wstring& resourceName)
	: m_GeneralScale{ generalScale }
	, m_pHarry{ pHarry }
	, m_Spell{ spell }
	, m_ResourceName{ resourceName }
{
}

void BeansProp::Initialize(const SceneContext& /*sceneContext*/)
{
	// castableComponent
	AddComponent(new BeansCastableComponent{ m_GeneralScale, m_pHarry, m_Spell });

	// If no resource given
	if (m_ResourceName == L"")
	{
		// Box
		AddChild(new CubePrefab{ 1.0f, 1.0f, 1.0f, static_cast<XMFLOAT4>(Colors::AliceBlue) });

		// Collision
		PxMaterial* pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

		RigidBodyComponent* pActor = AddComponent(new RigidBodyComponent(true));
		pActor->AddCollider(PxBoxGeometry{ 0.5f, 0.5f, 0.5f }, *pDefaultMaterial);
	}
	// Else, if resource given
	else
	{
		// Mesh
		auto pObject{ AddChild(new GameObject{}) };
		ModelComponent* pModel{ pObject->AddComponent(new ModelComponent{L"Meshes/Props/" + m_ResourceName + L"/" + m_ResourceName + L".ovm"}) };

		BasicMaterial_Deferred* pMaterial{ MaterialManager::Get()->CreateMaterial<BasicMaterial_Deferred>() };
		pMaterial->SetDiffuseMap(L"Textures/Props/" + m_ResourceName + L"/" + m_ResourceName + L".png");
		pModel->SetMaterial(pMaterial);

		pObject->GetTransform()->Scale(m_GeneralScale * 2.f);

		// Collision
		PxMaterial* pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

		RigidBodyComponent* pActor = AddComponent(new RigidBodyComponent(true));
		PxConvexMesh* pPxConvexMesh = ContentManager::Load<PxConvexMesh>(L"Meshes/Props/" + m_ResourceName + L"/" + m_ResourceName + L".ovpc");
		pActor->AddCollider(PxConvexMeshGeometry{ pPxConvexMesh, PxMeshScale{m_GeneralScale * 2.f} }, *pDefaultMaterial);
	}
}