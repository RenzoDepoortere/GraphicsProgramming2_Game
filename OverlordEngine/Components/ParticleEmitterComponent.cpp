#include "stdafx.h"
#include "ParticleEmitterComponent.h"
#include "Misc/ParticleMaterial.h"

ParticleMaterial* ParticleEmitterComponent::m_pParticleMaterial{};

ParticleEmitterComponent::ParticleEmitterComponent(const std::wstring& assetFile, const ParticleEmitterSettings& emitterSettings, UINT particleCount):
	m_ParticlesArray(new Particle[particleCount]),
	m_ParticleCount(particleCount), //How big is our particle buffer?
	m_MaxParticles(particleCount), //How many particles to draw (max == particleCount)
	m_AssetFile(assetFile),
	m_EmitterSettings(emitterSettings)
{
	m_enablePostDraw = true; //This enables the PostDraw function for the component
}

ParticleEmitterComponent::~ParticleEmitterComponent()
{
	//TODO_W9(L"Implement Destructor")
	delete[] m_ParticlesArray;
	m_ParticlesArray = nullptr;

	m_pVertexBuffer->Release();
}

void ParticleEmitterComponent::Initialize(const SceneContext& sceneContext)
{
	//TODO_W9(L"Implement Initialize")

	// Create material if doesn't have one already
	if (m_pParticleMaterial == nullptr || MaterialManager::Get()->GetMaterial(m_pParticleMaterial->GetMaterialId()) == nullptr)
	{
		m_pParticleMaterial = MaterialManager::Get()->CreateMaterial<ParticleMaterial>();
	}

	CreateVertexBuffer(sceneContext);
	m_pParticleTexture = ContentManager::Load<TextureData>(m_AssetFile);
}

void ParticleEmitterComponent::CreateVertexBuffer(const SceneContext& sceneContext)
{
	//TODO_W9(L"Implement CreateVertexBuffer")
	if (m_pVertexBuffer) m_pVertexBuffer->Release();

	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = m_ParticleCount * sizeof(VertexParticle);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData{};
	initData.pSysMem = malloc(bufferDesc.ByteWidth);

	HANDLE_ERROR(sceneContext.d3dContext.pDevice->CreateBuffer(&bufferDesc, &initData, &m_pVertexBuffer));

	delete initData.pSysMem;
}

void ParticleEmitterComponent::Update(const SceneContext& sceneContext)
{
	//TODO_W9(L"Implement Update")

	// Average particle emit treshold
	// ------------------------------
	float particleInterval{};
	particleInterval = m_EmitterSettings.maxEnergy / m_ParticleCount;

	// Increase timer
	// --------------
	m_LastParticleSpawn += sceneContext.pGameTime->GetElapsed();

	// Validate	particles and add to vertexBuffer
	// ------------------------------------------
	m_ActiveParticles = 0;

	// Map
	D3D11_MAPPED_SUBRESOURCE mappedResource{};
	HANDLE_ERROR(sceneContext.d3dContext.pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

	// Loop through particles
	// ----------------------
	VertexParticle* pBuffer{ reinterpret_cast<VertexParticle*>(mappedResource.pData) };
	const float elapsedTime{ sceneContext.pGameTime->GetElapsed() };
	for (UINT idx{}; idx < m_ParticleCount; ++idx)
	{
		// If is active
		if (m_ParticlesArray[idx].isActive)
		{
			UpdateParticle(m_ParticlesArray[idx], elapsedTime);
		}
		// Else
		else
		{
			if (particleInterval <= m_LastParticleSpawn)
			{
				m_LastParticleSpawn = 0.f;
				SpawnParticle(m_ParticlesArray[idx]);
			}
		}

		// Add to vertexBuffer
		if (m_ParticlesArray[idx].isActive)
		{
			pBuffer[m_ActiveParticles] = m_ParticlesArray[idx].vertexInfo;
			++m_ActiveParticles;
		}
	}

	// Unmap
	// -----
	sceneContext.d3dContext.pDeviceContext->Unmap(m_pVertexBuffer, 0);
}

void ParticleEmitterComponent::UpdateParticle(Particle& particle, float elapsedTime) const
{
	//TODO_W9(L"Implement UpdateParticle")
	if (particle.isActive == false) return;

	// Deplete energy
	// --------------
	particle.currentEnergy -= elapsedTime;
	if (particle.currentEnergy < 0)
	{
		particle.isActive = false;
		return;
	}

	// Update particle
	// ---------------

	// Position
	const XMVECTOR newPosition{ XMVectorAdd(XMVectorScale(XMLoadFloat3(&m_EmitterSettings.velocity), elapsedTime), XMLoadFloat3(&particle.vertexInfo.Position)) };
	XMStoreFloat3(&particle.vertexInfo.Position, newPosition);

	// Color
	const float lifePercent{ particle.currentEnergy / particle.totalEnergy };
	const float constant{ 2.f };
	particle.vertexInfo.Color = XMFLOAT4{ m_EmitterSettings.color.x, m_EmitterSettings.color.y, m_EmitterSettings.color.z, m_EmitterSettings.color.w * lifePercent * constant};

	// Size
	particle.vertexInfo.Size = MathHelper::Lerp<float>(particle.initialSize * particle.sizeChange, particle.initialSize, lifePercent);
}

void ParticleEmitterComponent::SpawnParticle(Particle& particle)
{
	//TODO_W9(L"Implement SpawnParticle")
	particle.isActive = true;

	// Energy Initialization
	// ---------------------
	particle.totalEnergy = MathHelper::randF(m_EmitterSettings.minEnergy, m_EmitterSettings.maxEnergy);
	particle.currentEnergy = particle.totalEnergy;

	// Position Initialization
	// -----------------------
	const XMFLOAT3 randomDirection{ 1, 0, 0 };
	const XMMATRIX randomRotationMatrix{ XMMatrixRotationRollPitchYaw(MathHelper::randF(-XM_PI, XM_PI), MathHelper::randF(-XM_PI, XM_PI), MathHelper::randF(-XM_PI, XM_PI)) };
	const XMVECTOR randomDirectionVector{ XMVector3TransformNormal(XMLoadFloat3(&randomDirection), randomRotationMatrix) };
	
	const float randomDistance{ MathHelper::randF(m_EmitterSettings.minEmitterRadius, m_EmitterSettings.maxEmitterRadius) };
	const XMFLOAT3 worldPos{ GetGameObject()->GetTransform()->GetWorldPosition() };
	XMStoreFloat3(&particle.vertexInfo.Position, XMVectorAdd(XMVectorScale(randomDirectionVector, randomDistance), XMLoadFloat3(&worldPos)));

	// Size Initialization
	// -------------------
	particle.vertexInfo.Size = MathHelper::randF(m_EmitterSettings.minSize, m_EmitterSettings.maxSize);
	particle.initialSize = particle.vertexInfo.Size;

	particle.sizeChange = MathHelper::randF(m_EmitterSettings.minScale, m_EmitterSettings.maxScale);

	// Rotation Initialization
	// -----------------------
	particle.vertexInfo.Rotation = MathHelper::randF(-XM_PI, XM_PI);

	// Color Initialization
	// --------------------
	particle.vertexInfo.Color = m_EmitterSettings.color;
}

void ParticleEmitterComponent::PostDraw(const SceneContext& sceneContext)
{
	//TODO_W9(L"Implement PostDraw")

	// Set shader variables
	CameraComponent* pCamera{ sceneContext.pCamera };
	if (m_pCustomCamera) pCamera = m_pCustomCamera;
	
	m_pParticleMaterial->SetVariable_Matrix(L"gWorldViewProj", pCamera->GetViewProjection());
	m_pParticleMaterial->SetVariable_Matrix(L"gViewInverse", pCamera->GetViewInverse());
	m_pParticleMaterial->SetVariable_Texture(L"gParticleTexture", m_pParticleTexture);

	const auto pDeviceContext{ sceneContext.d3dContext.pDeviceContext };
	auto techniqueContext{ m_pParticleMaterial->GetTechniqueContext() };

	// Set inputLayout
	pDeviceContext->IASetInputLayout(techniqueContext.pInputLayout);

	// Set primitiveTopology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	// Set the vertexBuffer
	const UINT offset{ 0 };
	const auto vertexParticleSize{ static_cast<UINT>(sizeof(VertexParticle)) };
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &vertexParticleSize, &offset);

	// Pass
	D3DX11_TECHNIQUE_DESC techDesc{};

	techniqueContext.pTechnique->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		techniqueContext.pTechnique->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->Draw(m_ActiveParticles, 0);
	}
}

void ParticleEmitterComponent::DrawImGui()
{
	if(ImGui::CollapsingHeader("Particle System"))
	{
		ImGui::SliderUInt("Count", &m_ParticleCount, 0, m_MaxParticles);
		ImGui::InputFloatRange("Energy Bounds", &m_EmitterSettings.minEnergy, &m_EmitterSettings.maxEnergy);
		ImGui::InputFloatRange("Size Bounds", &m_EmitterSettings.minSize, &m_EmitterSettings.maxSize);
		ImGui::InputFloatRange("Scale Bounds", &m_EmitterSettings.minScale, &m_EmitterSettings.maxScale);
		ImGui::InputFloatRange("Radius Bounds", &m_EmitterSettings.minEmitterRadius, &m_EmitterSettings.maxEmitterRadius);
		ImGui::InputFloat3("Velocity", &m_EmitterSettings.velocity.x);
		ImGui::ColorEdit4("Color", &m_EmitterSettings.color.x, ImGuiColorEditFlags_NoInputs);
	}
}