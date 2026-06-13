#!/usr/bin/env bash
set -e

TARGET=${1:-All}
CONFIG=${2:-Debug}

if [ "$TARGET" = "clean" ]; then
    echo "[C4ndy] Cleaning build outputs..."
    rm -rf Intermediate Binaries
    echo "[C4ndy] Clean done."
    exit 0
fi

if [ "$TARGET" = "cleanall" ]; then
    echo "[C4ndy] Cleaning build outputs and tool binaries..."
    rm -rf Intermediate Binaries Build/bin
    echo "[C4ndy] Full clean done."
    exit 0
fi

export DOTNET_SYSTEM_GLOBALIZATION_INVARIANT=1

OS="$(uname -s)"
ARCH="$(uname -m)"

if [ "$OS" = "Darwin" ]; then
    RID=$( [ "$ARCH" = "arm64" ] && echo "osx-arm64" || echo "osx-x64" )
else
    RID=$( [ "$ARCH" = "aarch64" ] && echo "linux-arm64" || echo "linux-x64" )
fi

TOOL="./Build/bin/${RID}/C4ndyBuildTool"

if [ ! -f "$TOOL" ]; then
    echo "[C4ndy] Build tool not found. Building it first..."
    DOTNET_SYSTEM_GLOBALIZATION_INVARIANT=1 dotnet publish ./Build/C4ndyBuildTool.csproj \
        -c Release -r "$RID" --self-contained \
        -p:PublishSingleFile=true \
        -p:InvariantGlobalization=true \
        -o "./Build/bin/${RID}" > /dev/null
fi

"$TOOL" "$TARGET" "$CONFIG"