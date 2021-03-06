#pragma once

#include <string>
#include <d2d1_1.h>
#include <SpriteBatch.h>
#include <unordered_map>
#include <wrl.h>

using namespace Microsoft::WRL;

//I can never get the D2D1_RECT_F things right
//left = The x-coordinate of the upper-left corner of the rectangle
//top = The y-coordinate of the upper-left corner of the rectangle.
//right = The x-coordinate of the lower-right corner of the rectangle
//bottom = The y-coordinate of the lower-right corner of the rectangle.

class ID3D11ShaderResourceView;

//Base widget class for in-game UI
class VWidget
{
public:
	virtual void Tick(float deltaTime);
	virtual void Start();

	ComPtr<ID3D11ShaderResourceView> CreateTexture(const std::wstring& filename);

	void Text(const std::wstring& text, D2D1_RECT_F layout);
	bool Button(const std::wstring& text, D2D1_RECT_F layout, float lineWidth = 1.0f);
	void Image(const std::wstring& filename, float x, float y);

	DirectX::SpriteBatch* spriteBatch;

	std::unordered_map<std::wstring, ID3D11ShaderResourceView*> texturesMap;

	bool bRender = true;
};
