using System.Diagnostics;
namespace C4ndy.Build;

public static class ShaderCompiler
{
    public static bool CompileAll(string root)
    {
        string? glslc = FindGlslc(root);
        if (glslc is null)
        {
            BuildUtils.Warn("glslc not found - skipping shader compilation.");
            return true;
        }

        string shaderDir = Path.Combine(root, "Engine", "Shaders");
        if (!Directory.Exists(shaderDir))
        {
            BuildUtils.Warn("Shader directory not found - skipping.");
            return true;
        }

        var shaderFiles = Directory.GetFiles(shaderDir, "*.*")
            .Where(f => f.EndsWith(".vert") || f.EndsWith(".frag"))
            .ToList();

        if (shaderFiles.Count == 0)
        {
            BuildUtils.Warn("No shader files found.");
            return true;
        }

        BuildUtils.Info($"Compiling {shaderFiles.Count} shader(s)...");

        bool allSucceeded = true;
        foreach (var shader in shaderFiles)
        {
            string output = shader + ".spv";

            if (File.Exists(output) && File.GetLastWriteTimeUtc(output) > File.GetLastWriteTimeUtc(shader))
            {
                BuildUtils.Info($"  [SKIP] {Path.GetFileName(shader)} (up to date)");
                continue;
            }

            bool success = CompileShader(glslc, shader, output);
            if (success)
                BuildUtils.Info($" [OK]  {Path.GetFileName(shader)}");
            else
            {
                BuildUtils.Error($"  [FAIL]  {Path.GetFileName(shader)}");
                allSucceeded = false;
            }
        }
        return allSucceeded;
    }

    private static string? FindGlslc(string root)
    {
        string platform = Platform.OS switch
        {
            HostPlatform.Windows => "Win",
            HostPlatform.Linux => "Linux",
            HostPlatform.macOS => "macOS",
            _ => "Win"
        };

        string exe = Platform.OS == HostPlatform.Windows ? "glslc.exe" : "glslc";

        string path = Path.Combine(root, "Engine", "ThirdParty", "Vulkan", "Bin", platform, exe);

        if (File.Exists(path))
            return path;

        BuildUtils.Warn($"glslc not found at: {path}");
        return null;
    }

    private static bool CompileShader(string glslc, string input, string output)
    {
        var psi = new ProcessStartInfo
        {
            FileName = glslc,
            Arguments = $"\"{input}\" -o \"{output}\"",
            RedirectStandardOutput = true,
            RedirectStandardError = true,
            UseShellExecute = false,
        };

        using var proc = Process.Start(psi);
        if (proc is null) return false;

        string stderr = proc.StandardError.ReadToEnd();
        proc.WaitForExit();

        if (!string.IsNullOrWhiteSpace(stderr))
            BuildUtils.Error(stderr);

        return proc.ExitCode == 0;
    }
}