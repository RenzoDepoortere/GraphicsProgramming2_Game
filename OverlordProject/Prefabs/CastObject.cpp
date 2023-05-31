#include "stdafx.h"
#include "CastObject.h"

#include "Prefabs/CubePrefab.h"
#include "Prefabs/Character.h"

#include "Materials/BasicMaterial_Deferred.h"

CastObject::CastObject(Character* pHarry)
	: m_pHarry{ pHarry }
{
}

void CastObject::Initialize(const SceneContext& /*sceneContext*/)
{
	// Particles
	// ---------
	ParticleEmitterSettings settings{};
	settings.velocity = { 0.f, -0.25f, 0.f };
	settings.minSize = 0.3f;
	settings.maxSize = 0.3f;
	settings.minEnergy = 1.f;
	settings.maxEnergy = 1.f;
	settings.minScale = 0.75f;
	settings.maxScale = 0.75f;
	settings.minEmitterRadius = -0.1f;
	settings.maxEmitterRadius = 0.1f;
	settings.color = { 1.f, 1.f, 1.f, .6f };

	m_pEmitter = AddComponent(new ParticleEmitterComponent(L"Textures/CastingObject/Casting.png", settings, 100));

	// Spell Mesh
	// ----------
	auto pObject{ AddChild(new GameObject{}) };
	m_pSpellObject = pObject->AddComponent(new ModelComponent{L"Meshes/Props/Spell/SpellPlane.ovm"});

	// Diffindo
	BasicMaterial_Deferred* pMaterial{ MaterialManager::Get()->CreateMaterial<BasicMaterial_Deferred>() };
	pMaterial->SetDiffuseMap(L"Textures/Spells/Diffindo.png");

	m_pMaterials[static_cast<int>(CastableComponent::Diffindo)] = pMaterial;

	// Rictusempra
	pMaterial = MaterialManager::Get()->CreateMaterial<BasicMaterial_Deferred>();
	pMaterial->SetDiffuseMap(L"Textures/Spells/Rictusempra.png");

	m_pMaterials[static_cast<int>(CastableComponent::Rictusempra)] = pMaterial;

	// Spongify
	pMaterial = MaterialManager::Get()->CreateMaterial<BasicMaterial_Deferred>();
	pMaterial->SetDiffuseMap(L"Textures/Spells/Spongify.png");

	m_pMaterials[static_cast<int>(CastableComponent::Spongify)] = pMaterial;


	// Set default
	m_CurrentSpell = CastableComponent::Diffindo;
	m_pSpellObject->SetMaterial(m_pMaterials[0]);

	// Transform
	m_pSpellObject->GetTransform()->Rotate(0.f, 0.f, 90.f);
	m_pSpellObject->GetTransform()->Scale(0.5f);
}

void CastObject::Update(const SceneContext& /*sceneContext*/)
{
	RotateObject();
	HandleSpell();
}

void CastObject::RotateObject()
{
	// Get character rotation
	const float totalYaw{ m_pHarry->GetTotalYaw() };
	float angleBuffer{ -90.0f };

	// Set rotation
	m_pSpellObject->GetTransform()->Rotate(0.f, totalYaw + angleBuffer, 90.f);
}
void CastObject::HandleSpell()
{
	// If is castable
	if (m_IsCastable)
	{
		// Show spell
		m_pSpellObject->GetTransform()->Translate(XMFLOAT3{});

		// Set spell
		m_pSpellObject->SetMaterial(m_pMaterials[static_cast<int>(m_CurrentSpell)]);
	}
	// Else
	else
	{
		// Hide spell
		m_pSpellObject->GetTransform()->Translate(XMFLOAT3{ 0.f, -50.f, 0.f });
	}
}