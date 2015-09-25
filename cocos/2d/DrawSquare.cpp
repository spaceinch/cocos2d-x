#include <stdarg.h>
#include "2d/DrawSquare.h"
#include "base/CCScriptSupport.h"
#include "platform/CCDevice.h"
#include "renderer/CCRenderer.h"
#include "renderer/ccGLStateCache.h"
#include "renderer/CCGLProgramState.h"
#include "base/CCDirector.h"

#ifdef DIRECTX_ENABLED
#include "platform/winrt/DirectXHelper.h"
#endif


NS_CC_BEGIN

DrawSquare::DrawSquare()
#ifdef DIRECTX_ENABLED
: _bufferVertex(nullptr)
, _bufferIndex(nullptr)
, _texture(0)
#endif
{
	// default blend function
	_blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
}

DrawSquare::~DrawSquare()
{
#ifdef DIRECTX_ENABLED
	DXResourceManager::getInstance().remove(&_bufferVertex);
	DXResourceManager::getInstance().remove(&_bufferIndex);
	disposeTexture();
#endif
}

/// blendFunc getter
const BlendFunc &DrawSquare::getBlendFunc() const
{
	return _blendFunc;
}
/// blendFunc setter
void DrawSquare::setBlendFunc(const BlendFunc &var)
{
	_blendFunc = var;
}

DrawSquare* DrawSquare::create()
{
	DrawSquare* ret = new (std::nothrow) DrawSquare();
	if (ret && ret->init())
	{
		ret->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(ret);
	}
	return ret;
}

DrawSquare * DrawSquare::create(const Color4B& color, GLfloat width, GLfloat height)
{
	DrawSquare * layer = new (std::nothrow) DrawSquare();
	if (layer && layer->initWithColor(color, width, height))
	{
		layer->autorelease();
		return layer;
	}
	CC_SAFE_DELETE(layer);
	return nullptr;
}

DrawSquare * DrawSquare::create(const Color4B& color)
{
	DrawSquare * layer = new (std::nothrow) DrawSquare();
	if (layer && layer->initWithColor(color))
	{
		layer->autorelease();
		return layer;
	}
	CC_SAFE_DELETE(layer);
	return nullptr;
}

bool DrawSquare::initDefault()
{
	Director * director = Director::getInstance();
	setContentSize(director->getWinSize());
	return true;
}

bool DrawSquare::init()
{
	Size s = Director::getInstance()->getWinSize();
	return initWithColor(Color4B(0, 0, 0, 0), s.width, s.height);
}

bool DrawSquare::initWithColor(const Color4B& color, GLfloat w, GLfloat h)
{
	if (initDefault())
	{
		createTexture("spine/cannon_flip.png");

		// default blend function
		_blendFunc = BlendFunc::ALPHA_NON_PREMULTIPLIED;

		_displayedColor.r = _realColor.r = color.r;
		_displayedColor.g = _realColor.g = color.g;
		_displayedColor.b = _realColor.b = color.b;
		_displayedOpacity = _realOpacity = color.a;

		for (size_t i = 0; i<sizeof(_squareVertices) / sizeof(_squareVertices[0]); i++)
		{
			_squareVertices[i].x = 0.0f;
			_squareVertices[i].y = 0.0f;
		}

		updateColor();
		setContentSize(Size(w, h));

		setGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR));
		return true;
	}
	return false;
}

bool DrawSquare::initWithColor(const Color4B& color)
{
	Size s = Director::getInstance()->getWinSize();
	this->initWithColor(color, s.width, s.height);
	return true;
}

/// override contentSize
void DrawSquare::setContentSize(const Size & size)
{
	_squareVertices[1].x = size.width;
	_squareVertices[2].y = size.height;
	_squareVertices[3].x = size.width;
	_squareVertices[3].y = size.height;

	Node::setContentSize(size);
}

void DrawSquare::changeWidthAndHeight(GLfloat w, GLfloat h)
{
	this->setContentSize(Size(w, h));
}

void DrawSquare::changeWidth(GLfloat w)
{
	this->setContentSize(Size(w, _contentSize.height));
}

void DrawSquare::changeHeight(GLfloat h)
{
	this->setContentSize(Size(_contentSize.width, h));
}

void DrawSquare::updateColor()
{
	for (unsigned int i = 0; i < 4; i++)
	{
		_squareColors[i].r = _displayedColor.r / 255.0f;
		_squareColors[i].g = _displayedColor.g / 255.0f;
		_squareColors[i].b = _displayedColor.b / 255.0f;
		_squareColors[i].a = _displayedOpacity / 255.0f;
	}
}

void DrawSquare::createTexture(const char* path)
{
	_texture = Director::getInstance()->getTextureCache()->addImage(path);
	_texture->retain();
}

void DrawSquare::disposeTexture()
{
	_texture->release();
}

void DrawSquare::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
	_customCommand.init(_globalZOrder, transform, flags);
	_customCommand.func = CC_CALLBACK_0(DrawSquare::onDraw, this, transform, flags);
	renderer->addCommand(&_customCommand);

	for (int i = 0; i < 4; ++i)
	{
		/*Vec4 pos;
		pos.x = _squareVertices[i].x; pos.y = _squareVertices[i].y; pos.z = _positionZ;
		pos.w = 1;*/
		//_modelViewTransform.transformVector(&pos);
		_noMVPVertices[i] = Vec3(_squareVertices[i].x,
								_squareVertices[i].y,
								_positionZ);//Vec3(pos.x, pos.y, pos.z) / pos.w
	}
}

void DrawSquare::onDraw(const Mat4& transform, uint32_t flags)
{
	getGLProgram()->use();
	getGLProgram()->setUniformsForBuiltins(transform);
	getGLProgram()->set();

	UpdateVertexBuffer();

	GLViewImpl *view = GLViewImpl::sharedOpenGLView();

	DXStateCache::getInstance().setPSTexture(0, _texture->getView());
	DXStateCache::getInstance().setBlend(_blendFunc.src, _blendFunc.dst);
	DXStateCache::getInstance().setVertexBuffer(_bufferVertex, sizeof(V3F_C4B_T2F), 0);
	DXStateCache::getInstance().setIndexBuffer(_bufferIndex);
	DXStateCache::getInstance().setPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	view->GetContext()->DrawIndexed(6, 0, 0);

	CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1, 4);
}

void DrawSquare::UpdateVertexBuffer()
{
	const int verticesCount = 4;
	const int quadCount = 1;

	V3F_C4B_T2F vertexData[verticesCount];
	for (int i = 0; i < verticesCount; ++i)
	{
		vertexData[i].vertices = _noMVPVertices[i];
		vertexData[i].colors = Color4B(_squareColors[i]);
		//vertexData[i].texCoords = Tex2F(0, 0);
	}
	vertexData[0].texCoords = Tex2F(0, 0);
	vertexData[1].texCoords = Tex2F(1, 0);
	vertexData[2].texCoords = Tex2F(0, 1);
	vertexData[3].texCoords = Tex2F(1, 1);

	GLViewImpl *view = GLViewImpl::sharedOpenGLView();

	if (!_bufferVertex)
	{
		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		vertexBufferData.pSysMem = vertexData;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;

		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(vertexData[0]) * verticesCount, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
		DX::ThrowIfFailed(view->GetDevice()->CreateBuffer(
			&vertexBufferDesc,
			&vertexBufferData,
			&_bufferVertex));

		DXResourceManager::getInstance().add(&_bufferVertex);
	}
	else
	{
		D3D11_MAPPED_SUBRESOURCE resource;
		view->GetContext()->Map(_bufferVertex, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
		memcpy(resource.pData, vertexData, sizeof(vertexData[0]) * verticesCount);
		view->GetContext()->Unmap(_bufferVertex, 0);
	}

	if (!_bufferIndex)
	{
		GLushort indices[quadCount * 6];

		for (int i = 0; i < quadCount; i++)
		{
			indices[i * 6 + 0] = (GLushort)(i * 4 + 0);
			indices[i * 6 + 1] = (GLushort)(i * 4 + 1);
			indices[i * 6 + 2] = (GLushort)(i * 4 + 2);
			indices[i * 6 + 3] = (GLushort)(i * 4 + 3);
			indices[i * 6 + 4] = (GLushort)(i * 4 + 2);
			indices[i * 6 + 5] = (GLushort)(i * 4 + 1);
		}

		D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
		indexBufferData.pSysMem = indices;
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;

		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(indices[0]) * quadCount * 6, D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(view->GetDevice()->CreateBuffer(
			&indexBufferDesc,
			&indexBufferData,
			&_bufferIndex));

		DXResourceManager::getInstance().add(&_bufferIndex);
	}
}

NS_CC_END
