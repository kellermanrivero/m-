//
// Created by Kellerman Rivero on 24/3/23.
//

#ifndef MPP_SAMPLE_META_MODEL_H
#define MPP_SAMPLE_META_MODEL_H

#include "../../metamodel.h"

struct SampleMetaModel: MetaModel<SampleMetaModel> {
    SCHEMA_PROPERTY(std::string, SchemaPropertyValueResolver, id);
    SCHEMA_PROPERTY(std::string, SchemaPropertyValueResolver, name);
    SCHEMA_PROPERTY(int, SchemaPropertyValueResolver, age);
};

#endif //MPP_SAMPLE_META_MODEL_H
