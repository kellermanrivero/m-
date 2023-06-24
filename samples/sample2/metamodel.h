// Copyright (c) 2023. Kellerman Rivero

#pragma once

#include <cxxabi.h>
#include <map>
#include <typeindex>
#include <typeinfo>
#include <vector>

inline std::string type_name(const std::type_index & type) {
    int status = 0;

    std::unique_ptr<char, void(*)(void*)> res {
            abi::__cxa_demangle(type.name(), nullptr, 0, &status),
            std::free
    };

    if (status != 0) throw status; // stub

    return res.get();
}

template<typename Type>
class ObjectProperty {
public:
    ObjectProperty& operator= (const Type& value) {
        m_value = value;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& out, const ObjectProperty<Type>& property)
    {
        out << property.m_value;
        return out;
    }
private:
    Type m_value;
};

class BasePropertyDefinition {
public:
    BasePropertyDefinition(std::string name, std::type_index type): m_name{name}, m_type{type} {}
    std::string GetName() {
        return m_name;
    }
    std::string GetType() {
        return type_name(m_type);
    }
protected:
    std::string m_name;
    std::type_index m_type;
};

template<class TObject, class TPropertyType>
class PropertyDefinition: public BasePropertyDefinition {
public:
    PropertyDefinition(std::string name, std::type_index type, ObjectProperty<TPropertyType> TObject::* property):
       BasePropertyDefinition(name, type), m_property{property} {}


private:
    ObjectProperty<TPropertyType> TObject::* m_property;
};

template<typename TObject>
class BaseObject {
public:
    static std::vector<BasePropertyDefinition*> GetProperties() {
        std::vector<BasePropertyDefinition*> properties;
        for(const auto &kv: m_properties) {
            properties.push_back(kv.second);
        }
        return properties;
    }
protected:
    template<class TPropertyType>
    static PropertyDefinition<TObject, TPropertyType> *RegisterProperty(ObjectProperty<TPropertyType> TObject::* property, std::string name, std::type_index type)  {
        auto definition = new PropertyDefinition<TObject, TPropertyType>(name, type, property);
        m_properties.insert(std::make_pair(name, definition));
        return definition;
    }
private:
    static std::map<std::string, BasePropertyDefinition*> m_properties;
};

template<typename TObject>
std::map<std::string, BasePropertyDefinition*> BaseObject<TObject>::m_properties = {};

/// Macros
#define SCHEMA_PROPERTY(parent, type, name)            \
public:                                                \
    ObjectProperty<type> (name);                       \
private:                                               \
    PropertyDefinition<parent, type> *name##_definition \
            = RegisterProperty(&parent::name, #name, std::type_index(typeid(type)));
