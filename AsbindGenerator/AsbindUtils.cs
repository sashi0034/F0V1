#nullable enable

using System.Xml.Linq;

namespace AsbindGenerator;

public static class AsbindUtils
{
    public static string[] GetHeaderFiles(string absolutePath)
    {
        if (!Path.IsPathRooted(absolutePath))
            throw new ArgumentException("Provided path is not an absolute path.");

        if (!Directory.Exists(absolutePath))
            throw new DirectoryNotFoundException("Specified directory does not exist.");

        return Directory.GetFiles(absolutePath, "*.h", SearchOption.AllDirectories);
    }

    public static string FindVcxprojFile(string path)
    {
        if (!Directory.Exists(path))
        {
            throw new DirectoryNotFoundException("Specified directory does not exist.");
        }

        string?[] files = Directory.GetFiles(path, "*.vcxproj");

        return files[0] ?? throw new FileNotFoundException("No .vcxproj file found in the specified directory.");
    }

    public static string[] GetAdditionalIncludeDirectories(string vcxprojPath)
    {
        if (!File.Exists(vcxprojPath))
            throw new FileNotFoundException("vcxproj file not found.", vcxprojPath);

        string baseDir = Path.GetDirectoryName(vcxprojPath)!;
        var doc = XDocument.Load(vcxprojPath);

        XNamespace ns = doc.Root?.Name.Namespace ?? "";

        var includeDirs = doc.Descendants(ns + "AdditionalIncludeDirectories")
            .SelectMany(e => e.Value.Split(';'))
            .Where(s => !string.IsNullOrWhiteSpace(s) && s != "%(AdditionalIncludeDirectories)")
            .Select(dir =>
            {
                string trimmed = dir.Trim();

                string expanded = Environment.ExpandEnvironmentVariables(trimmed);

                return Path.IsPathRooted(expanded)
                    ? Path.GetFullPath(expanded)
                    : Path.GetFullPath(Path.Combine(baseDir, expanded));
            })
            .Distinct()
            .ToArray();

        return includeDirs;
    }
}