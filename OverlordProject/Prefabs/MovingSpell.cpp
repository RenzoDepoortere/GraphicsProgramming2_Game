#include "stdafx.h"
#include "MovingSpell.h"

#include "Prefabs/CubePrefab.h"

MovingSpell::MovingSpell(float movementSpeed, GameObject* pHarry)
	: m_MovingSpeed{ movementSpeed }
	, m_pHarry{ pHarry }
{
}

void MovingSpell::SetActive(CastableComponent::Spell spell, const XMFLOAT3& desiredLocation, CastableComponent* pObjectToHit)
{
	m_CurrentSpell = spell;
	m_DesiredLocation = desiredLocation;
	m_pObjectToHit = pObjectToHit;

	m_IsActive = true;
}

void MovingSpell::Initialize(const SceneContext& /*sceneContext*/)
{
	const XMFLOAT3 boxDimension{ 0.3f, 0.3f, 0.3f };

	// Particles

	// Temp cube
	GameObject* pCube{ AddChild(new CubePrefab{1.0f, 1.0f, 1.0f, static_cast<XMFLOAT4>(Colors::GreenYellow)}) };
	pCube->GetTransform()->Scale(boxDimension);

	// Rigidbody
	PxMaterial* pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	auto pRigidbody = AddComponent(new RigidBodyComponent{});
	pRigidbody->SetKinematic(true);
	pRigidbody->AddCollider(PxBoxGeometry{ boxDimension.x / 2.f, boxDimension.y / 2.f, boxDimension.z / 2.f }, *pDefaultMaterial, true);

	// On collision
	SetOnTriggerCallBack([=](GameObject*, GameObject* pOther, PxTriggerAction action)
	{
		// If not active, return
		if (m_IsActive == false) return;

		// If was desiredObject
		if (action == PxTriggerAction::ENTER && pOther == m_pObjectToHit->GetGameObject())
		{
			// Activate object
			m_pObjectToHit->Activate(m_pHarry);

			// "Delete"
			GetTransform()->Translate(XMFLOAT3{});
			m_IsActive = false;
		}
	});
}

void MovingSpell::Update(const SceneContext& sceneContext)
{
	if (m_IsActive == false) return;

	// Calculate new position
	const XMFLOAT3 currentPosition{ GetTransform()->GetPosition() };
	const XMVECTOR currentVector{ XMLoadFloat3(&currentPosition) };
	const XMVECTOR desiredVector{ XMLoadFloat3(&m_DesiredLocation) };

	const XMVECTOR desiredDirection{ XMVector3Normalize(XMVectorSubtract(desiredVector, currentVector)) };
	const XMVECTOR newPos{ XMVectorAdd(currentVector, XMVectorScale(desiredDirection, m_MovingSpeed * sceneContext.pGameTime->GetElapsed())) };

	// Set new position
	GetTransform()->Translate(newPos);
}