#pragma once
#include "product.h"
#include <vector>
#include <string>
#include <mutex>

struct Product;
namespace crow { namespace json { struct wvalue; } }
crow::json::wvalue productToJson(const Product& p);

class Database {
public:
    Database(const std::string& db_path);
    ~Database();
    void init();
    void generateProducts(int count = 1000);
    std::vector<Product> getAllProducts();
    std::vector<Product> searchProducts(const std::string& query);
    std::vector<std::string> getCategories();
    std::vector<std::string> getBrands();
    crow::json::wvalue getFilters();
    std::vector<std::string> getSuggestions(const std::string& query);
private:
    void open();
    void close();
    void createTable();
    std::string db_path_;
    void* db_; // sqlite3*
    std::mutex mutex_;
};
