# Xenon
Xenon is a heavily modified version of the FitzQuake engine.

## Features
- Abstract SDL2 implementation for both video and input.
- [Platform library](https://github.com/xenon-tech/PlatformLibrary) to deal with platform-specific tasks as well as operating as an abstraction layer for a number of file formats.
- Heavily modified renderer with support for multiple viewports and cameras, including state tracking via Platform lib.
- GLSL shaders.
- Embedded mode.
- WYSIWYG editor environment.
- VBO support.
- FBO support.
- Blob and point-based planar shadows.
- Support for both MD2 and Unreal's 3D model formats.
- Support for TGA, FTX, DTX, VTF and TIFF image formats.
- Scripted material system with support for sphere-mapping and texture detail, variable animation rates, blend layers, multiple skins and contextual parameters.
- Tweakable file paths via script.
- Launcher, Engine, Game and Menu sub-systems
- Complete rewrite of game-logic in C/C++ (versus original QuakeC implementation)
- Additional bug fixes to existing code...

## Planned Features
- Mirrors
- Support for post-processing
- Entirely new level format and visibility system

*Copyright (C) 2011-2017 OldTimes Software*
