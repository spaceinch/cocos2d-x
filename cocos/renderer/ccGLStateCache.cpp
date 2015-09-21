/****************************************************************************
Copyright (c) 2011      Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (C) 2013-2014 Chukong Technologies Inc.

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

#include "renderer/ccGLStateCache.h"

#include "renderer/CCGLProgram.h"
#include "renderer/CCRenderState.h"
#include "base/CCDirector.h"
#include "base/ccConfig.h"
#include "base/CCConfiguration.h"

NS_CC_BEGIN

#ifndef DIRECTX_ENABLED

static const int MAX_ATTRIBUTES = 16;
static const int MAX_ACTIVE_TEXTURE = 16;

namespace
{
    static GLuint s_currentProjectionMatrix = -1;
    static uint32_t s_attributeFlags = 0;  // 32 attributes max

#if CC_ENABLE_GL_STATE_CACHE

    static GLuint    s_currentShaderProgram = -1;
    static GLuint    s_currentBoundTexture[MAX_ACTIVE_TEXTURE] =  {(GLuint)-1,(GLuint)-1,(GLuint)-1,(GLuint)-1, (GLuint)-1,(GLuint)-1,(GLuint)-1,(GLuint)-1, (GLuint)-1,(GLuint)-1,(GLuint)-1,(GLuint)-1, (GLuint)-1,(GLuint)-1,(GLuint)-1,(GLuint)-1, };
    static GLenum    s_blendingSource = -1;
    static GLenum    s_blendingDest = -1;
    static int       s_GLServerState = 0;
    static GLuint    s_VAO = 0;
    static GLenum    s_activeTexture = -1;

#endif // CC_ENABLE_GL_STATE_CACHE
}

// GL State Cache functions

namespace GL {

void invalidateStateCache( void )
{
    Director::getInstance()->resetMatrixStack();
    s_currentProjectionMatrix = -1;
    s_attributeFlags = 0;

#if CC_ENABLE_GL_STATE_CACHE
    s_currentShaderProgram = -1;
    for( int i=0; i < MAX_ACTIVE_TEXTURE; i++ )
    {
        s_currentBoundTexture[i] = -1;
    }

    s_blendingSource = -1;
    s_blendingDest = -1;
    s_GLServerState = 0;
    s_VAO = 0;
    
#endif // CC_ENABLE_GL_STATE_CACHE
}

void deleteProgram( GLuint program )
{
#if CC_ENABLE_GL_STATE_CACHE
    if(program == s_currentShaderProgram)
    {
        s_currentShaderProgram = -1;
    }
#endif // CC_ENABLE_GL_STATE_CACHE

    glDeleteProgram( program );
}

void useProgram( GLuint program )
{
#if CC_ENABLE_GL_STATE_CACHE
    if( program != s_currentShaderProgram ) {
        s_currentShaderProgram = program;
        glUseProgram(program);
    }
#else
    glUseProgram(program);
#endif // CC_ENABLE_GL_STATE_CACHE
}

static void SetBlending(GLenum sfactor, GLenum dfactor)
{
	if (sfactor == GL_ONE && dfactor == GL_ZERO)
    {
		glDisable(GL_BLEND);
        RenderState::StateBlock::_defaultState->setBlend(false);
	}
    else
    {
		glEnable(GL_BLEND);
		glBlendFunc(sfactor, dfactor);

        RenderState::StateBlock::_defaultState->setBlend(true);
        RenderState::StateBlock::_defaultState->setBlendSrc((RenderState::Blend)sfactor);
        RenderState::StateBlock::_defaultState->setBlendSrc((RenderState::Blend)dfactor);
    }
}

void blendFunc(GLenum sfactor, GLenum dfactor)
{
#if CC_ENABLE_GL_STATE_CACHE
    if (sfactor != s_blendingSource || dfactor != s_blendingDest)
    {
        s_blendingSource = sfactor;
        s_blendingDest = dfactor;
        SetBlending(sfactor, dfactor);
    }
#else
    SetBlending( sfactor, dfactor );
#endif // CC_ENABLE_GL_STATE_CACHE
}

void blendResetToCache(void)
{
	glBlendEquation(GL_FUNC_ADD);
#if CC_ENABLE_GL_STATE_CACHE
	SetBlending(s_blendingSource, s_blendingDest);
#else
	SetBlending(CC_BLEND_SRC, CC_BLEND_DST);
#endif // CC_ENABLE_GL_STATE_CACHE
}

void bindTexture2D(GLuint textureId)
{
    GL::bindTexture2DN(0, textureId);
}

void bindTexture2DN(GLuint textureUnit, GLuint textureId)
{
#if CC_ENABLE_GL_STATE_CACHE
	CCASSERT(textureUnit < MAX_ACTIVE_TEXTURE, "textureUnit is too big");
	if (s_currentBoundTexture[textureUnit] != textureId)
	{
		s_currentBoundTexture[textureUnit] = textureId;
		activeTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(GL_TEXTURE_2D, textureId);
	}
#else
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, textureId);
#endif
}

void bindTextureN(GLuint textureUnit, GLuint textureId, GLuint textureType/* = GL_TEXTURE_2D*/)
{
#if CC_ENABLE_GL_STATE_CACHE
    CCASSERT(textureUnit < MAX_ACTIVE_TEXTURE, "textureUnit is too big");
    if (s_currentBoundTexture[textureUnit] != textureId)
    {
        s_currentBoundTexture[textureUnit] = textureId;
        activeTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(textureType, textureId);
    }
#else
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(textureType, textureId);
#endif
}


void deleteTexture(GLuint textureId)
{
#if CC_ENABLE_GL_STATE_CACHE
    for (size_t i = 0; i < MAX_ACTIVE_TEXTURE; ++i)
    {
        if (s_currentBoundTexture[i] == textureId)
        {
            s_currentBoundTexture[i] = -1;
        }
    }
#endif // CC_ENABLE_GL_STATE_CACHE
    
	glDeleteTextures(1, &textureId);
}

void deleteTextureN(GLuint textureUnit, GLuint textureId)
{
    deleteTexture(textureId);
}

void activeTexture(GLenum texture)
{
#if CC_ENABLE_GL_STATE_CACHE
    if(s_activeTexture != texture) {
        s_activeTexture = texture;
        glActiveTexture(s_activeTexture);
    }
#else
    glActiveTexture(texture);
#endif
}

void bindVAO(GLuint vaoId)
{
    if (Configuration::getInstance()->supportsShareableVAO())
    {
    
#if CC_ENABLE_GL_STATE_CACHE
        if (s_VAO != vaoId)
        {
            s_VAO = vaoId;
            glBindVertexArray(vaoId);
        }
#else
        glBindVertexArray(vaoId);
#endif // CC_ENABLE_GL_STATE_CACHE
    
    }
}

// GL Vertex Attrib functions

void enableVertexAttribs(uint32_t flags)
{
    bindVAO(0);

    // hardcoded!
    for(int i=0; i < MAX_ATTRIBUTES; i++) {
        unsigned int bit = 1 << i;
        //FIXME:Cache is disabled, try to enable cache as before
        bool enabled = (flags & bit) != 0;
        bool enabledBefore = (s_attributeFlags & bit) != 0;
        if(enabled != enabledBefore) 
        {
            if( enabled )
                glEnableVertexAttribArray(i);
            else
                glDisableVertexAttribArray(i);
        }
    }
    s_attributeFlags = flags;
}

// GL Uniforms functions

void setProjectionMatrixDirty( void )
{
    s_currentProjectionMatrix = -1;
}

} // Namespace GL

#else

#include "DirectXHelper.h"

DXStateCache::DXStateCache()
{
	_depthStencilState = nullptr;
	_blendState = nullptr;
	_vertexBuffer = nullptr;
	_indexBuffer = nullptr;
	_rasterizerState = nullptr;
	_inputLayout = nullptr;
	_vertexShader = nullptr;
	_pixelShader = nullptr;

	_clearColor[0] = 0;
	_clearColor[1] = 0;
	_clearColor[2] = 0;
	_clearColor[3] = 1.0f;
	_clearDepth = 1.0f;
	_clearStencil = 0;

	_scissorScaling = 1.0f;

	invalidateStateCache();
}

void DXStateCache::invalidateStateCache()
{
	_view = GLViewImpl::sharedOpenGLView();
	CCASSERT(_view, "GLView not set.");

	DXResourceManager::getInstance().remove(&_depthStencilState);
	DXResourceManager::getInstance().remove(&_blendState);
	DXResourceManager::getInstance().remove(&_rasterizerState);

	_vertexBuffer = nullptr;
	_indexBuffer = nullptr;
	_rasterizerState = nullptr;
	_inputLayout = nullptr;
	_vertexShader = nullptr;
	_pixelShader = nullptr;
	_blendState = nullptr;
	_depthStencilState = nullptr;

	_blendDesc = CD3D11_BLEND_DESC(CD3D11_DEFAULT());

	_rasterizerDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
	_rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;

	_depthStencilDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
	_depthStencilDesc.DepthEnable = false;
	_depthStencilRef = 0xFF;

	_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;

	memset(_constantBufferVS, 0, sizeof(_constantBufferVS));
	memset(_constantBufferPS, 0, sizeof(_constantBufferPS));
	memset(_textureViewsVS, 0, sizeof(_textureViewsVS));
	memset(_textureViewsPS, 0, sizeof(_textureViewsPS));
	memset(&_viewportRect, 0, sizeof(_viewportRect));
	memset(&_scissorRect, 0, sizeof(_scissorRect));
}

void DXStateCache::setShaders(ID3D11VertexShader *vs, ID3D11PixelShader *ps)
{
	if (vs != _vertexShader)
	{
		_vertexShader = vs;
		_view->GetContext()->VSSetShader(_vertexShader, nullptr, 0);
	}

	if (ps != _pixelShader)
	{
		_pixelShader = ps;
		_view->GetContext()->PSSetShader(_pixelShader, nullptr, 0);
	}
}

void DXStateCache::setVertexBuffer(ID3D11Buffer *buffer, UINT stride, UINT offset)
{
	if (buffer != _vertexBuffer)
	{
		_vertexBuffer = buffer;
		_view->GetContext()->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
	}
}

void DXStateCache::setIndexBuffer(ID3D11Buffer *buffer)
{
	if (buffer != _indexBuffer)
	{
		_indexBuffer = buffer;
		_view->GetContext()->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	}
}

void DXStateCache::setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology)
{
	if (_primitiveTopology != topology)
	{
		_primitiveTopology = topology;
		_view->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
}

void DXStateCache::setInputLayout(ID3D11InputLayout* layout)
{
	if (_inputLayout != layout)
	{
		_inputLayout = layout;
		_view->GetContext()->IASetInputLayout(_inputLayout);
	}
}

void DXStateCache::setVSConstantBuffer(int index, ID3D11Buffer*const* buffer)
{
	CCASSERT(index < MAX_UNITS, "Invalid index of unit.");
	if (_constantBufferVS[index] != buffer)
	{
		_constantBufferVS[index] = buffer;
		_view->GetContext()->VSSetConstantBuffers(index, 1, _constantBufferVS[index]);
	}
}

void DXStateCache::setPSConstantBuffer(int index, ID3D11Buffer*const* buffer)
{
	CCASSERT(index < MAX_UNITS, "Invalid index of unit.");
	if (_constantBufferPS[index] != buffer)
	{
		_constantBufferPS[index] = buffer;
		_view->GetContext()->PSSetConstantBuffers(index, 1, _constantBufferPS[index]);
	}
}

void DXStateCache::setVSTexture(int index, ID3D11ShaderResourceView*const* textureView)
{
	CCASSERT(index < MAX_UNITS, "Invalid index of unit.");
	if (_textureViewsVS[index] != textureView)
	{
		_textureViewsVS[index] = textureView;
		_view->GetContext()->VSSetShaderResources(index, 1, _textureViewsVS[index]);
	}
}

void DXStateCache::setPSTexture(int index, ID3D11ShaderResourceView*const* textureView)
{
	CCASSERT(index < MAX_UNITS, "Invalid index of unit.");
	if (_textureViewsPS[index] != textureView)
	{
		_textureViewsPS[index] = textureView;
		_view->GetContext()->PSSetShaderResources(index, 1, _textureViewsPS[index]);
	}
}

void DXStateCache::setBlend(GLint GLsrc, GLint GLdst)
{
	D3D11_BLEND src = GetDXBlend(GLsrc);
	D3D11_BLEND dst = GetDXBlend(GLdst);

	bool change = src != _blendDesc.RenderTarget[0].SrcBlend || dst != _blendDesc.RenderTarget[0].DestBlend;
	if (change || _blendState == nullptr)
	{
		DXResourceManager::getInstance().remove(&_blendState);

		_blendDesc = CD3D11_BLEND_DESC(CD3D11_DEFAULT());
		_blendDesc.RenderTarget[0].BlendEnable = true;
		_blendDesc.RenderTarget[0].SrcBlend = src;
		_blendDesc.RenderTarget[0].DestBlend = dst;
		_blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
		_blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		DX::ThrowIfFailed(_view->GetDevice()->CreateBlendState(&_blendDesc, &_blendState));

		_view->GetContext()->OMSetBlendState(_blendState, nullptr, 0xffffffff);

		DXResourceManager::getInstance().add(&_blendState);
	}

	setRasterizer();
}

void DXStateCache::setDepthStencilState(const D3D11_DEPTH_STENCIL_DESC& depthStencilDesc, UINT depthStencilRef)
{
	bool stateChanged = false;
	bool changed = false;

	if (memcmp(&depthStencilDesc, &_depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC)) != 0)
	{
		_depthStencilDesc = CD3D11_DEPTH_STENCIL_DESC(depthStencilDesc);
		stateChanged = true;
	}

	if (_depthStencilRef != depthStencilRef)
	{
		_depthStencilRef = depthStencilRef;
		changed = true;
	}

	if (stateChanged || !_depthStencilState)
	{
		DXResourceManager::getInstance().remove(&_depthStencilState);
		DX::ThrowIfFailed(_view->GetDevice()->CreateDepthStencilState(&_depthStencilDesc, &_depthStencilState));
		DXResourceManager::getInstance().add(&_depthStencilState);

		changed = true;
	}

	if (changed)
	{
		_view->GetContext()->OMSetDepthStencilState(_depthStencilState, _depthStencilRef);
	}
}

const D3D11_DEPTH_STENCIL_DESC& DXStateCache::getDepthStencilState() const
{
	return _depthStencilDesc;
}

UINT DXStateCache::getStencilRef() const
{
	return _depthStencilRef;
}

void DXStateCache::clear()
{
	auto view = GLViewImpl::sharedOpenGLView();
	auto context = view->GetContext();

	context->ClearRenderTargetView(*view->GetRenderTargetView(), _clearColor);

	UINT clearFlag = 0;
	if (_depthStencilDesc.DepthEnable) {
		clearFlag |= D3D11_CLEAR_DEPTH;
	}
	if (_depthStencilDesc.StencilEnable) {
		clearFlag |= D3D11_CLEAR_STENCIL;
	}
	if (clearFlag != 0) {
		context->ClearDepthStencilView(view->GetDepthStencilView(), clearFlag, _clearDepth, _clearStencil);
	}
}

void DXStateCache::clearColor(float r, float g, float b, float a)
{
	FLOAT clearColor[] = {r, g, b, a};
	auto view = GLViewImpl::sharedOpenGLView();
	auto context = view->GetContext();

	context->ClearRenderTargetView(*view->GetRenderTargetView(), clearColor);
}

void DXStateCache::clearDepth(float z)
{
	auto view = GLViewImpl::sharedOpenGLView();
	auto context = view->GetContext();
	context->ClearDepthStencilView(view->GetDepthStencilView(), D3D11_CLEAR_DEPTH, z, 0);
}

void DXStateCache::setClearColor(float r, float g, float b, float a)
{
	_clearColor[0] = r;
	_clearColor[1] = g;
	_clearColor[2] = b;
	_clearColor[3] = a;
}

void DXStateCache::setClearDepth(float z)
{
	_clearDepth = z;
}

void DXStateCache::setClearStencil(uint8_t s)
{
	_clearStencil = s;
}

void DXStateCache::setDepthTest(bool enabled)
{
	if (enabled != _depthStencilDesc.DepthEnable) {
		D3D11_DEPTH_STENCIL_DESC newDepthStencilState = D3D11_DEPTH_STENCIL_DESC(_depthStencilDesc);
		newDepthStencilState.DepthEnable = enabled;
		newDepthStencilState.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		setDepthStencilState(newDepthStencilState, _depthStencilRef);
	}
}

bool DXStateCache::isDepthTestEnabled()
{
	return _depthStencilDesc.DepthEnable;
}

void DXStateCache::setDepthMask(bool enabled)
{
	if (enabled != isDepthMaskEnabled()) {
		D3D11_DEPTH_STENCIL_DESC newDepthStencilState = D3D11_DEPTH_STENCIL_DESC(_depthStencilDesc);
		newDepthStencilState.DepthWriteMask = enabled ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
		setDepthStencilState(newDepthStencilState, _depthStencilRef);
	}
}

bool DXStateCache::isDepthMaskEnabled()
{
	return (_depthStencilDesc.DepthWriteMask != D3D11_DEPTH_WRITE_MASK_ZERO);
}

void DXStateCache::setCullTest(bool enabled)
{
	// nothing to do
}

bool DXStateCache::isCullTestEnabled()
{
	return false;
}

void DXStateCache::setViewport(float x, float y, float w, float h)
{
	CD3D11_VIEWPORT viewport(x, y, w, h);
	if (viewport != _viewportRect)
	{
		_viewportRect = viewport;
		_view->GetContext()->RSSetViewports(1, &_viewportRect);
	}
}

void DXStateCache::setScissor(float x, float y, float w, float h)
{
	CD3D11_RECT rect(x * _scissorScaling, y * _scissorScaling, (x + w) * _scissorScaling, (y + h) * _scissorScaling);
	if (_scissorRect != rect)
	{
		_scissorRect = rect;
		_view->GetContext()->RSSetScissorRects(1, &_scissorRect);
	}
}

void DXStateCache::getScissor(Rect& rect) const
{
	rect.origin.x = _scissorRect.left / _scissorScaling;
	rect.origin.y = _scissorRect.top / _scissorScaling;
	rect.size.width = (_scissorRect.right / _scissorScaling) - _scissorRect.left;
	rect.size.height = (_scissorRect.bottom / _scissorScaling) - _scissorRect.top;
}

void DXStateCache::enableScissor(bool enable)
{
	if (_rasterizerDesc.ScissorEnable != (BOOL)enable)
	{
		_rasterizerDesc.ScissorEnable = enable;
		_rasterizerDirty = true;
	}
}

bool DXStateCache::isScissorEnabled() const
{
	return _rasterizerDesc.ScissorEnable != 0;
}

void DXStateCache::setScissorScaling(float scaling)
{
	_scissorScaling = scaling;
}

void DXStateCache::setRasterizer()
{
	if (_rasterizerDirty || _rasterizerState == nullptr)
	{
		DXResourceManager::getInstance().remove(&_rasterizerState);
		DX::ThrowIfFailed(_view->GetDevice()->CreateRasterizerState(&_rasterizerDesc, &_rasterizerState));
		DXResourceManager::getInstance().add(&_rasterizerState);

		_rasterizerDirty = false;
	}

	_view->GetContext()->RSSetState(_rasterizerState);
}

D3D11_BLEND DXStateCache::GetDXBlend(GLint glBlend) const
{
	if (glBlend == GL_ZERO)
		return D3D11_BLEND_ZERO;
	else if (glBlend == GL_SRC_COLOR)
		return D3D11_BLEND_SRC_COLOR;
	else if (glBlend == GL_ONE_MINUS_SRC_COLOR)
		return D3D11_BLEND_INV_SRC_COLOR;
	else if (glBlend == GL_SRC_ALPHA)
		return D3D11_BLEND_SRC_ALPHA;
	else if (glBlend == GL_ONE_MINUS_SRC_ALPHA)
		return D3D11_BLEND_INV_SRC_ALPHA;
	else if (glBlend == GL_DST_ALPHA)
		return D3D11_BLEND_DEST_ALPHA;
	else if (glBlend == GL_ONE_MINUS_DST_ALPHA)
		return D3D11_BLEND_INV_DEST_ALPHA;
	else if (glBlend == GL_DST_COLOR)
		return D3D11_BLEND_DEST_COLOR;
	else if (glBlend == GL_ONE_MINUS_DST_COLOR)
		return D3D11_BLEND_INV_DEST_COLOR;
	else if (glBlend == GL_SRC_ALPHA_SATURATE)
		return D3D11_BLEND_SRC_ALPHA_SAT;
	return D3D11_BLEND_ONE;
}

void DXResourceManager::clear()
{
	clearBuffer();
	clearTexture();
	clearInputLayout();
	clearShaderResourceView();
	clearVS();
	clearRasterizerState();
	clearPS();
	clearBlendState();
	clearDepthStencilState();
	clearDepthStencilView();
	clearRenderTargetView();
}

#endif

NS_CC_END
