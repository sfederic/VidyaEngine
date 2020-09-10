#pragma once

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxcompiler.lib")

#include "D3D11RenderSystem.h"
#include <d2d1_3.h>
#include <d3d11on12.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include "IRenderSystem.h"
#include <dxcapi.h>
#include <dwrite_1.h>

using namespace Microsoft::WRL;

class D3D12RenderSystem : public IRenderSystem
{
public:
	virtual void Tick() override;
	virtual void Init(HWND window) override;
	virtual void RenderSetup(float deltaTime) override;
	virtual void Render(float deltaTime) override;
	virtual void RenderEnd(float deltaTime) override;
	virtual void CreateDefaultBuffer() override;
	virtual void CreateVertexBuffer(unsigned int size, const void* data, ActorSystem* actor) override;
	virtual void CreateSamplerState(ActorSystem* actorSystem) override;
	virtual void CreateTexture(ActorSystem* actorSystem) override;
	virtual void CreateVertexShader() override;
	virtual void CreatePixelShader() override;
	virtual void CreateAllShaders() override;
	virtual void* GetSwapchain() override;
	virtual void Present() override;
	virtual void Flush() override;

	void ExecuteCommandLists();
	void InitD2D();
	void UpdateConstantBuffer(ID3D12Resource* constBuffer, int byteWidth, void* initData);
	void WaitForPreviousFrame();

	static const int swapchainCount = 2;

	ID2D1Factory3* d2dFactory;
	IDWriteFactory1* writeFactory;
	ID2D1SolidColorBrush* brushText;
	IDWriteTextFormat* textFormat;

	ID3D11On12Device* d3d11On12Device;
	ID3D11Device* d3d11Device;
	ID3D11DeviceContext* d3d11DeviceContext;

	IDXGIDevice* dxgiDevice;
	ID2D1Device2* d2dDevice;
	ID2D1DeviceContext1* d2dDeviceContext;
	ID2D1Bitmap1* d2dRenderTargets[swapchainCount];

	IDXGIFactory1* factory;
	ID3D12Device* device;
	ID3D12CommandQueue* cmdQueue;
	ID3D12GraphicsCommandList* cmdList;
	ID3D12CommandAllocator* cmdAlloc;
	IDXGISwapChain3* swapchain;
	ID3D12DescriptorHeap* rtvHeap;
	ID3D12DescriptorHeap* dsvHeap;
	ID3D12DescriptorHeap* cbHeap;
	ID3D12Resource* rtvs[swapchainCount];
	ID3D12RootSignature* rootSig;
	ID3D12PipelineState* pipelineState;
	ID3D12Fence* fence;

	ID3D11Resource* wrappedBackBuffers[swapchainCount];

	IDxcCompiler* dxcCompiler;
	IDxcLibrary* dxcLibrary;

	//ID3DBlob* vertexCode;
	//ID3DBlob* pixelCode;

	IDxcBlob* vertexCode;
	IDxcBlob* pixelCode;

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

	ComPtr<ID3D12Resource> vertexBuffer;
	ComPtr<ID3D12Resource> cbUploadBuffer;
	ComPtr<ID3D12Resource> cbUploadMaterialBuffer;

	D3D12_VIEWPORT viewport;
	D3D12_RECT scissorRect;

	UINT64 fenceVal;

	HANDLE fenceEvent;

	unsigned int currentBackBufferIndex;
	UINT rtvHeapSize;
	UINT cbHeapSize;
};

class ResourceBarrier
{
public:
	static D3D12_RESOURCE_BARRIER Transition(ID3D12Resource* resource,
		D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter)
	{
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Transition.pResource = resource;
		barrier.Transition.StateBefore = stateBefore;
		barrier.Transition.StateAfter = stateAfter;
		return barrier;
	}
};