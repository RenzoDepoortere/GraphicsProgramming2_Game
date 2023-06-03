#pragma once
class ChromaticAberration final : public PostProcessingMaterial
{
public:
	// Rule of five
	ChromaticAberration();
	~ChromaticAberration() override = default;

	ChromaticAberration(const ChromaticAberration& other) = delete;
	ChromaticAberration(ChromaticAberration&& other) noexcept = delete;
	ChromaticAberration& operator=(const ChromaticAberration& other) = delete;
	ChromaticAberration& operator=(ChromaticAberration&& other) noexcept = delete;

protected:
	void Initialize(const GameContext& /*gameContext*/) override {}
};

