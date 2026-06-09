using System.Diagnostics;
namespace C4ndy.Build;

public static class Locator
{
    private static Dictionary<string, string>? _cachedEnv;
    public static Dictionary<string, string>? GetMsvcEnvironment()
    {
        if (Platform.OS != HostPlatform.Windows)
            return [];
        if (_cachedEnv is not null) return _cachedEnv;

        string? vsPath = FindVisualStudio();
        if (vsPath is null)
        {
            BuildUtils.Error("Couldn't locate Visual Studio. " + "Please install VS 2022 with the c++ workload.");
            return null;
        }

        string vcvars = Path.Combine(vsPath, "VC", "Auxiliary", "Build", "vcvarsall.bat");
        if (!File.Exists(vcvars))
        {
            BuildUtils.Error($"vcvarsall.bat not found at: {vcvars}");
            return null;
        }

        BuildUtils.Info($"Found MSVC at: { vsPath}");
        _cachedEnv = SourceBatchFile(vcvars, "x64");
        return _cachedEnv;
    }

    private static string? FindVisualStudio()
    {
        string vswhere = Path.Combine(
            Environment.GetFolderPath(Environment.SpecialFolder.ProgramFilesX86),
            "Microsoft Visual Studio", "Installer", "vswhere.exe");

        if (!File.Exists(vswhere))
            return FindVisualStudioFallback();

        var psi = new ProcessStartInfo
        {
            FileName = vswhere,
            Arguments = "-latest -products * -requires Microsoft.VisualCpp.Tools.HostX64.TargetX64 -property installationPath",
            RedirectStandardOutput = true,
            UseShellExecute = false,
        };

        using var proc = Process.Start(psi);
        string? path = proc?.StandardOutput.ReadLine()?.Trim();
        proc?.WaitForExit();
        return string.IsNullOrEmpty(path) ? null : path;
    }

    private static string? FindVisualStudioFallback()
    {
        string pf = Environment.GetFolderPath(Environment.SpecialFolder.ProgramFiles);
        string pf86 = Environment.GetFolderPath(Environment.SpecialFolder.ProgramFilesX86);

        string[] roots = [pf, pf86];
        string[] editions = ["Enterprise", "Professional", "Community", "BuildTools"];
        string[] years = ["2022", "2019"];

        foreach (var root in roots)
        foreach (var year in years)
        foreach (var ed in editions)
        {
            string path = Path.Combine(root, "Microsoft Visual Studio", year, ed);
            if (Directory.Exists(path)) return path;
        }
        return null;
    }

    private static Dictionary<string, string> SourceBatchFile(string bat, string arch)
    {
        var env = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);

        var psi = new ProcessStartInfo
        {
            FileName = "cmd.exe",
            Arguments = $"/c \"\"{bat}\" {arch} > nul 2>&1 && set\"",
            RedirectStandardOutput = true,
            UseShellExecute = false,
        };

        using var proc = Process.Start(psi)!;
        while (!proc.StandardOutput.EndOfStream)
        {
            string? line = proc.StandardOutput.ReadLine();
            if (line is null) continue;
            int eq = line.IndexOf('=');
            if (eq < 1) continue;
            env[line[..eq]] = line[(eq + 1)..];
        }
        proc.WaitForExit();
        return env;
    }

    public static string ResolveTool(string exeName, Dictionary<string, string> env)
    {
        if (Platform.OS != HostPlatform.Windows)
            return exeName;
        if (env.TryGetValue("PATH", out var pathVar))
        {
            foreach (var dir in pathVar.Split(';', StringSplitOptions.RemoveEmptyEntries))
            {
                string full = Path.Combine(dir.Trim(), exeName);
                if (File.Exists(full)) return full;
            }
        }
        BuildUtils.Warn($"Could not resolve full path for {exeName}, using bare name.");
        return exeName;
    }
}