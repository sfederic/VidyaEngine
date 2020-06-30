#include "DXUtil.h"
#include "Win32Util.h"
#include "Actor.h"
#include "Camera.h"
#include "UIContext.h"

//Temp constant buffer data for base shader
struct Matrices
{
	XMMATRIX model;
	XMMATRIX view;
	XMMATRIX proj;
	XMMATRIX mvp;
}matrices;

void DXUtil::CreateDevice()
{
	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 };

	//BGRA support needed for DirectWrite and Direct2D
	HR(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		featureLevels, _countof(featureLevels), D3D11_SDK_VERSION, &device, &featureLevel, &context));
}

void DXUtil::CreateSwapchain()
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc = { (UINT)windowWidth, (UINT)windowHeight, {60, 1}, DXGI_FORMAT_R8G8B8A8_UNORM };
	sd.Windowed = TRUE;
	sd.SampleDesc.Count = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = mainWindow;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.BufferCount = frameCount;

	HR(CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory)));
	IDXGISwapChain* tmpSwapchain;
	HR(dxgiFactory->CreateSwapChain(device, &sd, &tmpSwapchain));
	HR(tmpSwapchain->QueryInterface(&swapchain));
	tmpSwapchain->Release();
}

void DXUtil::CreateRTVAndDSV()
{
	for (int i = 0; i < frameCount; i++)
	{
		ID3D11Texture2D* backBuffer;
		swapchain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
		HR(device->CreateRenderTargetView(backBuffer, nullptr, &rtvs[i]));
		backBuffer->Release();
	}

	D3D11_TEXTURE2D_DESC dsDesc = {};
	dsDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsDesc.ArraySize = 1;
	dsDesc.MipLevels = 1;
	dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dsDesc.SampleDesc.Count = 1;
	dsDesc.Width = windowWidth;
	dsDesc.Height = windowHeight;

	ID3D11Texture2D* depthStencilBuffer;
	HR(device->CreateTexture2D(&dsDesc, nullptr, &depthStencilBuffer));
	HR(device->CreateDepthStencilView(depthStencilBuffer, nullptr, &dsv));
}

void DXUtil::CreateShaders()
{
	vertexCode = CreateShaderFromFile(L"./shaders/shaders.hlsl", "VSMain", "vs_5_0");
	pixelCode = CreateShaderFromFile(L"./shaders/shaders.hlsl", "PSMain", "ps_5_0");

	HR(device->CreateVertexShader(vertexCode->GetBufferPointer(), vertexCode->GetBufferSize(), nullptr, &vertexShader));
	HR(device->CreatePixelShader(pixelCode->GetBufferPointer(), pixelCode->GetBufferSize(), nullptr, &pixelShader));

	context->VSSetShader(vertexShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);
}

void DXUtil::CreateInputLayout()
{
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, pos), D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(Vertex, uv), D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, normal), D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	HR(device->CreateInputLayout(inputDesc, _countof(inputDesc), vertexCode->GetBufferPointer(), vertexCode->GetBufferSize(), &inputLayout));
	context->IASetInputLayout(inputLayout);
}

void DXUtil::CreateRasterizerStates()
{
	D3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.CullMode = D3D11_CULL_BACK;
	rastDesc.DepthClipEnable = TRUE;
	rastDesc.FrontCounterClockwise = FALSE;

	HR(device->CreateRasterizerState(&rastDesc, &rastStateSolid));
	context->RSSetState(rastStateSolid);

	rastDesc.FillMode = D3D11_FILL_WIREFRAME;
	rastDesc.CullMode = D3D11_CULL_NONE;
	HR(device->CreateRasterizerState(&rastDesc, &rastStateWireframe));
}

void DXUtil::CreateVertexBuffer(UINT size, const void* data, Actor* actor)
{
	actor->vertexBuffer = CreateDefaultBuffer(size, D3D11_BIND_VERTEX_BUFFER, data);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &actor->vertexBuffer, &stride, &offset);
}

void DXUtil::CreateConstantBuffer()
{
	//TODO: cleanup constant buffer code to work later with multiple shaders and buffers
	matrices.model = XMMatrixIdentity();
	matrices.view = XMMatrixIdentity();
	matrices.proj = XMMatrixPerspectiveFovLH(XM_PI / 3, Win32Util::GetAspectRatio(), 0.01f, 1000.f);
	matrices.mvp = matrices.model * matrices.view * matrices.proj;

	cbMatrices = CreateDefaultBuffer(sizeof(Matrices), D3D11_BIND_CONSTANT_BUFFER, &matrices);
}

void DXUtil::Render(Camera* camera, UIContext* ui)
{
	context->Begin(disjointQuery);
	context->End(startTimeQuery);

	context->RSSetViewports(1, &viewport);

	const float clearColour[4] = { 0.2f, 0.2f, 0.2f, 1.f };
	UINT frameIndex = swapchain->GetCurrentBackBufferIndex();

	context->ClearRenderTargetView(rtvs[frameIndex], clearColour);
	context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	context->OMSetRenderTargets(1, &rtvs[frameIndex], dsv);
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (int i = 0; i < actors.size(); i++)
	{
		//Constant buffer work
		matrices.view = camera->view;
		matrices.model = actors[i].transform;
		matrices.mvp = matrices.model * matrices.view * matrices.proj;
		context->UpdateSubresource(cbMatrices, 0, nullptr, &matrices, 0, 0);
		context->VSSetConstantBuffers(0, 1, &cbMatrices);

		//Draw all actors
		DrawActor(&actors[i]);
	}


	//TODO: FIX THIS TOO THIS IS AWFUL
	//UI RENDERING 
	//TODO: Put render and d2d stuff into func for profiling
	ui->d2dRenderTarget->BeginDraw();
	wchar_t renderTimeText[32];
	_snwprintf_s(renderTimeText, sizeof(renderTimeText), L"Render: %f", renderTime);
	ui->d2dRenderTarget->DrawTextA(renderTimeText, 16, ui->textFormat, { 0, 0, 1000, 1000 }, ui->brush);

	ui->d2dRenderTarget->EndDraw();

	HR(swapchain->Present(1, 0));

	context->End(endTimeQuery);
	context->End(disjointQuery);


	while (context->GetData(disjointQuery, nullptr, 0, 0) == S_FALSE)
	{
		Sleep(1);
	}

	D3D11_QUERY_DATA_TIMESTAMP_DISJOINT freq = {};
	HR(context->GetData(disjointQuery, &freq, sizeof(freq), 0));

	//Is the thread polling necessary?
	while (context->GetData(startTimeQuery, nullptr, 0, 0) == S_FALSE)
	{
		Sleep(1);
	}

	while (context->GetData(endTimeQuery, nullptr, 0, 0) == S_FALSE)
	{
		Sleep(1);
	}

	//TODO: fucking clean this up
	UINT64 endTime = 0, startTime = 0;
	HR(context->GetData(startTimeQuery, &startTime, sizeof(UINT64), 0));
	HR(context->GetData(endTimeQuery, &endTime, sizeof(UINT64), 0));

	UINT64 realTime = endTime - startTime;
	double tick = 1.0 / freq.Frequency;
	double time = tick * (realTime);

	renderTime = time;

	return;
}

void DXUtil::DrawActor(Actor* actor)
{
	UINT strides = sizeof(Vertex);
	UINT offsets = 0;
	context->IASetVertexBuffers(0, 1, &actor->vertexBuffer, &strides, &offsets);
	context->Draw(actor->modelData.verts.size(), 0);
}

ID3DBlob* DXUtil::CreateShaderFromFile(const wchar_t* filename, const char* entry, const char* target)
{
	UINT flags = D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG;
	ID3DBlob* code;
	ID3DBlob* error;
	D3DCompileFromFile(filename, nullptr, nullptr, entry, target, flags, 0, &code, &error);
	if (error)
	{
		const char* errMsg = (char*)error->GetBufferPointer();
		OutputDebugString(errMsg);
		MessageBox(0, errMsg, entry, 0);
	}

	return code;
}

ID3D11Buffer* DXUtil::CreateDefaultBuffer(UINT byteWidth, UINT bindFlags, const void* initData)
{
	ID3D11Buffer* buffer;

	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = byteWidth;
	desc.BindFlags = bindFlags;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = initData;

	HR(device->CreateBuffer(&desc, &data, &buffer));

	return buffer;
}

void DXTrace(HRESULT hr, const char* filename, const char* func, int line)
{
	_com_error err(hr);
	char errmsg[1024];
	snprintf(errmsg, sizeof(errmsg), "HR: %s\nFile: %s\nFunction: %s\nLine: %d", err.ErrorMessage(), filename, func, line);
	MessageBox(0, errmsg, "Error", 0);
}