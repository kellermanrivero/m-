//
// Created by Kellerman Rivero on 24/3/23.
//

#ifndef MPP_METAMODEL_H
#define MPP_METAMODEL_H

#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include <optional>
#include <type_traits>
#include <typeinfo>
#include <typeindex>
#include <cxxabi.h>

inline std::string type_name(const std::type_info & type) {
    int status = 0;

    std::unique_ptr<char, void(*)(void*)> res {
            abi::__cxa_demangle(type.name(), nullptr, 0, &status),
            std::free
    };

    if (status != 0) throw status; // stub

    return res.get();
}

/// Context object passed across
class ResolutionContext {};

/// Resolver for schema properties
class SchemaPropertyValueResolver {
public:
    SchemaPropertyValueResolver() = default;
};

#pragma Schema Property

/// Schema property definition
template<typename TSchema, typename TPropertyType>
class SchemaProperty {
public:
    SchemaProperty(std::string name, const std::type_info & type):
        name_(std::move(name)),
        type_(type) {
    }

    TPropertyType getValue() {
        return value_;
    }
private:
    TPropertyType value_ = {};
    std::string name_;
    std::type_index type_;
};

template<typename TSchema, typename TResolver>
class SchemaPropertyDefinition {
public:
    SchemaPropertyDefinition() = default;
};

#pragma Schema Builder

typedef std::unordered_map<std::string, std::shared_ptr<SchemaPropertyValueResolver>> SchemaPropertyValueResolverMap;
typedef std::unordered_map<std::string, const std::type_info &> SchemaPropertyTypeMap;

template<typename TSchema>
class SchemaHolder {
public:
    SchemaHolder() = default;

    template<typename TPropertyType>
    static SchemaProperty<TSchema, TPropertyType>
            create_property(
                std::string name,
                const std::type_info & type);

    template<typename TPropertyType, typename TPropertyValueResolver>
    static SchemaPropertyDefinition<TSchema, TPropertyValueResolver>
            create_property_definition(
                std::string name,
                const std::type_info & type);

    static std::vector<std::pair<std::string, SchemaPropertyValueResolver>> get_properties() {
        std::vector<std::pair<std::string, SchemaPropertyValueResolver>> vals;
        vals.reserve(s_properties_->size());

        for(const auto& kv : *s_properties_) {
            vals.emplace_back(kv.first, *kv.second);
        }
        return vals;
    }
private:
    inline static std::unique_ptr<SchemaPropertyValueResolverMap> s_properties_ = {};
    inline static std::unique_ptr<SchemaPropertyTypeMap> s_properties_types = {};
};


template<typename TSchema>
template<typename TPropertyType>
SchemaProperty<TSchema, TPropertyType> SchemaHolder<TSchema>::create_property(std::string name, const std::type_info & type) {
    std::cout << "Creating an instance property: " << name << " of type: " << type_name(type) << std::endl;
    return SchemaProperty<TSchema, TPropertyType>(name, type);
}

template<typename TSchema>
template<typename TPropertyType, typename TPropertyValueResolver>
SchemaPropertyDefinition<TSchema, TPropertyValueResolver>
SchemaHolder<TSchema>::create_property_definition(std::string name, const std::type_info & type) {
    if (s_properties_ == nullptr) {
        s_properties_ = std::make_unique<SchemaPropertyValueResolverMap>();
    }

    if (s_properties_types == nullptr) {
        s_properties_types = std::make_unique<SchemaPropertyTypeMap>();
    }

    std::shared_ptr<TPropertyValueResolver> resolver = std::make_shared<TPropertyValueResolver>();
    std::pair<std::string, std::shared_ptr<TPropertyValueResolver>> resolver_pair = std::pair<std::string, std::shared_ptr<TPropertyValueResolver>>(name, resolver);

    std::pair<std::string, const std::type_info &> type_pair = std::pair<std::string, const std::type_info &>(name, type);

    s_properties_->insert(resolver_pair);
    s_properties_types->insert(type_pair);

    std::cout << "Registering a property: " << name << "(" << type_name(type)  << ")" <<  " with resolver of type: " << typeid(TPropertyValueResolver).name() << std::endl;
    return SchemaPropertyDefinition<TSchema, TPropertyValueResolver>();
}


/// Macros
#define SCHEMA_PROPERTY(schema, type, resolver, name) \
    private:                                          \
inline static const SchemaPropertyDefinition<schema, resolver> \
    name##_property_resolver =                        \
        SchemaHolder<schema>::create_property_definition<type, resolver>(#name, typeid(type)); \
public:                                               \
SchemaProperty<schema, type>                          \
    name =                                          \
        SchemaHolder<schema>::create_property<type>(#name, typeid(type));

template<typename TSchema>
class MetaModel {
private:
    inline static SchemaHolder<TSchema> schema_;
public:
    SchemaHolder<TSchema> get_schema() {
        return schema_;
    }
};
#endif //MPP_METAMODEL_H

