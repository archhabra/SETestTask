#include "database.h"
#include "crow_all.h"

int main() {
    Database db("products.db");
    db.init();
    crow::SimpleApp app;

    CROW_ROUTE(app, "/products/categories").methods("GET"_method)([&db](const crow::request& req){
        auto categories = db.getCategories();
        std::vector<crow::json::wvalue> arr;
        for (const auto& c : categories) arr.push_back(c);
        crow::json::wvalue result(arr);
        crow::response res(200, result);
        res.add_header("Access-Control-Allow-Origin", "*");
        return res;
    });

    CROW_ROUTE(app, "/products/brands").methods("GET"_method)([&db](const crow::request& req){
        auto brands = db.getBrands();
        std::vector<crow::json::wvalue> arr;
        for (const auto& b : brands) arr.push_back(b);
        crow::json::wvalue result(arr);
        crow::response res(200, result);
        res.add_header("Access-Control-Allow-Origin", "*");
        return res;
    });

    CROW_ROUTE(app, "/products/filters").methods("GET"_method)([&db](const crow::request& req){
        auto filters = db.getFilters();
        crow::response res(200, filters);
        res.add_header("Access-Control-Allow-Origin", "*");
        return res;
    });

    CROW_ROUTE(app, "/products/suggestions").methods("GET"_method)([&db](const crow::request& req){
        auto query = req.url_params.get("q");
        auto suggestions = db.getSuggestions(query ? query : "");
        std::vector<crow::json::wvalue> arr;
        for (const auto& s : suggestions) arr.push_back(s);
        crow::json::wvalue result(arr);
        crow::response res(200, result);
        res.add_header("Access-Control-Allow-Origin", "*");
        return res;
    });


    CROW_ROUTE(app, "/products/generate").methods("POST"_method)([&db](const crow::request& req){
        db.generateProducts(1000);
    crow::response res(200, "Products generated");
    res.add_header("Access-Control-Allow-Origin", "*");
    return res;
    });


    CROW_ROUTE(app, "/products").methods("GET"_method)([&db](const crow::request& req){
        auto products = db.getAllProducts();
        std::vector<crow::json::wvalue> arr;
        for (const auto& p : products) {
            arr.push_back(productToJson(p));
        }
    crow::response res(200, crow::json::wvalue(arr));
    res.add_header("Access-Control-Allow-Origin", "*");
    return res;
    });


    CROW_ROUTE(app, "/products/search").methods("GET"_method)([&db](const crow::request& req){
        auto query = req.url_params.get("q");
        auto products = db.searchProducts(query ? query : "");
        std::vector<crow::json::wvalue> arr;
        for (const auto& p : products) {
            arr.push_back(productToJson(p));
        }
    crow::response res(200, crow::json::wvalue(arr));
    res.add_header("Access-Control-Allow-Origin", "*");
    return res;
    });

    app.port(8080).multithreaded().run();
    return 0;
}
