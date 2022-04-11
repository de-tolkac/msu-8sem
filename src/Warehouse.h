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

    Warehouse(const std::string& file) 
        : productsFileName(file)
        , storage()
    {
        parseProductsFromFile();
    }

    void parseProductsFromFile() {
        std::ifstream file(productsFileName);

        std::string line, buf, productName;
        char c;
        int quantity, qntInPackage, expDate;
        double price;

        while (std::getline(file, line)) {
            std::istringstream tokenizer(line);

            std::getline(tokenizer, productName, '|');

            std::getline(tokenizer, buf, '|');
            quantity = std::stoi(buf);

            std::getline(tokenizer, buf, '|');
            qntInPackage = std::stoi(buf);
    
            std::getline(tokenizer, buf, '|');
            expDate = std::stoi(buf);

            std::getline(tokenizer, buf);
            price = std::stod(buf);

            storage[productName] = {Package(Product(productName, price, expDate), qntInPackage, 1), quantity};
        }

        file.close();

        storageBackup = storage;
    } 

    void handleOrders(const ProductsRequest& orders) {
        dayProfit = 0.0;
        dayLoss = 0.0;

        completedOrders.clear();

        storageBackup = storage;

        for (const auto& request : orders.products) {
            CompletedOrders order;

            for (const auto& product : request.products) {
                CompletedOrdersElement productOrder(product.first, product.second);

                int packageCapacity = storageBackup[product.first].first.quantity;
                int packagesAvailable = storageBackup[product.first].second;
                double productPrice = storageBackup[product.first].first.product.price
                                        - (storageBackup[product.first].first.product.price * storageBackup[product.first].first.discountPercent / 100.0);
                int needPackages = product.second / packageCapacity + (product.second % packageCapacity != 0);

                if (needPackages <= packagesAvailable) {
                    storageBackup[product.first].second -= needPackages;
                    productOrder.soldPackagesQuantity = needPackages;
                    productOrder.price = needPackages * productPrice * packageCapacity;                    
                } else {
                    storageBackup[product.first].second = 0;
                    productOrder.soldPackagesQuantity = packagesAvailable;
                    productOrder.price =  packagesAvailable * productPrice * packageCapacity;
                }

                dayProfit += productOrder.price;
                dayLoss += productOrder.price * storageBackup[product.first].first.discountPercent 
                            / (100 - storageBackup[product.first].first.discountPercent);
                            
                order.orders.push_back(productOrder);
            }

            completedOrders.orders.push_back(order);
        }
    }

    void updateStorage() {
        dayProfit = 0.0;
        dayLoss = 0.0;

        storage = storageBackup;
    }

    void orderProductsFromSupplier(int currentDay, int minDeliveryTime, int maxDeliveryTime, int replenishmentSize) {
        for (const auto& [name, package] : storageBackup) {
            if (package.second < minPackages) {
                int deliveryDate = currentDay + minDeliveryTime + rand() % ((maxDeliveryTime + 1) - minDeliveryTime);
                replenishment.products.push_back(ReplenishmentRequestElement(name, replenishmentSize, deliveryDate));
            }
        }
    }

    void handleReplenishment(int currentDay) {
        ReplenishmentRequest newReplenishment;
        for (const auto& rep : replenishment.products) {
            if (rep.deliveryDate == currentDay) {
                storageBackup[rep.name].second += rep.quantity;
            } else {
                newReplenishment.products.push_back(rep); 
            }
        }

        replenishment = newReplenishment;
    }
};

std::ostream& operator<<(std::ostream& out, const Warehouse& w) {
    for (const auto& p : w.storage) {
        out << p.first << " " << p.second.second << "\n";
    }

    return out;
}