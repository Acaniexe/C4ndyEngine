@echo off
setlocal

set TARGET=%1
set CONFIG=%2

if "%TARGET%"=="" set TARGET=All
if "%CONFIG%"=="" set CONFIG=Debug

if /i "%TARGET%"=="clean" (
    echo [C4ndy] Cleaning build outputs...
    if exist Intermediate rmdir /s /q Intermediate
    if exist Binaries     rmdir /s /q Binaries
    if exist vc140.pdb del vc140.pdb
    echo [C4ndy] Clean done.
    exit /b 0
)

if /i "%TARGET%"=="cleanall" (
    echo [C4ndy] Cleaning build outputs and tool binaries...
    if exist Intermediate rmdir /s /q Intermediate
    if exist Binaries     rmdir /s /q Binaries
    if exist Build\bin    rmdir /s /q Build\bin
    if exist Build\obj    rmdir /s /q Build\obj
    if exist vc140.pdb del vc140.pdb
    echo [C4ndy] Full clean done.
    exit /b 0
)

if /i "%TARGET%"=="buildtool" (
    echo [C4ndy] Cleaning tool...
    if exist Intermediate rmdir /s /q Intermediate
    if exist Binaries     rmdir /s /q Binaries
    if exist Build\bin    rmdir /s /q Build\bin
    if exist Build\obj    rmdir /s /q Build\obj

    echo [C4ndy] Clean done. Building tool...
    dotnet publish .\Build\C4ndyBuildTool.csproj -c Release -r win-x64 --self-contained ^
        -p:PublishSingleFile=true -o .\Build\bin\win-x64 > nul
    if exist Build\bin\win-x64\C4ndyBuildTool.exe (
        echo [C4ndy] Build successful.
        exit /b 0
    ) else (
        echo [C4ndy] Build Failed.
        exit /b 1
    )
)

set TOOL=.\Build\bin\win-x64\C4ndyBuildTool.exe

if not exist "%TOOL%" (
    echo [C4ndy] Build tool not found. Building it first...
    dotnet publish .\Build\C4ndyBuildTool.csproj -c Release -r win-x64 --self-contained ^
        -p:PublishSingleFile=true -o .\Build\bin\win-x64 > nul
    if errorlevel 1 (
        echo [C4ndy] Failed to build C4ndyBuildTool.
        exit /b 1
    )
)

"%TOOL%" %TARGET% %CONFIG%