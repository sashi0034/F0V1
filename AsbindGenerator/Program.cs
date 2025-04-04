// See https://aka.ms/new-console-template for more information

using CppAst;
using Irony.Parsing;

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

        var headers = CppUtils.GetHeaderFiles(rootPath);

        Console.WriteLine(Directory.GetCurrentDirectory());
        foreach (var header in headers)
        {
            var content = File.ReadAllText(header);

            // Parse a C++ files
            var option = new CppParserOptions().ConfigureForWindowsMsvc();
            option.IncludeFolders.Add(Path.GetDirectoryName(header));
            option.IncludeFolders.Add(rootPath);
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
        }
    }
}