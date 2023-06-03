#include "stdafx.h"
#include "BeansProp.h"

#include "Components/BeansCastableComponent.h"

#include "Prefabs/CubePrefab.h"
#include "Prefabs/HarryCharacter.h"

#include "Materials/BasicMaterial_Deferred.h"

BeansProp::BeansProp(float generalScale, HarryCharacter* pHarry, CastableComponent::Spell spell, float jumpForce, float torqueForce,
					float scaleMultiplier, const std::vector<FMOD::Sound*>& pHitSounds, const std::wstring& resourceName)
	: m_GeneralScale{ generalScale }
	, m_pHarry{ pHarry }
	, m_Spell{ spell }
	, m_JumpForce{ jumpForce }
	, m_TorqueForce{ torqueForce }
	, m_ScaleMultiplier{ scaleMultiplier }
	, m_pHitSounds{ pHitSounds }
	, m_ResourceName{ resourceName }
{
}

void BeansProp::Initialize(const SceneContext& /*sceneContext*/)
{
	// castableComponent
	AddComponent(new BeansCastableComponent{ m_GeneralScale, m_pHarry, m_Spell, this });

	// If no resource given
	if (m_ResourceName == L"")
	{
		// Box
		AddChild(new CubePrefab{ 1.0f, 1.0f, 1.0f, static_cast<XMFLOAT4>(Colors::AliceBlue) });

		// Collision
		PxMaterial* pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

		RigidBodyComponent* pActor = AddComponent(new RigidBodyComponent(false));
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

		pObject->GetTransform()->Scale(m_GeneralScale * m_ScaleMultiplier);

		// Collision
		PxMaterial* pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

		m_pRigidBody = AddComponent(new RigidBodyComponent(false));
		PxConvexMesh* pPxConvexMesh = ContentManager::Load<PxConvexMesh>(L"Meshes/Props/" + m_ResourceName + L"/" + m_ResourceName + L".ovpc");
		m_pRigidBody->AddCollider(PxConvexMeshGeometry{ pPxConvexMesh, PxMeshScale{m_GeneralScale * m_ScaleMultiplier} }, *pDefaultMaterial);

		m_pRigidBody->SetConstraint(RigidBodyConstraint::TransX, false);
		m_pRigidBody->SetConstraint(RigidBodyConstraint::TransZ, false);
	}
}

void BeansProp::Update(const SceneContext& /*sceneContext*/)
{
	if (m_HasToJump)
	{
		m_HasToJump = false;

		// Force
		XMFLOAT3 direction{ 0.f, 1.f, 0.f };
		XMVECTOR forceVector{ XMVectorScale(XMLoadFloat3(&direction), m_JumpForce) };

		XMFLOAT3 desiredForce{};
		XMStoreFloat3(&desiredForce, forceVector);

		m_pRigidBody->AddForce(desiredForce, PxForceMode::eIMPULSE);

		// Torque
		direction = { 0.f, 0.f, 1.f };
		forceVector = XMVectorScale(XMLoadFloat3(&direction), m_TorqueForce);

		XMStoreFloat3(&desiredForce, forceVector);
		m_pRigidBody->AddTorque(desiredForce, PxForceMode::eIMPULSE);

		// Play sound
		const int randomIdx{ rand() % static_cast<int>(m_pHitSounds.size()) };
		m_pHarry->SetSpellHitSoundToPlay(m_pHitSounds[randomIdx]);
	}
}