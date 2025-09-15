#include "pch.h"
#include "Product.h"
#include <fstream>
#include <sstream>
#include <iomanip>

CProduct::CProduct()
    : m_strCode(""), m_strName(""), m_dPrice(0.0), m_nQuantity(0), m_dDiscount(0.0)
{
}

CProduct::CProduct(const std::string& code, const std::string& name, double price, int quantity, double discount)
    : m_strCode(code), m_strName(name), m_dPrice(price), m_nQuantity(quantity), m_dDiscount(discount)
{
}

CProduct::~CProduct()
{
}

void CProduct::ReduceQuantity(int count)
{
    if (count <= m_nQuantity)
    {
        m_nQuantity -= count;
    }
}

double CProduct::GetDiscountedPrice() const
{
    return m_dPrice * (1.0 - m_dDiscount);
}

std::string CProduct::ToString() const
{
    std::stringstream ss;
    ss << "代码: " << m_strCode << ", 名称: " << m_strName 
       << ", 价格: " << std::fixed << std::setprecision(2) << m_dPrice
       << ", 库存: " << m_nQuantity;
    if (m_dDiscount > 0)
    {
        ss << ", 折扣: " << (m_dDiscount * 100) << "%";
    }
    return ss.str();
}

CProductManager::CProductManager()
{
}

CProductManager::~CProductManager()
{
}

bool CProductManager::LoadFromFile(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        // 如果文件不存在，创建默认商品数据 - 调整为更实际的价格
        m_products.clear();
        
        // 水果类 (001-012) - 按公斤计价
        m_products.push_back(CProduct("001", "苹果", 8.98, 100, 0.1));      // 苹果 8.98元/公斤，打9折
        m_products.push_back(CProduct("002", "香蕉", 6.80, 80, 0.0));       // 香蕉 6.80元/公斤
        m_products.push_back(CProduct("003", "橙子", 7.50, 60, 0.0));       // 橙子 7.50元/公斤
        m_products.push_back(CProduct("004", "葡萄", 15.80, 45, 0.05));     // 葡萄 15.80元/公斤，打95折
        m_products.push_back(CProduct("005", "芒果", 12.90, 30, 0.0));      // 芒果 12.90元/公斤
        m_products.push_back(CProduct("006", "梨子", 4.80, 25, 0.0));       // 梨子 4.80元/公斤
        m_products.push_back(CProduct("007", "草莓", 28.80, 35, 0.0));      // 草莓 28.80元/公斤
        m_products.push_back(CProduct("008", "猕猴桃", 18.60, 30, 0.0));    // 猕猴桃 18.60元/公斤
        m_products.push_back(CProduct("009", "柠檬", 9.80, 50, 0.0));       // 柠檬 9.80元/公斤
        m_products.push_back(CProduct("010", "西瓜", 3.20, 40, 0.0));       // 西瓜 3.20元/公斤
        m_products.push_back(CProduct("011", "哈密瓜", 8.50, 30, 0.0));     // 哈密瓜 8.50元/公斤
        m_products.push_back(CProduct("012", "火龙果", 11.80, 35, 0.0));    // 火龙果 11.80元/公斤
        
        // 奶制品类 (101-108) - 按袋/盒计价
        m_products.push_back(CProduct("101", "牛奶", 4.50, 50, 0.0));       // 牛奶 4.50元/袋
        m_products.push_back(CProduct("102", "酸奶", 6.80, 40, 0.1));       // 酸奶 6.80元/杯，打9折
        m_products.push_back(CProduct("103", "奶酪", 15.80, 20, 0.0));      // 奶酪 15.80元/盒
        m_products.push_back(CProduct("104", "黄油", 22.50, 30, 0.0));      // 黄油 22.50元/盒
        m_products.push_back(CProduct("105", "酸奶", 3.80, 35, 0.0));       // 小杯酸奶 3.80元/杯
        m_products.push_back(CProduct("106", "奶粉", 58.00, 15, 0.2));      // 奶粉 58.00元/罐，打8折
        m_products.push_back(CProduct("107", "芝士片", 12.90, 25, 0.0));    // 芝士片 12.90元/盒
        m_products.push_back(CProduct("108", "奶昔粉", 18.60, 20, 0.0));    // 奶昔粉 18.60元/盒
        
        // 主食类 (201-210) - 按包/袋计价
        m_products.push_back(CProduct("201", "面包", 5.20, 40, 0.0));       // 面包 5.20元/袋
        m_products.push_back(CProduct("202", "馒头", 2.80, 60, 0.0));       // 馒头 2.80元/袋
        m_products.push_back(CProduct("203", "大米", 6.50, 80, 0.0));       // 大米 6.50元/公斤
        m_products.push_back(CProduct("204", "面条", 4.20, 35, 0.0));       // 面条 4.20元/袋
        m_products.push_back(CProduct("205", "面粉", 3.80, 40, 0.0));       // 面粉 3.80元/公斤
        m_products.push_back(CProduct("206", "挂面", 3.50, 100, 0.0));      // 挂面 3.50元/袋
        m_products.push_back(CProduct("207", "全麦面包", 8.80, 25, 0.0));   // 全麦面包 8.80元/袋
        m_products.push_back(CProduct("208", "吐司", 6.90, 20, 0.0));       // 吐司 6.90元/袋
        m_products.push_back(CProduct("209", "包子", 3.50, 50, 0.0));       // 包子 3.50元/个
        m_products.push_back(CProduct("210", "花卷", 2.80, 30, 0.0));       // 花卷 2.80元/个
        
        // 生鲜肉类 (301-310) - 按公斤计价
        m_products.push_back(CProduct("301", "猪肉", 28.80, 30, 0.0));      // 猪肉 28.80元/公斤
        m_products.push_back(CProduct("302", "牛肉", 68.00, 25, 0.0));      // 牛肉 68.00元/公斤
        m_products.push_back(CProduct("303", "羊肉", 58.60, 20, 0.0));      // 羊肉 58.60元/公斤
        m_products.push_back(CProduct("304", "鸡肉", 15.80, 15, 0.0));      // 鸡肉 15.80元/公斤
        m_products.push_back(CProduct("305", "鸭肉", 18.60, 20, 0.0));      // 鸭肉 18.60元/公斤
        m_products.push_back(CProduct("306", "鸡蛋", 9.80, 50, 0.0));       // 鸡蛋 9.80元/公斤
        m_products.push_back(CProduct("307", "鸭蛋", 12.80, 18, 0.0));      // 鸭蛋 12.80元/公斤
        m_products.push_back(CProduct("308", "虾", 88.00, 12, 0.0));        // 虾 88.00元/公斤
        m_products.push_back(CProduct("309", "鱼肉", 25.80, 22, 0.0));      // 鱼肉 25.80元/公斤
        m_products.push_back(CProduct("310", "香肠", 35.60, 35, 0.0));      // 香肠 35.60元/公斤
        
        // 蔬菜类 (401-412) - 按公斤计价
        m_products.push_back(CProduct("401", "白菜", 2.80, 50, 0.0));       // 白菜 2.80元/公斤
        m_products.push_back(CProduct("402", "萝卜", 3.20, 45, 0.0));       // 萝卜 3.20元/公斤
        m_products.push_back(CProduct("403", "土豆", 4.50, 60, 0.0));       // 土豆 4.50元/公斤
        m_products.push_back(CProduct("404", "洋葱", 3.80, 35, 0.0));       // 洋葱 3.80元/公斤
        m_products.push_back(CProduct("405", "黄瓜", 5.80, 40, 0.0));       // 黄瓜 5.80元/公斤
        m_products.push_back(CProduct("406", "茄子", 6.50, 45, 0.0));       // 茄子 6.50元/公斤
        m_products.push_back(CProduct("407", "青椒", 8.50, 35, 0.0));       // 青椒 8.50元/公斤
        m_products.push_back(CProduct("408", "西红柿", 7.80, 35, 0.0));     // 西红柿 7.80元/公斤
        m_products.push_back(CProduct("409", "韭菜", 6.50, 30, 0.0));       // 韭菜 6.50元/公斤
        m_products.push_back(CProduct("410", "菠菜", 5.20, 40, 0.0));       // 菠菜 5.20元/公斤
        m_products.push_back(CProduct("411", "芹菜", 4.80, 25, 0.0));       // 芹菜 4.80元/公斤
        m_products.push_back(CProduct("412", "豆角", 9.80, 20, 0.0));       // 豆角 9.80元/公斤
        
        // 饮料类 (501-512) - 按瓶/罐计价
        m_products.push_back(CProduct("501", "矿泉水", 2.00, 0, 0.0));      // 矿泉水 2.00元/瓶 (库存为0)
        m_products.push_back(CProduct("502", "可乐", 3.50, 80, 0.0));       // 可乐 3.50元/瓶
        m_products.push_back(CProduct("503", "橙汁", 5.80, 40, 0.0));       // 橙汁 5.80元/瓶
        m_products.push_back(CProduct("504", "绿茶", 4.20, 55, 0.0));       // 绿茶 4.20元/瓶
        m_products.push_back(CProduct("505", "咖啡", 8.90, 45, 0.0));       // 咖啡 8.90元/瓶
        m_products.push_back(CProduct("506", "牛奶", 6.80, 35, 0.0));       // 盒装牛奶 6.80元/盒
        m_products.push_back(CProduct("507", "果汁", 9.20, 40, 0.0));       // 果汁 9.20元/瓶
        m_products.push_back(CProduct("508", "绿茶", 8.50, 30, 0.0));       // 高级绿茶 8.50元/瓶
        m_products.push_back(CProduct("509", "汽水", 4.50, 50, 0.0));       // 汽水 4.50元/瓶
        m_products.push_back(CProduct("510", "柠檬水", 6.80, 60, 0.0));     // 柠檬水 6.80元/瓶
        m_products.push_back(CProduct("511", "运动饮料", 7.50, 45, 0.0));   // 运动饮料 7.50元/瓶
        m_products.push_back(CProduct("512", "纯净水", 1.80, 100, 0.0));    // 纯净水 1.80元/瓶
        
        // 零食类 (601-612) - 按包/袋计价
        m_products.push_back(CProduct("601", "薯片", 6.80, 30, 0.15));      // 薯片 6.80元/包，打85折
        m_products.push_back(CProduct("602", "巧克力", 15.80, 25, 0.0));    // 巧克力 15.80元/块
        m_products.push_back(CProduct("603", "饼干", 8.90, 40, 0.05));      // 饼干 8.90元/包，打95折
        m_products.push_back(CProduct("604", "坚果", 28.60, 20, 0.0));      // 坚果 28.60元/包
        m_products.push_back(CProduct("605", "瓜子仁", 9.50, 45, 0.0));     // 瓜子仁 9.50元/包
        m_products.push_back(CProduct("606", "爆米花", 3.50, 80, 0.0));     // 爆米花 3.50元/包
        m_products.push_back(CProduct("607", "口香糖", 5.80, 20, 0.0));     // 口香糖 5.80元/包
        m_products.push_back(CProduct("608", "牛肉干", 38.60, 15, 0.0));    // 牛肉干 38.60元/包
        m_products.push_back(CProduct("609", "果冻", 12.80, 35, 0.0));      // 果冻 12.80元/盒
        m_products.push_back(CProduct("610", "糖果", 8.90, 40, 0.0));       // 糖果 8.90元/包
        m_products.push_back(CProduct("611", "花生米", 15.60, 30, 0.0));    // 花生米 15.60元/包
        m_products.push_back(CProduct("612", "话梅", 12.20, 50, 0.0));      // 话梅 12.20元/包
        
        // 日用品类 (701-710) - 按包/瓶计价
        m_products.push_back(CProduct("701", "牙膏", 15.80, 25, 0.0));      // 牙膏 15.80元/支
        m_products.push_back(CProduct("702", "洗衣液", 32.50, 18, 0.2));    // 洗衣液 32.50元/瓶，打8折
        m_products.push_back(CProduct("703", "香皂", 4.60, 50, 0.0));       // 香皂 4.60元/块
        m_products.push_back(CProduct("704", "纸巾", 12.80, 35, 0.0));      // 纸巾 12.80元/包
        m_products.push_back(CProduct("705", "洗发液", 28.90, 20, 0.05));   // 洗发液 28.90元/瓶，打95折
        m_products.push_back(CProduct("706", "湿巾", 8.90, 50, 0.0));       // 湿巾 8.90元/包
        m_products.push_back(CProduct("707", "牙刷", 12.50, 30, 0.0));      // 牙刷 12.50元/支
        m_products.push_back(CProduct("708", "沐浴露", 25.80, 22, 0.0));    // 沐浴露 25.80元/瓶
        m_products.push_back(CProduct("709", "洗手液", 16.50, 28, 0.0));    // 洗手液 16.50元/瓶
        m_products.push_back(CProduct("710", "卫生纸", 18.60, 35, 0.0));    // 卫生纸 18.60元/提
        SaveToFile(filename);
        return true;
    }

    m_products.clear();
    std::string line;
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string code, name;
        double price, discount;
        int quantity;
        
        if (std::getline(ss, code, ',') &&
            std::getline(ss, name, ',') &&
            ss >> price && ss.ignore() &&
            ss >> quantity && ss.ignore() &&
            ss >> discount)
        {
            m_products.push_back(CProduct(code, name, price, quantity, discount));
        }
    }
    
    file.close();
    return true;
}

bool CProductManager::SaveToFile(const std::string& filename)
{
    std::ofstream file(filename);
    if (!file.is_open())
        return false;

    for (const auto& product : m_products)
    {
        file << product.GetCode() << "," << product.GetName() << ","
             << product.GetPrice() << "," << product.GetQuantity() << ","
             << product.GetDiscount() << std::endl;
    }
    
    file.close();
    return true;
}

CProduct* CProductManager::FindProductByCode(const std::string& code)
{
    for (auto& product : m_products)
    {
        if (product.GetCode() == code)
            return &product;
    }
    return nullptr;
}

std::vector<CProduct*> CProductManager::GetAvailableProducts()
{
    std::vector<CProduct*> available;
    for (auto& product : m_products)
    {
        if (product.IsAvailable())
            available.push_back(&product);
    }
    return available;
}

void CProductManager::AddProduct(const CProduct& product)
{
    m_products.push_back(product);
}

void CProductManager::UpdateProduct(const CProduct& product)
{
    for (auto& p : m_products)
    {
        if (p.GetCode() == product.GetCode())
        {
            p = product;
            break;
        }
    }
}