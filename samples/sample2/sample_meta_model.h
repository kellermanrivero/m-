// Copyright (c) 2023. Kellerman Rivero

#pragma once
#include "metamodel.h"

struct SampleMetaModel: BaseObject<SampleMetaModel> {
    SCHEMA_PROPERTY(SampleMetaModel, std::string, id);
    SCHEMA_PROPERTY(SampleMetaModel, std::string, name);
    SCHEMA_PROPERTY(SampleMetaModel, int, age);
};
