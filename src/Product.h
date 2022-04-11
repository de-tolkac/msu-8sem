#pragma once

#include <string>

struct Product {
    std::string name;
    double price;
    int expDate;
    
    Product() {}

    Product(const std::string& name, double price, int expDate)
        : name(name)
        , price(price)
        , expDate(expDate)
    {}

    bool operator==(const Product &other) const { 
        return (name == other.name
                && price == other.price
                && expDate == other.expDate);
    }
};  