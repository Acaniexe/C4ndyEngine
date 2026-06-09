namespace C4ndy.Build;

public class ResolvedModule
{
    public string Name { get; set; } = "";
    public string Kind { get; set; } = "StaticLib";
    public List<string> SourceFiles { get; set; } = new();
    public List<string> IncludeDirs { get; set; } = new();
    public List<string> Dependencies { get; set; } = new();
    public List<string> Defines { get; set; } = new();
}

public class ModuleResolver
{
    private readonly BuildDescriptor _desc;
    private readonly string _root;

    public ModuleResolver(BuildDescriptor desc, string root)
    {
        _desc = desc;
        _root = root;
    }

    public List<ResolvedModule> Resolve(string target)
    {
        var candidates = target.Equals("All", StringComparison.OrdinalIgnoreCase)
            ? _desc.Modules
            : _desc.Modules.Where(m => m.Target.Equals(target, StringComparison.OrdinalIgnoreCase)).ToList();

        var map = _desc.Modules.ToDictionary(m => m.Name, m => m);

        var result = new List<ModuleDescriptor>();
        var visited = new HashSet<string>();

        void Visit(ModuleDescriptor m)
        {
            if (visited.Contains(m.Name)) return;
            visited.Add(m.Name);
            foreach (var dep in m.Dependencies)
                if (map.TryGetValue(dep, out var depModule))
                    Visit(depModule);
            result.Add(m);
        }
        foreach (var m in candidates) Visit(m);
        return result.Select(m => Resolve(m)).ToList();
    }

    private ResolvedModule Resolve(ModuleDescriptor m)
    {
        var sources = new List<string>();
        foreach (var dir in m.SourceDirs)
        {
            string abs = Path.GetFullPath(Path.Combine(_root, dir));
            if (Directory.Exists(abs))
                sources.AddRange(Directory.GetFiles(abs, "*.cpp", SearchOption.AllDirectories));
        }

        var includes = m.IncludeDirs
            .Select(d => Path.GetFullPath(Path.Combine(_root, d)))
            .ToList();

        return new ResolvedModule
        {
            Name = m.Name,
            Kind = m.Kind,
            SourceFiles = sources,
            IncludeDirs = includes,
            Dependencies = m.Dependencies,
            Defines = m.Defines,
        };
    }
}