#pragma once
#include "Components/CastableComponent.h"

class MovingSpell final : public GameObject
{
public:
	// Rule of five
	MovingSpell(float movementSpeed, GameObject* pHarry);
	~MovingSpell() override = default;

	MovingSpell(const MovingSpell& other) = delete;
	MovingSpell(MovingSpell&& other) noexcept = delete;
	MovingSpell& operator=(const MovingSpell& other) = delete;
	MovingSpell& operator=(MovingSpell&& other) noexcept = delete;

	// Functionality
	void SetActive(CastableComponent::Spell spell, const XMFLOAT3& desiredLocation, CastableComponent* pObjectToHit);

protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;

private:
	// Member variables
	// ----------------
	float m_MovingSpeed{};
	bool m_IsActive{ false };

	CastableComponent::Spell m_CurrentSpell{};
	XMFLOAT3 m_DesiredLocation{};
	CastableComponent* m_pObjectToHit{ nullptr };

	GameObject* m_pHarry{ nullptr };
};