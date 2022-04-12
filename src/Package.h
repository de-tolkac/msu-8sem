#pragma once

#include "Product.h"

struct Package {
    Product product;

    int discountPercent;
    int quantity;
    int deliveryDate;

    Package() {}

    Package(const Product& product, int quantity, int deliveryDate) 
        : product(product)
        , discountPercent(0)
        , quantity(quantity)
        , deliveryDate(deliveryDate)
    {}

    void increaseDiscount() {
        if (discountPercent < 95) {
            discountPercent += 5;
        }
    }

    void decreaseDiscount() {
        if (discountPercent > 0) 
            discountPercent -= 5;
    }
};