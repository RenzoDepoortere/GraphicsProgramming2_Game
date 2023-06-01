#pragma once
class Snail;

class Trail final : public GameObject
{
public:
	// Rule of five
	Trail(GameObject* pHarry, Snail* pSnail);
	~Trail() override = default;

	Trail(const Trail& other) = delete;
	Trail(Trail&& other) noexcept = delete;
	Trail& operator=(const Trail& other) = delete;
	Trail& operator=(Trail&& other) noexcept = delete;

protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;

private:
	// Variables
	// ---------
	GameObject* m_pHarry{};
	Snail* m_pSnail{};

	float m_Scale{};
	float m_DamageCooldown{};

	// Functions
	// ---------
	void ScaleDown(const SceneContext& sceneContext);
	void CheckHarry();
};