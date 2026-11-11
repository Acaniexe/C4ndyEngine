using System.Formats.Tar;
using System.IO.Compression;
using System.Security.Cryptography;
using System.Text.Json;
using System.Text.Json.Serialization;

namespace C4ndy.Build;

public class DepsManifest
{
    [JsonPropertyName("version")] public int Version { get; set; }
    [JsonPropertyName("release_base")] public string ReleaseBase { get; set; } = "";
    [JsonPropertyName("dependencies")] public List<DepEntry> Dependencies { get; set; } = new();
}

public class DepEntry
{
    [JsonPropertyName("name")] public string Name { get; set; } = "";
    [JsonPropertyName("platform")] public string PlatformName { get; set; } = "all";
    [JsonPropertyName("file")] public string File { get; set; } = "";
    [JsonPropertyName("sha256")] public string Sha256 { get; set; } = "";
    [JsonPropertyName("extract_to")] public string ExtractTo { get; set; } = "";
    [JsonPropertyName("check")] public string? Check { get; set; }
    [JsonPropertyName("executable")] public bool Executable { get; set; }
}

public static class DependencySetup
{
    public static bool Run(string root, bool verifyOnly)
    {
        string manifestPath = Path.Combine(root, "c4ndy.deps.json");
        if (!System.IO.File.Exists(manifestPath))
        {
            BuildUtils.Error($"Manifest not found: {manifestPath}");
            return false;
        }

        DepsManifest? manifest;
        try { manifest = JsonSerializer.Deserialize<DepsManifest>(System.IO.File.ReadAllText(manifestPath)); }
        catch (JsonException ex)
        {
            BuildUtils.Error($"Could not read c4ndy.deps.json: {ex.Message}");
            return false;
        }
        if (manifest is null) return false;

        BuildUtils.Info(verifyOnly ? "Verifying dependencies..." : "Setting up dependencies...");

        string markerDir = Path.Combine(root, "Engine", "ThirdParty", ".deps");
        bool ok = true;

        foreach (var dep in manifest.Dependencies.Where(d => AppliesToHost(d.PlatformName)))
            ok &= ProcessEntry(root, manifest, dep, markerDir, verifyOnly);

        if (ok) BuildUtils.Info(verifyOnly ? "All dependencies present." : "Setup complete.");
        else BuildUtils.Error(verifyOnly ? "Some dependencies are missing. Run setup." : "Setup failed.");
        return ok;
    }

    private static bool AppliesToHost(string platform) => platform.ToLowerInvariant() switch
    {
        "all"     => true,
        "windows" => Platform.OS == HostPlatform.Windows,
        "linux"   => Platform.OS == HostPlatform.Linux,
        "macos"   => Platform.OS == HostPlatform.macOS,
        _         => false
    };

    private static bool ProcessEntry(string root, DepsManifest manifest, DepEntry dep, string markerDir, bool verifyOnly)
    {
        string dest = Path.GetFullPath(Path.Combine(root, dep.ExtractTo));
        string marker = Path.Combine(markerDir, dep.Name + ".sha256");

        bool present = string.IsNullOrEmpty(dep.Check)
            ? Directory.Exists(dest) && Directory.EnumerateFileSystemEntries(dest).Any()
            : System.IO.File.Exists(Path.Combine(dest, dep.Check));

        string? installedHash = System.IO.File.Exists(marker) ? System.IO.File.ReadAllText(marker).Trim() : null;
        bool upToDate = installedHash != null && installedHash.Equals(dep.Sha256, StringComparison.OrdinalIgnoreCase);

        if (present && upToDate)
        {
            BuildUtils.Info($"  [OK]    {dep.Name}");
            return true;
        }

        if (present && installedHash is null)
        {
            BuildUtils.Info($"  [OK]    {dep.Name} (found, not managed by setup)");
            return true;
        }

        if (verifyOnly)
        {
            BuildUtils.Error($"  [MISSING] {dep.Name}" + (present ? " (out of date)" : ""));
            return false;
        }

        return Install(root, manifest, dep, dest, marker);
    }

    private static bool Install(string root, DepsManifest manifest, DepEntry dep, string dest, string marker)
    {
        string tmp = Path.Combine(Path.GetTempPath(), "c4ndy-" + dep.File);
        try
        {
            BuildUtils.Info($"  [GET]   {dep.Name}");
            if (!Fetch(root, manifest.ReleaseBase, dep.File, tmp)) return false;

            string actual;
            using (var fs = System.IO.File.OpenRead(tmp))
                actual = Convert.ToHexString(SHA256.HashData(fs));

            if (!actual.Equals(dep.Sha256, StringComparison.OrdinalIgnoreCase))
            {
                BuildUtils.Error($"  [FAIL]  {dep.Name}: hash mismatch (corrupt or changed download)");
                BuildUtils.Error($"          expected {dep.Sha256}");
                BuildUtils.Error($"          actual   {actual}");
                return false;
            }

            Directory.CreateDirectory(dest);

            if (dep.File.EndsWith(".zip", StringComparison.OrdinalIgnoreCase))
                ExtractZip(tmp, dest);
            else if (dep.File.EndsWith(".tar.gz", StringComparison.OrdinalIgnoreCase))
                ExtractTarGz(tmp, dest);
            else
            {
                BuildUtils.Error($"  [FAIL]  {dep.Name}: unknown archive type");
                return false;
            }

            if (dep.Executable && !string.IsNullOrEmpty(dep.Check))
                MarkExecutable(Path.Combine(dest, dep.Check));

            if (!string.IsNullOrEmpty(dep.Check) && !System.IO.File.Exists(Path.Combine(dest, dep.Check)))
            {
                BuildUtils.Error($"  [FAIL]  {dep.Name}: extracted, but '{dep.Check}' is missing");
                return false;
            }

            Directory.CreateDirectory(Path.GetDirectoryName(marker)!);
            System.IO.File.WriteAllText(marker, dep.Sha256);
            BuildUtils.Info($"  [OK]    {dep.Name} installed");
            return true;
        }
        catch (Exception ex)
        {
            BuildUtils.Error($"  [FAIL]  {dep.Name}: {ex.Message}");
            return false;
        }
        finally
        {
            if (System.IO.File.Exists(tmp)) System.IO.File.Delete(tmp);
        }
    }

    private static bool Fetch(string root, string releaseBase, string file, string destFile)
    {
        if (releaseBase.StartsWith("http://") || releaseBase.StartsWith("https://"))
        {
            using var client = new HttpClient { Timeout = TimeSpan.FromMinutes(10) };
            client.DefaultRequestHeaders.UserAgent.ParseAdd("C4ndyBuildTool");
            string url = releaseBase.TrimEnd('/') + "/" + file;

            using var response = client.Send(new HttpRequestMessage(HttpMethod.Get, url),
                HttpCompletionOption.ResponseHeadersRead);
            if (!response.IsSuccessStatusCode)
            {
                BuildUtils.Error($"  [FAIL]  download failed ({(int)response.StatusCode}): {url}");
                return false;
            }

            using var src = response.Content.ReadAsStream();
            using var dst = System.IO.File.Create(destFile);
            src.CopyTo(dst);
            return true;
        }

        string local = Path.GetFullPath(Path.Combine(root, releaseBase, file));
        if (!System.IO.File.Exists(local))
        {
            BuildUtils.Error($"  [FAIL]  local file not found: {local}");
            return false;
        }
        System.IO.File.Copy(local, destFile, true);
        return true;
    }

    private static void ExtractZip(string archive, string dest)
    {
        using var zip = ZipFile.OpenRead(archive);
        foreach (var entry in zip.Entries)
        {
            string name = entry.FullName.Replace('\\', '/');
            string target = Path.GetFullPath(Path.Combine(dest, name));

            if (!IsInside(dest, target))
                throw new IOException($"Blocked unsafe path in archive: {entry.FullName}");

            if (name.EndsWith('/'))
            {
                Directory.CreateDirectory(target);
                continue;
            }

            Directory.CreateDirectory(Path.GetDirectoryName(target)!);
            entry.ExtractToFile(target, overwrite: true);
        }
    }

    private static void ExtractTarGz(string archive, string dest)
    {
        using var file = System.IO.File.OpenRead(archive);
        using var gz = new GZipStream(file, CompressionMode.Decompress);
        TarFile.ExtractToDirectory(gz, dest, overwriteFiles: true);
    }

    private static bool IsInside(string dest, string target)
    {
        string rel = Path.GetRelativePath(dest, target);
        return rel != ".." && !rel.StartsWith(".." + Path.DirectorySeparatorChar) && !Path.IsPathRooted(rel);
    }

    private static void MarkExecutable(string path)
    {
        if (OperatingSystem.IsWindows() || !System.IO.File.Exists(path)) return;
        var mode = System.IO.File.GetUnixFileMode(path);
        System.IO.File.SetUnixFileMode(path,
            mode | UnixFileMode.UserExecute | UnixFileMode.GroupExecute | UnixFileMode.OtherExecute);
    }
}
