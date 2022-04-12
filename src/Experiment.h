#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "Warehouse.h"
#include "ProductsRequest.h"
#include "Random.h"

struct Experiment {
    Warehouse warehouse;
    
    ProductsRequest currentOrders;

    int productsNumber; // K
    int shopsNumber;    // M
    int daysNumber;     // N
    int minDeliveryTime, maxDeliveryTime;
    int maxRequestQuantity;
    int minPackagesNumber;
    int replenishmentSize;

    int currentDay;

    double profit;
    double loss;

    bool varsDefined;

    Experiment();
    Experiment(const std::string&);
    Experiment(const std::string&, int, int, int, int, int);

    void nextDay();
    void skipDays();
    void generateOrders();
};