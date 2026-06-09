using System.Runtime.InteropServices;
namespace C4ndy.Build;

public enum HostPlatform { Windows, macOS, Linux }
public enum HostArch     { x64, Arm64 }

public class Platform
{
    public static readonly HostPlatform OS;
    public static readonly HostArch     Arch;
    public static readonly string       RID;

    static Platform()
    {
        if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows)) OS = HostPlatform.Windows;
        else if (RuntimeInformation.IsOSPlatform(OSPlatform.OSX)) OS = HostPlatform.macOS;
        else if (RuntimeInformation.IsOSPlatform(OSPlatform.Linux)) OS = HostPlatform.Linux;
        else throw new PlatformNotSupportedException("C4ndy BuildTool: unsupported host OS.");

        Arch = RuntimeInformation.OSArchitecture == Architecture.Arm64 ? HostArch.Arm64 : HostArch.x64;

        RID = OS switch
        {
            HostPlatform.Windows => Arch == HostArch.Arm64 ? "win-arm64" : "win-x64",
            HostPlatform.Linux => Arch == HostArch.Arm64 ? "linux-arm64" : "linux-x64",
            HostPlatform.macOS => Arch == HostArch.Arm64 ? "osx-arm64" : "osx-x64",
            _ => "win-x64"
        };
    }

    public static string CC => OS switch
    {
        HostPlatform.Windows => "cl.exe",
        HostPlatform.Linux => "g++",
        HostPlatform.macOS => "clang++",
        _ => "g++"
    };

    public static string AR => OS switch
    {
        HostPlatform.Windows => "lib.exe",
        _ => "ar"
    };

    public static string LD => OS switch
    {
        HostPlatform.Windows => "link.exe",
        _ => CC
    };

    public static string ObjectExt => OS switch
    {
        HostPlatform.Windows => ".obj",
        _ => ".o"
    };

    public static string StaticLibName(string name) => OS switch
    {
        HostPlatform.Windows => $"{name}.lib",
        _ => $"lib{name}.a"
    };

    public static string ExecutableName(string name) => OS switch
    {
        HostPlatform.Windows => $"{name}.exe",
        _ => name
    };

    public static string[] CompileFlags => OS switch
    {
        HostPlatform.Windows => [],
        HostPlatform.Linux => ["-std=c++20", "-Wall", "-Wextra", "-fvisibility=hidden", "-fPIC"],
        HostPlatform.macOS => ["-std=c++20", "-Wall", "-Wextra", "-fvisibility=hidden", "-mmacosx-version-min=12.0"],
        _ => []
    };

    public static string[] SystemLibs => OS switch
    {
        HostPlatform.Windows => ["kernel32.lib", "user32.lib", "gdi32.lib", "shell32.lib", "ole32.lib", "uuid.lib", "xaudio2.lib"],
        HostPlatform.macOS => ["-framework Cocoa", "-framework Metal", "-framework AudioToolbox", "-framework CoreAudio"],
        HostPlatform.Linux => ["-lX11", "-lXrandr", "-lXi", "-ldl", "-lpthread", "-lasound"],
        _ => []
    };

    public static string[] DebugFlags => OS switch
    {
        HostPlatform.Windows => [],
        _ => ["-g", "-O0", "-D-DEBUG"]
    };

    public static string[] ReleaseFlags => OS switch
    {
        HostPlatform.Windows => [],
        _ => ["-O2", "-DNDEBUG"]
    };

    public static string Description() => $"{OS} / {Arch} ({RID})";
}