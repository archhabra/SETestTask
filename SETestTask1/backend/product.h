#pragma once
#include <string>

struct Product {
    int id;
    std::string name;
    std::string description;
    std::string category;
    double price;
    int stock_quantity;
    std::string release_date;
    std::string availability_status;
    double customer_rating;
    std::string colors;
    std::string sizes;
};
