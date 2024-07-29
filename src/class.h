#pragma once

#include "core.h"


template <typename T>
class Object {
public:
    Object(meta_hpp::class_type type, meta_hpp::uvalue&& instance)
        : __type(type), __instance(std::move(instance)), __casted(__instance.as<T>()) {}

    template <typename... Args>
    auto invoke(const std::string& method_name, Args&&... args) {
        const auto method = __type.get_method(method_name);
        if (!method) {
            throw std::runtime_error("Method '" + method_name + "' not found");
        }
        return method.invoke(__instance, std::forward<Args>(args)...);
    }

    void list_methods() const {
        for (const auto& method : __type.get_methods()) {
            fmt::print("  + {}/{}\n", method.get_name(), method.get_type().get_arity());
        }
    }

    T* operator->() { return &__casted; }

private:
    meta_hpp::class_type __type;
    meta_hpp::uvalue __instance;
    T& __casted;

};


class ClassFactory {
public:
    template <typename T>
    Object<T> create() {
        auto type = meta_hpp::resolve_type<T>();
        auto instance = type.create();
        return Object<T>(type, std::move(instance));
    }

    template <typename T>
    std::shared_ptr<Object<T>> create_shared() {
        auto type = meta_hpp::resolve_type<T>();
        auto instance = type.create();
        return std::make_shared<Object<T>>(type, std::move(instance));
    }

    static ClassFactory& get_instance() {
        static ClassFactory instance;
        return instance;
    }

    ClassFactory(const ClassFactory&) = delete;
    ClassFactory& operator=(const ClassFactory&) = delete;

private:
    ClassFactory() = default;
    ~ClassFactory() = default;

};

