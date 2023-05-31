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
	ParticleEmitterSettings settings{};
	settings.velocity = { 0.f, -0.25f, 0.f };
	settings.minSize = 0.5;
	settings.maxSize = 0.5f;
	settings.minEnergy = 1.f;
	settings.maxEnergy = 1.f;
	settings.minScale = 0.75f;
	settings.maxScale = 0.75f;
	settings.minEmitterRadius = -0.1f;
	settings.maxEmitterRadius = 0.1f;
	
	XMFLOAT4 spellColor{};
	switch (m_CurrentSpell)
	{
	case CastableComponent::Diffindo:
		spellColor = XMFLOAT4{ 0.f, 0.9f, 1.f, 0.6f };
		break;
	case CastableComponent::Rictusempra:
		spellColor = XMFLOAT4{ 0.9f, 0.f, 0.f, 0.6f };
		break;
	case CastableComponent::Spongify:
		spellColor = XMFLOAT4{ 1.f, 0.5f, 0.f, 0.6f };
		break;
	}
	settings.color = spellColor;

	AddComponent(new ParticleEmitterComponent(L"Textures/CastingObject/Casting.png", settings, 100));

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