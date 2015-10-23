/****************************************************************************
 Copyright (c) 2011      Ricardo Quesada
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2011      Zynga Inc.
 Copyright (C) 2013-2015 Chukong Technologies Inc.

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

#ifndef __CCGLSTATE_H__
#define __CCGLSTATE_H__

#include <cstdint>

#include "platform/CCGL.h"
#include "platform/CCPlatformMacros.h"

#ifdef DIRECTX_ENABLED
	#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8)
		#include "wp8/CCGLViewImpl-wp8.h"
	#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
		#include "winrt/CCGLViewImpl-winrt.h"
	#endif
#endif

NS_CC_BEGIN

/**
 * @addtogroup renderer
 * @{
 */

class GLProgram;

#ifndef DIRECTX_ENABLED
namespace GL {

/** Vertex attrib flags. */
enum {
    VERTEX_ATTRIB_FLAG_NONE       = 0,

    VERTEX_ATTRIB_FLAG_POSITION   = 1 << 0,
    VERTEX_ATTRIB_FLAG_COLOR      = 1 << 1,
    VERTEX_ATTRIB_FLAG_TEX_COORD = 1 << 2,
    VERTEX_ATTRIB_FLAG_NORMAL = 1 << 3,
    VERTEX_ATTRIB_FLAG_BLEND_WEIGHT = 1 << 4,
    VERTEX_ATTRIB_FLAG_BLEND_INDEX = 1 << 5,
    
    VERTEX_ATTRIB_FLAG_POS_COLOR_TEX = (VERTEX_ATTRIB_FLAG_POSITION | VERTEX_ATTRIB_FLAG_COLOR | VERTEX_ATTRIB_FLAG_TEX_COORD),
};

/** 
 * Invalidates the GL state cache.
 *
 * If CC_ENABLE_GL_STATE_CACHE it will reset the GL state cache.
 * @since v2.0.0
 */
void CC_DLL invalidateStateCache(void);

/** 
 * Uses the GL program in case program is different than the current one.

 * If CC_ENABLE_GL_STATE_CACHE is disabled, it will the glUseProgram() directly.
 * @since v2.0.0
 */
void CC_DLL useProgram(GLuint program);

/** 
 * Deletes the GL program. If it is the one that is being used, it invalidates it.
 *
 * If CC_ENABLE_GL_STATE_CACHE is disabled, it will the glDeleteProgram() directly.
 * @since v2.0.0
 */
void CC_DLL deleteProgram(GLuint program);

/** 
 * Uses a blending function in case it not already used.
 *
 * If CC_ENABLE_GL_STATE_CACHE is disabled, it will the glBlendFunc() directly.
 * @since v2.0.0
 */
void CC_DLL blendFunc(GLenum sfactor, GLenum dfactor);

/** 
 * Resets the blending mode back to the cached state in case you used glBlendFuncSeparate() or glBlendEquation().
 *
 * If CC_ENABLE_GL_STATE_CACHE is disabled, it will just set the default blending mode using GL_FUNC_ADD.
 * @since v2.0.0
 */
void CC_DLL blendResetToCache(void);

/** 
 * Sets the projection matrix as dirty.
 * @since v2.0.0
 */
void CC_DLL setProjectionMatrixDirty(void);

/** 
 * Will enable the vertex attribs that are passed as flags.
 * Possible flags:
 * 
 *    * VERTEX_ATTRIB_FLAG_POSITION
 *    * VERTEX_ATTRIB_FLAG_COLOR
 *    * VERTEX_ATTRIB_FLAG_TEX_COORDS
 * 
 * These flags can be ORed. The flags that are not present, will be disabled.
 * 
 * @since v2.0.0
 */
void CC_DLL enableVertexAttribs(uint32_t flags);

/** 
 * If the texture is not already bound to texture unit 0, it binds it.
 *
 * If CC_ENABLE_GL_STATE_CACHE is disabled, it will call glBindTexture() directly.
 * @since v2.0.0
 */
void CC_DLL bindTexture2D(GLuint textureId);

/** 
 * If the texture is not already bound to a given unit, it binds it.
 *
 * If CC_ENABLE_GL_STATE_CACHE is disabled, it will call glBindTexture() directly.
 * @since v2.1.0
 */
void CC_DLL bindTexture2DN(GLuint textureUnit, GLuint textureId);

/** If the texture is not already bound to a given unit, it binds it.
 * If CC_ENABLE_GL_STATE_CACHE is disabled, it will call glBindTexture() directly.
 * @since v3.6
 */
void CC_DLL bindTextureN(GLuint textureUnit, GLuint textureId, GLuint textureType = GL_TEXTURE_2D);

/** 
 * It will delete a given texture. If the texture was bound, it will invalidate the cached.
 *
 * If CC_ENABLE_GL_STATE_CACHE is disabled, it will call glDeleteTextures() directly.
 * @since v2.0.0
 */
void CC_DLL deleteTexture(GLuint textureId);

/** 
 * It will delete a given texture. If the texture was bound, it will invalidate the cached for the given texture unit.
 *
 * If CC_ENABLE_GL_STATE_CACHE is disabled, it will call glDeleteTextures() directly.
 * @since v2.1.0
 */
CC_DEPRECATED_ATTRIBUTE void CC_DLL deleteTextureN(GLuint textureUnit, GLuint textureId);

/** 
 * Select active texture unit.
 *
 * If CC_ENABLE_GL_STATE_CACHE is disabled, it will call glActiveTexture() directly.
 * @since v3.0
 */
void CC_DLL activeTexture(GLenum texture);

/** 
 * If the vertex array is not already bound, it binds it.
 *
 * If CC_ENABLE_GL_STATE_CACHE is disabled, it will call glBindVertexArray() directly.
 * @since v2.0.0
 */
void CC_DLL bindVAO(GLuint vaoId);

// end of support group
/// @}

} // Namespace GL

#else

class GLView;

class CC_DLL DXStateCache
{
public:
	static DXStateCache& getInstance()
	{
		static auto ptr = std::unique_ptr<DXStateCache>(new DXStateCache);
		return *ptr.get();
	}

	D3D11_BLEND GetDXBlend(GLint glBlend) const;

	void invalidateStateCache();

	void setShaders(ID3D11VertexShader* vs, ID3D11PixelShader* ps);

	void setVertexBuffer(ID3D11Buffer* buffer, UINT stride, UINT offset);
	void setIndexBuffer(ID3D11Buffer* buffer);

	void setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology);
	void setInputLayout(ID3D11InputLayout* layout);

	void setVSConstantBuffer(int index, ID3D11Buffer*const* buffer);
	void setPSConstantBuffer(int index, ID3D11Buffer*const* buffer);

	void setVSTexture(int index, ID3D11ShaderResourceView*const* view);
	void setPSTexture(int index, ID3D11ShaderResourceView*const* view);

	void setBlend(GLint src, GLint dst);

	void setViewport(float x, float y, float w, float h);

	void setScissor(float x, float y, float w, float h);
	void getScissor(Rect& rect) const;
	void enableScissor(bool enable);
	bool isScissorEnabled() const;
	void setScissorScaling(float scaling);

	void setRasterizer();

	void setDepthStencilState(const D3D11_DEPTH_STENCIL_DESC& depthStencilDesc, UINT depthStencilRef);
	const D3D11_DEPTH_STENCIL_DESC& getDepthStencilState() const;
	UINT getStencilRef() const;

	void clear();
	void clearColor(float r, float g, float b, float a);
	void clearDepth(float z);
	void setClearColor(float r, float g, float b, float a);
	const FLOAT* getClearColor() const;
	void setClearDepth(float z);
	void setClearStencil(uint8_t s);

	void setDepthTest(bool enabled);
	bool isDepthTestEnabled();
	void setDepthMask(bool enabled);
	bool isDepthMaskEnabled();
	void setCullTest(bool enabled);
	bool isCullTestEnabled();

private:
	DXStateCache();

	static const int MAX_UNITS = 8;

	GLViewImpl* _view;

	DXStateCache(const DXStateCache&);
	DXStateCache& operator=(const DXStateCache&);

	// Cached values
	ID3D11Buffer* _vertexBuffer;
	ID3D11Buffer* _indexBuffer;
	ID3D11VertexShader* _vertexShader;
	ID3D11PixelShader* _pixelShader;
	ID3D11Buffer*const* _constantBufferPS[MAX_UNITS];
	ID3D11Buffer*const* _constantBufferVS[MAX_UNITS];
	D3D11_PRIMITIVE_TOPOLOGY _primitiveTopology;
	ID3D11InputLayout* _inputLayout;
	ID3D11ShaderResourceView*const* _textureViewsPS[MAX_UNITS];
	ID3D11ShaderResourceView*const* _textureViewsVS[MAX_UNITS];

	bool _rasterizerDirty;
	CD3D11_RASTERIZER_DESC _rasterizerDesc;
	ID3D11RasterizerState* _rasterizerState;

	float _scissorScaling;
	CD3D11_RECT _scissorRect;

	D3D11_VIEWPORT _viewportRect;

	CD3D11_BLEND_DESC _blendDesc;
	ID3D11BlendState* _blendState;

	CD3D11_DEPTH_STENCIL_DESC _depthStencilDesc;
	UINT _depthStencilRef;
	ID3D11DepthStencilState *_depthStencilState;

	FLOAT _clearColor[4];
	FLOAT _clearDepth;
	UINT8 _clearStencil;
};

class CC_DLL DXResourceManager
{
public:
	static DXResourceManager & getInstance()
	{
		static auto ptr = std::unique_ptr<DXResourceManager>(new DXResourceManager);
		return *ptr.get();
	}

#define DX_MANAGER_PROPERTY_DEF(type, name) \
public: \
	void add(type *##name) \
	{ \
	if (*##name) \
	_##name.insert(##name); \
	} \
	\
	void remove(type * ##name) \
	{ \
	if (*##name) \
	{ \
	auto it = _##name.find(##name); \
	if (it != _##name.end()) \
	{ \
	(*##name)->Release(); \
	_##name.erase(it); \
	*##name = nullptr; \
	} \
	} \
	} \
private: \
	::std::set<type *> _##name; \
	\
	void clear##name() \
	{ \
	for (auto a : _##name) \
	{ \
	(*a)->Release(); \
	*a = nullptr; \
	} \
	_##name.clear(); \
	}

public:
	DX_MANAGER_PROPERTY_DEF(ID3D11Buffer *, Buffer);
	DX_MANAGER_PROPERTY_DEF(ID3D11Texture2D *, Texture);
	DX_MANAGER_PROPERTY_DEF(ID3D11InputLayout *, InputLayout);
	DX_MANAGER_PROPERTY_DEF(ID3D11ShaderResourceView *, ShaderResourceView);
	DX_MANAGER_PROPERTY_DEF(ID3D11VertexShader *, VS);
	DX_MANAGER_PROPERTY_DEF(ID3D11RasterizerState *, RasterizerState);
	DX_MANAGER_PROPERTY_DEF(ID3D11PixelShader *, PS);
	DX_MANAGER_PROPERTY_DEF(ID3D11BlendState *, BlendState);
	DX_MANAGER_PROPERTY_DEF(ID3D11DepthStencilView *, DepthStencilView);
	DX_MANAGER_PROPERTY_DEF(ID3D11RenderTargetView *, RenderTargetView);
	DX_MANAGER_PROPERTY_DEF(ID3D11DepthStencilState *, DepthStencilState);

public:
	void clear();
};

#endif

NS_CC_END
    

#endif /* __CCGLSTATE_H__ */
