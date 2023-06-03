#include "stdafx.h"
#include "Bean.h"

#include "Materials/BasicMaterial_Deferred.h"

#include "Prefabs/HarryCharacter.h"
#include "Prefabs/Character.h"

Bean::Bean(float generalScale, HarryCharacter* pHarry, const XMFLOAT3& spawnLocation, const XMFLOAT3& forceDirection, bool playSpawnSound)
	: m_GeneralScale{ generalScale }
	, m_pHarry{ pHarry }
	, m_SpawnLocation{ spawnLocation }
	, m_NrBeans{ 16 }
	, m_ForceDirection{ forceDirection }
	, m_PlaySpawnSound{ playSpawnSound }
	, m_Force{ 5.f }
	, m_RotationSpeed{ 50.f }
	, m_NrBounceSounds{ 3 }
{
}

void Bean::Initialize(const SceneContext& /*sceneContext*/)
{
	// Sound
	FMOD::System* pFmod = SoundManager::Get()->GetSystem();

	pFmod->createSound("Resources/Sounds/Props/Beans/Bean_Spawn_1.wav", FMOD_DEFAULT, nullptr, &m_pSpawnSound[0]);
	pFmod->createSound("Resources/Sounds/Props/Beans/Bean_Spawn_2.wav", FMOD_DEFAULT, nullptr, &m_pSpawnSound[1]);
	pFmod->createSound("Resources/Sounds/Props/Beans/Bean_Spawn_3.wav", FMOD_DEFAULT, nullptr, &m_pSpawnSound[2]);

	if (m_PlaySpawnSound)
	{
		const int randomIdx{ rand() % static_cast<int>(m_pSpawnSound.size()) };
		m_pHarry->SetSpellHitSoundToPlay(m_pSpawnSound[randomIdx]);
	}

	pFmod->createSound("Resources/Sounds/Props/Beans/Bean_PickUp.wav", FMOD_DEFAULT, nullptr, &m_pPickUpSound);

	pFmod->createSound("Resources/Sounds/Props/Beans/Bean_Bounce.wav", FMOD_DEFAULT, nullptr, &m_pBounceSound);

	// Mesh
	auto pObject{ AddChild(new GameObject{}) };
	ModelComponent* pModel{ pObject->AddComponent(new ModelComponent{L"Meshes/Props/Beans/Bean.ovm"}) };

	// Random texture
	std::wstring baseTextureName{ L"Textures/Props/Beans/bean_" };
	const int randID{ rand() % m_NrBeans };
	baseTextureName += std::to_wstring(randID) + L".png";

	BasicMaterial_Deferred* pMaterial{ MaterialManager::Get()->CreateMaterial<BasicMaterial_Deferred>() };
	pMaterial->SetDiffuseMap(baseTextureName);
	pModel->SetMaterial(pMaterial);

	// Transform
	pObject->GetTransform()->Scale(m_GeneralScale);

	// Collision
	PxMaterial* pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.2f, 0.7f);

	RigidBodyComponent* pActor = AddComponent(new RigidBodyComponent(false));
	PxConvexMesh* pPxConvexMesh = ContentManager::Load<PxConvexMesh>(L"Meshes/Props/Beans/Bean.ovpc");
	pActor->AddCollider(PxConvexMeshGeometry{ pPxConvexMesh, PxMeshScale{m_GeneralScale} }, *pDefaultMaterial);
	
	pActor->SetConstraint(RigidBodyConstraint::RotX, false);
	pActor->SetConstraint(RigidBodyConstraint::RotY, false);
	pActor->SetConstraint(RigidBodyConstraint::RotZ, false);

	// Shoot in direction
	const float minForce{ 0.3f };
	const float maxForce{ 0.7f };

	const float randomForce{ MathHelper::randF(minForce, maxForce) };
	m_ForceDirection.y += randomForce;

	const XMVECTOR force{ XMVectorScale(XMVector3Normalize(XMLoadFloat3(&m_ForceDirection)), m_Force) };
	XMFLOAT3 calculatedForce{};
	XMStoreFloat3(&calculatedForce, force);

	pActor->AddForce(calculatedForce, PxForceMode::eIMPULSE);

	//// Bounce sound
	//pObject = AddChild(new GameObject{});
	//pActor = pObject->AddComponent(new RigidBodyComponent(true));
	//pPxConvexMesh = ContentManager::Load<PxConvexMesh>(L"Meshes/Props/Beans/Bean.ovpc");
	//pActor->AddCollider(PxConvexMeshGeometry{ pPxConvexMesh, PxMeshScale{m_GeneralScale} }, *pDefaultMaterial, true);

	//// On collision
	//pObject->SetOnTriggerCallBack([=](GameObject*, GameObject* /*pOther*/, PxTriggerAction /*action*/)
	//{
	//	if (m_NrBounceSounds == 0) return;
	//	--m_NrBounceSounds;

	//	m_pHarry->SetSpellHitSoundToPlay(m_pBounceSound);
	//});
}

void Bean::Update(const SceneContext& sceneContext)
{
	if (m_IsActivated == false) return;

	// Rotate
	m_TotalYaw += m_RotationSpeed * sceneContext.pGameTime->GetElapsed();
	GetTransform()->Rotate(0.f, m_TotalYaw, 0.f);

	// Check if Harry close enough
	const float minSqrdDistanceBetwn{ 2.5f };

	const XMFLOAT3 harryPos{ m_pHarry->GetCharacter()->GetTransform()->GetWorldPosition()};
	const XMFLOAT3 direction{ MathHelper::DirectionTo(GetTransform()->GetWorldPosition(), harryPos, false) };
	const float sqrdDistance{ XMVectorGetX(XMVector3LengthSq(XMLoadFloat3(&direction))) };

	if (sqrdDistance <= minSqrdDistanceBetwn)
	{
		// Sound
		m_pHarry->SetSpellHitSoundToPlay(m_pPickUpSound);

		// Particle

		// Add bean
		m_pHarry->AddBean();

		// Delete
		GetTransform()->Translate(XMFLOAT3{});
		m_IsActivated = false;
		//GetParent()->RemoveChild(this, true);
	}
}