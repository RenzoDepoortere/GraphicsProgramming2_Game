#pragma once
class HarryCharacter;
class PauseMenu;

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

	// Functionality
	void RestartLevel();

private:
	// Variables
	// ---------
	HarryCharacter* m_pHarry{ nullptr };
	PauseMenu* m_pPauseMenu{ nullptr };

	bool m_DrawShadowMap{ false };
	float m_ShadowMapScale{};

	bool m_CenterMouse{ true };

	float m_PauseTimer{};
	bool m_HasToReset{ false };

	bool m_IsPaused{ false };

	// Functions
	// ---------

	// Scene
	void Initialize() override;
	void Update() override;
	void PostDraw() override;
	void OnGUI() override;

	// Own
	void HandleInput();
	void HandlePauseMenu();
	void DeleteChildren();
};

