#include "stdafx.h"
#include "MovingSpell.h"

#include "Prefabs/HarryCharacter.h"
#include "Prefabs/CubePrefab.h"
#include "Prefabs/Character.h"

MovingSpell::MovingSpell(float movementSpeed, HarryCharacter* pHarry, CastableComponent::Spell spell, CastableComponent* pObjectToHit, const XMFLOAT3& hitPosition)
	: m_MovingSpeed{ movementSpeed }
	, m_pHarry{ pHarry }
	, m_CurrentSpell{spell}
	, m_pObjectToHit{ pObjectToHit }
	, m_HitPosition{ hitPosition }
{
}

void MovingSpell::Initialize(const SceneContext& sceneContext)
{
	const XMFLOAT3 boxDimension{ 0.3f, 0.3f, 0.3f };

	// Particles
	// ---------
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
	case CastableComponent::Alahomora:
		spellColor = XMFLOAT4{ 0.f, 0.f, 0.9f, 0.6f };
		break;
	}
	settings.color = spellColor;

	AddComponent(new ParticleEmitterComponent(L"Textures/CastingObject/Casting.png", settings, 100));

	// Rigidbody
	// ---------
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
			// Sound
			FMOD::System* pFmod{ SoundManager::Get()->GetSystem() };
			FMOD::Sound* pSound{ nullptr };
			pFmod->createSound("Resources/Sounds/Spell/Hit/Spell_Hit.wav", FMOD_DEFAULT, nullptr, &pSound);
			m_pHarry->SetSpellHitSoundToPlay(pSound);

			// Activate object
			m_pObjectToHit->Activate(m_pHarry->GetCharacter());

			// Set for deletion
			m_HasToBeDeleted = true;
		}
	});

	// Lights
	// ------
	Light light = {};
	const auto& worldPos{ GetTransform()->GetWorldPosition() };
	const XMFLOAT4 lightPos{ worldPos.x, worldPos.y, worldPos.z, 1.f };

	light.isEnabled = true;
	light.position = lightPos;
	light.color = spellColor;
	light.intensity = 0.25f;
	light.range = 2.5f;
	light.type = LightType::Point;

	m_LightID = sceneContext.pLights->AddLight(light);
}

void MovingSpell::Update(const SceneContext& sceneContext)
{
	// SpellPos
	// --------

	// Calculate new position
	const XMFLOAT3 currentPosition{ GetTransform()->GetPosition() };
	const XMVECTOR currentVector{ XMLoadFloat3(&currentPosition) };
	
	XMVECTOR desiredVector{ XMLoadFloat3(&m_HitPosition) };
	if (MathHelper::XMFloat3Equals(m_HitPosition, {}))
	{
		desiredVector = XMLoadFloat3(&m_pObjectToHit->GetTransform()->GetWorldPosition());
	}

	const XMVECTOR desiredDirection{ XMVector3Normalize(XMVectorSubtract(desiredVector, currentVector)) };
	const XMVECTOR newPos{ XMVectorAdd(currentVector, XMVectorScale(desiredDirection, m_MovingSpeed * sceneContext.pGameTime->GetElapsed())) };

	// Set new position
	GetTransform()->Translate(newPos);

	// LightPos
	// --------
	const auto& worldPos{ GetTransform()->GetWorldPosition() };
	const XMFLOAT4 lightPos{ worldPos.x, worldPos.y, worldPos.z, 1.f };

	sceneContext.pLights->GetLight(m_LightID).position = lightPos;

	// Delete, if necessary
	// --------------------
	if (m_HasToBeDeleted)
	{
		// Light
		sceneContext.pLights->RemoveLight(m_LightID);

		// Spell
		GetParent()->RemoveChild(this, true);
	}
}