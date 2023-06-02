#include "stdafx.h"
#include "EnemiesPrefab.h"

#include "Prefabs/Snail.h"
#include "Prefabs/PressurePlates.h"

EnemiesPrefab::EnemiesPrefab(float generalScale, HarryCharacter* pHarry)
	: m_GeneralScale{ generalScale }
	, m_pHarry{ pHarry }
{
}

void EnemiesPrefab::Initialize(const SceneContext& /*sceneContext*/)
{
	// Snails
	// ------
	std::vector<Snail*> pSnails{};

	// First snail
	Snail* pSnail = AddChild(new Snail{ m_GeneralScale, m_pHarry });

	XMFLOAT3 snailPosition = XMFLOAT3{ -3.1f, -8.8f, -48.2f };
	pSnail->GetTransform()->Translate(snailPosition);
	
	pSnail->AddPathPosition(snailPosition);
	snailPosition = XMFLOAT3{ 15.8f, -8.8f, -47.9f };
	pSnail->AddPathPosition(snailPosition);

	pSnails.emplace_back(pSnail);

	// Presure plates
	// --------------
	AddChild(new PressurePlates{ m_GeneralScale, m_pHarry, pSnails });
}