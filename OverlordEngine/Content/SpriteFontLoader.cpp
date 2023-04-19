#include "stdafx.h"
#include "SpriteFontLoader.h"

SpriteFont* SpriteFontLoader::LoadContent(const ContentLoadInfo& loadInfo)
{
	const auto pReader = new BinaryReader();
	pReader->Open(loadInfo.assetFullPath);

	if (!pReader->Exists())
	{
		Logger::LogError(L"Failed to read the assetFile!\nPath: \'{}\'", loadInfo.assetSubPath);
		return nullptr;
	}

	//TODO_W7(L"Implement SpriteFontLoader >> Parse .fnt file")
	//See BMFont Documentation for Binary Layout

	//Parse the Identification bytes (B,M,F)
	std::wstring fileId{};
	fileId.push_back(pReader->Read<char>());
	fileId.push_back(pReader->Read<char>());
	fileId.push_back(pReader->Read<char>());


	//If Identification bytes doesn't match B|M|F,
	if (fileId != L"BMF")
	{
		// Log Error (SpriteFontLoader::LoadContent > Not a valid .fnt font) &
		std::cout << "SpriteFontLoader::LoadContent > Not a valid .fnt font" << std::endl;

		// return nullptr
		return nullptr;
	}

	//Parse the version (version 3 required)
	//If version is < 3,
	if (static_cast<int>(pReader->Read<char>()) < 3)
	{
		//Log Error (SpriteFontLoader::LoadContent > Only .fnt version 3 is supported)
		std::cout << "SpriteFontLoader::LoadContent > Only .fnt version 3 is supported" << std::endl;
	
		//return nullptr
		return nullptr;
	}

	//Valid .fnt file >> Start Parsing!
	//use this SpriteFontDesc to store all relevant information (used to initialize a SpriteFont object)
	SpriteFontDesc fontDesc{};


	//**********
	// BLOCK 0 *
	//**********
	
	//Retrieve the blockId and blockSize
	int blockId = static_cast<int>(pReader->Read<char>());
	int blockSize = pReader->Read<int>();

	//Retrieve the FontSize [fontDesc.fontSize]
	fontDesc.fontSize = static_cast<short>(pReader->Read<int>());
	
	//Move the binreader to the start of the FontName [BinaryReader::MoveBufferPosition(...) or you can set its position using BinaryReader::SetBufferPosition(...))
	pReader->MoveBufferPosition(10);

	//Retrieve the FontName [fontDesc.fontName]
	fontDesc.fontName = pReader->ReadNullString();


	//**********
	// BLOCK 1 *
	//**********
	//Retrieve the blockId and blockSize
	blockId = static_cast<int>(pReader->Read<char>());
	blockSize = pReader->Read<int>();
	
	//Retrieve Texture Width & Height [fontDesc.textureWidth/textureHeight]
	pReader->MoveBufferPosition(4);
	
	fontDesc.textureWidth = pReader->Read<short int>();
	fontDesc.textureHeight = pReader->Read<short int>();
	
	//Retrieve PageCount
	short int pageCount = pReader->Read<short int>();

	//> if pagecount > 1
	if (pageCount > 1)
	{
		//	> Log Error (Only one texture per font is allowed!)
		std::cout << "Only one texture per font is allowed!" << std::endl;
	}

	//Advance to Block2 (Move Reader)
	pReader->MoveBufferPosition(5);


	//**********
	// BLOCK 2 *
	//**********
	
	//Retrieve the blockId and blockSize
	blockId = static_cast<int>(pReader->Read<char>());
	blockSize = pReader->Read<int>();

	//Retrieve the PageName (BinaryReader::ReadNullString)
	std::wstring pageName = pReader->ReadNullString();

	//Construct the full path to the page texture file
	//	>> page texture should be stored next to the .fnt file, pageName contains the name of the texture file
	//	>> full texture path = asset parent_path of .fnt file (see loadInfo.assetFullPath > get parent_path) + pageName (filesystem::path::append)
	std::wstring pageTextureFilePath = loadInfo.assetFullPath.parent_path().append(pageName);

	//	>> Load the texture (ContentManager::Load<TextureData>) & Store [fontDesc.pTexture]
	fontDesc.pTexture = ContentManager::Load<TextureData>(pageTextureFilePath);


	//**********
	// BLOCK 3 *
	//**********
	
	//Retrieve the blockId and blockSize
	blockId = static_cast<int>(pReader->Read<char>());
	blockSize = pReader->Read<int>();

	//Retrieve Character Count (see documentation)
	int nrChars = blockSize / 20;
	
	//Create loop for Character Count, and:
	for (int idx{}; idx < nrChars; ++idx)
	{
		//> Retrieve CharacterId (store Local) and cast to a 'wchar_t'
		wchar_t characterId = std::to_wstring(pReader->Read<int>())[0];

		//> Create instance of FontMetric (struct)
		FontMetric fontStruct{};

		//	> Set Character (CharacterId) [FontMetric::character]
		fontStruct.character = characterId;

		//	> Retrieve Xposition (store Local)
		short int xPos = pReader->Read<short int>();

		//	> Retrieve Yposition (store Local)
		short int yPos = pReader->Read<short int>();

		//	> Retrieve & Set Width [FontMetric::width]
		fontStruct.width = static_cast<unsigned short>(pReader->Read<short int>());

		//	> Retrieve & Set Height [FontMetric::height]
		fontStruct.height = static_cast<unsigned short>(pReader->Read<short int>());

		//	> Retrieve & Set OffsetX [FontMetric::offsetX]
		fontStruct.offsetX = pReader->Read<short int>();

		//	> Retrieve & Set OffsetY [FontMetric::offsetY]
		fontStruct.offsetY = pReader->Read<short int>();

		//	> Retrieve & Set Page [FontMetric::page]
		fontStruct.advanceX = pReader->Read<short int>();

		auto value = pReader->Read<char>();
		fontStruct.page = static_cast<unsigned char>(value);

		//	> Retrieve Channel (BITFIELD!!!) 
			//		> See documentation for BitField meaning [FontMetrix::channel]
		fontStruct.channel = static_cast<unsigned char>(pReader->Read<char>());

		//	> Calculate Texture Coordinates using Xposition, Yposition, fontDesc.TextureWidth & fontDesc.TextureHeight [FontMetric::texCoord]
		fontStruct.texCoord.x = xPos / static_cast<float>(fontDesc.textureWidth);
		fontStruct.texCoord.y = yPos / static_cast<float>(fontDesc.textureHeight);

		//> Insert new FontMetric to the metrics [font.metrics] map
			//	> key = (wchar_t) charId
			//	> value = new FontMetric
		fontDesc.metrics[characterId] = fontStruct;

		//(loop restarts till all metrics are parsed)
	}

	//Done!
	delete pReader;
	return new SpriteFont(fontDesc);
}

void SpriteFontLoader::Destroy(SpriteFont* objToDestroy)
{
	SafeDelete(objToDestroy);
}
