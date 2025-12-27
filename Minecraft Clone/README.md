**A modern, cross-platform voxel game engine written in C++20, built from the ground up with a focus on performance, modularity, and data-driven design.**

This project is a deep dive into the architecture of modern game engines, implementing many of the powerful, data-oriented patterns used in professional game development. It is heavily inspired by the robust, moddable architecture of Minecraft and the performance-centric designs of mods like Sodium.

![alt text](image-1.png)

---

## ✨ Core Architectural Features

This engine is built on a set of clean, decoupled, and scalable systems.

*   **Multi-threaded Client-Server Model:** A fully functional client-server architecture where even single-player mode runs an integrated server on a separate thread, ensuring identical logic for both single-player and multiplayer.
*   **Data-Driven Design:** All game content—blocks, items, models, shaders, and UI—is defined in external JSON files. The engine is designed to be highly moddable, loading all content from resource packs at runtime.
*   **Entity Component System (ECS):** Game logic is built on a high-performance ECS. Entities are simple IDs, Components are pure data, and Systems contain all the logic, ensuring excellent cache performance and flexible game mechanics.
*   **Advanced Rendering Pipeline:**
    *   **Multi-backend Abstraction:** A clean rendering abstraction layer separates generic render resources from the platform-specific backend (currently OpenGL 4.6).
    *   **GPU-Driven Rendering:** Employs modern techniques like **Bindless Textures** and **Multi-Draw Indirect (MDI)** for terrain rendering, drastically reducing CPU overhead.
    *   **Dynamic Texture Atlasing:** Uses on-demand, render-to-texture atlasing for item icons, ensuring high quality and efficient memory usage.
*   **Hierarchical UI System:**
    *   A powerful UI framework based on a **Widget/Component** architecture.
    *   Features a responsive **Anchor/Pivot/Offset** layout system inspired by Unity's RectTransform.
    *   Uses a stack-based `ScreenManager` for clean UI state transitions.

---

## 🚀 Key Technical Systems

A brief look at some of the core systems we've designed:

| System | Description | Key Classes |
| :--- | :--- | :--- |
| **Asset Pipeline** | A multi-stage, data-driven pipeline for discovering, loading, and baking game assets from resource packs. | `AssetManager`, `ModelManager`, `TextureAtlas`, `RenderableModel` |
| **Render Engine** | A modern renderer with a clear separation between GPU resource definitions and backend implementation. | `RenderResourceManager`, `Texture`, `Shader`, `RenderTarget`, `Blitter` |
| **World & Chunks** | Manages the voxel world data, including block states, lighting, and chunk lifecycle. | `Dimension`, `ChunkMap`, `Palette`, `ThreadedLightEngine` |
| **UI Framework** | A complete, retained-mode UI system for creating complex, responsive user interfaces. | `UIManager`, `Screen`, `Widget`, `Component`, `DynamicAtlasManager` |
| **AI System** | A flexible, goal-oriented AI system based on the Strategy pattern. | `AIComponent`, `AIGoal`, `AISystem` |

---

## 🛠️ Building & Running


**Dependencies:**
*   [CMake](https://cmake.org/) (version 3.20+)
*   [GLFW](https://www.glfw.org/)
*   [GLEW](http://glew.sourceforge.net/)
*   [GLM](https://glm.g-truc.net/0.9.9/index.html)
*   [nlohmann json](https://github.com/nlohmann/json)
*   [stb](https://github.com/nothings/stb)

**Build Instructions:**
```bash
# Clone the repository
git clone https://github.com/hiheyok/Voxel-Game.git
cd Voxel-Game

# Configure and build with CMake
cmake -S . -B build
cmake --build ./build/ --config Release --target all -j 20

# Run the executable
./build/VoxelGame
```

---

## 📜 Goals & Future Work

This project is a continuous exploration of game engine development. Future goals include:

*   [ ] Implementing a Vulkan rendering backend.
*   [ ] Implementing a Metal rendering backend.
*   [ ] Building a complete, multi-threaded chunk meshing and rendering pipeline.
*   [ ] Expanding the AI system with more complex goals.
*   [ ] Implementing a scripting language for game logic.

---

*This project is a personal learning endeavor and is not affiliated with Mojang or Microsoft.*