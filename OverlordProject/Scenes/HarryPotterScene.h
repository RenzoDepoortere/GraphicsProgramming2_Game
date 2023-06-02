#pragma once
class HarryCharacter;
class MainMenu;
class PauseMenu;

class HarryPotterScene final : public GameScene
{
public:
	// Rule of Five
	HarryPotterScene();
	~HarryPotterScene() override = default;
	HarryPotterScene(const HarryPotterScene& other) = delete;
	HarryPotterScene(HarryPotterScene&& other) noexcept = delete;
	HarryPotterScene& operator=(const HarryPotterScene& other) = delete;
	HarryPotterScene& operator=(HarryPotterScene&& other) noexcept = delete;

	// Functionality
	void SetMainMenu(bool goToMenu);
	void RestartLevel();

private:
	// Variables
	// ---------
	HarryCharacter* m_pHarry{ nullptr };
	MainMenu* m_pMainMenu{ nullptr };
	PauseMenu* m_pPauseMenu{ nullptr };

	bool m_DrawShadowMap{ false };
	float m_ShadowMapScale{};

	bool m_CenterMouse{ false };

	float m_PauseTimer{};
	bool m_HasToReset{ false };

	bool m_InStartMenu{ true };
	bool m_IsPaused{ false };

	// Functions
	// ---------

	// Scene
	void Initialize() override;
	void Update() override;
	void PostDraw() override;
	void OnGUI() override;

	// Own
	void HandleMouse();
	void HandleInput();
	void HandlePauseMenu();
	void HandleMainMenu();

	void DeleteChildren();
};

