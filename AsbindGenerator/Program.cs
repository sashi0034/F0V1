// See https://aka.ms/new-console-template for more information

using CppAst;

namespace AsbindGenerator;

static class Program
{
    static void Main(string[] args)
    {
        if (args.Length != 1)
        {
            Console.WriteLine("Usage: program.exe <absolute_path>");
            return;
        }

        string rootPath = args[0];

        var vcxprojjPath = AsbindUtils.FindVcxprojFile(rootPath);
        var additionalIncludeDirectories = AsbindUtils.GetAdditionalIncludeDirectories(vcxprojjPath);

        var headers = AsbindUtils.GetHeaderFiles(rootPath);

        const string pchFileName = "pch.h";
        var pchPath = Path.Combine(rootPath, pchFileName);
        var pchContent = File.ReadAllText(pchPath);

        Console.WriteLine(Directory.GetCurrentDirectory());

        foreach (var header in headers)
        {
            if (header.EndsWith(pchFileName)) continue;

            Console.WriteLine(header);

            var content = File.ReadAllText(header);

            // Parse a C++ files
            var option = new CppParserOptions().ConfigureForWindowsMsvc();

            // option.IncludeFolders.Add(Path.GetDirectoryName(header));
            option.IncludeFolders.AddRange(additionalIncludeDirectories);
            option.SystemIncludeFolders.Add(@"C:\Program Files\LLVM\lib\clang\17\include");

            option.AdditionalArguments.Add("-std=c++20");

            var compilation = CppParser.Parse(content, option);

            // Print diagnostic messages
            foreach (var message in compilation.Diagnostics.Messages)
            {
                Console.WriteLine(message);
            }

            // Print All enums
            foreach (var cppEnum in compilation.Enums)
            {
                Console.WriteLine(cppEnum);
            }

            // Print All functions
            foreach (var cppFunction in compilation.Functions)
            {
                Console.WriteLine(cppFunction);
            }

            // Print All classes, structs
            foreach (var cppClass in compilation.Classes)
            {
                Console.WriteLine(cppClass);
            }

            Console.WriteLine("-----------------------------------------------");
        }
    }
}