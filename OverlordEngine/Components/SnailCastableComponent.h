#pragma once
#include "CastableComponent.h"

class Snail;

class SnailCastableComponent final : public CastableComponent
{
public:
	// Rule of five
	SnailCastableComponent(Spell stunSpell, Spell pushSpell, Snail* pSnail);
	virtual ~SnailCastableComponent() override = default;

	SnailCastableComponent(const SnailCastableComponent& other) = delete;
	SnailCastableComponent(SnailCastableComponent&& other) noexcept = delete;
	SnailCastableComponent& operator=(const SnailCastableComponent& other) = delete;
	SnailCastableComponent& operator=(SnailCastableComponent&& other) noexcept = delete;

	// Functionality
	virtual Spell GetSpell() const override { return m_CurrentSpell; }

	virtual void Activate(GameObject* pHarry) override;

protected:
	void Initialize(const SceneContext& sceneContext) override;

private:
	// Member variables
	Snail* m_pSnail{};

	Spell m_StunSpell{};
	Spell m_PushSpell{};

	Spell m_CurrentSpell{};

	// Member functions
	void SwitchSpell();
};

