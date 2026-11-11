#!/usr/bin/env bash
set -e
export DOTNET_SYSTEM_GLOBALIZATION_INVARIANT=1

MODE=${1:-setup}

if ! command -v dotnet >/dev/null 2>&1; then
    echo "[C4ndy] .NET 8 SDK not found."
    echo "        Install it from https://dotnet.microsoft.com/download/dotnet/8.0"
    exit 1
fi

# Ubuntu/Debian package names
missing=()
command -v g++ >/dev/null 2>&1 || missing+=(g++)
[ -f /usr/include/X11/Xlib.h ] || missing+=(libx11-dev)
if [ ${#missing[@]} -gt 0 ]; then
    echo "[C4ndy] Missing packages: ${missing[*]}"
    echo "        sudo apt install ${missing[*]}"
    exit 1
fi

OS="$(uname -s)"
ARCH="$(uname -m)"
if [ "$OS" = "Darwin" ]; then
    RID=$( [ "$ARCH" = "arm64" ] && echo "osx-arm64" || echo "osx-x64" )
else
    RID=$( [ "$ARCH" = "aarch64" ] && echo "linux-arm64" || echo "linux-x64" )
fi
TOOL="./Build/bin/${RID}/C4ndyBuildTool"

echo "[C4ndy] Building build tool..."
dotnet publish ./Build/C4ndyBuildTool.csproj -c Release -r "$RID" --self-contained \
    -p:PublishSingleFile=true -p:InvariantGlobalization=true -v q --nologo \
    -o "./Build/bin/${RID}"
chmod +x "$TOOL"

"$TOOL" "$MODE"
