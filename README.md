# Docked

<p align="center">
  <strong>A streamlined project, build, and dependency manager for C and C++</strong>
</p>

<p align="center">
  <a href="README.pt-br.md">Versão em Português</a> •
  <a href="README.md">English Version</a>
</p>

---

## Overview

Developing in **C** and **C++** frequently requires orchestrating disparate tools for compiling, building, and managing dependencies (such as GCC, Clang, MSVC, CMake, Make, Ninja, vcpkg, and Conan). While powerful, stitching these tools together creates friction and steep learning curves for both newcomers and seasoned developers.

Inspired by integrated toolchains like **Cargo** (Rust), **Docked** provides a centralized, cohesive developer experience. Through a single `docked.toml` configuration file and a simple CLI, Docked coordinates project scaffolding, dependency resolution, high-speed compilation, and execution.

---

## Features

- **Integrated CLI Workflow**: Manage the entire lifecycle with `docked init`, `docked build`, and `docked run`.
- **Fast Ninja Builds**: Generates tailored [Ninja](https://ninja-build.org/) files with automatic header dependency tracking (`-MMD -MF`), avoiding unnecessary rebuilds.
- **C & C++ First-Class Support**:
  - **C**: Support for C89, C90, C99, C11, C17, and C23.
  - **C++**: Support for C++11, C++14, C++17, C++20, C++23, and C++26.
- **Compiler Agnostic & Auto-Detection**:
  - Supports **GCC**, **Clang**, and **MSVC** (`cl`).
  - Automatically identifies and uses the best compiler available on Linux, macOS, and Windows.
  - Gracefully falls back to the closest supported standard on constrained compilers (e.g., MSVC).
- **Git-Based Dependency Resolution**:
  - Declarative dependency specification via Git URL and commit/tag/branch.
  - Automatic cloning and checkout directly into `.docked/dependencies/`.
- **Convention over Configuration**: Sensible directory structure (`src/` for source code, `build/` for build artifacts).

---

## Prerequisites

To build and run Docked, ensure you have the following installed on your system:

- **C++17 Compiler** (e.g., GCC >= 9, Clang >= 10, or MSVC 2019+)
- **CMake** (version 3.15 or newer)
- **Boost** (specifically `boost_program_options`)
- **Ninja** (`ninja` or `ninja-build`)
- **Git** (for resolving project dependencies)

On Ubuntu/Debian:
```bash
sudo apt update
sudo apt install build-essential cmake ninja-build libboost-program-options-dev git
```

On Fedora/RHEL:
```bash
sudo dnf install gcc-c++ cmake ninja-build boost-devel git
```

On macOS (using Homebrew):
```bash
brew install cmake ninja boost git
```

---

## Building and Installing

Clone the repository and build using CMake:

```bash
# Clone the repository
git clone https://github.com/pedroafonso/Docked.git
cd Docked

# Configure and compile
cmake -B build -S .
cmake --build build

# Run the test suite
ctest --test-dir build --output-on-failure
```

The resulting executable will be located at `build/docked`. You can copy or symlink it into your system's `PATH` (e.g., `/usr/local/bin/`).

---

## Quick Start

### 1. Initialize a Project

Create a new directory and initialize a Docked project:

```bash
# Initialize in current directory
docked init

# Or create a named project in a subfolder
docked init my-app --name "my-app"
cd my-app
```

This generates a project structure:

```
my-app/
├── docked.toml
└── src/
    └── main.cpp (or main.c)
```

### 2. Configure `docked.toml`

Edit `docked.toml` to customize your project:

#### C++ Project Example
```toml
[project]
name = "MyCppApp"
version = 1.0
language = "C++"       # "C++" or "C"
compiler = "default"   # "default", "G++", "CLANG", or "MSVC"
standard = 20          # 11, 14, 17, 20, 23, or 26

[dependencies]
# Example Git dependency
# name = "googletest"
# git_url = "https://github.com/google/googletest.git"
# git_tag = "v1.14.0"
```

#### C Project Example
```toml
[project]
name = "MyCApp"
version = 1.0
language = "C"         # Compile with C toolchain
compiler = "default"   # "default", "GCC", "CLANG", or "MSVC"
standard = 17          # 89, 90, 99, 11, 17, or 23

[dependencies]
```

### 3. Build the Project

```bash
docked build
```

Docked will:
1. Resolve and clone dependencies listed in `docked.toml`.
2. Inspect source files inside `src/`.
3. Generate `build/build.ninja` with the appropriate compiler, flags, and targets.
4. Run `ninja -C build` to produce the final executable under `build/`.

### 4. Run the Executable

```bash
docked run
```

This immediately runs the compiled binary from the `build/` directory.

---

## CLI Reference

| Command | Arguments | Description |
|---|---|---|
| `docked init [path]` | `[path]` (default: `.`)<br>`--name <name>` | Scaffolds a new project with `docked.toml` and `src/main.cpp`. |
| `docked build` | *None* | Resolves dependencies, generates `build.ninja`, and compiles the application. |
| `docked run` | *None* | Executes the compiled application binary located in `build/`. |
| `docked --help` / `-h` | *None* | Displays command-line usage information. |

---

## Configuration Reference (`docked.toml`)

### `[project]` Section

| Field | Type | Default | Description |
|---|---|---|---|
| `name` | string | `"project"` | Name of the project and resulting binary. |
| `version` | float | `1.0` | Semantic project version. |
| `language` | string | `"C++"` | Programming language: `"C"` or `"C++"`. |
| `compiler` | string | `"default"` | Selected compiler: `"default"`, `"GCC"` / `"G++"`, `"CLANG"`, or `"MSVC"`. |
| `standard` | integer | `26` (C++) / `23` (C) | Language standard version (see table below). |

#### Supported Standards

| Language | Supported `standard` Values | GCC / Clang Flag | MSVC (`cl`) Flag |
|---|---|---|---|
| **C** | `89`, `90` | `-std=c89`, `-std=c90` | `/std:c11` *(closest available)* |
| **C** | `99` | `-std=c99` | `/std:c11` *(closest available)* |
| **C** | `11` | `-std=c11` | `/std:c11` |
| **C** | `17` | `-std=c17` | `/std:c17` |
| **C** | `23` | `-std=c23` | `/std:clatest` |
| **C++** | `11` | `-std=c++11` | `/std:c++14` *(closest available)* |
| **C++** | `14` | `-std=c++14` | `/std:c++14` |
| **C++** | `17` | `-std=c++17` | `/std:c++17` |
| **C++** | `20` | `-std=c++20` | `/std:c++20` |
| **C++** | `23` | `-std=c++23` | `/std:c++latest` |
| **C++** | `26` (default) | `-std=c++26` | `/std:c++latest` |

---

### `[dependencies]` Section

Declare external libraries managed via Git:

```toml
[dependencies]
name = "cjson"
git_url = "https://github.com/DaveGamble/cJSON.git"
git_tag = "v1.7.18"
```

Multiple dependencies can also be declared as nested tables:

```toml
[dependencies.cjson]
name = "cjson"
git_url = "https://github.com/DaveGamble/cJSON.git"
git_tag = "v1.7.18"

[dependencies.raylib]
name = "raylib"
git_url = "https://github.com/raysan5/raylib.git"
git_tag = "5.0"
```

Dependencies are cloned into `.docked/dependencies/<name>` and checked out to the specified `git_tag`.

---

## How It Works Under the Hood

```
   ┌────────────────┐
   │  docked.toml   │
   └───────┬────────┘
           │ Parse configuration
           ▼
   ┌────────────────┐
   │  Dependencies  │ ──► Git clone & checkout to .docked/dependencies/
   └───────┬────────┘
           │ Discover sources in src/
           ▼
   ┌────────────────┐
   │ Ninja Generator│ ──► Generates build/build.ninja (rules, deps, flags)
   └───────┬────────┘
           │ Invoke ninja -C build
           ▼
   ┌────────────────┐
   │ Executable Out │ ──► Placed in build/<project_name>
   └────────────────┘
```

1. **Config Parsing**: Reads `docked.toml` using `toml++`.
2. **Dependency Management**: Clones remote Git repositories into `.docked/dependencies/` with tag checkout support.
3. **Compiler & Standard Resolution**: Resolves appropriate flags based on compiler and target language (C or C++).
4. **Ninja Generation**: Recursively gathers all matching sources (`.c` or `.cpp`) from `src/`, defines dependency rules (`-MMD -MF`), and writes `build/build.ninja`.
5. **Execution**: Invokes Ninja to produce optimized native binaries.

---

## Academic Context

Docked was originally conceived and developed by **Pedro Afonso Silva da Costa** under the academic advisement of **Prof. Daniel Ricardo de Souza** at the **Universidade Luterana do Brasil (Ulbra)** – Campus Torres/RS, within the Systems Analysis and Development program (Análise e Desenvolvimento de Sistemas).

---

## License

This project is open-source. See the repository for license details.
