#pragma once

#include <vector>

class DrawTools
{
public: 
	struct Image
	{
		int id = 0;
		float posX = 0.0f;
		float posY = 0.0f;
		int textureId = 0;
		float width = 0.0f;
		float height = 0.0f;
		float color[4] = { 1.0f,1.0f,1.0f,1.0f };
		bool colorBox = false;
	};

	struct Text
	{
		int id = 0;
		float posX = 0.0f;
		float posY = 0.0f;
		float color[4] = { 1.0f,1.0f,1.0f,1.0f };
		float xscale = 1.0f;
		float yscale = 1.0f;
		std::string text = "";
	};

	DrawTools();
	~DrawTools();

	void OnUpdate();

	int Add(Text text);
	int Add(Image image, const char* texturePath);
	int Add(Image image, int textureId);

	void OnDisconnect(int reason, const char* message);
	void RemoveTextOrImageById(int id);
	void ClearScreen();

private:

	std::vector<Text> m_TextData;
	std::vector<Image> m_ImageData;
	int m_idGenerator = 0;
};
