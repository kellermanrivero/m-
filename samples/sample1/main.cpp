#include <iostream>
#include "sample_meta_model.h"

int main() {

    SampleMetaModel x = {};
    SampleMetaModel y = {};

    for(auto [name, definition]: x.get_schema().get_properties<SampleMetaModel>()) {
        std::cout << "Found definition " << definition.get_type_name() << " for property: " << name << std::endl;
    }

    for(auto property: y.get_properties()) {
        auto definition = property->get_definition();
        std::cout << "Property '" << definition.get_name() << "' is of type: " << type_name(definition.get_type()) << std::endl;
    }

    return 0;
}
