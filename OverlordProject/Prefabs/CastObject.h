#pragma once
#include "Components/CastableComponent.h"

class Character;
class BasicMaterial_Deferred;

class CastObject final : public GameObject
{
public:
	// Rule of five
	CastObject(Character* pHarry);
	~CastObject() override = default;

	CastObject(const CastObject& other) = delete;
	CastObject(CastObject&& other) noexcept = delete;
	CastObject& operator=(const CastObject& other) = delete;
	CastObject& operator=(CastObject&& other) noexcept = delete;

	// Functionality
	void SetSpell(CastableComponent::Spell spell) { m_CurrentSpell = spell; }
	void SetIsCastable(bool isCastable) { m_IsCastable = isCastable; }

protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;

private:
	// Variables
	// ---------
	Character* m_pHarry{};
	ModelComponent* m_pSpellObject{};
	
	bool m_IsCastable{};
	CastableComponent::Spell m_CurrentSpell{};
	BasicMaterial_Deferred* m_pMaterials[CastableComponent::Spell::NR_SPELLS]{};

	// Member-Functions
	// ----------------
	void RotateObject();
	void HandleSpell();
};