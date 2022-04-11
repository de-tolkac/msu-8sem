#pragma once

#include <vector>

#include "ProductsRequestElement.h"


struct ProductsRequest {
    std::vector<ProductsRequestElement> products;

    void clear() {
        products.clear();
    }

    size_t size() {
        return products.size();
    }
    
    void addRequest(const ProductsRequestElement& req) {
        products.push_back(req);
    }
};