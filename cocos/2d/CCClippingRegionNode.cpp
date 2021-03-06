
#include "CCClippingRegionNode.h"
#include "base/CCDirector.h"
#include "renderer/CCRenderer.h"
#include "math/Vec2.h"
#include "CCGLView.h"

NS_CC_BEGIN

ClippingRegionNode* ClippingRegionNode::create(const Rect& clippingRegion)
{
    ClippingRegionNode* node = new ClippingRegionNode();
    if (node && node->init()) {
        node->setClippingRegion(clippingRegion);
        node->autorelease();
    } else {
        CC_SAFE_DELETE(node);
    }
    
    return node;
}

ClippingRegionNode* ClippingRegionNode::create(void)
{
    ClippingRegionNode* node = new ClippingRegionNode();
    if (node && node->init()) {
        node->autorelease();
    } else {
        CC_SAFE_DELETE(node);
    }
    
    return node;
}

void ClippingRegionNode::setClippingRegion(const Rect &clippingRegion)
{
    m_clippingRegion = clippingRegion;
}

void ClippingRegionNode::onBeforeVisitScissor()
{
    if (m_clippingEnabled) {
        glEnable(GL_SCISSOR_TEST);
        
        float scaleX = _scaleX;
        float scaleY = _scaleY;
        Node *parent = this->getParent();
        while (parent) {
            scaleX *= parent->getScaleX();
            scaleY *= parent->getScaleY();
            parent = parent->getParent();
        }
        
        const Point pos = convertToWorldSpace(Point(m_clippingRegion.origin.x, m_clippingRegion.origin.y));
        GLView* glView = Director::getInstance()->getOpenGLView();
        glView->setScissorInPoints(pos.x * scaleX,
                                   pos.y * scaleY,
                                   m_clippingRegion.size.width * scaleX,
                                   m_clippingRegion.size.height * scaleY);
    }
}

void ClippingRegionNode::onAfterVisitScissor()
{
    if (m_clippingEnabled)
    {
        glDisable(GL_SCISSOR_TEST);
    }
}

void ClippingRegionNode::visit(Renderer *renderer, const Mat4 &parentTransform, uint32_t parentFlags)
{
    _beforeVisitCmdScissor.init(_globalZOrder);
    _beforeVisitCmdScissor.func = CC_CALLBACK_0(ClippingRegionNode::onBeforeVisitScissor, this);
    renderer->addCommand(&_beforeVisitCmdScissor);
    
    Node::visit(renderer, parentTransform, parentFlags);
    
    _afterVisitCmdScissor.init(_globalZOrder);
    _afterVisitCmdScissor.func = CC_CALLBACK_0(ClippingRegionNode::onAfterVisitScissor, this);
    renderer->addCommand(&_afterVisitCmdScissor);
}

NS_CC_END
