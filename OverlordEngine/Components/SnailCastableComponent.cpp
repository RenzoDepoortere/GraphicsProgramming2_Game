#include "stdafx.h"
#include "SnailCastableComponent.h"

#include "../OverlordProject/Prefabs/Snail.h"

SnailCastableComponent::SnailCastableComponent(Spell stunSpell, Spell pushSpell, Snail* pSnail)
	: CastableComponent{ stunSpell }
	, m_StunSpell{ stunSpell }
	, m_PushSpell{ pushSpell }
	, m_pSnail{ pSnail }
	, m_CurrentSpell{ stunSpell }
{
}

void SnailCastableComponent::Initialize(const SceneContext& /*sceneContext*/)
{
}

void SnailCastableComponent::SwitchSpell()
{
	if (m_CurrentSpell == m_StunSpell) m_CurrentSpell = m_PushSpell;
	else							   m_CurrentSpell = m_StunSpell;
}

void SnailCastableComponent::Activate(GameObject* pHarry)
{
	// Either stun or push snail
	if (m_CurrentSpell == m_StunSpell)
	{
		m_pSnail->SetStunned();
	}
	else
	{
		m_pSnail->Push(pHarry->GetTransform()->GetWorldPosition());
	}

	// Switch spell
	SwitchSpell();
}