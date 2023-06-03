#pragma once
#include <array>
#include "CastableComponent.h"

class HarryCharacter;

class DestroyCastableComponent final : public CastableComponent
{
public:
	// Rule of five
	DestroyCastableComponent(Spell spell, HarryCharacter* pHarry);
	virtual ~DestroyCastableComponent() override = default;

	DestroyCastableComponent(const DestroyCastableComponent& other) = delete;
	DestroyCastableComponent(DestroyCastableComponent&& other) noexcept = delete;
	DestroyCastableComponent& operator=(const DestroyCastableComponent& other) = delete;
	DestroyCastableComponent& operator=(DestroyCastableComponent&& other) noexcept = delete;

	// Functionality
	virtual void Activate(GameObject* pHarry) override;

protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;

private:
	// Member variables
	HarryCharacter* m_pHarry{ nullptr };

	bool m_HasToDelete{ false };
	RigidBodyComponent* m_pRigidBodyComponent{ nullptr };

	std::array<FMOD::Sound*, 2> m_pDestroySounds{};
};

