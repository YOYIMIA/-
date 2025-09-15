#pragma once
#include "Product.h"
#include <vector>
#include <string>

class CShoppingItem
{
public:
    CShoppingItem(CProduct* product, int quantity);
    ~CShoppingItem();

    CProduct* GetProduct() const { return m_pProduct; }
    int GetQuantity() const { return m_nQuantity; }
    void SetQuantity(int quantity) { m_nQuantity = quantity; }
    double GetTotalPrice() const;

private:
    CProduct* m_pProduct;
    int m_nQuantity;
};

class CShoppingCart
{
public:
    CShoppingCart();
    ~CShoppingCart();

    void Clear();
    bool AddItem(CProduct* product, int quantity = 1);
    bool RemoveItem(const std::string& productCode);
    bool UpdateItemQuantity(const std::string& productCode, int quantity);
    
    std::vector<CShoppingItem>& GetItems() { return m_items; }
    double GetTotalAmount() const;
    int GetTotalItemCount() const;
    
    std::string GenerateReceipt() const;

private:
    std::vector<CShoppingItem> m_items;
};

class CSalesRecord
{
public:
    CSalesRecord();
    ~CSalesRecord();

    void AddSale(const CShoppingCart& cart);
    double GetTotalSales() const { return m_dTotalSales; }
    int GetTotalTransactions() const { return m_nTotalTransactions; }
    std::string GetSalesReport() const;
    
    bool SaveToFile(const std::string& filename);
    bool LoadFromFile(const std::string& filename);

private:
    double m_dTotalSales;
    int m_nTotalTransactions;
    std::vector<double> m_transactionAmounts;
};