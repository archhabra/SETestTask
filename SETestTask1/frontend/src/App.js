
import React, { useState, useEffect } from 'react';
import './App.css';

function App() {
  const [products, setProducts] = useState([]);
  const [search, setSearch] = useState('');
  const [loading, setLoading] = useState(false);
  const [categories, setCategories] = useState([]);
  const [brands, setBrands] = useState([]);
  const [selectedCategory, setSelectedCategory] = useState('');
  const [selectedBrand, setSelectedBrand] = useState('');
  const [filters, setFilters] = useState({ categories: [], brands: [] });
  const [suggestions, setSuggestions] = useState([]);

  useEffect(() => {
    fetchProducts();
    fetchCategories();
    fetchBrands();
    fetchFilters();
  }, []);

  const fetchProducts = async (query = '', category = '', brand = '') => {
    setLoading(true);
    let url = 'http://localhost:8080/products';
    if (query) {
      url = `http://localhost:8080/products/search?q=${encodeURIComponent(query)}`;
    }
    try {
      const res = await fetch(url);
      let data = await res.json();
      if (category) {
        data = data.filter((p) => p.category === category);
      }
      if (brand) {
        data = data.filter((p) => p.brand === brand);
      }
      setProducts(data);
    } catch (err) {
      setProducts([]);
    }
    setLoading(false);
  };

  const fetchCategories = async () => {
    try {
      const res = await fetch('http://localhost:8080/products/categories');
      const data = await res.json();
      setCategories(data);
    } catch {}
  };

  const fetchBrands = async () => {
    try {
      const res = await fetch('http://localhost:8080/products/brands');
      const data = await res.json();
      setBrands(data);
    } catch {}
  };

  const fetchFilters = async () => {
    try {
      const res = await fetch('http://localhost:8080/products/filters');
      const data = await res.json();
      setFilters(data);
    } catch {}
  };

  const fetchSuggestions = async (query) => {
    if (!query) {
      setSuggestions([]);
      return;
    }
    try {
      const res = await fetch(`http://localhost:8080/products/suggestions?q=${encodeURIComponent(query)}`);
      const data = await res.json();
      setSuggestions(data);
    } catch {
      setSuggestions([]);
    }
  };
  const handleSearch = (e) => {
    const value = e.target.value;
    setSearch(value);
    fetchProducts(value, selectedCategory, selectedBrand);
    fetchSuggestions(value);
  };

  const handleCategoryChange = (e) => {
    setSelectedCategory(e.target.value);
    fetchProducts(search, e.target.value, selectedBrand);
  };

  const handleBrandChange = (e) => {
    setSelectedBrand(e.target.value);
    fetchProducts(search, selectedCategory, e.target.value);
  };

  return (
    <div className="App">
      <h1>Product Catalog</h1>
      <div style={{ marginBottom: '16px' }}>
        <input
          type="text"
          placeholder="Search products..."
          value={search}
          onChange={handleSearch}
          style={{ padding: '8px', width: '300px', marginRight: '16px' }}
          list="suggestions"
        />
        <datalist id="suggestions">
          {suggestions.map((s, idx) => (
            <option key={idx} value={s} />
          ))}
        </datalist>
        <select value={selectedCategory} onChange={handleCategoryChange} style={{ marginRight: '8px', padding: '8px' }}>
          <option value="">All Categories</option>
          {filters.categories && filters.categories.map((c, idx) => (
            <option key={idx} value={c.category}>{c.category} ({c.count})</option>
          ))}
        </select>
        <select value={selectedBrand} onChange={handleBrandChange} style={{ padding: '8px' }}>
          <option value="">All Brands</option>
          {filters.brands && filters.brands.map((b, idx) => (
            <option key={idx} value={b.brand}>{b.brand} ({b.count})</option>
          ))}
        </select>
      </div>
      {loading ? (
        <p>Loading...</p>
      ) : (
        <table style={{ width: '100%', borderCollapse: 'collapse' }}>
          <thead>
            <tr>
              <th>Name</th>
              <th>Category</th>
              <th>Brand</th>
              <th>Price</th>
              <th>Stock</th>
              <th>Status</th>
            </tr>
          </thead>
          <tbody>
            {products.map((p) => (
              <tr key={p.id}>
                <td>{p.name}</td>
                <td>{p.category}</td>
                <td>{p.brand}</td>
                <td>${p.price.toFixed(2)}</td>
                <td>{p.stock_quantity}</td>
                <td>{p.availability_status}</td>
              </tr>
            ))}
          </tbody>
        </table>
      )}
    </div>
  );
}

export default App;
