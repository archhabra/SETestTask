## Assignment for Dynamic Product Catalog Filter

A full-stack project with a C++ backend (Crow + SQLite) and a React frontend for a dynamic product catalog with search/filter functionality.

## Assumptions Made

- Product data format: Assumed standard e-commerce fields (name, description, category, brand, price, etc.) would be sufficient
- Search scope: Implemented search across name, description, and category only (not all fields like SKU, brand)
- Data volume: Optimized for 1000 products as specified, but architecture can scale

## Backend (C++ Crow + SQLite)

### Features
- REST API for product generation, retrieval, and search
- SQLite database for persistent storage
- CORS support for frontend integration
- Unit tests with Catch2

### Setup
1. **Dependencies:**
   - Crow (single header: `crow_all.h`)
   - SQLite3 (`libsqlite3-dev`)
   - Catch2 (single header: `catch.hpp`)
2. **Build:**
   ```bash
   cd backend
   g++ -std=c++17 -I. main.cpp database.cpp -lsqlite3 -lpthread -o server
   ```
3. **Run server:**
   ```bash
   ./server
   ```
   The server runs on port 8080.



### API Endpoints

- `POST /products/generate` : Generate random products
- `GET /products` : Get all products
- `GET /products/search?q=QUERY` : Search products by name/description

### Additional APIs/Features
- `GET /products/categories` : Get all product categories
- `GET /products/brands` :  Get all product brands
- `GET /products/filters` : Get available filters (categories/brands with counts)
- `GET /products/suggestions?q=QUERY` :  Get product name suggestions for autocomplete

### Improvements with More Time:

- Enhanced Search:

   Advanced filters (price range, categories, ratings)

- Performance Optimizations:

   1. Database indexing on searchable fields
   2. Query result caching (Redis)

- Better Architecture:

   1. Connection pooling for database
   2. Logging and monitoring

### Frontend

React app in `frontend/` folder. The frontend uses all backend endpoints for dynamic product filtering, category/brand selection, and search suggestions. See below for usage examples.

### Example Usage

- Product list: fetch `/products`
- Search: fetch `/products/search?q=QUERY`
- Categories: fetch `/products/categories`
- Brands: fetch `/products/brands`
- Filters: fetch `/products/filters`
- Suggestions: fetch `/products/suggestions?q=QUERY`

To run frontend:
```bash
cd frontend
npm install
npm start
```

To run backend:
```bash
cd backend
g++ -std=c++17 -I. main.cpp database.cpp -lsqlite3 -lpthread -o server
./server
```

### Testing
1. **Build and run tests:**
   ```bash
   g++ -std=c++17 -I. test_database.cpp database.cpp -lsqlite3 -o test_database
   ./test_database
   ```
   (Requires Catch2 header in `backend/catch.hpp`)

## Frontend (React)

### Features
- Product list table
- Search bar for dynamic filtering
- Fetches data from backend API

### Setup
1. **Install dependencies:**
   ```bash
   cd frontend
   npm install
   ```
2. **Run frontend:**
   ```bash
   npm start
   ```
   The app runs on [http://localhost:3000](http://localhost:3000)

### Notes
- Ensure the backend server is running on port 8080 before starting the frontend.
- If you change backend port, update API URLs in `frontend/src/App.js`.
- CORS headers are set in backend for frontend compatibility.

## Folder Structure
```
backend/
  main.cpp
  database.cpp
  database.h
  product.h
  crow_all.h
  catch.hpp
  products.db
  test_database.cpp
  ...
frontend/
  package.json
  src/
    App.js
    ...
```

## Authors
- Arti Chhabra

