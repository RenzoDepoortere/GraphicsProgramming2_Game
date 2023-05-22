#pragma once
class HarryCharacter;

class HarryPotterScene final : public GameScene
{
public:
	// Rule of Five
	HarryPotterScene() : GameScene(L"HarryPotterScene") {	}
	~HarryPotterScene() override = default;
	HarryPotterScene(const HarryPotterScene& other) = delete;
	HarryPotterScene(HarryPotterScene&& other) noexcept = delete;
	HarryPotterScene& operator=(const HarryPotterScene& other) = delete;
	HarryPotterScene& operator=(HarryPotterScene&& other) noexcept = delete;

private:
	// Variables
	// ---------
	HarryCharacter* m_pHarry{ nullptr };

	// Functions
	// ---------

	// Scene
	void Initialize() override;
	void Update() override;
	void OnGUI() override;
};

