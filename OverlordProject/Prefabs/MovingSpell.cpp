#include "stdafx.h"
#include "MovingSpell.h"

#include "Prefabs/CubePrefab.h"

MovingSpell::MovingSpell(float movementSpeed, GameObject* pHarry, CastableComponent::Spell spell, CastableComponent* pObjectToHit)
	: m_MovingSpeed{ movementSpeed }
	, m_pHarry{ pHarry }
	, m_CurrentSpell{spell}
	, m_pObjectToHit{ pObjectToHit }
{
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
		// If not deleting, return
		if (m_HasToBeDeleted) return;

		// If was desiredObject
		if (action == PxTriggerAction::ENTER && pOther == m_pObjectToHit->GetGameObject())
		{
			// Activate object
			m_pObjectToHit->Activate(m_pHarry);

			// Set for deletion
			m_HasToBeDeleted = true;
		}
	});
}

void MovingSpell::Update(const SceneContext& sceneContext)
{
	// Calculate new position
	const XMFLOAT3 currentPosition{ GetTransform()->GetPosition() };
	const XMVECTOR currentVector{ XMLoadFloat3(&currentPosition) };
	const XMVECTOR desiredVector{ XMLoadFloat3(&m_pObjectToHit->GetTransform()->GetWorldPosition()) };

	const XMVECTOR desiredDirection{ XMVector3Normalize(XMVectorSubtract(desiredVector, currentVector)) };
	const XMVECTOR newPos{ XMVectorAdd(currentVector, XMVectorScale(desiredDirection, m_MovingSpeed * sceneContext.pGameTime->GetElapsed())) };

	// Set new position
	GetTransform()->Translate(newPos);

	// Delete, if necessary
	if (m_HasToBeDeleted)
	{
		GetParent()->RemoveChild(this, true);
	}
}