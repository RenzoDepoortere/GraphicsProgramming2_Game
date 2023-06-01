#pragma once

class BasicMaterial_Deferred;

class HUD final : public GameObject
{
public:
	// Rule of five
	HUD(int maxHP);
	~HUD() override = default;

	HUD(const HUD& other) = delete;
	HUD(HUD&& other) noexcept = delete;
	HUD& operator=(const HUD& other) = delete;
	HUD& operator=(HUD&& other) noexcept = delete;

	// Functionality
	void SetHP(int hpAmount);
	void AddBean();

protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;

private:
	// Variables
	int m_MaxHP{};
	std::vector<SpriteComponent*> m_pHealthIcons{};


	SpriteComponent* m_pBeanHUD{ nullptr };


	int m_NrBeans{};
	float m_HoldTime{};
	bool m_BeanGained{ false };
	bool m_Dissapearing{ false };
};