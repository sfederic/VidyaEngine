#pragma once

#pragma comment(lib, "d3d11")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d2d1.lib")

#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <dxgi1_6.h>
#include <d2d1_1.h>
#include <comdef.h>
#include <DirectXMath.h>
#include "Win32Util.h"
#include <vector>

using namespace DirectX;

struct Vertex
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
	XMFLOAT3 normal;
};

extern Vertex debugLineData[2];

void DXTrace(HRESULT hr, const char* filename, const char* func, int line);
#define HR(hr) if(hr != S_OK) { DXTrace(hr, __FILE__, #hr, __LINE__); throw; }

class DXUtil
{
public:
	void Tick();
	void CreateDevice();
	void CreateSwapchain();
	void CreateRTVAndDSV();
	void CreateShaders();
	void CreateInputLayout();
	void CreateRasterizerStates();
	void CreateVertexBuffer(UINT size, const void* data, class ActorSystem* actor);
	void CreateConstantBuffer(class Camera& camera);

	void RenderSetup(class Camera* camera, class UIContext* ui, class DXUtil* dx, struct ID3D11Buffer* debugBuffer, float deltaTime);
	void RenderActorSystem(class ActorSystem* actorSystem, class Camera* camera);
	void RenderBounds(class World* world, class Camera* camera);
	void RenderEnd(class UIContext* ui, class World* world, float deltaTime);

	std::vector<IDXGIAdapter1*> adapters;
	std::vector<DXGI_ADAPTER_DESC1> adaptersDesc;

	std::vector<Vertex> debugLines;
	std::vector<XMMATRIX> debugLineMatrices;

	ID3DBlob* CreateShaderFromFile(const wchar_t* filename, const char* entry, const char* target);
	ID3D11Buffer* CreateDefaultBuffer(UINT byteWidth, UINT bindFlags, const void* initData);
	ID3D11Buffer* CreateDyamicBuffer(UINT byteWidth, UINT bindFlags, const void* initData);

	static const int frameCount = 2;

	D3D11_VIEWPORT viewport = { 0.f, 0.f, (float)windowWidth, (float)windowHeight, 0.f, 1.f };

	ID3D11Device* device;
	ID3D11DeviceContext* context;
	IDXGISwapChain3* swapchain;
	ID3D11RenderTargetView* rtvs[frameCount];
	ID3D11DepthStencilView* dsv;
	ID3D11InputLayout* inputLayout;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11RasterizerState* rastStateSolid;
	ID3D11RasterizerState* rastStateWireframe;
	ID3D11RasterizerState* activeRastState;
	IDXGIFactory6* dxgiFactory;

	ID3D11Query* disjointQuery;
	ID3D11Query* startTimeQuery;
	ID3D11Query* endTimeQuery;

	ID3D11Buffer* cbMatrices;

	ID3DBlob* vertexCode;
	ID3DBlob* pixelCode;

	D3D_FEATURE_LEVEL featureLevel;

	double renderTime;

	bool bDrawBoundingBoxes = false;
	bool bDrawBoundingSpheres = false;
	bool bQueryGPU = false;
	bool bQueryGPUInner = false;
};

static DXUtil dx;