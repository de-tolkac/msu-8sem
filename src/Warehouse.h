#pragma once

#include <unordered_map>
#include <string>
#include <fstream>
#include <sstream>
#include <utility>

#include "Package.h"
#include "ProductsRequest.h"
#include "CompletedOrdersList.h"
#include "ReplenishmentRequest.h"

struct Warehouse {
    std::unordered_map<std::string, std::pair<Package, int>> storage, storageBackup;
    
    CompletedOrdersList completedOrders;
    ReplenishmentRequest replenishment;

    double dayProfit;
    double dayLoss;

    int minPackages;

    bool generatedOrders;
    
    std::string productsFileName;

    Warehouse() {}
    Warehouse(const std::string&);

    void parseProductsFromFile();
    void handleOrders(const ProductsRequest&);
    void updateStorage();
    void orderProductsFromSupplier(int, int, int, int);
    void handleReplenishment(int);

    int checkReplenishment(const std::string& name);
};