#include "pch.h"
#include "ShoppingCart.h"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <ctime>

CShoppingItem::CShoppingItem(CProduct* product, int quantity)
    : m_pProduct(product), m_nQuantity(quantity)
{
}

CShoppingItem::~CShoppingItem()
{
}

double CShoppingItem::GetTotalPrice() const
{
    if (m_pProduct)
        return m_pProduct->GetDiscountedPrice() * m_nQuantity;
    return 0.0;
}

CShoppingCart::CShoppingCart()
{
}

CShoppingCart::~CShoppingCart()
{
}

void CShoppingCart::Clear()
{
    m_items.clear();
}

bool CShoppingCart::AddItem(CProduct* product, int quantity)
{
    if (!product || quantity <= 0 || product->GetQuantity() < quantity)
        return false;

    // 检查是否已存在该商品
    for (auto& item : m_items)
    {
        if (item.GetProduct()->GetCode() == product->GetCode())
        {
            int newQuantity = item.GetQuantity() + quantity;
            if (product->GetQuantity() >= newQuantity)
            {
                item.SetQuantity(newQuantity);
                return true;
            }
            return false;
        }
    }

    // 添加新商品
    m_items.push_back(CShoppingItem(product, quantity));
    return true;
}

bool CShoppingCart::RemoveItem(const std::string& productCode)
{
    m_items.erase(
        std::remove_if(m_items.begin(), m_items.end(),
            [&productCode](const CShoppingItem& item) {
                return item.GetProduct()->GetCode() == productCode;
            }),
        m_items.end());
    return true;
}

bool CShoppingCart::UpdateItemQuantity(const std::string& productCode, int quantity)
{
    for (auto& item : m_items)
    {
        if (item.GetProduct()->GetCode() == productCode)
        {
            if (quantity <= 0)
            {
                RemoveItem(productCode);
                return true;
            }
            if (item.GetProduct()->GetQuantity() >= quantity)
            {
                item.SetQuantity(quantity);
                return true;
            }
            return false;
        }
    }
    return false;
}

double CShoppingCart::GetTotalAmount() const
{
    double total = 0.0;
    for (const auto& item : m_items)
    {
        total += item.GetTotalPrice();
    }
    return total;
}

int CShoppingCart::GetTotalItemCount() const
{
    int count = 0;
    for (const auto& item : m_items)
    {
        count += item.GetQuantity();
    }
    return count;
}

std::string CShoppingCart::GenerateReceipt() const
{
    std::stringstream ss;

    // 获取当前时间
    time_t now = time(0);
    struct tm timeinfo;
    char timeStr[100];

    if (localtime_s(&timeinfo, &now) == 0)
    {
        strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &timeinfo);
    }
    else
    {
        strcpy_s(timeStr, sizeof(timeStr), "Unknown Time");
    }

    // 收据头部
    ss << "=========== 购物收据 ===========\n";
    ss << "时间: " << timeStr << "\n";
    ss << "================================\n";

    // 表格标题行
    ss << "商品\t数量\t单价\t金额\n";
    ss << "--------------------------------\n";

    // 商品详情 - 修改对齐方式
    double totalOriginalAmount = 0.0;
    double totalDiscountAmount = 0.0;

    for (const auto& item : m_items)
    {
        CProduct* product = item.GetProduct();
        double itemOriginalPrice = product->GetPrice() * item.GetQuantity();
        double itemActualPrice = item.GetTotalPrice();
        double itemSavings = itemOriginalPrice - itemActualPrice;

        totalOriginalAmount += itemOriginalPrice;
        totalDiscountAmount += itemSavings;

        // 获取商品名称
        std::string productName = product->GetName();

        // 截断过长的商品名
        if (productName.length() > 8) {
            productName = productName.substr(0, 7) + "...";
        }

        // 使用更完全的格式化方式
        ss << std::left << std::setw(8) << productName;
        ss << std::left << std::setw(8) << item.GetQuantity();
        ss << std::left << std::setw(8) << std::fixed << std::setprecision(2) << product->GetDiscountedPrice();
        ss << std::left << std::setw(8) << std::fixed << std::setprecision(2) << itemActualPrice;

        ss << "\n";
    }

    ss << "--------------------------------\n";

    // 折扣详情
    bool hasDiscount = false;
    for (const auto& item : m_items)
    {
        if (item.GetProduct()->GetDiscount() > 0)
        {
            hasDiscount = true;
            break;
        }
    }

    if (hasDiscount)
    {
        ss << "折扣详情:\n";
        for (const auto& item : m_items)
        {
            CProduct* product = item.GetProduct();
            if (product->GetDiscount() > 0)
            {
                double itemSavings = (product->GetPrice() - product->GetDiscountedPrice()) * item.GetQuantity();
                ss << product->GetName() << " 折扣"
                    << std::fixed << std::setprecision(0) << (product->GetDiscount() * 100)
                    << "% 节省" << std::fixed << std::setprecision(2) << itemSavings << "元\n";
            }
        }
        ss << "--------------------------------\n";
    }

    ss << "================================\n";

    // 结算信息
    ss << "商品种类: " << m_items.size() << "种\t";
    ss << "商品总数: " << GetTotalItemCount() << "件\t";

    if (totalDiscountAmount > 0)
    {
        ss << "\n原始总价: " << std::fixed << std::setprecision(2) << totalOriginalAmount << "元\n\n";
        ss << "\t\t优惠总额: " << std::fixed << std::setprecision(2) << totalDiscountAmount << "元\t\t";
    }

    ss << "应付金额: " << std::fixed << std::setprecision(2) << GetTotalAmount() << "元\t";

    return ss.str();
}

CSalesRecord::CSalesRecord()
    : m_dTotalSales(0.0), m_nTotalTransactions(0)
{
}

CSalesRecord::~CSalesRecord()
{
}

void CSalesRecord::AddSale(const CShoppingCart& cart)
{
    double amount = cart.GetTotalAmount();
    m_dTotalSales += amount;
    m_nTotalTransactions++;
    m_transactionAmounts.push_back(amount);
}

std::string CSalesRecord::GetSalesReport() const
{
    std::stringstream ss;

    ss << "========== 销售数据统计 ==========\n";
    ss << "交易笔数: " << std::setw(8) << m_nTotalTransactions << "\n";
    ss << "销售额度: " << std::setw(8) << std::fixed << std::setprecision(2) << m_dTotalSales << " 元\n";

    if (m_nTotalTransactions > 0)
    {
        ss << "平均交易额: " << std::setw(8) << std::fixed << std::setprecision(2)
            << (m_dTotalSales / m_nTotalTransactions) << " 元\n";
    }

    ss << "==================================\n";

    return ss.str();
}

bool CSalesRecord::SaveToFile(const std::string& filename)
{
    std::ofstream file(filename);
    if (!file.is_open())
        return false;

    file << m_dTotalSales << std::endl;
    file << m_nTotalTransactions << std::endl;

    for (double amount : m_transactionAmounts)
    {
        file << amount << std::endl;
    }

    file.close();
    return true;
}

bool CSalesRecord::LoadFromFile(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
        return false;

    m_transactionAmounts.clear();

    if (!(file >> m_dTotalSales >> m_nTotalTransactions))
        return false;

    double amount;
    while (file >> amount)
    {
        m_transactionAmounts.push_back(amount);
    }

    file.close();
    return true;
}