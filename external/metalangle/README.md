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

KNOWN BLOCKER (iOS Simulator, iOS 27 runtime, as of 2026-10-03): the Debug
build crashes on first frame with
`(Metal) MTLDebugValidateMTLPixelFormat, line 1641: error 'pixelFormat (42)
is not a valid MTLPixelFormat.'`, raised from
`rx::mtl::Texture::Make2DTexture` while cocos2d creates the debug FPS
stats-label texture (`Director::createStatsLabel` ->
`TextureCache::addImage` -> `Texture2D::initWithImage`). With the stats
overlay disabled (`Director::setDisplayStats(false)`) the crash disappears,
but the title/garden scene then renders a solid black screen indefinitely
with no further GL/Metal errors logged — i.e. MetalANGLE's GLES->Metal
texture-format translation is not correctly handling at least one format
cocos2d-x uses, and on-screen rendering has not been proven correct in the
Simulator. Not yet tested on a physical device. This is the open blocker
for the MetalANGLE approach; next steps would be bisecting which GL texture
format(s) misbehave in MetalANGLE's `rx::mtl::Format` mapping, trying a
newer/different prebuilt ANGLE revision, or reverting to EAGL until a fix
is found.
