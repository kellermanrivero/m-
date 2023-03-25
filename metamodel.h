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

inline std::string type_name(const std::type_index & type) {
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

class SchemaPropertyDefinition {
public:
    explicit SchemaPropertyDefinition(
            std::string  name,
            const std::type_info & type,
            SchemaPropertyValueResolver *resolver):
        name_(std::move(name)), type_(type), resolver_(resolver) { };
    std::string get_name() { return name_; }
    std::type_index get_type() { return type_; }
    std::string get_type_name() { return type_.name(); }
private:
    std::string name_;
    std::type_index type_;
    SchemaPropertyValueResolver *resolver_;
};

class SchemaPropertyValue {
public:
    SchemaPropertyValue() = default;
};

/// Schema property definition
class SchemaProperty {
public:
    explicit SchemaProperty(const SchemaPropertyDefinition & definition):
            definition_(definition) {}

    SchemaPropertyValue get_value() {
        return value_;
    }

    SchemaPropertyDefinition get_definition() {
        return definition_;
    }

private:
    SchemaPropertyValue value_;
    SchemaPropertyDefinition definition_;
};

#pragma Schema Builder

typedef std::pair<std::string, SchemaPropertyDefinition> SchemaPropertyDefinitionPair;
typedef std::unordered_map<std::string, SchemaPropertyDefinition> SchemaDefinitionMap;
typedef std::pair<std::type_index, SchemaDefinitionMap> SchemaPair;
typedef std::unordered_map<std::type_index, SchemaDefinitionMap> Schemas;

template<typename TSchema>
class SchemaHolder {
public:
    SchemaHolder() = default;

    static SchemaProperty
            create_property(TSchema *schema, SchemaPropertyDefinition definition);

    static SchemaPropertyDefinition
            create_property_definition(
                const std::string & name,
                const std::type_info & type,
                SchemaPropertyValueResolver *resolver);

    static std::vector<SchemaPropertyDefinitionPair> get_properties() {
        std::vector<SchemaPropertyDefinitionPair> values;
        std::type_index schema_id(typeid(TSchema));
        auto schemas = *schemas_;
        auto size = schemas[schema_id].size();

        values.reserve(size);

        for(const auto& kv : schemas[schema_id]) {
            values.emplace_back(kv.first, kv.second);
        }

        return values;
    }
private:
    inline static std::unique_ptr<Schemas> schemas_ = {};
};

template<typename TSchema>
SchemaProperty
SchemaHolder<TSchema>::create_property(TSchema *schema, SchemaPropertyDefinition definition) {
    std::cout << "Creating an instance property: " << definition.get_name() << "(" << definition.get_type_name() << ")" << std::endl;
    auto property = std::make_shared<SchemaProperty>(definition);
    schema->register_property(property);
    return *property;
}


template<typename TSchema>
SchemaPropertyDefinition
SchemaHolder<TSchema>::create_property_definition(
        const std::string & name,
        const std::type_info & type,
        SchemaPropertyValueResolver *resolver) {
    if (schemas_ == nullptr) {
        schemas_ = std::make_unique<Schemas>();
    }

    SchemaPropertyDefinition definition(name, type, resolver);
    SchemaPropertyDefinitionPair definition_pair(name, definition);
    std::type_index schema_id(typeid(TSchema));

    auto it = schemas_->find(schema_id);
    if(it != schemas_->end()) {
        it->second.insert(definition_pair);
    } else {
        SchemaDefinitionMap definition_map {};
        definition_map.insert(definition_pair);
        SchemaPair schema_pair(schema_id, definition_map);
        schemas_->insert(schema_pair);
    }

    std::cout << "Registering a property definition: " << name <<  " => " << type_name(typeid(resolver)) << std::endl;
    return definition;
}

template<typename TSchema>
class MetaModel {
protected:
    inline static SchemaHolder<TSchema> schema_;
    std::vector<std::shared_ptr<SchemaProperty>> properties_;
public:
    static SchemaHolder<TSchema> get_schema() {
        return schema_;
    }

    std::vector<std::shared_ptr<SchemaProperty>> get_properties() {
        return properties_;
    }

    void register_property(const std::shared_ptr<SchemaProperty>& property) {
        properties_.push_back(property);
    }
};

/// Macros
#define SCHEMA_PROPERTY(type, resolver, name) \
    private:                                  \
inline static const SchemaPropertyDefinition  \
    name##_property_definition =              \
        schema_.create_property_definition(#name, typeid(type), new resolver()); \
public:                                       \
SchemaProperty                                \
    name =                                    \
        schema_.create_property(this, name##_property_definition);


#endif //MPP_METAMODEL_H

