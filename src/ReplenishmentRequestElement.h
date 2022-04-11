#pragma once

#include <string>

struct  ReplenishmentRequestElement {
    std::string name;
    int quantity;
    int deliveryDate;

    ReplenishmentRequestElement() {}

    ReplenishmentRequestElement(const std::string& name, int quantity, int deliveryDate)
        : name(name)
        , quantity(quantity)
        , deliveryDate(deliveryDate)
    {}
};
