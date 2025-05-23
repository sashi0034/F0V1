﻿#pragma once

namespace ZG::asapi_detail
{
    constexpr std::string_view k_namespace{};

    inline std::vector<std::function<void(asIScriptEngine*)>> g_typeBindHandlers{};

    inline std::vector<std::function<void(asIScriptEngine*)>> g_globalBindHandlers{};

    inline std::vector<std::function<void()>> g_deferBindHandlers{};

    std::function<std::string(std::string)> MacroPreprocessor(const std::map<std::string, std::string>& macros);
}

#define ASAPI_IMPL_CONCATENATE_INNER(x, y) x##y

#define ASAPI_IMPL_CONCATENATE(x, y) ASAPI_IMPL_CONCATENATE_INNER(x, y)

#define ASAPI_IMPL_UNIQUE_NAME(name) \
    ASAPI_IMPL_CONCATENATE(name, __LINE__)

// #define ASAPI_VALUE_CLASS_AS(decl, name, flags) \
//     static inline std::vector<std::function<void(asbind20::value_class<name>)>> asapi_bindHandlers{}; \
//     static inline std::function<std::string(std::string)> asapi_preprocessor{}; \
//     static void RegisterScript(asIScriptEngine* engine) { \
//         const std::string declaration = asapi_preprocessor ? asapi_preprocessor(decl) : (decl); \
//         auto bind = asbind20::value_class<name>(engine, declaration.data(), flags) \
//             .behaviours_by_traits(); \
//         ::ZG::asapi_detail::g_deferBindHandlers.push_back([bind]() { \
//             for (const auto& handler : asapi_bindHandlers) \
//             { \
//                 handler(bind); \
//             } \
//         }); \
//     } \
//     using asapi_BindTarget = name;

#define ASAPI_VALUE_CLASS_AS(decl, name, flags) \
    private: \
    using asapi_BindTarget = name; \
    using asapi_class_type = asbind20::value_class<name>; \
    static inline std::vector<std::function<void(asapi_class_type)>> asapi_bindHandlers{}; \
    static inline std::function<std::string(std::string)> asapi_preprocessor{}; \
    static inline struct ASAPI_IMPL_UNIQUE_NAME(asapi_struct_) { \
        ASAPI_IMPL_UNIQUE_NAME(asapi_struct_)() { \
            ::ZG::asapi_detail::g_typeBindHandlers.push_back([](asIScriptEngine* engine) { \
                const std::string declaration = asapi_preprocessor ? asapi_preprocessor(decl) : (decl); \
                auto bind = asapi_class_type(engine, declaration.data(), flags) \
                    .behaviours_by_traits(); \
                ::ZG::asapi_detail::g_deferBindHandlers.push_back([bind]() { \
                    for (const auto& handler : asapi_bindHandlers) \
                    { \
                        handler(bind); \
                    } \
                }); \
            }); \
        } \
    } ASAPI_IMPL_UNIQUE_NAME(asapi_scriptBind_); public:

// The asbind20 will handle common flags for you. However, due to limitation of C++,
// the following flags still need user to provide them manually.
/// - asOBJ_APP_CLASS_MORE_CONSTRUCTORS
/// - asOBJ_APP_CLASS_ALLINTS
/// - asOBJ_APP_CLASS_ALLFLOATS,
/// - asOBJ_APP_CLASS_ALIGN8
/// - asOBJ_APP_CLASS_UNION

/// Usage: @code
/// ASAPI_VALUE_CLASS(KeyboardInput, asOBJ_POD | asOBJ_APP_CLASS_ALLINTS);
#define ASAPI_VALUE_CLASS(name, flags) \
    ASAPI_VALUE_CLASS_AS(#name, name, flags)

#define ASAPI_REF_CLASS(name, flags) \
    private: \
    using asapi_class_type = asbind20::ref_class<name>; \
    static inline std::vector<std::function<void(asapi_class_type)>> asapi_bindHandlers{}; \
    static inline std::function<std::string(std::string)> asapi_preprocessor{}; \
    static inline struct ASAPI_IMPL_UNIQUE_NAME(asapi_struct_) { \
        ASAPI_IMPL_UNIQUE_NAME(asapi_struct_)() { \
            ::ZG::asapi_detail::g_typeBindHandlers.push_back([](asIScriptEngine* engine) { \
                const std::string declaration = asapi_preprocessor ? asapi_preprocessor(#name) : (#name); \
                auto bind = asapi_class_type(engine, declaration.data(), flags); \
                ::ZG::asapi_detail::g_deferBindHandlers.push_back([bind]() { \
                    for (const auto& handler : asapi_bindHandlers) \
                    { \
                        handler(bind); \
                    } \
                }); \
            }); \
        } \
    } ASAPI_IMPL_UNIQUE_NAME(asapi_scriptBind_); public:

#define ASAPI_MACRO_PREPROCESSOR(...) \
    private: static inline struct ASAPI_IMPL_UNIQUE_NAME(asapi_struct_) { \
        ASAPI_IMPL_UNIQUE_NAME(asapi_struct_)() { \
            const auto macro = [](std::map<std::string, std::string> macros){ \
                asapi_preprocessor = ::ZG::asapi_detail::MacroPreprocessor(macros); \
            }; \
            __VA_ARGS__ \
        } \
    } ASAPI_IMPL_UNIQUE_NAME(asapi_scriptBind_); public:

/// Usage: @code
/// ASAPI_CLASS_BIND(opEquals());
#define ASAPI_CLASS_BIND(...) \
    private: static inline struct ASAPI_IMPL_UNIQUE_NAME(asapi_struct_) \
    { \
        ASAPI_IMPL_UNIQUE_NAME(asapi_struct_)() \
        { \
            asapi_bindHandlers.push_back([](asapi_class_type bind) \
            { \
                using namespace asbind20; \
                const auto t = [](std::string str) { return asapi_preprocessor ? asapi_preprocessor(str) : str; }; \
                bind.__VA_ARGS__; \
            }); \
        } \
    } ASAPI_IMPL_UNIQUE_NAME(asapi_scriptBind_); public:

/// Usage: @code
/// ASAPI_CLASS_CONSTRUCTOR(
///      <value_type, value_type>
///      (t("$value_type x, $value_type y")));
#define ASAPI_CLASS_CONSTRUCTOR_WHEN(condition, ...) \
    ASAPI_CLASS_BIND(template constructor __VA_ARGS__)

/// Usage: @code
/// ASAPI_CLASS_CONSTRUCTOR(
///      <value_type, value_type>
///      (t("$value_type x, $value_type y")));
#define ASAPI_CLASS_CONSTRUCTOR(...) \
    ASAPI_CLASS_CONSTRUCTOR_WHEN(true, __VA_ARGS__ )

/// Usage: @code
/// ASAPI_CLASS_METHOD("$Value2D withX($value_type newX) const", withX);
#define ASAPI_CLASS_METHOD(decl, method_name) \
    ASAPI_CLASS_BIND(method(t(decl).data(), &asapi_BindTarget::method_name))

#define ASAPI_CLASS_METHOD_BY(decl, ...) \
    ASAPI_CLASS_BIND(method(t(decl).data(), overload_cast __VA_ARGS__))

/// Usage: @code
/// ASAPI_CLASS_PROPERTY("$value_type x", x);
#define ASAPI_CLASS_PROPERTY(decl, method_name) \
    ASAPI_CLASS_BIND(property(t(decl).data(), &asapi_BindTarget::method_name))

/// Usage: @code
/// ASAPI_CLASS_OPERATOR(_this + const_this);
#define ASAPI_CLASS_OPERATOR(operator) \
    ASAPI_CLASS_BIND(use(operator));

// -----------------------------------------------

#define ASAPI_NAMESPACE(decl) \
    namespace asapi_detail { \
        constexpr std::string_view k_namespace = decl; \
    };

#define ASAPI_GLOBAL_BIND(...) \
    namespace asapi_detail { inline struct ASAPI_IMPL_UNIQUE_NAME(asapi_struct_) \
    { \
        ASAPI_IMPL_UNIQUE_NAME(asapi_struct_)() \
        { \
            ::ZG::asapi_detail::g_globalBindHandlers.push_back([](asIScriptEngine* engine) \
            { \
                using namespace asbind20; \
                const auto ns = asbind20::namespace_(engine, k_namespace); \
                asbind20::global(engine).__VA_ARGS__; \
            }); \
        } \
    } ASAPI_IMPL_UNIQUE_NAME(asapi_scriptBind_); }

/// Usage: @code
/// ASAPI_GLOBAL_PROPERTY("const KeyboardInput KeyEnter", KeyEnter);
#define ASAPI_GLOBAL_PROPERTY(decl, name) \
    ASAPI_GLOBAL_BIND(property(decl, name))

#define ASAPI_GLOBAL_FUNCTION(decl, name) \
    ASAPI_GLOBAL_BIND(function(decl, name))

#define ASAPI_GLOBAL_FUNCTION_BY(decl, ...) \
    ASAPI_GLOBAL_BIND(function(decl, overload_cast __VA_ARGS__))
