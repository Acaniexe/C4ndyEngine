using C4ndy.Build;

class Program
{
    static int Main(string[] args)
    {
        Console.ForegroundColor = ConsoleColor.Cyan;
        Console.WriteLine("=== C4ndy Build Tool ===");
        Console.ResetColor();

        string target = "All";
        string config = "Debug";

        foreach (var arg in args)
        {
            if (arg.Equals("Debug", StringComparison.OrdinalIgnoreCase) ||
                arg.Equals("Release", StringComparison.OrdinalIgnoreCase))
                config = arg;
            else
                target = arg;
        }

        Console.WriteLine($"   Target : {target}");
        Console.WriteLine($"   Config : {config}");
        Console.WriteLine();

        string? root = BuildUtils.FindRoot();
        if (root is null)
        {
            BuildUtils.Error("Could not find c4ndy.build.json. Run from inside the C4ndyEngine directory.");
            return 1;
        }

        Console.WriteLine($"   Root     : {root}");
        Console.WriteLine($"   Platform : {Platform.Description()}");
        Console.WriteLine();

        BuildDescriptor? descriptor = BuildDescriptor.Load(Path.Combine(root, "c4ndy.build.json"));
        if (descriptor is null)
        {
            BuildUtils.Error("Failed to parse c4ndy.build.json.");
            return 1;
        }

        var resolver = new ModuleResolver(descriptor, root);
        List<ResolvedModule> modules = resolver.Resolve(target);

        if (modules.Count == 0)
        {
            BuildUtils.Error($"No modules found for target '{target}'.");
            return 1;
        }

        Console.WriteLine($"Resolved {modules.Count} module(s) to build:");
        foreach (var m in modules)
            Console.WriteLine($"   - {m.Name} ({m.Kind})");
        Console.WriteLine();

        var compiler = new Compiler(root, config);
        bool success = compiler.BuildAll(modules);

        Console.WriteLine();
        if (success)
        {
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("Build succeeded.");
        }
        else
        {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("Build FAILED.");
        }
        Console.ResetColor();
        
        return success ? 0 : 1;
    }
}