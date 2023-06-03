#pragma once
class EyeAdaptation final : public PostProcessingMaterial
{
public:
	// Rule of five
	EyeAdaptation();
	~EyeAdaptation() override = default;

	EyeAdaptation(const EyeAdaptation& other) = delete;
	EyeAdaptation(EyeAdaptation&& other) noexcept = delete;
	EyeAdaptation& operator=(const EyeAdaptation& other) = delete;
	EyeAdaptation& operator=(EyeAdaptation&& other) noexcept = delete;

protected:
	void Initialize(const GameContext& /*gameContext*/) override {}
};

