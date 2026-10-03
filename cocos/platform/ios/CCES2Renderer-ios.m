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

#import "platform/ios/CCES2Renderer-ios.h"
#import "platform/CCPlatformMacros.h"
#import "platform/ios/OpenGL_Internal-ios.h"

#if !defined(COCOS2D_DEBUG) || COCOS2D_DEBUG == 0
#define NSLog(...)       do {} while (0)
#endif

@implementation CCES2Renderer

@synthesize context=context_;

// Create an OpenGL ES 2.0 context backed by MetalANGLE (Metal instead of
// Apple's deprecated OpenGLES driver). Unlike the EAGL version, there is no
// up-front framebuffer/renderbuffer object to create here: MGLLayer (bound
// in -resizeFromLayer:) owns and sizes its own Metal-backed storage,
// including the optional depth/stencil buffer, based on the
// drawableColorFormat/drawableDepthFormat/drawableStencilFormat set on it
// below.
- (id) initWithDepthFormat:(unsigned int)depthFormat withPixelFormat:(unsigned int)pixelFormat withSharegroup:(id)sharegroup withMultiSampling:(BOOL) multiSampling withNumberOfSamples:(unsigned int) requestedSamples
{
    self = [super init];
    if (self)
    {
        // Pluck never requests a shared context or multisampling (see
        // CCGLViewImpl-ios.mm), so neither is implemented here.
        NSAssert(!sharegroup, @"Shared MGLContexts are not implemented by this Metal renderer");
        NSAssert(!multiSampling, @"Multisampling is not implemented by this Metal renderer");

        context_ = [[MGLContext alloc] initWithAPI:kMGLRenderingAPIOpenGLES2];

        if (!context_ || ![MGLContext setCurrentContext:context_] )
        {
            [self release];
            return nil;
        }

        depthFormat_ = depthFormat;
        pixelFormat_ = pixelFormat;

        CHECK_GL_ERROR();
    }

    return self;
}

- (BOOL)resizeFromLayer:(MGLLayer *)layer
{
    layer_ = layer;

    // Translate the GL enums CCGLViewImpl-ios.mm picked (see convertAttrs())
    // into the drawable formats MGLLayer wants, then let it (re)allocate its
    // own Metal-backed storage for the new size.
    layer.drawableColorFormat = (pixelFormat_ == GL_RGB565) ? MGLDrawableColorFormatRGB565 : MGLDrawableColorFormatRGBA8888;

    if (depthFormat_ == GL_DEPTH24_STENCIL8_OES)
    {
        layer.drawableDepthFormat = MGLDrawableDepthFormat24;
        layer.drawableStencilFormat = MGLDrawableStencilFormat8;
    }
    else if (depthFormat_ == GL_DEPTH_COMPONENT16)
    {
        layer.drawableDepthFormat = MGLDrawableDepthFormat16;
        layer.drawableStencilFormat = MGLDrawableStencilFormatNone;
    }
    else
    {
        layer.drawableDepthFormat = MGLDrawableDepthFormatNone;
        layer.drawableStencilFormat = MGLDrawableStencilFormatNone;
    }

    [MGLContext setCurrentContext:context_ forLayer:layer];
    [layer bindDefaultFrameBuffer];

    CGSize size = layer.drawableSize;
    backingWidth_ = (GLint)size.width;
    backingHeight_ = (GLint)size.height;

    NSLog(@"cocos2d: surface size: %dx%d", (int)backingWidth_, (int)backingHeight_);

    CHECK_GL_ERROR();

    return YES;
}

-(CGSize) backingSize
{
    return CGSizeMake( backingWidth_, backingHeight_);
}

- (NSString*) description
{
    return [NSString stringWithFormat:@"<%@ = %08X | size = %ix%i>", [self class], (unsigned int)self, backingWidth_, backingHeight_];
}

// Unused under MetalANGLE (MGLLayer manages its own renderbuffer storage);
// kept only so CCEAGLView-ios.mm's dead (Pluck never enables multiSampling)
// MSAA branch still type-checks against the CCESRenderer protocol.
- (unsigned int) colorRenderBuffer
{
    return 0;
}

- (unsigned int) defaultFrameBuffer
{
    return layer_.defaultOpenGLFrameBufferID;
}

- (unsigned int) msaaFrameBuffer
{
    return 0;
}

- (unsigned int) msaaColorBuffer
{
    return 0;
}

- (void)dealloc
{
//    CCLOGINFO("deallocing CCES2Renderer: %p", self);

    // Tear down context
    if ([MGLContext currentContext] == context_)
        [MGLContext setCurrentContext:nil];

    [context_ release];
    context_ = nil;

    [super dealloc];
}

@end

#endif // CC_PLATFORM_IOS
