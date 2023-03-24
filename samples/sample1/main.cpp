#include <iostream>
#include "sample_meta_model.h"

int main() {

    SampleMetaModel x = {};
    SampleMetaModel y = {};

    for(auto [name, resolver]: x.get_schema().get_properties()) {
        std::cout << "Found resolver of type" << typeid(resolver).name() << " for property: " << name << std::endl;
    }

    return 0;
}
