#include <iostream>
#include "sample_meta_model.h"

int main() {

    SampleMetaModel x = {};
    x.name = "Kellerman Rivero";
    x.age = 33;
    x.id = "1";

    std::cout << "My ID is: " << x.id << std::endl;
    std::cout << "My name is: " << x.name << std::endl;
    std::cout << "My age is: " << x.age << std::endl;

    for(auto definition: x.GetProperties()) {
        //auto definition = property->get_definition();
        std::cout << "Property '" << definition->GetName() << "' is of type: " << definition->GetType() << std::endl;
    }

    return 0;
}
