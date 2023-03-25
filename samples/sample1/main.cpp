#include <iostream>
#include "sample_meta_model.h"

int main() {

    SampleMetaModel x = {};

    for(auto [name, definition]: SampleMetaModel::get_schema().get_properties()) {
        std::cout << "Found definition " << type_name(definition.get_type()) << " for property: " << name << std::endl;
    }

    for(auto property: x.get_properties()) {
        auto definition = property->get_definition();
        std::cout << "Property '" << definition.get_name() << "' is of type: " << type_name(definition.get_type()) << std::endl;
    }

    return 0;
}
