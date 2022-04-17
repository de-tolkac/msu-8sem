#include "Experiment.h"

Experiment::Experiment()
    : currentDay(0)
    , profit(0)
    , loss(0)
    , varsDefined(false)
{
    srand(time(NULL));
}

Experiment::Experiment(const std::string& productsFileName)
    : warehouse(productsFileName)
    , currentDay(0)
    , profit(0)
    , loss(0)
    , varsDefined(false)
{
    srand(time(NULL));
}

Experiment::Experiment(const std::string& productsFileName, int K, int M, int N, int minT, int maxT) 
    : warehouse(productsFileName)
    , productsNumber(K)
    , shopsNumber(M)
    , daysNumber(N)
    , minDeliveryTime(minT)
    , maxDeliveryTime(maxT)
    , currentDay(0)
    , profit(0)
    , loss(0)
    , varsDefined(true)
{
    srand(time(NULL));
}

void Experiment::nextDay() {
    if (currentDay != daysNumber && warehouse.generatedOrders) {
        ++currentDay;
        
        currentOrders.clear();

        warehouse.generatedOrders = false;

        warehouse.completedOrders.clear();

        generateOrders();

        profit += warehouse.dayProfit;
        loss += warehouse.dayLoss;
        
        warehouse.handleReplenishment(currentDay);

        warehouse.updateStorage();
    }
}
void Experiment::skipDays() {
    while (currentDay != daysNumber) {
        nextDay();
    }
}

void Experiment::generateOrders() {
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