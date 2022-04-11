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

    std::unordered_map<std::string, double> prices;

    Experiment()
        : varsDefined(false)
        , currentDay(0)
        , profit(0)
        , loss(0)
    {
        srand(time(NULL));
    }

    Experiment(const std::string& productsFileName)
        : varsDefined(false)
        , currentDay(0)
        , profit(0)
        , warehouse(productsFileName)
    {
        srand(time(NULL));
    }

    Experiment(const std::string& productsFileName, int K, int M, int N, int minT, int maxT) 
        : productsNumber(K)
        , shopsNumber(M)
        , daysNumber(N)
        , minDeliveryTime(minT)
        , maxDeliveryTime(maxT)
        , warehouse(productsFileName)
        , currentDay(0)
        , profit(0)
        , varsDefined(true)
    {
        srand(time(NULL));
    }

    void nextDay() {
        if (currentDay != daysNumber && warehouse.generatedOrders) {
            ++currentDay;
            profit += warehouse.dayProfit;
            loss += warehouse.dayLoss;
            
            currentOrders.clear();
            warehouse.generatedOrders = false;

            warehouse.completedOrders.clear();

            warehouse.handleReplenishment(currentDay);

            warehouse.updateStorage();
        }
    }
    void skipDays() {
        while (currentDay != daysNumber) {
            generateOrders();
            nextDay();
        }
    }

    void generateOrders() {
        if (warehouse.generatedOrders)
            return;
        
        warehouse.generatedOrders = true;

        currentOrders.clear();

        std::vector<std::string> products;

        for (const auto& p : warehouse.storage) {
            int percent = p.second.first.discountPercent;
            while (percent >= 0) {
                products.push_back(p.first);
                percent -= 10;
            }
        }

        for (int i = 0; i < shopsNumber; ++i) {
            bool shopPlacesOrder = static_cast<bool>(rand() % 2);

            if (shopPlacesOrder) {
                int productsQnt = 1 + rand() % (int)products.size();

                random_unique(products.begin(), products.end(), productsQnt);

                ProductsRequestElement request;
                std::unordered_map<std::string, int> reqProducts;
                for (int i = 0; i < productsQnt; ++i) {
                    int qnt = 1 + rand() % maxRequestQuantity;
                    
                    reqProducts[products[i]] += qnt;
                }

                for (const auto& p : reqProducts) {
                    request.products.push_back(p);
                }

                currentOrders.addRequest(request);
            }
        }

        warehouse.handleOrders(currentOrders);

        warehouse.orderProductsFromSupplier(currentDay, minDeliveryTime, maxDeliveryTime, replenishmentSize);
    }

};