#pragma once
#include <string>
#include <vector>

class CProduct
{
public:
    CProduct();
    CProduct(const std::string& code, const std::string& name, double price, int quantity, double discount = 0.0);
    ~CProduct();

    // 访问器
    std::string GetCode() const { return m_strCode; }
    std::string GetName() const { return m_strName; }
    double GetPrice() const { return m_dPrice; }
    int GetQuantity() const { return m_nQuantity; }
    double GetDiscount() const { return m_dDiscount; }
    bool IsAvailable() const { return m_nQuantity > 0; }

    // 修改器
    void SetCode(const std::string& code) { m_strCode = code; }
    void SetName(const std::string& name) { m_strName = name; }
    void SetPrice(double price) { m_dPrice = price; }
    void SetQuantity(int quantity) { m_nQuantity = quantity; }
    void SetDiscount(double discount) { m_dDiscount = discount; }

    // 业务方法
    void ReduceQuantity(int count);
    double GetDiscountedPrice() const;
    std::string ToString() const;

private:
    std::string m_strCode;    // 条码
    std::string m_strName;    // 商品名称
    double m_dPrice;          // 价格
    int m_nQuantity;          // 库存数量
    double m_dDiscount;       // 折扣 (0.0 - 1.0)
};

class CProductManager
{
public:
    CProductManager();
    ~CProductManager();

    bool LoadFromFile(const std::string& filename);
    bool SaveToFile(const std::string& filename);
    
    std::vector<CProduct>& GetProducts() { return m_products; }
    CProduct* FindProductByCode(const std::string& code);
    std::vector<CProduct*> GetAvailableProducts();
    
    void AddProduct(const CProduct& product);
    void UpdateProduct(const CProduct& product);
    
private:
    std::vector<CProduct> m_products;
};