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
