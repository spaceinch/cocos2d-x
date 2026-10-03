/****************************************************************************
 Copyright (c) 2010      Ricardo Quesada
 Copyright (c) 2010-2012 cocos2d-x.org
 Corpyight (c) 2011      Zynga Inc.
 Copyright (c) 2013-2017 Chukong Technologies Inc.

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

// Only compile this code on iOS. These files should NOT be included on your Mac project.
// But in case they are included, it won't be compiled.

#include "platform/CCPlatformConfig.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS

#import "platform/ios/CCESRenderer-ios.h"

#import <MetalANGLE/MGLKit.h>

#import "platform/CCPlatformMacros.h"

// MGLLayer (MetalANGLE's CAEAGLLayer equivalent) owns and sizes its own
// Metal-backed framebuffer/depth-stencil storage, so unlike the EAGL-based
// renderer this no longer hand-manages GL renderbuffer/framebuffer objects.
// It just tracks the layer's drawable size and the context, and forwards
// -defaultFrameBuffer to the layer's own framebuffer id. Pluck never
// requests multisampling (see CCGLViewImpl-ios.mm's multiSampling:NO), so
// the EAGL version's MSAA bookkeeping is not reimplemented here; the MSAA
// protocol accessors are kept only so CCEAGLView-ios.mm's (unused-for-Pluck)
// multisampling branch still type-checks.
@interface CCES2Renderer : NSObject <CCESRenderer>
{
    // The pixel dimensions of the MGLLayer's drawable
    GLint backingWidth_;
    GLint backingHeight_;

    unsigned int    depthFormat_;
    unsigned int    pixelFormat_;

    MGLContext *context_;
    MGLLayer *layer_; // weak; set by -resizeFromLayer:
}

/** MGLContext */
@property (nonatomic,readonly) MGLContext* context;

- (BOOL)resizeFromLayer:(MGLLayer *)layer;
@end


#endif // CC_PLATFORM_IOS
