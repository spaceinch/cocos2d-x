
#include "CCClippingRectangleNode.h"
#include "base/CCDirector.h"
#include "renderer/CCRenderer.h"
#include "math/Vec2.h"
#include "CCGLView.h"

NS_CC_BEGIN

ClippingRectangleNode* ClippingRectangleNode::create(const Rect& clippingRegion)
{
    ClippingRectangleNode* node = new ClippingRectangleNode();
    if (node && node->init()) {
        node->setClippingRegion(clippingRegion);
        node->autorelease();
    } else {
        CC_SAFE_DELETE(node);
    }
    
    return node;
}

ClippingRectangleNode* ClippingRectangleNode::create()
{
    ClippingRectangleNode* node = new ClippingRectangleNode();
    if (node && node->init()) {
        node->autorelease();
    } else {
        CC_SAFE_DELETE(node);
    }
    
    return node;
}

void ClippingRectangleNode::setClippingRegion(const Rect &clippingRegion)
{
    _clippingRegion = clippingRegion;
}

void ClippingRectangleNode::onBeforeVisitScissor()
{
#ifdef DIRECTX_ENABLED
	CCASSERT(false, "ClippingRectangleNode::onBeforeVisitScissor is not supported");
#else
    if (_clippingEnabled) {
        glEnable(GL_SCISSOR_TEST);

        GLView* glView = Director::getInstance()->getOpenGLView();
        glView->setScissorInPoints(_clippingRegion.origin.x,
                                   _clippingRegion.origin.y,
                                   _clippingRegion.size.width,
                                   _clippingRegion.size.height);
    }
#endif
}

void ClippingRectangleNode::onAfterVisitScissor()
{
#ifdef DIRECTX_ENABLED
	CCASSERT(false, "ClippingRectangleNode::onAfterVisitScissor is not supported");
#else
    if (_clippingEnabled)
    {
        glDisable(GL_SCISSOR_TEST);
    }
#endif
}

void ClippingRectangleNode::visit(Renderer *renderer, const Mat4 &parentTransform, uint32_t parentFlags)
{
    _beforeVisitCmdScissor.init(_globalZOrder);
    _beforeVisitCmdScissor.func = CC_CALLBACK_0(ClippingRectangleNode::onBeforeVisitScissor, this);
    renderer->addCommand(&_beforeVisitCmdScissor);
    
    Node::visit(renderer, parentTransform, parentFlags);
    
    _afterVisitCmdScissor.init(_globalZOrder);
    _afterVisitCmdScissor.func = CC_CALLBACK_0(ClippingRectangleNode::onAfterVisitScissor, this);
    renderer->addCommand(&_afterVisitCmdScissor);
}

NS_CC_END
