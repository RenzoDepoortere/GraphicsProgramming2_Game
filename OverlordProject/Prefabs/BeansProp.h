#pragma once
#include "Components/CastableComponent.h"

class BeansProp final : public GameObject
{
public:
	// Rule of five
	BeansProp(float generalScale, CastableComponent::Spell spell);
	~BeansProp() override = default;

	BeansProp(const BeansProp& other) = delete;
	BeansProp(BeansProp&& other) noexcept = delete;
	BeansProp& operator=(const BeansProp& other) = delete;
	BeansProp& operator=(BeansProp&& other) noexcept = delete;

protected:
	void Initialize(const SceneContext& sceneContext) override;

private:
	// Variables
	// ---------
	float m_GeneralScale{};
	CastableComponent::Spell m_Spell{};
};