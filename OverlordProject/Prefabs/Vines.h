#pragma once
class HarryCharacter;

class Vines final : public GameObject
{
public:
	// Rule of five
	Vines(float generalScale, HarryCharacter* m_pHarry);
	~Vines() override = default;

	Vines(const Vines& other) = delete;
	Vines(Vines&& other) noexcept = delete;
	Vines& operator=(const Vines& other) = delete;
	Vines& operator=(Vines&& other) noexcept = delete;

protected:
	void Initialize(const SceneContext& sceneContext) override;

private:
	// Variables
	// ---------
	float m_GeneralScale{};
	HarryCharacter* m_pHarry{ nullptr };
};
