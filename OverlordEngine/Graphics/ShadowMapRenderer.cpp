#include "stdafx.h"
#include "ShadowMapRenderer.h"
#include "Misc/ShadowMapMaterial.h"

ShadowMapRenderer::~ShadowMapRenderer()
{
	SafeDelete(m_pShadowRenderTarget)
}

void ShadowMapRenderer::Initialize()
{
	//TODO_W8(L"Implement Initialize")
	
	//1. Create a separate RenderTarget (see RenderTarget class), store in m_pShadowRenderTarget
	//	- RenderTargets are initialized with the RenderTarget::Create function, requiring a RENDERTARGET_DESC
	//	- Initialize the relevant fields of the RENDERTARGET_DESC (enableColorBuffer:false, enableDepthSRV:true, width & height)
	RENDERTARGET_DESC renderDesc{};
	renderDesc.enableColorBuffer = false;
	renderDesc.enableDepthSRV = true;
	renderDesc.width = m_GameContext.windowWidth;
	renderDesc.height = m_GameContext.windowHeight;
	
	m_pShadowRenderTarget = new RenderTarget(m_GameContext.d3dContext);
	m_pShadowRenderTarget->Create(renderDesc);

	//2. Create a new ShadowMapMaterial, this will be the material that 'generated' the ShadowMap, store in m_pShadowMapGenerator
	//	- The effect has two techniques, one for static meshes, and another for skinned meshes (both very similar, the skinned version also transforms the vertices based on a given set of boneTransforms)
	//	- We want to store the TechniqueContext (struct that contains information about the Technique & InputLayout required for rendering) for both techniques in the m_GeneratorTechniqueContexts array.
	//	- Use the ShadowGeneratorType enum to retrieve the correct TechniqueContext by ID, and also use that ID to store it inside the array (see BaseMaterial::GetTechniqueContext)
	m_pShadowMapGenerator = MaterialManager::Get()->CreateMaterial<ShadowMapMaterial>();

	ShadowGeneratorType techniqueType{ ShadowGeneratorType::Static };
	int techniqueID{ static_cast<int>(techniqueType) };
	m_GeneratorTechniqueContexts[techniqueID] = m_pShadowMapGenerator->GetTechniqueContext(techniqueID);

	techniqueType = ShadowGeneratorType::Skinned;
	techniqueID = static_cast<int>(techniqueType);
	m_GeneratorTechniqueContexts[techniqueID] = m_pShadowMapGenerator->GetTechniqueContext(techniqueID);
}

void ShadowMapRenderer::UpdateMeshFilter(const SceneContext& sceneContext, MeshFilter* pMeshFilter) const
{
	//TODO_W8(L"Implement UpdateMeshFilter")
	//Here we want to Update the MeshFilter of ModelComponents that need to be rendered to the ShadowMap
	//Updating the MeshFilter means that we want to create a corresponding VertexBuffer for our ShadowGenerator material

	//1. Figure out the correct ShadowGeneratorType (either Static, or Skinned) with information from the incoming MeshFilter
	const ShadowGeneratorType techniqueType = (pMeshFilter->HasAnimations()) ? ShadowGeneratorType::Skinned : ShadowGeneratorType::Static;
	const int techniqueID{ static_cast<int>(techniqueType) };

	//2. Retrieve the corresponding TechniqueContext from m_GeneratorTechniqueContexts array (Static/Skinned)
	const MaterialTechniqueContext techniqueContext{ m_GeneratorTechniqueContexts[techniqueID] };

	//3. Build a corresponding VertexBuffer with data from the relevant TechniqueContext you retrieved in Step2 (MeshFilter::BuildVertexBuffer)
	pMeshFilter->BuildVertexBuffer(sceneContext, techniqueContext.inputLayoutID, techniqueContext.inputLayoutSize, techniqueContext.pInputLayoutDescriptions);
}

void ShadowMapRenderer::Begin(const SceneContext& sceneContext)
{
	//TODO_W8(L"Implement Begin")
	//This function is called once right before we start the Shadow Pass (= generating the ShadowMap)
	//This function is responsible for setting the pipeline into the correct state, meaning
	//	- Making sure the ShadowMap is unbound from the pipeline as a ShaderResource (SRV), so we can bind it as a RenderTarget (RTV)
	//	- Calculating the Light ViewProjection, and updating the relevant Shader variables
	//	- Binding the ShadowMap RenderTarget as Main Game RenderTarget (= Everything we render is rendered to this rendertarget)
	//	- Clear the current (which should be the ShadowMap RT) rendertarget

	//1. Making sure that the ShadowMap is unbound from the pipeline as ShaderResourceView (SRV) is important, because we cannot use the same resource as a ShaderResourceView (texture resource inside a shader) and a RenderTargetView (target everything is rendered too) at the same time. In case this happens, you'll see an error in the output of visual studio - warning you that a resource is still bound as a SRV and cannot be used as an RTV.
	//	-> Unbinding an SRV can be achieved using DeviceContext::PSSetShaderResource [I'll give you the implementation for free] - double check your output because depending on your usage of ShaderResources, the actual slot the ShadowMap is using can be different, but you'll see a warning pop-up with the correct slot ID in that case.
	constexpr ID3D11ShaderResourceView* const pSRV[] = { nullptr };
	sceneContext.d3dContext.pDeviceContext->PSSetShaderResources(3, 1, pSRV);

	//2. Calculate the Light ViewProjection and store in m_LightVP
	// - Use XMMatrixOrtographicLH to create Projection Matrix (constants used for the demo below - feel free to change)
	//		*viewWidth> 100.f * aspectRatio
	//		*viewHeight> 100.f
	//		*nearZ>0.1f
	//		*farZ>500.f
	//- Use XMMatrixLookAtLH to create a View Matrix
	//		*eyePosition: Position of the Direction Light (SceneContext::pLights > Retrieve Directional Light)
	//		*focusPosition: Calculate using the Direction Light position and direction
	//- Use the Projection & View Matrix to calculate the ViewProjection of this Light, store in m_LightVP
	
	// Projection
	const float viewWidth{ 100.f * sceneContext.aspectRatio };
	const float viewHeight{ 100.f };
	const float nearZ{ 0.1f };
	const float farZ{ 50.f };

	const XMMATRIX projectionMatrix{ XMMatrixOrthographicLH(viewWidth, viewHeight, nearZ, farZ) };

	// View
	const Light directionalLight{ sceneContext.pLights->GetDirectionalLight() };
	const XMFLOAT4 worldUp{ 0.f,1.f,0.f, 0.f };

	const XMVECTOR eyePosition{ XMLoadFloat4(&directionalLight.position) };
	const XMVECTOR eyeDirection{ XMLoadFloat4(&directionalLight.direction) };
	const XMVECTOR focusPosition{ XMVectorAdd(eyePosition, eyeDirection) };
	const XMVECTOR upDirection{ XMLoadFloat4(&worldUp) };

	const XMMATRIX viewMatrix{ XMMatrixLookAtLH(eyePosition, focusPosition, upDirection) };

	// ViewProjection
	const XMMATRIX viewProjectionMatrix{ XMMatrixMultiply(viewMatrix, projectionMatrix) };
	XMStoreFloat4x4(&m_LightVP, viewProjectionMatrix);

	//3. Update this matrix (m_LightVP) on the ShadowMapMaterial effect
	m_pShadowMapGenerator->SetVariable_Matrix(L"gLightViewProj", m_LightVP);

	//4. Set the Main Game RenderTarget to m_pShadowRenderTarget (OverlordGame::SetRenderTarget) - Hint: every Singleton object has access to the GameContext...
	m_GameContext.pGame->SetRenderTarget(m_pShadowRenderTarget);

	//5. Clear the ShadowMap rendertarget (RenderTarget::Clear)
	m_pShadowRenderTarget->Clear();
}

void ShadowMapRenderer::DrawMesh(const SceneContext& sceneContext, MeshFilter* pMeshFilter, const XMFLOAT4X4& meshWorld, const std::vector<XMFLOAT4X4>& meshBones)
{
	//TODO_W8(L"Implement DrawMesh")
	//This function is called for every mesh that needs to be rendered on the shadowmap (= cast shadows)

	//1. Figure out the correct ShadowGeneratorType (Static or Skinned)
	const ShadowGeneratorType techniqueType = (pMeshFilter->HasAnimations()) ? ShadowGeneratorType::Skinned : ShadowGeneratorType::Static;
	const int techniqueID{ static_cast<int>(techniqueType) };

	//2. Retrieve the correct TechniqueContext for m_GeneratorTechniqueContexts
	const MaterialTechniqueContext techniqueContext{ m_GeneratorTechniqueContexts[techniqueID] };
	
	//3. Set the relevant variables on the ShadowMapMaterial
	//		- world of the mesh
	m_pShadowMapGenerator->SetVariable_Matrix(L"gWorld", meshWorld);
	//		- if animated, the boneTransforms
	if (pMeshFilter->HasAnimations())
	{
		XMFLOAT4X4* pMeshBonesData{ const_cast<XMFLOAT4X4*>(meshBones.data()) };
		m_pShadowMapGenerator->SetVariable_MatrixArray(L"gBones", reinterpret_cast<float*>(pMeshBonesData), static_cast<UINT>(meshBones.size()));
	}

	//4. Setup Pipeline for Drawing (Similar to ModelComponent::Draw, but for our ShadowMapMaterial)
	const auto pDeviceContext{ sceneContext.d3dContext.pDeviceContext };

	//	- Set InputLayout (see TechniqueContext)
	pDeviceContext->IASetInputLayout(techniqueContext.pInputLayout);
	
	//	- Set PrimitiveTopology
	pDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//	- Iterate the SubMeshes of the MeshFilter (see ModelComponent::Draw), for each SubMesh:
	for (const auto& subMesh : pMeshFilter->GetMeshes())
	{
		//		- Set VertexBuffer
		const UINT offset{ 0 };
		const auto& vertexBufferData{ pMeshFilter->GetVertexBufferData(techniqueContext.inputLayoutID, subMesh.id) };
		pDeviceContext->IASetVertexBuffers(0, 1, &vertexBufferData.pVertexBuffer, &vertexBufferData.VertexStride, &offset);

		//		- Set IndexBuffer
		pDeviceContext->IASetIndexBuffer(subMesh.buffers.pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		//		- Set correct TechniqueContext on ShadowMapMaterial - use ShadowGeneratorType as ID (BaseMaterial::SetTechnique)
		m_pShadowMapGenerator->SetTechnique(techniqueID);

		//		- Perform Draw Call (same as usual, iterate Technique Passes, Apply, Draw - See ModelComponent::Draw for reference)
		auto tech = m_pShadowMapGenerator->GetTechniqueContext(techniqueID).pTechnique;
		D3DX11_TECHNIQUE_DESC techDesc{};

		tech->GetDesc(&techDesc);
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			tech->GetPassByIndex(p)->Apply(0, pDeviceContext);
			pDeviceContext->DrawIndexed(subMesh.indexCount, 0, 0);
		}
	}
}

void ShadowMapRenderer::End(const SceneContext&) const
{
	//TODO_W8(L"Implement End")
	//This function is called at the end of the Shadow-pass, all shadow-casting meshes should be drawn to the ShadowMap at this point.
	//Now we can reset the Main Game Rendertarget back to the original RenderTarget, this also Unbinds the ShadowMapRenderTarget as RTV from the Pipeline, and can safely use it as a ShaderResourceView after this point.

	//1. Reset the Main Game RenderTarget back to default (OverlordGame::SetRenderTarget)
	//		- Have a look inside the function, there is a easy way to do this...
	m_GameContext.pGame->SetRenderTarget(nullptr);
}

ID3D11ShaderResourceView* ShadowMapRenderer::GetShadowMap() const
{
	return m_pShadowRenderTarget->GetDepthShaderResourceView();
}

void ShadowMapRenderer::Debug_DrawDepthSRV(const XMFLOAT2& /*position*/, const XMFLOAT2& /*scale*/, const XMFLOAT2& /*pivot*/) const
{
	if (m_pShadowRenderTarget->HasDepthSRV())
	{
		QuadRenderer::Get()->Draw(m_pShadowRenderTarget->GetDepthShaderResourceView());
		//SpriteRenderer::Get()->DrawImmediate(m_GameContext.d3dContext, m_pShadowRenderTarget->GetDepthShaderResourceView(), position, XMFLOAT4{ Colors::White }, pivot, scale);

		//Remove from Pipeline
		constexpr ID3D11ShaderResourceView* const pSRV[] = { nullptr };
		m_GameContext.d3dContext.pDeviceContext->PSSetShaderResources(0, 1, pSRV);
	}
}
