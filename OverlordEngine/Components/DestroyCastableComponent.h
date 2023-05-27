#pragma once
#include "CastableComponent.h"

class DestroyCastableComponent final : public CastableComponent
{
public:
	// Rule of five
	DestroyCastableComponent(Spell spell);
	virtual ~DestroyCastableComponent() override = default;

	DestroyCastableComponent(const DestroyCastableComponent& other) = delete;
	DestroyCastableComponent(DestroyCastableComponent&& other) noexcept = delete;
	DestroyCastableComponent& operator=(const DestroyCastableComponent& other) = delete;
	DestroyCastableComponent& operator=(DestroyCastableComponent&& other) noexcept = delete;

	// Functionality
	virtual void Activate(const XMFLOAT3& spellPosition) override;

protected:
	void Initialize(const SceneContext& sceneContext) override;

private:
	// Member variables
	bool m_HasBeenActivated{ false };
	RigidBodyComponent* m_pRigidBodyComponent{ nullptr };
};

