@echo off
setlocal

set MODE=%1
if "%MODE%"=="" set MODE=setup

where dotnet >nul 2>nul
if errorlevel 1 (
    echo [C4ndy]  .NET 8 SDK not found.
    echo          Install it from https://dotnet.microsoft.com/download/dotnet/8.0
    exit /b 1
)

echo [C4ndy] Building build tool...
dotnet publish .\Build\C4ndyBuildTool.csproj -c Release -r win-x64 --self-contained ^
    -p:PublishSingleFile=true -v q --nologo -o .\Build\bin\win-x64
if errorlevel 1 (
    echo [C4ndy] Failed to build the build tool.
    exit /b 1
)

.\Build\bin\win-x64\C4ndyBuildTool.exe %MODE%