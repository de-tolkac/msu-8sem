#pragma once

#include "CompletedOrders.h"

struct CompletedOrdersList {
    std::vector<CompletedOrders> orders;

    void clear() {
        orders.clear();
    }
};
