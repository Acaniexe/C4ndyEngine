# C4ndy Build Guide

This repository uses a custom build tool located in `Build/` and a Windows wrapper script at `build.bat`.

## Prerequisites

Before building, ensure the following are installed:

* Windows, Linux, or macOS
* .NET 8 SDK
* A supported C++ toolchain:

  * Windows: Visual Studio C++ toolset
  * Linux: `g++`
  * macOS: `clang++`

Run all build commands from the repository root (the directory containing `c4ndy.build.json`).

---

## Build Wrapper (Windows)

Use the build wrapper script:

```bat
build.bat [target] [config]
```

### Defaults

If no arguments are supplied:

* `target = All`
* `config = Debug`

### Supported Targets

| Target    | Description                                       |
| --------- | ------------------------------------------------- |
| `All`     | Build every module                                |
| `Engine`  | Build engine libraries only                       |
| `Editor`  | Build the editor executable and its dependencies  |
| `Sandbox` | Build the sandbox executable and its dependencies |

### Supported Configurations

* `Debug`
* `Release`

### Examples

```bat
build.bat
build.bat All Release
build.bat Engine Debug
build.bat Editor Release
build.bat Sandbox Debug
```

---

## Cleaning Builds

### Clean Intermediate Outputs

```bat
build.bat clean
```

Removes:

* `Intermediate`
* `Binaries`

### Full Clean

```bat
build.bat cleanall
```

Removes:

* `Intermediate`
* `Binaries`
* `bin`
* `obj`

---

## Direct Tool Invocation

The build tool can be invoked directly once it has been built or published.

### Windows

```bat
.\Build\bin\win-x64\C4ndyBuildTool.exe Sandbox Release
```

### Linux

```bash
./Build/bin/linux-x64/C4ndyBuildTool Sandbox Release
```

### macOS

```bash
./Build/bin/osx-arm64/C4ndyBuildTool Sandbox Release
```

Replace the RID folder with the platform and architecture you published for.

---

## Supported Runtime Identifiers (RIDs)

| Platform | x64         | ARM64         |
| -------- | ----------- | ------------- |
| Windows  | `win-x64`   | `win-arm64`   |
| Linux    | `linux-x64` | `linux-arm64` |
| macOS    | `osx-x64`   | `osx-arm64`   |

---

## Publishing the Build Tool

If the build tool has not yet been published for your platform:

### Linux x64

```bash
dotnet publish ./Build/C4ndyBuildTool.csproj -c Release -r linux-x64 --self-contained -p:PublishSingleFile=true -o ./Build/bin/linux-x64
chmod +x ./Build/bin/linux-x64/C4ndyBuildTool
```

### Linux ARM64

```bash
dotnet publish ./Build/C4ndyBuildTool.csproj -c Release -r linux-arm64 --self-contained -p:PublishSingleFile=true -o ./Build/bin/linux-arm64
chmod +x ./Build/bin/linux-arm64/C4ndyBuildTool
```

### macOS x64

```bash
dotnet publish ./Build/C4ndyBuildTool.csproj -c Release -r osx-x64 --self-contained -p:PublishSingleFile=true -o ./Build/bin/osx-x64
chmod +x ./Build/bin/osx-x64/C4ndyBuildTool
```

### macOS ARM64

```bash
dotnet publish ./Build/C4ndyBuildTool.csproj -c Release -r osx-arm64 --self-contained -p:PublishSingleFile=true -o ./Build/bin/osx-arm64
chmod +x ./Build/bin/osx-arm64/C4ndyBuildTool
```

---

## Expected Output

Build outputs should be generated in:

```text
Intermediate\<Config>\...
Binaries\<Config>\...
```

For example:

```text
Intermediate\Debug\...
Binaries\Debug\...
```

If build outputs appear in the repository root, the build tool may be misconfigured or stale build artifacts may still exist.

---

## Build Tool Details

`build.bat` will:

* Default the target to `All`
* Default the configuration to `Debug`
* Automatically build `C4ndyBuildTool.exe` if it is missing

The build graph, module definitions, and dependency information are defined in:

```text
c4ndy.build.json
```

---

## Notes

* Build commands should always be executed from the repository root.
* `c4ndy.build.json` defines module targets and dependencies.
* Linux builds require `g++` to be available on `PATH`.
* macOS builds require `clang++` to be available on `PATH`.
* A `build.sh` wrapper script has not yet been implemented.
* If build results do not appear under `Intermediate` or `Binaries`, perform a clean build and verify the build tool configuration.