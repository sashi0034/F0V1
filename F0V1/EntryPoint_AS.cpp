#include "pch.h"
#include "EntryPoint_AS.h"

#include "add_on/datetime/datetime.h"
#include "add_on/scriptany/scriptany.h"
#include "add_on/scriptarray/scriptarray.h"
#include "add_on/scriptbuilder/scriptbuilder.h"
#include "add_on/scriptdictionary/scriptdictionary.h"
#include "add_on/scriptfile/scriptfile.h"
#include "add_on/scriptfile/scriptfilesystem.h"
#include "add_on/scriptgrid/scriptgrid.h"
#include "add_on/scripthelper/scripthelper.h"
#include "add_on/scriptmath/scriptmath.h"
#include "add_on/scriptstdstring/scriptstdstring.h"
#include "add_on/weakref/weakref.h"
#include "ZG/KeyboardInput.h"
#include "ZG/Rect.h"
#include "ZG/System.h"
#include "ZG/Texture.h"
#include "ZG/TextureSource.h"
#include "ZG/Value2D.h"
#include "ZG/Script/ScriptPredefinedGenerator.h"

using namespace ZG;

namespace
{
    void script_print(const std::string& message)
    {
        printf("%s", message.c_str());
    }

    void println(const std::string& message)
    {
        printf("%s\n", message.c_str());
    }

    void MessageCallback(const asSMessageInfo* msg, void* param)
    {
        auto type = "[error]";
        if (msg->type == asMSGTYPE_WARNING)
            type = "[warn] ";
        else if (msg->type == asMSGTYPE_INFORMATION)
            type = "[info] ";

        const auto message = std::format("{} {}({},{}) {}", type, msg->section, msg->row, msg->col, msg->message);
        if (msg->type == asMSGTYPE_INFORMATION)
        {
            std::cout << message << std::endl;
        }
        else
        {
            std::cerr << message << std::endl;
        }
    }

    std::string preprocessDeclaration(std::string declarations, std::map<std::string, std::string> macros)
    {
        for (const auto& [key, value] : macros)
        {
            size_t pos = 0;
            while ((pos = declarations.find(key, pos)) != std::string::npos)
            {
                declarations.replace(pos, key.length(), value);
                pos += value.length(); // 次の位置へ進む
            }
        }

        return declarations;
    }

    std::function<std::string(const std::string&)> makePreprocessFunction(std::map<std::string, std::string> macros)
    {
        return [macros](const std::string& declarations)
        {
            return preprocessDeclaration(declarations, macros);
        };
    }

    void registerEngine(const asbind20::script_engine& engine)
    {
        // script extenstion: https://www.angelcode.com/angelscript/sdk/docs/manual/doc_addon_script.html
        RegisterStdString(engine);
        RegisterScriptArray(engine, true);
        RegisterScriptAny(engine);
        RegisterScriptWeakRef(engine);
        RegisterScriptDictionary(engine);
        RegisterScriptFile(engine);
        RegisterScriptMath(engine);
        RegisterScriptGrid(engine);
        RegisterScriptDateTime(engine);
        RegisterScriptFileSystem(engine);

        RegisterExceptionRoutines(engine);

        // asbind20: https://github.com/HenryAWE/asbind20/
        asbind20::global(engine)
            .message_callback(&MessageCallback)
            .function("void print(const string& in message)", &script_print)
            .function("void println(const string& in message)", &println);

        // asbind20::value_class<flag_t>(engine, "flag_t", asOBJ_VALUE)
        //     .behaviours_by_traits()
        //     .constructor<bool>("bool flag")
        //     .opEquals()
        //     .opConv<bool>()
        //     .opImplConv<bool>()
        //     .property("bool flag", &flag_t::flag);

        asbind20::ref_class<ID3D12Resource>(engine, "ID3D12Resource", asOBJ_NOCOUNT);

        {
            const auto ns = asbind20::namespace_(engine, "System");;
            asbind20::global(engine)
                .function("bool Update()", &System::Update);
        }

        // -----------------------------------------------

        for (const auto& handler : asapi_detail::g_typeBindHandlers)
        {
            handler(engine);
        }

        for (const auto& handler : asapi_detail::g_globalBindHandlers)
        {
            handler(asbind20::global(engine));
        }

        for (const auto& handler : asapi_detail::g_deferBindHandlers)
        {
            handler();
        }
    }

    int includeCallback(const char* include, const char* from, CScriptBuilder* builder, void* userParam)
    {
        namespace fs = std::filesystem;
        const fs::path fromDir{from};
        const fs::path includePath{include};

        fs::path primaryPath = fromDir.parent_path() / includePath;
        if (fs::exists(primaryPath))
        {
            builder->AddSectionFromFile(primaryPath.string().c_str());
            return 0;
        }

        // const fs::path fallbackDir = fs::current_path() / "include_fallback";
        // const fs::path fallbackPath = fallbackDir / includePath;
        //
        // if (fs::exists(fallbackPath))
        // {
        //     builder->AddSectionFromFile(fallbackPath.string().c_str());
        //     return 0;
        // }

        return -1;
    }
}

void EntryPoint_AS()
{
    std::string moduleName{"script/main.as"};

    // -----------------------------------------------

    const auto engine = asbind20::make_script_engine();

    registerEngine(engine);

    ZG::Script::GeneratePredefined(engine, "script/as.predefined");

    CScriptBuilder builder{};
    builder.SetIncludeCallback(includeCallback, nullptr);

    if (builder.StartNewModule(engine, moduleName.c_str()) < 0)
    {
        std::cerr << "Failed to start a new module" << std::endl;
        return;
    }

    if (builder.AddSectionFromFile(moduleName.c_str()) < 0)
    {
        std::cerr << "Failed to add section from file" << std::endl;
        return;
    }

    if (builder.BuildModule() < 0)
    {
        std::cerr << "Failed to build module" << std::endl;
        return;
    }

    asIScriptModule* module = engine->GetModule(moduleName.c_str());
    if (not module)
    {
        std::cerr << "Failed to find module" << std::endl;
        return;
    }

    // -----------------------------------------------

    asIScriptFunction* func = module->GetFunctionByDecl("void main()");
    if (not func)
    {
        std::cerr << "Failed to find 'void main()'" << std::endl;
        return;
    }

    const asbind20::request_context ctx{engine};
    const auto result = asbind20::script_invoke<void>(ctx, func);
    if (not result.has_value())
    {
        std::cerr << "Failed to execute the script: " << result.error() << std::endl;
        // TODO: print exception
    }

    module->Discard();
}
