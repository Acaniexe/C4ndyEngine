using System.Diagnostics;
namespace C4ndy.Build;

public class Compiler
{
    private readonly string _root;
    private readonly string _config;
    private readonly string _intDir;
    private readonly string _outDir;
    private Dictionary<string, string>? _msvcEnv;

    public Compiler(string root, string config)
    {
        _root = root;
        _config = config;
        _intDir = Path.Combine(root, "Intermediate", config);
        _outDir = Path.Combine(root, "Binaries", config);
        Directory.CreateDirectory(_intDir);
        Directory.CreateDirectory(_outDir);
    }

    public bool BuildAll(List<ResolvedModule> modules)
    {
        var libOutputs = new Dictionary<string, string>();

        foreach (var m in modules)
        {
            Console.ForegroundColor = ConsoleColor.Yellow;
            Console.WriteLine($"[{m.Name}] Compiling...");
            Console.ResetColor();

            var objFiles = new List<string>();

            foreach (var src in m.SourceFiles)
            {
                string objName = Path.GetFileNameWithoutExtension(src) + "_" + Math.Abs(src.GetHashCode()) + Platform.ObjectExt;
                string objPath = Path.Combine(_intDir, m.Name, objName);
                Directory.CreateDirectory(Path.GetDirectoryName(objPath)!);

                bool ok = RunProcess(Platform.CC, BuildCompileArgs(m, src, objPath));
                if (!ok) return false;
                objFiles.Add(objPath);
            }

            if (objFiles.Count == 0)
            {
                BuildUtils.Warn($"[{m.Name}] No source files found - skipping.");
                continue;
            }

            bool linked = LinkModule(m, objFiles, libOutputs);
            if (!linked) return false;
        }
        return true;
    }

    private bool RunProcess(string exe, string args)
    {
        _msvcEnv ??= Locator.GetMsvcEnvironment();
        if (_msvcEnv is null)
        {
            BuildUtils.Error("MSVC environment could not be initialised.");
            return false;
        }

        string fullExe = Locator.ResolveTool(exe, _msvcEnv);

        var psi = new ProcessStartInfo
        {
            FileName = fullExe,
            Arguments = args,
            RedirectStandardOutput = true,
            RedirectStandardError = true,
            UseShellExecute = false,
        };

        foreach (var (k, v) in _msvcEnv)
            psi.Environment[k] = v;

        using var proc = Process.Start(psi);
        if (proc is null)
        {
            BuildUtils.Error($"Failed to start {exe}.");
            return false;
        }

        string stdout = proc.StandardOutput.ReadToEnd();
        string stderr = proc.StandardError.ReadToEnd();
        proc.WaitForExit();

        if (!string.IsNullOrWhiteSpace(stdout))Console.Write(stdout);
        if (!string.IsNullOrWhiteSpace(stderr)) Console.Error.Write(stderr);

        return proc.ExitCode == 0;
    }

    private string BuildCompileArgs(ResolvedModule m, string src, string obj)
    {
        var args = new List<string>();

        if (Platform.OS == HostPlatform.Windows)
        {
            args.AddRange(["/nologo", "/std:c++20", "/W4", "/WX-", "/MP", "/EHsc", "/utf-8"]);
            args.AddRange(_config == "Release" ? ["/O2", "/DNDEBUG", "/MD"] : ["/Od", "/Zi", "/D_DEBUG", "/MDd"]);

            foreach (var inc in m.IncludeDirs) args.Add($"/I\"{inc}\"");
            foreach (var def in m.Defines) args.Add($"/D{def}");
            args.Add($"/Fo\"{obj}\"");
            args.Add("/c");
            args.Add($"\"{src}\"");
        }
        else
        {
            args.AddRange(Platform.CompileFlags);
            args.AddRange(_config == "Release" ? Platform.ReleaseFlags : Platform.DebugFlags);

            foreach (var inc in m.IncludeDirs) args.Add($"-I\"{inc}\"");
            foreach (var def in m.Defines) args.Add($"-D{def}");
            args.Add($"-o \"{obj}\"");
            args.Add("-c");
            args.Add($"\"{src}\"");
        }
        return string.Join(" ", args);
    }

    private bool LinkModule(ResolvedModule m, List<string> objs, Dictionary<string, string> libOutputs)
    {
        if (m.Kind == "Executable")
        {
            string outExe = Path.Combine(_outDir, Platform.ExecutableName(m.Name));
            var args = new List<string>();

            if (Platform.OS == HostPlatform.Windows)
            {
                args.AddRange(["/nologo", $"/OUT:\"{outExe}\""]);
                if (_config == "Debug") args.Add("/DEBUG");
                args.AddRange(objs.Select(o => $"\"{o}\""));
                foreach (var dep in m.Dependencies)
                    if (libOutputs.TryGetValue(dep, out var lib)) args.Add($"\"{lib}\"");
                args.AddRange(Platform.SystemLibs);
                return RunProcess(Platform.LD, string.Join(" ", args));
            }
            else
            {
                args.AddRange(Platform.CompileFlags);
                args.AddRange(objs.Select(o => $"\"{o}\""));
                foreach (var dep in m.Dependencies)
                    if (libOutputs.TryGetValue(dep, out var lib)) args.Add($"\"{lib}\"");
                args.AddRange(Platform.SystemLibs);
                args.Add($"-o \"{outExe}\"");
                return RunProcess(Platform.LD, string.Join(" ", args));
            }
        }
        else
        {
            string outLib = Path.Combine(_outDir, Platform.StaticLibName(m.Name));
            libOutputs[m.Name] = outLib;
            string objList = string.Join(" ", objs.Select(o => $"\"{o}\""));

            if (Platform.OS == HostPlatform.Windows)
                return RunProcess(Platform.AR, $"/nologo /OUT:\"{outLib}\" {objList}");
            else
                return RunProcess(Platform.AR, $"rcs \"{outLib}\" {objList}");
        }
    }
}