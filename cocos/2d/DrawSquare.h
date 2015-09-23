#pragma once

#include "2d/CCNode.h"
#include "base/CCProtocols.h"
#include "renderer/CCCustomCommand.h"

NS_CC_BEGIN

class CC_DLL DrawSquare : public Node
{
public:
	/** creates a fullscreen black layer */
	static DrawSquare* create();
	/** creates a Layer with color, width and height in Points */
	static DrawSquare* create(const Color4B& color, GLfloat width, GLfloat height);
	/** creates a Layer with color. Width and height are the window size. */
	static DrawSquare* create(const Color4B& color);

CC_CONSTRUCTOR_ACCESS:
	DrawSquare();
	virtual ~DrawSquare();

	bool initDefault();
	virtual bool init() override;
	bool initWithColor(const Color4B& color, GLfloat width, GLfloat height);
	bool initWithColor(const Color4B& color);

public:

	/** change width in Points*/
	void changeWidth(GLfloat w);
	/** change height in Points*/
	void changeHeight(GLfloat h);
	/** change width and height in Points
	@since v0.8
	*/
	void changeWidthAndHeight(GLfloat w, GLfloat h);

	virtual void setContentSize(const Size & var) override;

	//
	// Overrides
	//
	virtual void draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) override;
	
	virtual const BlendFunc& getBlendFunc() const;
	virtual void setBlendFunc(const BlendFunc& blendFunc);

protected:
	void onDraw(const Mat4& transform, uint32_t flags);

	virtual void updateColor() override;
		
	BlendFunc _blendFunc;
	Vec2 _squareVertices[4];
	Color4F  _squareColors[4];
	CustomCommand _customCommand;
	Vec3 _noMVPVertices[4];

	ID3D11Buffer *_bufferVertex;
	ID3D11Buffer *_bufferIndex;
	bool _bufferDirty;

	void UpdateVertexBuffer();
};

NS_CC_END