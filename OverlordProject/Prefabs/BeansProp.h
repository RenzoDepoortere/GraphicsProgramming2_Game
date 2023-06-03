#pragma once
#include "Components/CastableComponent.h"

class HarryCharacter;

class BeansProp final : public GameObject
{
public:
	// Rule of five
	BeansProp(float generalScale, HarryCharacter* pHarry, CastableComponent::Spell spell, float jumpForce,
			  float scaleMultiplier, const std::vector<FMOD::Sound*>& pHitSounds, const std::wstring& resourceName = L"");
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
	HarryCharacter* m_pHarry{};
	RigidBodyComponent* m_pRigidBody{};

	float m_ScaleMultiplier{};

	std::vector<FMOD::Sound*> m_pHitSounds{};

	CastableComponent::Spell m_Spell{};
	std::wstring m_ResourceName{};

	float m_JumpForce{};
	bool m_HasToJump{ false };
};