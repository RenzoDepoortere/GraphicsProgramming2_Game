#pragma once
class HarryCharacter;
#include "Components/CastableComponent.h"

class MovingSpell final : public GameObject
{
public:
	// Rule of five
	MovingSpell(float movementSpeed, HarryCharacter* pHarry, CastableComponent::Spell spell, CastableComponent* pObjectToHit, const XMFLOAT3& hitPosition = {});
	~MovingSpell() override = default;

	MovingSpell(const MovingSpell& other) = delete;
	MovingSpell(MovingSpell&& other) noexcept = delete;
	MovingSpell& operator=(const MovingSpell& other) = delete;
	MovingSpell& operator=(MovingSpell&& other) noexcept = delete;

protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;

private:
	// Member variables
	// ----------------
	float m_MovingSpeed{};
	bool m_HasToBeDeleted{ false };

	CastableComponent::Spell m_CurrentSpell{};
	
	CastableComponent* m_pObjectToHit{ nullptr };
	XMFLOAT3 m_HitPosition{};

	HarryCharacter* m_pHarry{ nullptr };
};