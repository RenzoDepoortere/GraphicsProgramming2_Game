#pragma once
class HarryCharacter;

class Star final : public GameObject
{
public:
	// Rule of five
	Star(float generalScale, HarryCharacter* pHarry);
	~Star() override = default;

	Star(const Star& other) = delete;
	Star(Star&& other) noexcept = delete;
	Star& operator=(const Star& other) = delete;
	Star& operator=(Star&& other) noexcept = delete;

	// Functionality
	void SetActive() { m_IsActivated = true; }

protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;

private:
	// Variables
	// ---------
	float m_GeneralScale{};
	HarryCharacter* m_pHarry{ nullptr };

	bool m_IsActivated{ false };
	float m_TotalYaw{};

	// Functions
	// ---------
	void HandleRotation(const SceneContext& sceneContext);
	void CheckHarry();
};
