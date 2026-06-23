## CodeGraph — Always Use First for Code Navigation

This project has **CodeGraph** — a tree-sitter AST index of all 50 source files
(1,888 symbols, 6,403 cross-reference edges). **Always use CodeGraph MCP tools
before grep/glob/Read** for browsing and searching code.

Available MCP tools (invoke directly by name):
- `codegraph_codegraph_status` — Index health
- `codegraph_codegraph_files` — File tree listing
- `codegraph_codegraph_search` — Find symbol by name
- `codegraph_node` — Symbol detail + source
- `codegraph_callers` / `codegraph_callees` — Who calls / is called
- `codegraph_codegraph_impact` — Blast radius of a change
- `codegraph_codegraph_explore` — Deep multi-file survey with full source

See `CodeGraph.md` for detailed usage.

---

## Project Overview

Ikemen GO (M.U.G.E.N engine) with a custom JIT-compiled scripting language called **SSZ**. The engine is a **static plugin architecture**: 14 subsystems register exported functions with the SSZ runtime.

| Component | Key Files | Lines |
|---|---|---|
| Entry point | `main/main.cpp` | 165 |
| SSZ JIT compiler | `main/ssz/jitcompiler.hpp` | 8,886 |
| SSZ source tree | `main/ssz/sourcetree.hpp` | 8,583 |
| SDL renderer plugin | `main/sdlplugin/sdlplugin.cpp` | 5,826 |
| x86 codegen backend | `main/ssz/x86.hpp` | 3,680 |
| Plugin registry | `main/ssz/static_plugin_registry.hpp` | 170 |
| Platform abstraction | `main/ssz/sszdef.h` | 176 |
| Type ID definitions | `main/ssz/typeid.h` | 35 |
| 13 plugin sources | `main/*/` | 50–200 each |
| 13 static headers | `main/*_static.hpp` | 30–240 each |

**External dependencies:** Lua 5.2.4, SDL2, SDL2_image, SDL2_ttf, SDL2_mixer, FLAC, libogg, libvorbis, Freetype, libpng, zlib, GLEW, VLC, PortAudio, OpenGL.

---

## Build Instructions

### Windows — w64devkit (MinGW/GCC, recommended)

Toolchain: [w64devkit x86](https://github.com/skeeto/w64devkit/releases/download/v2.8.0/w64devkit-x86-2.8.0.7z.exe)
Toolset: GCC (MinGW), x86_64.
Uses the `x86.hpp` raw-byte emitter for code generation.

```bash
# Prerequisites: Install w64devkit
# Extract to C:\x86devkit

# Open a command prompt or PowerShell, then:
set PATH=C:\x86devkit\bin;%PATH%
cd /d "C:\Projects\NEW-IK~1"

make CONFIG=Release

# Clean rebuild
make clean
```

- All 19 external libraries compiled from source (~800 source files) into 19 static archives
- Output: `build/Release/ikemen.exe`
- **Note:** The Makefile sets `PATH` internally for `as` and `ld` — just having `g++.exe` in PATH is sufficient

### Linux (Makefile, experimental)

```bash
make CONFIG=Release
```

- Uses system `g++` with `-std=c++17`
- Architecture detection via `uname -m` (supports `-m32` for x86)
- Also builds Lua 5.2.4 as a static library

### Short path for compilation

The project path contains spaces and parentheses. Use the 8.3 short name when invoking compilers from outside MSYS2:

```
C:\Projects\NEW-IK~1
```
