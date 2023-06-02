#pragma once
class PauseMenu final : public GameObject
{
public:
	// Rule of five
	PauseMenu() = default;
	~PauseMenu() override = default;

	PauseMenu(const PauseMenu& other) = delete;
	PauseMenu(PauseMenu&& other) noexcept = delete;
	PauseMenu& operator=(const PauseMenu& other) = delete;
	PauseMenu& operator=(PauseMenu&& other) noexcept = delete;

	// Functionality
	void SetActive(bool isActive);

	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;

private:
	// Variables
	bool m_IsActive{ false };
	XMFLOAT2 m_ButtonSize{};

	SpriteComponent* m_pBackground{ nullptr };

	SpriteComponent* m_pMainMenu{ nullptr };
	SpriteComponent* m_pRestart{ nullptr };
	SpriteComponent* m_pExit{ nullptr };

	SpriteFont* m_pFont{ nullptr };
	std::vector<std::pair<std::wstring, XMFLOAT2>> m_FontData{};

	// Functions
	void HandleText();
	void HandleButtons(const SceneContext& sceneContext);

	bool IsInsideRange(float left, float right, int index, const POINT& mousePos);
};