#include "VWidget.h"
#include "UISystem.h"
#include "Input.h"
#include <WICTextureLoader.h>
#include "RenderSystem.h"

void VWidget::Tick(float deltaTime)
{
	Text(L"Hello baby", { 0,0,200,200 });
	Button(L"button", { 200, 200, 300, 300 });
	Image(L"penguin.png", 200, 200);
}

void VWidget::Start()
{
	spriteBatch = new DirectX::SpriteBatch(gRenderSystem.context);
}

ComPtr<ID3D11ShaderResourceView> VWidget::CreateTexture(const std::wstring& filename)
{
	ComPtr<ID3D11ShaderResourceView> textureView;
	std::wstring filepath = L"Textures/" + filename;
	CreateWICTextureFromFile(gRenderSystem.device, filepath.c_str(), nullptr, &textureView);
	assert(textureView && "texture filename will be wrong");

	return textureView;
}

void VWidget::Text(const std::wstring& text, D2D1_RECT_F layout)
{
	gUISystem.d2dRenderTarget->DrawTextA(text.c_str(), text.size(), 
		gUISystem.textFormat, layout, gUISystem.brushText);
}

//Make sure the buttons layout isn't backwards(bottom and top less than left and right)
//else the mouse check won't work.
bool VWidget::Button(const std::wstring& text, D2D1_RECT_F layout, float lineWidth)
{
	gUISystem.d2dRenderTarget->DrawRectangle(layout, gUISystem.brushShapes, lineWidth);
	Text(text, layout);

	if (gUISystem.mousePos.x > layout.left && gUISystem.mousePos.x < layout.right)
	{
		if (gUISystem.mousePos.y > layout.top && gUISystem.mousePos.y < layout.bottom)
		{
			if (gInputSystem.GetMouseLeftUpState())
			{
				return true;
			}
		}
	}

	return false;
}

//REF:https://github.com/Microsoft/DirectXTK/wiki/Sprites-and-textures
void VWidget::Image(const std::wstring& filename, float x, float y)
{
	//TODO: sprite rendering is fucked up. There's something going wrong with its state changes
	//and maybe its shader binding, models get messed up when this function is called.
	//REF:https://github.com/Microsoft/DirectXTK/wiki/SpriteBatch#state-management
	//REF:https://stackoverflow.com/questions/35558178/directxspritefont-spritebatch-prevents-3d-scene-from-drawing
	/*auto textureIt = texturesMap.find(filename);
	if (textureIt == texturesMap.end())
	{
		auto texture = CreateTexture(filename);
		texturesMap[filename] = texture.Get();
	}
	else
	{
		DirectX::XMFLOAT2 screenPos(x, y);
		DirectX::XMFLOAT2 origin(0.f, 0.f);

		spriteBatch->Begin();
		spriteBatch->Draw(textureIt->second, screenPos, nullptr, Colors::White, 0.f, origin);
		spriteBatch->End();
	}*/
}
