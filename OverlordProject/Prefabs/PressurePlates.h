#pragma once
class Snail;
class HarryCharacter;

class PressurePlates final : public GameObject
{
public:
	// Rule of five
	PressurePlates(float generalScale, HarryCharacter* pHarry, const std::vector<Snail*>& snails);
	~PressurePlates() override = default;

	PressurePlates(const PressurePlates& other) = delete;
	PressurePlates(PressurePlates&& other) noexcept = delete;
	PressurePlates& operator=(const PressurePlates& other) = delete;
	PressurePlates& operator=(PressurePlates&& other) noexcept = delete;

protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;

private:
	// Variables
	// ---------
	float m_GeneralScale{};
	HarryCharacter* m_pHarry{ nullptr };
	std::vector<Snail*> m_pSnails{ nullptr };

	bool m_FirstActivated{ false };
	bool m_SecondActivated{ false };

	bool m_StarSpawned{ false };
};
