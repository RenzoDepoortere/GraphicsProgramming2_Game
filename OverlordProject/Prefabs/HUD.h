#pragma once

class BasicMaterial_Deferred;

class HUD final : public GameObject
{
public:
	// Rule of five
	HUD() = default;
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
	//void Draw(const SceneContext& sceneContext) override;

private:
	// Variables
	std::vector<SpriteComponent*> m_pHealthIcons{};

	int m_NrBeans{};
	GameObject* m_pBeanHUD{ nullptr };
};