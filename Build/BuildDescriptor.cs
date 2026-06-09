using System.Text.Json;
using System.Text.Json.Serialization;
namespace C4ndy.Build;

public class BuildDescriptor
{
    [JsonPropertyName("engine_name")]
    public string EngineName { get; set; } = "C4ndyEngine";

    [JsonPropertyName("version")]
    public string Version { get; set; } = "0.1.0";

    [JsonPropertyName("modules")]
    public List<ModuleDescriptor> Modules { get; set; } = new();

    public static BuildDescriptor? Load(string path)
    {
        if (!File.Exists(path))
        {
            BuildUtils.Error($"{path} Not found.");
            return null;
        }
        try
        {
            string json = File.ReadAllText(path);
            return JsonSerializer.Deserialize<BuildDescriptor>(json);
        }
        catch (Exception ex)
        {
            BuildUtils.Error($"Failed to parse build file: {ex.Message}");
            return null;
        }
    }
}

public class ModuleDescriptor
{
    [JsonPropertyName("name")]
    public string Name { get; set; } = "";

    [JsonPropertyName("kind")]
    public string Kind { get; set; } = "StaticLib";

    [JsonPropertyName("target")]
    public string Target { get; set; } = "Engine";

    [JsonPropertyName("source_dirs")]
    public List<string> SourceDirs { get; set; } = new();

    [JsonPropertyName("include_dirs")]
    public List<string> IncludeDirs { get; set; } = new();

    [JsonPropertyName("dependencies")]
    public List<string> Dependencies { get; set; } = new();

    [JsonPropertyName("defines")]
    public List<string> Defines { get; set; } = new();
}