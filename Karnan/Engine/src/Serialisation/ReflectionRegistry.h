#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

enum class FieldType {
    Int,
    Float,
    Bool,
    String,
    Vec3,
    Vec2
};

struct FieldInfo {
    std::string Name;
    FieldType Type;
    size_t Offset;  // byte offset from base pointer
};

struct ClassInfo {
    std::string Name;
    std::function<void* ()> Factory;
    std::vector<FieldInfo> Fields;
};

class ReflectionRegistry {
public:
    static std::unordered_map<std::string, ClassInfo>& Registry() {
        static std::unordered_map<std::string, ClassInfo> inst;
        return inst;
    }

    static void RegisterClass(const ClassInfo& info) {
        Registry()[info.Name] = info;
    }

    static ClassInfo* Find(const std::string& name) {
        auto it = Registry().find(name);
        return it == Registry().end() ? nullptr : &it->second;
    }
};

#define REGISTER_CLASS(ClassName) \
    static ClassRegistrar<ClassName> _reg_##ClassName(#ClassName)

template <typename T>
struct ClassRegistrar {
    ClassRegistrar(const char* name) {
        ClassInfo info;
        info.name = name;
        info.factory = []() { return new T(); };
        T::RegisterFields(info.fields);
        ReflectionRegistry::RegisterClass(info);
    }
};

#define REGISTER_FIELD(Class, field, fieldType) \
    Fields.push_back({#field, fieldType, offsetof(Class, field)})

class Reflectable {
public:
    virtual ~Reflectable() = default;
    virtual const char* GetClassName() const = 0;
};