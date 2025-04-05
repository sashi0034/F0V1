#pragma once

#define ASAPI_VALUE_CLASS(name, flags) \
    static inline std::vector<std::function<void(asbind20::value_class<name>)>> s_asapi_bindHandlers{}; \
    static auto RegisterScript(asIScriptEngine* engine) { \
        auto bind = asbind20::value_class<name>(engine, #name, flags) \
            .behaviours_by_traits(); \
        for (const auto& handler : s_asapi_bindHandlers) \
        { \
            handler(bind); \
        } \
    } \
    using asapi_BindTarget = name;

#define ASAPI_CLASS_METHOD(decl, method_name) \
    struct asapi_##method_name \
    { \
        asapi_##method_name() \
        { \
            s_asapi_bindHandlers.push_back([](asbind20::value_class<asapi_BindTarget> bind) \
            { \
                bind.method(decl, &asapi_BindTarget::method_name); \
            }); \
        } \
    }; \
    static inline asapi_##method_name s_asapi_scriptBind_##method_name{};

#define ASAPI_GLOBAL_PROPERTY(decl, name) \
    struct asapi_##name \
    { \
        asapi_##name() \
        { \
            g_asapi_globalBindHandlers.push_back([](asbind20::global<false> bind) \
            { \
                bind.property(decl, name); \
            }); \
        } \
    }; \
    inline asapi_##name s_asapi_scriptBind_##name{};
