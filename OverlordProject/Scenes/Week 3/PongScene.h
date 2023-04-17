#pragma once

class CubePrefab;
class SpherePrefab;

class PongScene final : public GameScene
{
public:
	PongScene();
	~PongScene() override = default;

	PongScene(const PongScene& other) = delete;
	PongScene(PongScene&& other) noexcept = delete;
	PongScene& operator=(const PongScene& other) = delete;
	PongScene& operator=(PongScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void OnGUI() override;

private:
	// Member variables
	CubePrefab* m_pLeftCube{ nullptr };
	CubePrefab* m_pRightCube{ nullptr };

	SpherePrefab* m_pBall{ nullptr };

	int m_BallHorizontalMovementDirection{};
	int m_BallVerticalMovementDirection{};

	float m_UpperBoxBoundary{};
	float m_LowerBoxBoundary{};

	// Member functions
	void Input();
	void BallBouncing();
	void ResetBall();
};

