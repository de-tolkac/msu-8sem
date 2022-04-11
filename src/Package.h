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
        , quantity(quantity)
        , deliveryDate(deliveryDate)
        , discountPercent(0)
    {}

    bool operator==(const Package &other) const { 
        return (quantity == other.quantity
            && deliveryDate == other.deliveryDate
            && product == other.product);
    }

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