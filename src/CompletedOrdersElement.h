#pragma once

#include <string>

struct CompletedOrdersElement {
    std::string name;
    
    int requestedQuantity;
    int soldPackagesQuantity;

    double price;

    CompletedOrdersElement() {}

    CompletedOrdersElement(const std::string& name, int requestedQuantity) 
        : name(name)
        , requestedQuantity(requestedQuantity)
    {}
};