#pragma once

// inline std::vector<std::function<void(asIScriptEngine*)>> asapi_typeBindHandlers{};

inline std::vector<std::function<void(asbind20::global<false>)>> asapi_globalBindHandlers{};

inline std::vector<std::function<void()>> asapi_deferBindHandlers{};

#define ASAPI_IMPL_CONCATENATE_INNER(x, y) x##y

#define ASAPI_IMPL_CONCATENATE(x, y) ASAPI_IMPL_CONCATENATE_INNER(x, y)

#define ASAPI_IMPL_UNIQUE_NAME(name) \
    ASAPI_IMPL_CONCATENATE(name, __LINE__)

#define ASAPI_VALUE_CLASS_AS(decl, name, flags) \
    static inline std::vector<std::function<void(asbind20::value_class<name>)>> asapi_bindHandlers{}; \
    static inline std::function<std::string(std::string)> asapi_preprocessor{}; \
    static void RegisterScript(asIScriptEngine* engine) { \
        const std::string declaration = asapi_preprocessor ? asapi_preprocessor(decl) : (decl); \
        auto bind = asbind20::value_class<name>(engine, declaration.data(), flags) \
            .behaviours_by_traits(); \
        asapi_deferBindHandlers.push_back([bind]() { \
            for (const auto& handler : asapi_bindHandlers) \
            { \
                handler(bind); \
            } \
        }); \
    } \
    using asapi_BindTarget = name;

// #define ASAPI_VALUE_CLASS_AS(decl, name, flags) \
//     static inline std::vector<std::function<void(asbind20::value_class<name>)>> asapi_bindHandlers{}; \
//     static inline std::function<std::string(std::string)> asapi_preprocessor{}; \
//     static inline struct ASAPI_IMPL_UNIQUE_NAME(asapi_struct_) { \
//         ASAPI_IMPL_UNIQUE_NAME(asapi_struct_)() { \
//             asapi_typeBindHandlers.push_back([](asIScriptEngine* engine) { \
//                 const std::string declaration = asapi_preprocessor ? asapi_preprocessor(decl) : (decl); \
//                 auto bind = asbind20::value_class<name>(engine, declaration.data(), flags) \
//                     .behaviours_by_traits(); \
//                 asapi_deferBindHandlers.push_back([bind]() { \
//                     for (const auto& handler : asapi_bindHandlers) \
//                     { \
//                         handler(bind); \
//                     } \
//                 }); \
//             }); \
//         } \
//     } ASAPI_IMPL_UNIQUE_NAME(asapi_scriptBind_); \
//     using asapi_BindTarget = name;

#define ASAPI_VALUE_CLASS(name, flags) \
    ASAPI_VALUE_CLASS_AS(#name, name, flags)

#define ASAPI_CLASS_CONSTRUCTOR(...) \
    static inline struct ASAPI_IMPL_UNIQUE_NAME(asapi_struct_) \
    { \
        ASAPI_IMPL_UNIQUE_NAME(asapi_struct_)() \
        { \
            asapi_bindHandlers.push_back([](asbind20::value_class<asapi_BindTarget> bind) \
            { \
                using namespace asbind20; \
                const auto t = [](std::string str) { return asapi_preprocessor ? asapi_preprocessor(str) : str; }; \
                bind.template constructor __VA_ARGS__; \
            }); \
        } \
    } ASAPI_IMPL_UNIQUE_NAME(asapi_scriptBind_);

#define ASAPI_CLASS_METHOD(decl, method_name) \
    static inline struct ASAPI_IMPL_UNIQUE_NAME(asapi_struct_) \
    { \
        ASAPI_IMPL_UNIQUE_NAME(asapi_struct_)() \
        { \
            asapi_bindHandlers.push_back([](asbind20::value_class<asapi_BindTarget> bind) \
            { \
                const std::string declaration = asapi_preprocessor ? asapi_preprocessor(decl) : decl; \
                bind.method(declaration.data(), &asapi_BindTarget::method_name); \
            }); \
        } \
    } ASAPI_IMPL_UNIQUE_NAME(asapi_scriptBind_);

#define ASAPI_CLASS_PROPERTY(decl, method_name) \
    static inline struct ASAPI_IMPL_UNIQUE_NAME(asapi_struct_) \
    { \
        ASAPI_IMPL_UNIQUE_NAME(asapi_struct_)() \
        { \
            asapi_bindHandlers.push_back([](asbind20::value_class<asapi_BindTarget> bind) \
            { \
                const std::string declaration = asapi_preprocessor ? asapi_preprocessor(decl) : decl; \
                bind.property(declaration.data(), &asapi_BindTarget::method_name); \
            }); \
        } \
    } ASAPI_IMPL_UNIQUE_NAME(asapi_scriptBind_);

#define ASAPI_CLASS_OPERATOR(operator) \
    static inline struct ASAPI_IMPL_UNIQUE_NAME(asapi_struct_) \
    { \
        ASAPI_IMPL_UNIQUE_NAME(asapi_struct_)() \
        { \
            asapi_bindHandlers.push_back([](asbind20::value_class<asapi_BindTarget> bind) \
            { \
                using namespace asbind20; \
                bind.use(operator); \
            }); \
        } \
    } ASAPI_IMPL_UNIQUE_NAME(asapi_scriptBind_);

#define ASAPI_GLOBAL_PROPERTY(decl, name) \
    struct asapi_##name \
    { \
        asapi_##name() \
        { \
            asapi_globalBindHandlers.push_back([](asbind20::global<false> bind) \
            { \
                bind.property(decl, name); \
            }); \
        } \
    }; \
    inline asapi_##name asapi_scriptBind_##name{};
