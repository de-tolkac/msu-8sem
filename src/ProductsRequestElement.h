#pragma once

#include <vector>
#include <utility>
#include <string>

struct ProductsRequestElement {
    std::vector<std::pair<std::string, int>> products;

    ProductsRequestElement(int qnt = 0)
        : products(qnt, std::pair<std::string, int>())
    {}
};