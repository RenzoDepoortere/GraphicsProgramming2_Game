#pragma once
#include "Components/CastableComponent.h"

class BeansProp final : public GameObject
{
public:
	// Rule of five
	BeansProp(float generalScale, GameObject* pHarry, CastableComponent::Spell spell, const std::wstring& resourceName = L"");
	~BeansProp() override = default;

	BeansProp(const BeansProp& other) = delete;
	BeansProp(BeansProp&& other) noexcept = delete;
	BeansProp& operator=(const BeansProp& other) = delete;
	BeansProp& operator=(BeansProp&& other) noexcept = delete;

	// Functionality
	void SetToJump() { m_HasToJump = true; }

protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;

private:
	// Variables
	// ---------
	float m_GeneralScale{};
	GameObject* m_pHarry{};
	RigidBodyComponent* m_pRigidBody{};

	CastableComponent::Spell m_Spell{};
	std::wstring m_ResourceName{};

	bool m_HasToJump{ false };
};