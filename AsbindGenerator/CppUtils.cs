#nullable enable

namespace AsbindGenerator;

public static class CppUtils
{
    public static string[] GetHeaderFiles(string absolutePath)
    {
        if (!Path.IsPathRooted(absolutePath))
            throw new ArgumentException("Provided path is not an absolute path.");

        if (!Directory.Exists(absolutePath))
            throw new DirectoryNotFoundException("Specified directory does not exist.");

        return Directory.GetFiles(absolutePath, "*.h", SearchOption.AllDirectories);
    }
}