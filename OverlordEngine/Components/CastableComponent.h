#pragma once
class CastableComponent : public BaseComponent
{
public:
	// Enums
	enum Spell
	{
		Diffindo,
		Rictusempra,
		Spongify,
		Alahomora,
		NR_SPELLS
	};

	// Rule of five
	CastableComponent(Spell spell);
	virtual ~CastableComponent() override = default;
	
	CastableComponent(const CastableComponent& other) = delete;
	CastableComponent(CastableComponent&& other) noexcept = delete;
	CastableComponent& operator=(const CastableComponent& other) = delete;
	CastableComponent& operator=(CastableComponent&& other) noexcept = delete;

	// Functionality
	void SetSpell(Spell newSpell) { m_CurrentSpell = newSpell; }
	virtual Spell GetSpell() const { return m_CurrentSpell; }

	void SetCastedTo(bool castedTo) { m_HasBeenCastedTo = castedTo; }
	bool GetCastedTo() const { return m_HasBeenCastedTo; }

	virtual void Activate(GameObject* pHarry) = 0;

protected:
	void Initialize(const SceneContext& sceneContext) override = 0;
	//void Update(const SceneContext& sceneContext) override = 0;
	//void Draw(const SceneContext& sceneContext) override;

private:
	// Member variables
	Spell m_CurrentSpell{};
	bool m_HasBeenCastedTo{ false };
};
