# 3D Renderer from the Ground Up

A simple 3D Renderer implemented from scratch — showcasing how computer graphics engines work at the lowest level. This project is a CPU-based graphics engine written entirely in C++, primarily utilizing WIN32 APIs for windowing and system integration. The renderer demonstrates fundamental principles of 3D graphics, from mathematical groundwork to rendering pipelines, and is built with attention to cross-platform support (main codebase).

## Features

- **Complete implementation in C++**  
  All rendering logic, engine architecture, and math utilities are coded natively.

- **WIN32 API Integration**  
  Uses WIN32 APIs for window creation and event handling (native support for Windows platforms).

- **SDL Support for Cross-Platform**  
  Integrated SDL backend provides cross-platform windowing and input outside of Windows environments.

- **CPU-Based Rendering**  
  No hardware acceleration — pure software rendering for learning and demonstration purposes.

- ~~Physics Engine Implementation
  Includes a basic physics engine for object movement, collision handling, and game simulation.~~

- ~~**Integrated Arweave Storage**  
  The graphics engine's resource manager leverages [Arweave](https://www.arweave.org/) for decentralized and permanent storage of game assets, enabling secure and durable asset management.~~

- **Understandable, from-the-ground-up design**  
  Meant for learning: easy to read, modify, and expand.

## Getting Started

### Prerequisites

- C++17 or newer compiler
- Windows OS (for full WIN32 API demonstration)
- SDL2 development libraries (for cross-platform GUI)
- For cross-platform: adapt main code to your desired OS and platform windowing

### Building (Windows Example)

```sh
git clone https://github.com/AnasElii/3d_renderer.git
cd 3d_renderer
mkdir build
cd build
cmake ..
cmake --build .
```

Alternatively, open the project in Visual Studio and build.

### Run

- On Windows, launch the executable from `build/` after compilation (native WIN32 and optional SDL).
- For Linux/macOS, build with SDL enabled and launch normally.

## Project Structure

```
src/
  main.cpp          // Entry point, platform abstraction
  renderer.cpp      // Core rendering logic
  renderer.h
  physics/          // Physics engine components
  resource/         // Resource manager with Arweave support
  math/             // Matrix, vector, geometry helpers
  platform/         // WIN32-specific routines
  sdl/              // SDL cross-platform routines
  ...
```

## How it Works

- Sets up a window using WIN32 APIs (native) or SDL (cross-platform).
- Draws a 3D scene using custom CPU-based algorithms (transformations, projections, rasterization).
- Integrated physics engine enables simulation and interaction within the scene.
- Resource manager allows game assets to be stored and retrieved via Arweave for decentralized, permanent storage.
- Handles input and event loops manually for learning purposes.
- Main rendering pipeline decoupled for easy adaptation to non-Windows platforms.

## Why Use This Project?

- Learn fundamental computer graphics and physics by reading and running code.
- See how a 3D renderer can work *without* GPU hardware acceleration.
- Experiment with decentralized asset storage using Arweave.
- Use SDL for easy cross-platform GUI and input handling.
- Basis for experimenting with new features, algorithms, or extending to real-time engines.

## License

MIT License — see [LICENSE](LICENSE) for details.

## Author

[AnasElii](https://github.com/AnasElii)

---

> **Note:** For native Windows support, use the WIN32 API backend. For cross-platform builds, enable the SDL backend and make sure SDL libraries are installed. Arweave integration is used within the resource manager for asset permanence.
