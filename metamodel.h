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

/// Context object passed across
class ResolutionContext {};

/// Resolver for schema properties
class SchemaPropertyValueResolver {
public:
    SchemaPropertyValueResolver() {}
};

#pragma Schema Property

/// Schema property definition
template<typename TSchema, typename TPropertyType>
class SchemaProperty {
public:
    SchemaProperty() { }
    TPropertyType getValue() {
        return value_;
    }
private:
    TPropertyType value_ = {};
};

template<typename TSchema, typename TResolver>
class SchemaPropertyDefinition {
public:
    SchemaPropertyDefinition() { }
};

#pragma Schema Builder

typedef std::unordered_map<std::string, std::shared_ptr<SchemaPropertyValueResolver>> SchemaPropertyValueResolverMap;

template<typename TSchema>
class SchemaHolder {
public:
    SchemaHolder() {}

    template<typename TPropertyType>
    static SchemaProperty<TSchema, TPropertyType>
            create_property(
                std::string name,
                std::string type);

    template<typename TPropertyType, typename TPropertyValueResolver>
    static SchemaPropertyDefinition<TSchema, TPropertyValueResolver>
            create_property_definition(
                std::string name);

    static std::vector<std::pair<std::string, SchemaPropertyValueResolver>> get_properties() {
        std::vector<std::pair<std::string, SchemaPropertyValueResolver>> vals;
        vals.reserve(s_properties_->size());

        for(const auto& kv : *s_properties_) {
            vals.push_back(std::make_pair(kv.first, *kv.second));
        }
        return vals;
    }
private:
    inline static std::unique_ptr<SchemaPropertyValueResolverMap> s_properties_ = {};
};


template<typename TSchema>
template<typename TPropertyType>
SchemaProperty<TSchema, TPropertyType> SchemaHolder<TSchema>::create_property(std::string name, std::string type) {
    std::cout << "Registering a property: " << name << " of type: " << type << std::endl;
    return SchemaProperty<TSchema, TPropertyType>();
}

template<typename TSchema>
template<typename TPropertyType, typename TPropertyValueResolver>
SchemaPropertyDefinition<TSchema, TPropertyValueResolver>
SchemaHolder<TSchema>::create_property_definition(std::string name) {
    if (s_properties_ == nullptr) {
        s_properties_ = std::make_unique<SchemaPropertyValueResolverMap>();
    }
    std::shared_ptr<TPropertyValueResolver> resolver = std::make_shared<TPropertyValueResolver>();
    std::pair<std::string, std::shared_ptr<TPropertyValueResolver>> pair = std::pair<std::string, std::shared_ptr<TPropertyValueResolver>>(name, resolver);
    s_properties_->insert(pair);
    std::cout << "Registering a property: " << name << " with resolver of type: " << typeid(TPropertyValueResolver).name() << std::endl;
    return SchemaPropertyDefinition<TSchema, TPropertyValueResolver>();
}


/// Macros
#define SCHEMA_PROPERTY(schema, type, resolver, name) \
    private:                                          \
inline static const SchemaPropertyDefinition<schema, resolver> \
    name##_property_resolver =                        \
        SchemaHolder<schema>::create_property_definition<type, resolver>(#name); \
public:                                               \
SchemaProperty<schema, type>                          \
    name =                                          \
        SchemaHolder<schema>::create_property<type>(#name, #type);

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

