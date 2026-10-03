# MetalANGLE

Vendored prebuilt `MetalANGLE.xcframework` (device arm64 + simulator
arm64/x86_64/i386), used to route cocos2d-x's iOS OpenGL ES 2.0 rendering
through Metal instead of Apple's deprecated OpenGLES framework.

- Source: https://github.com/kakashidinho/metalangle
- Release: gles3-0.0.8 (2022-07-12)
  - https://github.com/kakashidinho/metalangle/releases/download/gles3-0.0.8/MetalANGLE.framework.ios.zip
  - https://github.com/kakashidinho/metalangle/releases/download/gles3-0.0.8/MetalANGLE.framework.ios.simulator.zip
- License: BSD-3-Clause-style ANGLE license (see https://github.com/kakashidinho/metalangle/blob/master/LICENSE).
  Permissive; no attribution requirement beyond retaining copyright/license text in redistributions.
- The xcframework was created locally from the two prebuilt per-platform
  `MetalANGLE.framework` zips above with:
  `xcodebuild -create-xcframework -framework <device>/MetalANGLE.framework -framework <sim>/MetalANGLE.framework -output MetalANGLE.xcframework`

Risk: metalangle appears largely unmaintained upstream (last tagged release
2022); if Apple removes OpenGLES entirely in a future iOS this will need
re-evaluating (e.g. a from-source ANGLE build, or the cocos2d-x 4.x Metal
backend).

RESOLVED BLOCKERS (iOS Simulator, iOS 27 runtime, found 2026-10-03, fixed
same day): two independent bugs, both fixed in cocos2d, not in MetalANGLE
itself.

1. The Debug build crashed on first frame with `(Metal)
   MTLDebugValidateMTLPixelFormat, line 1641: error 'pixelFormat (42) is
   not a valid MTLPixelFormat.'`, raised from `rx::mtl::Texture::Make2DTexture`
   while cocos2d created the debug FPS stats-label texture
   (`Director::createStatsLabel`) as RGBA4444. MetalANGLE translates
   RGBA4444 to the packed 16-bit `MTLPixelFormatABGR4Unorm`, which only
   exists on Apple-silicon iOS GPUs -- not the Mac GPU backing the
   Simulator. Fix: `Director::createStatsLabel` now uses RGBA8888 for this
   texture on iOS (`cocos/base/CCDirector.cpp`); Pluck's own textures are
   all RGBA8888 already and were never affected.

2. With the stats overlay off, the game rendered a solid, static-colored
   screen (whatever the current scene's clear color was) with no sprites,
   labels, or other draws visible, and no GL/Metal errors logged. The
   actual cause (not obvious without instrumenting `GLProgram::link()` to
   log `glGetProgramInfoLog`, which it didn't do before): **every single
   cocos2d shader program failed to link** with "Precisions of uniform
   'CC_PMatrix' differ between VERTEX and FRAGMENT shaders." The shared
   uniform block cocos2d prepends to both the vertex and fragment
   compilation units (`COCOS2D_SHADER_UNIFORMS` in
   `cocos/renderer/CCGLProgram.cpp`) declared `CC_PMatrix` and friends with
   no explicit precision qualifier, so each picked up its stage's
   differing *default* precision (`GLProgram::compileShader` sets vertex
   default to `highp`, fragment to `mediump`). Apple's old EAGL/GLES
   driver never enforced the GLSL ES spec rule that a uniform shared
   between stages must have matching precision; MetalANGLE's ANGLE-based
   Metal backend does enforce it, so linking failed uniformly and nothing
   ever drew past the initial `glClear`. Fix: pin an explicit, matching
   `highp` precision on every uniform in `COCOS2D_SHADER_UNIFORMS`.

With both fixes, Debug and Release-equivalent Simulator builds render
Pluck's intro scene, level 1 board, tutorial card, and post-tutorial
gameplay correctly, matching origin/develop (pre-Metal) screenshots, via
MetalANGLE's "ANGLE (Metal Renderer: Apple iOS simulator GPU)" / "OpenGL
ES 2.0.0 (ANGLE 2.1.0.850c87ba5b74)". Not yet tested on a physical device.
