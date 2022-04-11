#pragma once

#include <vector>

#include "ReplenishmentRequestElement.h"

struct ReplenishmentRequest {
    std::vector<ReplenishmentRequestElement> products;
};