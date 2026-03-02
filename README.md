# Karnan Engine
_A vulkan engine_

## Features
- Ability to load OBJ files in as models
- Loading png, jpg/jpeg images in as textures
- Basic Material system
- Deferred Rendering with up to 128 lights
  - Point lights
- Support for specularity maps
- Editor built with ImGUI
- Internal messaging system used to communcate between systems on seperate threads

## General information
Despite being a vulkan engine, the coordinate system used by all game objects is Left-handed, where Y points up.

## Editor v Engine split
The engine can run independently of the editor. At program startup the editor is created seperately and attached to the engine. If the editor is not attached to the engine, the engine will simply load up and run the specified scene in the code.

## Adding Meshes to the engine
OBJ files need to be added before engine startup, in the models folder in 'assets'. At engine startup, the models folder is parsed and any OBJs that don't have an associated kmesh binary file, will have one generated. It is the kmesh file which is loaded in runtime whenever models are read into memory. .kmesh file generation and loading into memory happens on a seperate thread to the main thread, so large models don't interupt engine usage.

# Instructions for Building/Opening the project
1. Install the Vulkan SDK (Version 1.4.309.0) to C drive.
2. Open Visual Studio project.
