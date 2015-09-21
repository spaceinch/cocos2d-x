/****************************************************************************
Copyright (c) 2013 cocos2d-x.org
Copyright (c) Microsoft Open Technologies, Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "DirectXBase.h"
#include "DirectXHelper.h"
#include "platform/winrt/DirectXHelper.h"

// Constructor.
DirectXBase::DirectXBase()
{
}

// Initialize the Direct3D resources required to run.
void DirectXBase::Initialize(_In_ ID3D11Device1 *device)
{
	m_d3dDevice = device;
	CreateDeviceResources();
}

// These are the resources that depend on the device.
void DirectXBase::CreateDeviceResources()
{
#ifdef _DEBUG
	ID3D11InfoQueue *infoQueue;
	HRESULT result = m_d3dDevice.Get()->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)&infoQueue);
	
	if (SUCCEEDED(result)) {
		D3D11_MESSAGE_ID hideMessages[]  = {
			D3D11_MESSAGE_ID_DEVICE_DRAW_SAMPLER_NOT_SET,
		};
		
		D3D11_INFO_QUEUE_FILTER filter = {0};
		filter.DenyList.NumIDs = ARRAYSIZE(hideMessages);
		filter.DenyList.pIDList = hideMessages;
		
		infoQueue->AddStorageFilterEntries(&filter);
		infoQueue->Release();
	}
#endif
}

void Direct3DBase::UpdateDevice(_In_ ID3D11Device1* device, _In_ ID3D11DeviceContext1* context, _In_ ID3D11RenderTargetView* renderTargetView)
{
	m_d3dContext = context;
	m_renderTargetView = renderTargetView;

	if (m_d3dDevice.Get() != device)
	{
		m_d3dDevice = device;
		CreateDeviceResources();

		m_renderTargetSize.Width = -1;
		m_renderTargetSize.Height = -1;
	}

	if (m_d3dContext.Get() != context)
	{
		m_renderTargetSize.Width = -1;
		m_renderTargetSize.Height = -1;
	}

	Microsoft::WRL::ComPtr<ID3D11Resource> renderTargetViewResource;
	m_renderTargetView->GetResource(&renderTargetViewResource);

	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	DX::ThrowIfFailed(
		renderTargetViewResource.As(&backBuffer)
		);

	D3D11_TEXTURE2D_DESC backBufferDesc;
	backBuffer->GetDesc(&backBufferDesc);

	if (m_renderTargetSize.Width != static_cast<float>(backBufferDesc.Width) || m_renderTargetSize.Height != static_cast<float>(backBufferDesc.Height))
	{
		m_renderTargetSize.Width = static_cast<float>(backBufferDesc.Width);
		m_renderTargetSize.Height = static_cast<float>(backBufferDesc.Height);
		CreateWindowSizeDependentResources();
	}

	// Set the rendering viewport to target the entire window.
	CD3D11_VIEWPORT viewport(
		0.0f,
		0.0f,
		m_renderTargetSize.Width,
		m_renderTargetSize.Height
		);

	m_d3dContext->RSSetViewports(1, &viewport);
}

void Direct3DBase::CreateWindowSizeDependentResources()
{
	CD3D11_TEXTURE2D_DESC depthStencilDescription
	(
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		static_cast<UINT>(m_renderTargetSize.Width),
		static_cast<UINT>(m_renderTargetSize.Height),
		1,
		1,
		D3D11_BIND_DEPTH_STENCIL
	);

	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencil;
	DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(
		&depthStencilDescription,
		nullptr,
		&depthStencil));

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDescription
	(
		D3D11_DSV_DIMENSION_TEXTURE2D
	);

	DX::ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(
		depthStencil.Get(),
		&depthStencilViewDescription,
		&m_depthStencilView
		));
}

void Direct3DBase::UpdateForWindowSizeChange(float width, float height)
{
	m_windowBounds.Width = width;
	m_windowBounds.Height = height;
}

void Direct3DBase::Clear()
{
	if (m_d3dDevice)
	{
#ifdef _DEBUG
		Microsoft::WRL::ComPtr<ID3D11Debug> pDebug;
		m_d3dDevice.Get()->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void **>(pDebug.GetAddressOf()));
		if (pDebug)
		{
			pDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		}
#endif

		cocos2d::DXResourceManager::getInstance().clear();
		cocos2d::DXStateCache::getInstance().invalidateStateCache();

		m_renderTargetView = nullptr;
		m_depthStencilView = nullptr;

		// m_d3dContext->ClearState(); // Breaks AdMob 
		m_d3dContext->Flush();
		m_d3dContext = nullptr; 

#ifdef _DEBUG
		if (pDebug)
		{
			pDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
			pDebug = nullptr;
		}
#endif

		m_d3dDevice = nullptr;
	}
}
