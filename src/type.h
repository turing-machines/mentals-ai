#pragma once

#include "core.h"

#include "meta/factory.hpp"
#include "meta/meta.hpp"


using meta_hash = std::hash<std::string_view>;


class TypeRegistry {
public:
    static TypeRegistry& instance() {
        static TypeRegistry instance;
        return instance;
    }

    template<typename T>
    void register_type(const std::string& name) {
        auto hash = meta_hash{}(name);
        meta::reflect<T>(hash);
    }

    meta::type get_type(const std::string& name) const {
        auto hash = meta_hash{}(name);
        return meta::resolve(hash);
    }

private:
    TypeRegistry() = default;
    TypeRegistry(const TypeRegistry&) = delete;
    TypeRegistry& operator=(const TypeRegistry&) = delete;

};


template<typename T>
class MetaBuilder {
public:
    explicit MetaBuilder(const std::string& name)
        : __factory(meta::reflect<T>(meta_hash{}(name))) {
    }

    template<auto Property>
    MetaBuilder& add_property(const std::string& name) {
        __factory.template data<Property>(meta_hash{}(name));
        return *this;
    }

    template<auto Method>
    MetaBuilder& add_method(const std::string& name) {
        __factory.template func<Method>(meta_hash{}(name));
        return *this;
    }

    template<typename... Args>
    MetaBuilder& add_constructor() {
        __factory.template ctor<Args...>();
        return *this;
    }

    meta::factory<T>& get_factory() { return __factory; }

private:
    meta::factory<T> __factory;

};


template<typename T>
class Object {
public:
    Object(meta::any instance)
        : __instance(std::move(instance)), __object(__instance.try_cast<T>()) {
        if (!__object) {
            throw std::runtime_error(fmt::format(
                "Failed to cast instance to the provided type '{}'", typeid(T).name()
            ));
        }
    }

    template<typename MemberType>
    MemberType get_member(const std::string& name) {
        auto data = __instance.type().data(meta_hash{}(name));
        if (data) {
            return data.get(__instance).cast<MemberType>();
        } else {
            fmt::print("Data member {} not found.\n", name);
            return MemberType{};
        }
    }

    template<typename... Args>
    meta::any invoke(const std::string& name, Args&&... args) {
        auto func = __instance.type().func(meta_hash{}(name));
        if (func) {
            if (sizeof...(Args) != func.size()) {
                fmt::print("Argument count mismatch for method: {}\n", name);
                return {};
            }
            auto result = func.invoke(__instance, std::forward<Args>(args)...);
            if (!result) {
                fmt::print("Invocation failed for method: {}\n", name);
            }
            return result;
        } else {
            fmt::print("Method {} not found.\n", name);
            return {};
        }
    }

    T* operator->() { return __object; }
    const T* operator->() const { return __object; }


private:
    meta::any __instance;
    T* __object;

};


class Factory {
public:
    template<typename T, typename... Args>
    static std::shared_ptr<Object<T>> create_object(const std::string& class_name, Args&&... args) {
        auto instance = create_instance(class_name, std::forward<Args>(args)...);
        if (instance) {
            return std::make_shared<Object<T>>(instance);
        }
        return nullptr;
    }

    // template<typename T, typename... Args>
    // static std::shared_ptr<Object<T>> create_object(Args&&... args) {
    //     return create_object<T>(typeid(T).name(), std::forward<Args>(args)...);
    // }

private:
    template<typename... Args>
    static meta::any create_instance(const std::string& class_name, Args&&... args) {
        auto type = TypeRegistry::instance().get_type(class_name);
        if (type) {
            return type.construct(std::forward<Args>(args)...);
        }
        return {};
    }

};


#define CTOR(...) meta_builder.add_constructor<__VA_ARGS__>();
#define PROPERTY(NAME) meta_builder.add_property<&T::NAME>(#NAME);
#define METHOD(NAME) meta_builder.add_method<&T::NAME>(#NAME);
#define REGISTER_TYPE(TYPE, ...) \
    namespace { \
        template <typename T = TYPE> \
        struct __registerer_##TYPE { \
            __registerer_##TYPE() { \
                MetaBuilder<TYPE> meta_builder(#TYPE); \
                __VA_ARGS__ \
            } \
        }; \
        static __registerer_##TYPE<> __register_##TYPE; \
    }

