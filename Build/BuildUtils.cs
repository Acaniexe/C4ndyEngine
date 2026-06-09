namespace C4ndy.Build;

public static class BuildUtils
{
    public static string? FindRoot()
    {
        string? dir = Directory.GetCurrentDirectory();
        while (dir is not null)
        {
            if (File.Exists(Path.Combine(dir, "c4ndy.build.json")))
                return dir;
            dir = Directory.GetParent(dir)?.FullName;
        }
        return null;
    }

    public static void Error(string msg)
    {
        Console.ForegroundColor = ConsoleColor.Red;
        Console.Error.WriteLine($"[Error] {msg}");
        Console.ResetColor();
    }

    public static void Warn(string msg)
    {
        Console.ForegroundColor = ConsoleColor.DarkYellow;
        Console.Error.WriteLine($"[Warn] {msg}");
        Console.ResetColor();
    }

    public static void Info(string msg)
    {
        Console.WriteLine($"[Info] {msg}");
    }
}