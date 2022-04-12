#include "Warehouse.h"


Warehouse::Warehouse(const std::string& file) 
    : storage()
    , productsFileName(file)
{
    parseProductsFromFile();
}

void Warehouse::parseProductsFromFile() {
    std::ifstream file(productsFileName);

    std::string line, buf, productName;
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

void Warehouse::handleOrders(const ProductsRequest& orders) {
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

void Warehouse::updateStorage() {
    dayProfit = 0.0;
    dayLoss = 0.0;

    storage = storageBackup;
}

int Warehouse::checkReplenishment(const std::string& name) {
    for (int i = 0; i < (int)replenishment.products.size(); ++i) {
        if (replenishment.products[i].name == name)
            return i;
    }

    return -1;
}

void Warehouse::orderProductsFromSupplier(int currentDay, int minDeliveryTime, int maxDeliveryTime, int replenishmentSize) {
    for (const auto& [name, package] : storageBackup) {
        if (package.second < minPackages && checkReplenishment(name) == -1) {
            int deliveryDate = currentDay + minDeliveryTime + rand() % ((maxDeliveryTime + 1) - minDeliveryTime);
            replenishment.products.push_back(ReplenishmentRequestElement(name, replenishmentSize, deliveryDate));
        }
    }
}

void Warehouse::handleReplenishment(int currentDay) {
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