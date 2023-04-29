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
		ASSERT_IF(ReadMtlFile(assetPath), L"Failed to read .mtl file!\nPath: {}", assetPath.wstring())
	}

	// Return
	return m_CreatedTextures.data();
}

void TextureContainerLoader::Destroy(TextureData** objToDestroy)
{
	SafeDelete(objToDestroy);
}

bool TextureContainerLoader::ReadMtlFile(const std::filesystem::path& assetFile)
{
	// Init variables
	ID3D11Resource* pTexture{};
	ID3D11ShaderResourceView* pShaderResourceView{};
	TexMetadata info{};

	auto image = new ScratchImage();

	// Open file
	if (auto input = std::ifstream(assetFile); input.is_open())
	{

	}

	// Delete image
	SafeDelete(image);

	// Return
	return true;
}