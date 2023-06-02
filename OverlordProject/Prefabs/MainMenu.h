#pragma once
class MainMenu final : public GameObject
{
public:
	// Rule of five
	MainMenu(bool isActive);
	~MainMenu() override = default;

	MainMenu(const MainMenu& other) = delete;
	MainMenu(MainMenu&& other) noexcept = delete;
	MainMenu& operator=(const MainMenu& other) = delete;
	MainMenu& operator=(MainMenu&& other) noexcept = delete;

	// Functionality
	void SetActive(bool isActive);

	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;

private:
	// Variables
	bool m_IsActive{ false };
	XMFLOAT2 m_ButtonSize{};
	XMFLOAT2 m_LeftRightPos{};

	SpriteComponent* m_pBackground{ nullptr };

	SpriteComponent* m_pStart{ nullptr };
	SpriteComponent* m_pControls{ nullptr };
	SpriteComponent* m_pExit{ nullptr };

	bool m_InControls{ false };
	SpriteComponent* m_pControlsBG{ nullptr };
	SpriteComponent* m_pControlButton{ nullptr };
	XMFLOAT2 m_ControlLeftRightPos{};

	SpriteFont* m_pFont{ nullptr };
	std::vector<std::pair<std::wstring, XMFLOAT2>> m_FontData{};

	// Functions
	void HandleText();
	void HandleButtons(const SceneContext& sceneContext);

	bool IsInsideRange(float left, float right, int index, const POINT& mousePos, bool lowerWithHalf = false);

	void SetControls(bool isActive);
};