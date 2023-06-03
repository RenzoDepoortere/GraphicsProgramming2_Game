#include "stdafx.h"
#include "PropsPrefab.h"

#include "Prefabs/Vines.h"
#include "Prefabs/BeansProp.h"

#include "Components/CastableComponent.h"

PropsPrefab::PropsPrefab(float generalScale, HarryCharacter* pHarry)
	: m_GeneralScale{ generalScale }
	, m_pHarry{ pHarry }
{
}

void PropsPrefab::Initialize(const SceneContext& /*sceneContext*/)
{
	// Vines
	GetScene()->AddChild(new Vines(m_GeneralScale, m_pHarry));

	// Bean props
	// ==========
	FMOD::System* pFmod{ SoundManager::Get()->GetSystem() };

	// Chests
	// ------
	std::vector<FMOD::Sound*> pSounds{};
	FMOD::Sound* pSound{};

	pFmod->createSound("Resources/Sounds/Spell/Hit/Spell_AlohomoraHit.wav", FMOD_DEFAULT, nullptr, &pSound);
	pSounds.emplace_back(pSound);

	// Chest 1
	GameObject* pProp{ GetScene()->AddChild(new BeansProp{m_GeneralScale, m_pHarry, CastableComponent::Alahomora, 5.f, 2.f, pSounds, L"Chest"})};
	pProp->GetTransform()->Translate(20.f, -7.6f, -83.4f);
	pProp->GetTransform()->Rotate(0.f, 90.f, 0.f);

	// Chests 2
	pProp = GetScene()->AddChild(new BeansProp{m_GeneralScale, m_pHarry, CastableComponent::Alahomora, 5.f, 2.f, pSounds, L"Chest"});
	pProp->GetTransform()->Translate(11.f, -7.6f, -62.5f);
	pProp->GetTransform()->Rotate(0.f, 180.f, 0.f);

	pSounds.clear();

	// Pots
	// ----
	pFmod->createSound("Resources/Sounds/Props/Pot/Pot_Breaking.wav", FMOD_DEFAULT, nullptr, &pSound);
	pSounds.emplace_back(pSound);

	// Pot 1
	pProp = GetScene()->AddChild(new BeansProp{ m_GeneralScale, m_pHarry, CastableComponent::Diffindo, 2.5f, 4.f, pSounds, L"Pot" });
	pProp->GetTransform()->Translate(21.9f, -6.2f, -55.f);

	// Pot 2
	pProp = GetScene()->AddChild(new BeansProp{ m_GeneralScale, m_pHarry, CastableComponent::Diffindo, 2.5f, 4.f, pSounds, L"Pot" });
	pProp->GetTransform()->Translate(-2.2f, -8.f, -22.8f);

	pSounds.clear();

	// Cauldron
	pFmod->createSound("Resources/Sounds/Props/Cauldron/Cauldron.wav", FMOD_DEFAULT, nullptr, &pSound);
	pSounds.emplace_back(pSound);

	pProp = GetScene()->AddChild(new BeansProp{ m_GeneralScale, m_pHarry, CastableComponent::Diffindo, 2.5f, 1.f, pSounds, L"Cauldron" });
	pProp->GetTransform()->Translate(3.9f, -7.6f, -56.7f);

	pSounds.clear();
}