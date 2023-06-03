#pragma once
class Torch final : public GameObject
{
public:
	// Rule of five
	Torch(float generalScale);
	~Torch() override = default;

	Torch(const Torch& other) = delete;
	Torch(Torch&& other) noexcept = delete;
	Torch& operator=(const Torch& other) = delete;
	Torch& operator=(Torch&& other) noexcept = delete;

protected:
	void Initialize(const SceneContext& sceneContext) override;

private:
	// Variables
	// ---------
	float m_GeneralScale{};
	ParticleEmitterComponent* m_pEmitter{ nullptr };
};