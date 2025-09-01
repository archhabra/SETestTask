#include "crow_all.h"
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "database.h"
#include <cstdio>

struct DatabaseFixture {
    DatabaseFixture() {
        std::remove("test_products.db");
    }
    ~DatabaseFixture() {
        std::remove("test_products.db");
    }
};

TEST_CASE("GenerateProducts", "[database]") {
    DatabaseFixture fixture;
    Database db("test_products.db");
    db.init();
    db.generateProducts(10);
    auto products = db.getAllProducts();
    REQUIRE(products.size() == 10);
}

TEST_CASE("SearchProducts", "[database]") {
    DatabaseFixture fixture;
    Database db("test_products.db");
    db.init();
    db.generateProducts(5);
    auto products = db.searchProducts("Electronics");
    REQUIRE(products.size() > 0);
}


TEST_CASE("GetCategories", "[database]") {
    DatabaseFixture fixture;
    Database db("test_products.db");
    db.init();
    db.generateProducts(10);
    auto categories = db.getCategories();
    REQUIRE(categories.size() > 0);
}

TEST_CASE("GetBrands", "[database]") {
    DatabaseFixture fixture;
    Database db("test_products.db");
    db.init();
    // Generate enough products to ensure we get some brands
    db.generateProducts(20);
    auto brands = db.getBrands();
    REQUIRE(brands.size() > 0);
}

TEST_CASE("GetFilters", "[database]") {
    DatabaseFixture fixture;
    Database db("test_products.db");
    db.init();
    // Generate enough products to ensure we get both categories and brands
    db.generateProducts(20);
    auto filters = db.getFilters();
    REQUIRE(filters["categories"].size() > 0);
    REQUIRE(filters["brands"].size() > 0);
}

TEST_CASE("GetSuggestions", "[database]") {
    DatabaseFixture fixture;
    Database db("test_products.db");
    db.init();
    db.generateProducts(10);
    auto suggestions = db.getSuggestions("Product");
    REQUIRE(suggestions.size() > 0);
}


