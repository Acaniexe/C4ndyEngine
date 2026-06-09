# C4ndy Engine

C4ndy Engine is an experimental cross-platform game engine and editor written in C++ with a custom build system written in C#.

The project is heavily inspired by Unreal Engine's architecture and development workflow while remaining an independent implementation.

> **Project Status**
>
> C4ndy Engine is currently in early development.
>
> The custom build system is functional and supports Windows, Linux, and macOS. Most engine and editor systems are still under active development.

---

## Goals

* Modern C++ game engine architecture
* Cross-platform development
* Vulkan-based rendering backend
* Custom editor workflow
* Modular engine design
* Fast, dependency-aware builds

---

## Current Status

### Completed

* Custom C# build tool
* Module dependency resolution
* Cross-platform toolchain support
* Windows, Linux, and macOS build targets
* Build target and configuration system

### In Progress

* Core engine systems
* Vulkan renderer
* Editor framework
* Asset pipeline
* ECS implementation
* Physics integration
* Audio systems

---

## Technology

| Component    | Technology            |
| ------------ | --------------------- |
| Engine       | C++                   |
| Build System | C# / .NET 8           |
| Graphics API | Vulkan                |
| Platforms    | Windows, Linux, macOS |

---

## Building

Detailed build instructions can be found in:

```text
BUILDING.md
```

Quick start on Windows:

```bat
build.bat
```

This will build all available targets using the default `Debug` configuration.

---

## Project Structure

```text
C4ndy/
├── Build/                  # Custom build system
├── Engine/
│   ├── Source/
│   │   ├── Core/
│   │   ├── Math/
│   │   ├── RHI/
│   │   ├── Renderer/
│   │   ├── ECS/
│   │   ├── Physics/
│   │   ├── Audio/
│   │   └── Assets/
│   ├── Shaders/
│   └── ThirdParty/
|       └──────Vulkan/
├── Editor/
│   └── Source/
├── Sandbox/
│   └── Source/
├── c4ndy.build.json
├── build.bat
├── build.sh
└── BUILDING.md
```

---

## Build Targets

| Target    | Description                                |
| --------- | ------------------------------------------ |
| `All`     | Build all modules                          |
| `Engine`  | Build engine libraries                     |
| `Editor`  | Build editor and dependencies              |
| `Sandbox` | Build sandbox application and dependencies |

---

## Roadmap

### Phase 1 — Foundation

* [x] Build system
* [x] Module graph
* [x] Cross-platform compilation support
* [ ] Core runtime
* [ ] Logging
* [ ] Memory systems

### Phase 2 — Rendering

* [ ] Vulkan RHI
* [ ] Shader pipeline
* [ ] Resource management
* [ ] Scene rendering

### Phase 3 — Engine Systems

* [ ] ECS
* [ ] Asset management
* [ ] Physics
* [ ] Audio

### Phase 4 — Editor

* [ ] Editor UI
* [ ] Content browser
* [ ] Scene editor
* [ ] Asset import pipeline

---

## License

License information will be added when the project becomes public.
