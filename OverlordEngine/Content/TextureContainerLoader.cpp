#include "stdafx.h"
#include "TextureContainerLoader.h"

TextureData** TextureContainerLoader::LoadContent(const ContentLoadInfo& loadInfo)
{
	// Clear vector
	m_CreatedTextures.clear();

	//Find Extension
	const auto& assetPath = loadInfo.assetFullPath;
	ASSERT_IF(!assetPath.has_extension(), L"Invalid File Extensions!\nPath: {}", assetPath.wstring())
	const auto extension = assetPath.extension().wstring();

	// Check if is .mtl file
	if (extension == L".mtl")
	{
		ASSERT_IF(ReadMtlFile(loadInfo, assetPath), L"Failed to read .mtl file!\nPath: {}", assetPath.wstring())
	}

	// Return
	return m_CreatedTextures.data();
}

void TextureContainerLoader::Destroy(TextureData** objToDestroy)
{
	SafeDelete(objToDestroy);
}

bool TextureContainerLoader::ReadMtlFile(const ContentLoadInfo&, const std::filesystem::path& assetFile)
{
	// ---------------------------------------------------------------------------------------------
	// Currently ignoring different material-attributes since they are all the same in this use case
	// Also, this use case only uses png's, so the received texture will be loaded with that thought
	// ---------------------------------------------------------------------------------------------

	// Init variables
	ID3D11Resource* pTexture{};
	ID3D11ShaderResourceView* pShaderResourceView{};
	TexMetadata info{};

	auto image = new ScratchImage();

	// Open file
	if (auto input = std::wifstream(assetFile); input.is_open())
	{
		// Attribute names
		//const std::wstring newMaterialString{ L"newmtl" };
		//const std::wstring ambientColor{ L"Ka" };
		//const std::wstring diffuseColor{ L"Kd" };
		//const std::wstring specularColor{ L"Ks" };
		//const std::wstring specularExponent{ L"Ns" };
		//const std::wstring illumination{ L"illum" };
		const std::wstring diffuseMap{ L"map_Kd" };

		// Init strings
		std::wstring readString{};
		std::wstring textureName{};
		std::wstring assetSubPathBegin{ L"Textures/Map/" };
		std::wstring assetSubPath{};
		std::wstring assetFullPath{};

		// Read line by line
		while (std::getline(input, readString))
		{
			// If line contains a diffuseMap
			if (readString.contains(diffuseMap))
			{
				// Get textureName and load
				textureName = readString.substr(diffuseMap.size() + 1);
				assetSubPath = assetSubPathBegin + textureName;
				assetFullPath = ContentManager::GetFullAssetPath(assetSubPath);
				HANDLE_ERROR(LoadFromWICFile(assetFullPath.c_str(), DirectX::WIC_FLAGS_NONE, &info, *image));

				// Create texture
				HANDLE_ERROR(CreateTexture(m_GameContext.d3dContext.pDevice, image->GetImages(), image->GetImageCount(), image->GetMetadata(), &pTexture));
				HANDLE_ERROR(CreateShaderResourceView(m_GameContext.d3dContext.pDevice, image->GetImages(), image->GetImageCount(), image->GetMetadata(), &pShaderResourceView));

				// Create textureData and push back
				m_CreatedTextures.push_back(new TextureData(pTexture, pShaderResourceView, assetSubPath));
			}
		}
	}

	// Delete image
	SafeDelete(image);

	// Return
	return false;
}