#include "crow_all.h"
#include "database.h"
#include "product.h"
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <sqlite3.h>
std::vector<std::string> Database::getCategories() {
    std::lock_guard<std::mutex> lock(mutex_);
    open();
    std::vector<std::string> categories;
    const char* sql = "SELECT DISTINCT category FROM products WHERE category IS NOT NULL AND category != '';";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2((sqlite3*)db_, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const unsigned char* category = sqlite3_column_text(stmt, 0);
            if (category) {
                categories.emplace_back(reinterpret_cast<const char*>(category));
            }
        }
        sqlite3_finalize(stmt);
        stmt = nullptr;
    }
    close();
    return categories;
}

std::vector<std::string> Database::getBrands() {
    std::lock_guard<std::mutex> lock(mutex_);
    open();
    std::vector<std::string> brands;
    const char* sql = "SELECT DISTINCT brand FROM products WHERE brand IS NOT NULL AND brand != '';";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2((sqlite3*)db_, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const unsigned char* brand = sqlite3_column_text(stmt, 0);
            if (brand) {
                brands.emplace_back(reinterpret_cast<const char*>(brand));
            }
        }
        sqlite3_finalize(stmt);
        stmt = nullptr;
    }
    close();
    return brands;
}

std::vector<Product> Database::searchProducts(const std::string& query) {
    std::lock_guard<std::mutex> lock(mutex_);
    open();
    std::vector<Product> products;
    
    const char* sql = "SELECT id, name, description, category, price, stock_quantity, "
                     "release_date, availability_status, customer_rating, colors, sizes "
                     "FROM products WHERE name LIKE ? OR description LIKE ? OR category LIKE ?;";
    
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2((sqlite3*)db_, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        std::string pattern = "%" + query + "%";
        sqlite3_bind_text(stmt, 1, pattern.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, pattern.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, pattern.c_str(), -1, SQLITE_TRANSIENT);
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Product p;
            p.id = sqlite3_column_int(stmt, 0);
            
            const unsigned char* name = sqlite3_column_text(stmt, 1);
            p.name = name ? reinterpret_cast<const char*>(name) : "";
            
            const unsigned char* desc = sqlite3_column_text(stmt, 2);
            p.description = desc ? reinterpret_cast<const char*>(desc) : "";
            
            const unsigned char* cat = sqlite3_column_text(stmt, 3);
            p.category = cat ? reinterpret_cast<const char*>(cat) : "";
            
            p.price = sqlite3_column_double(stmt, 4);
            p.stock_quantity = sqlite3_column_int(stmt, 5);
            
            const unsigned char* date = sqlite3_column_text(stmt, 6);
            p.release_date = date ? reinterpret_cast<const char*>(date) : "";
            
            const unsigned char* status = sqlite3_column_text(stmt, 7);
            p.availability_status = status ? reinterpret_cast<const char*>(status) : "";
            
            p.customer_rating = sqlite3_column_double(stmt, 8);
            
            const unsigned char* colors = sqlite3_column_text(stmt, 9);
            p.colors = colors ? reinterpret_cast<const char*>(colors) : "";
            
            const unsigned char* sizes = sqlite3_column_text(stmt, 10);
            p.sizes = sizes ? reinterpret_cast<const char*>(sizes) : "";
            
            products.push_back(std::move(p));
        }
        sqlite3_finalize(stmt);
    }
    close();
    return products;
}

crow::json::wvalue productToJson(const Product& p) {
    crow::json::wvalue x;
    x["id"] = p.id;
    x["name"] = p.name;
    x["description"] = p.description;
    x["category"] = p.category;
    x["price"] = p.price;
    x["stock_quantity"] = p.stock_quantity;
    x["release_date"] = p.release_date;
    x["availability_status"] = p.availability_status;
    x["customer_rating"] = p.customer_rating;
    x["colors"] = p.colors;
    x["sizes"] = p.sizes;
    return x;
}
#include "database.h"
#include <sqlite3.h>
#include <random>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include "crow_all.h"
#include <vector>
#include <string>

Database::Database(const std::string& db_path) : db_path_(db_path), db_(nullptr) {}
Database::~Database() { close(); }

void Database::open() {
    if (sqlite3_open(db_path_.c_str(), (sqlite3**)&db_) != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg((sqlite3*)db_) << std::endl;
    }
}

void Database::close() {
    if (db_) sqlite3_close((sqlite3*)db_);
    db_ = nullptr;
}

void Database::init() {
    std::lock_guard<std::mutex> lock(mutex_);
    open();
    createTable();
    close();
}

void Database::createTable() {
    const char* sql = "CREATE TABLE IF NOT EXISTS products ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "description TEXT,"
        "category TEXT,"
        "brand TEXT,"
        "sku TEXT,"
        "price REAL DEFAULT 0.0,"
        "stock_quantity INTEGER DEFAULT 0,"
        "release_date TEXT,"
        "availability_status TEXT,"
        "customer_rating REAL DEFAULT 0.0,"
        "colors TEXT,"
        "sizes TEXT"
        ");";
    
    char* errMsg = nullptr;
    if (sqlite3_exec((sqlite3*)db_, sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Create table error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
}

void Database::generateProducts(int count) {
    std::lock_guard<std::mutex> lock(mutex_);
    open();
    const char* sql = "INSERT INTO products ("
        "name, description, category, brand, price, stock_quantity, "
        "release_date, availability_status, customer_rating, colors, sizes"
        ") VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
        
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2((sqlite3*)db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg((sqlite3*)db_) << std::endl;
        return;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> price_dist(10.0, 1000.0);
    std::uniform_int_distribution<> stock_dist(0, 500);
    std::uniform_real_distribution<> rating_dist(1.0, 5.0);
    
    const char* categories[] = {"Electronics", "Books", "Clothing", "Home", "Toys"};
    const char* brands[] = {"Apple", "Samsung", "Sony", "Dell", "LG"};
    const char* statuses[] = {"Available", "Out of Stock", "Preorder"};
    const char* colors = "Red,Blue,Green";
    const char* sizes = "S,M,L,XL";

    for (int i = 0; i < count; ++i) {
        std::string name = "Product " + std::to_string(i);
        std::string description = "Description for product " + std::to_string(i);
        
        double price = price_dist(gen);
        int stock = stock_dist(gen);
        double rating = rating_dist(gen);
        
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, description.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, categories[i % 5], -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, brands[i % 5], -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 5, price);
        sqlite3_bind_int(stmt, 6, stock);
        sqlite3_bind_text(stmt, 7, "2025-09-01", -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 8, statuses[i % 3], -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 9, rating);
        sqlite3_bind_text(stmt, 10, colors, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 11, sizes, -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::cerr << "Insert error: " << sqlite3_errmsg((sqlite3*)db_) << std::endl;
        }
        
        sqlite3_reset(stmt);
        sqlite3_clear_bindings(stmt);
    }
    
    sqlite3_finalize(stmt);
    close();
}

std::vector<Product> Database::getAllProducts() {
    std::lock_guard<std::mutex> lock(mutex_);
    open();
    std::vector<Product> products;
    const char* sql = "SELECT id, name, description, category, price, stock_quantity, "
                     "release_date, availability_status, customer_rating, colors, sizes "
                     "FROM products;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2((sqlite3*)db_, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Product p;
            p.id = sqlite3_column_int(stmt, 0);
            
            const unsigned char* name = sqlite3_column_text(stmt, 1);
            p.name = name ? reinterpret_cast<const char*>(name) : "";
            
            const unsigned char* desc = sqlite3_column_text(stmt, 2);
            p.description = desc ? reinterpret_cast<const char*>(desc) : "";
            
            const unsigned char* cat = sqlite3_column_text(stmt, 3);
            p.category = cat ? reinterpret_cast<const char*>(cat) : "";
            
            p.price = sqlite3_column_double(stmt, 4);
            p.stock_quantity = sqlite3_column_int(stmt, 5);
            
            const unsigned char* date = sqlite3_column_text(stmt, 6);
            p.release_date = date ? reinterpret_cast<const char*>(date) : "";
            
            const unsigned char* status = sqlite3_column_text(stmt, 7);
            p.availability_status = status ? reinterpret_cast<const char*>(status) : "";
            
            p.customer_rating = sqlite3_column_double(stmt, 8);
            
            const unsigned char* colors = sqlite3_column_text(stmt, 9);
            p.colors = colors ? reinterpret_cast<const char*>(colors) : "";
            
            const unsigned char* sizes = sqlite3_column_text(stmt, 10);
            p.sizes = sizes ? reinterpret_cast<const char*>(sizes) : "";
            
            products.push_back(std::move(p));
        }
        sqlite3_finalize(stmt);
        stmt = nullptr;
    }
    close();
    return products;
}

crow::json::wvalue Database::getFilters() {
    open();
    crow::json::wvalue filters;
    sqlite3_stmt* stmt = nullptr;

    // Categories with counts
    const char* cat_sql = "SELECT COALESCE(category, '') as category, COUNT(*) FROM products WHERE category IS NOT NULL AND category != '' GROUP BY category;";
    if (sqlite3_prepare_v2((sqlite3*)db_, cat_sql, -1, &stmt, nullptr) == SQLITE_OK) {
        std::vector<crow::json::wvalue> cat_arr;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const char* category = (const char*)sqlite3_column_text(stmt, 0);
            if (category && *category) {  // Make sure category is not NULL or empty
                crow::json::wvalue item;
                item["category"] = category;
                item["count"] = sqlite3_column_int(stmt, 1);
                cat_arr.push_back(std::move(item));
            }
        }
        filters["categories"] = crow::json::wvalue(cat_arr);
        sqlite3_finalize(stmt);
        stmt = nullptr;
    }

    // Brands with counts
    const char* brand_sql = "SELECT COALESCE(brand, '') as brand, COUNT(*) FROM products WHERE brand IS NOT NULL AND brand != '' GROUP BY brand;";
    if (sqlite3_prepare_v2((sqlite3*)db_, brand_sql, -1, &stmt, nullptr) == SQLITE_OK) {
        std::vector<crow::json::wvalue> brand_arr;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const char* brand = (const char*)sqlite3_column_text(stmt, 0);
            if (brand && *brand) {  // Make sure brand is not NULL or empty
                crow::json::wvalue item;
                item["brand"] = brand;
                item["count"] = sqlite3_column_int(stmt, 1);
                brand_arr.push_back(std::move(item));
            }
        }
        filters["brands"] = crow::json::wvalue(brand_arr);
        sqlite3_finalize(stmt);
    }

    close();
    return filters;
}

std::vector<std::string> Database::getSuggestions(const std::string& query) {
    std::lock_guard<std::mutex> lock(mutex_);
    open();
    std::vector<std::string> suggestions;
    
    const char* sql = "SELECT name FROM products WHERE name LIKE ? LIMIT 10;";
    sqlite3_stmt* stmt = nullptr;
    
    if (sqlite3_prepare_v2((sqlite3*)db_, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        std::string pattern = "%" + query + "%";
        sqlite3_bind_text(stmt, 1, pattern.c_str(), -1, SQLITE_TRANSIENT);
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const unsigned char* name = sqlite3_column_text(stmt, 0);
            if (name) {
                suggestions.emplace_back(reinterpret_cast<const char*>(name));
            }
        }
        sqlite3_finalize(stmt);
        stmt = nullptr;
    }
    close();
    return suggestions;
}


