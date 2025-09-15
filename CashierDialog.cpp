#include "pch.h"
#include "CashierDialog.h"
#include "afxdialogex.h"
#include "Resource.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>

// CCashierDialog
IMPLEMENT_DYNAMIC(CCashierDialog, CDialogEx)

CCashierDialog::CCashierDialog(CWnd* pParent)
    : CDialogEx(IDD_CASHIER_DIALOG, pParent)
{
    m_productManager.LoadFromFile("products.txt");
    m_salesRecord.LoadFromFile("sales.txt");
}

CCashierDialog::~CCashierDialog()
{
    m_productManager.SaveToFile("products.txt");
    m_salesRecord.SaveToFile("sales.txt");
}

void CCashierDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PRODUCT_LIST, m_productList);
    DDX_Control(pDX, IDC_CART_LIST, m_cartList);
    DDX_Control(pDX, IDC_QUANTITY_EDIT, m_quantityEdit);
    DDX_Control(pDX, IDC_TOTAL_AMOUNT, m_totalAmountStatic);
}

BEGIN_MESSAGE_MAP(CCashierDialog, CDialogEx)
    ON_BN_CLICKED(IDC_ADD_TO_CART, &CCashierDialog::OnBnClickedAddToCart)
    ON_BN_CLICKED(IDC_REMOVE_FROM_CART, &CCashierDialog::OnBnClickedRemoveFromCart)
    ON_BN_CLICKED(IDC_CLEAR_CART, &CCashierDialog::OnBnClickedClearCart)
    ON_BN_CLICKED(IDC_CHECKOUT, &CCashierDialog::OnBnClickedCheckout)
    ON_BN_CLICKED(IDC_NEW_CUSTOMER, &CCashierDialog::OnBnClickedNewCustomer)
    ON_BN_CLICKED(IDC_SHOW_SALES_REPORT, &CCashierDialog::OnBnClickedShowSalesReport)
    ON_BN_CLICKED(IDC_ADD_PRODUCT_BTN, &CCashierDialog::OnBnClickedAddProduct)
    ON_LBN_SELCHANGE(IDC_PRODUCT_LIST, &CCashierDialog::OnLbnSelchangeProductList)
    ON_LBN_SELCHANGE(IDC_CART_LIST, &CCashierDialog::OnLbnSelchangeCartList)
END_MESSAGE_MAP()

BOOL CCashierDialog::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    SetWindowText(_T("收银台仿真系统"));
    
    // 设置默认数量
    m_quantityEdit.SetWindowText(_T("1"));
    
    // 初始化商品列表控件（表格模式）
    InitProductListCtrl();
    
    RefreshProductList();
    RefreshCartList();
    UpdateTotalAmount();

    return TRUE;
}

void CCashierDialog::InitProductListCtrl()
{
    // 设置表格样式
    DWORD dwStyle = m_productList.GetExtendedStyle();
    dwStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
    m_productList.SetExtendedStyle(dwStyle);
    
    // 优化列宽配置 - 适应新的更宽的商品列表区域（236像素）
    m_productList.InsertColumn(0, _T("编码"), LVCFMT_LEFT, 60);      // 编码
    m_productList.InsertColumn(1, _T("商品名称"), LVCFMT_LEFT, 120);   // 商品名称
    m_productList.InsertColumn(2, _T("原价/元"), LVCFMT_RIGHT, 70);     // 原价
    m_productList.InsertColumn(3, _T("现价/元"), LVCFMT_RIGHT, 70);     // 现价
    m_productList.InsertColumn(4, _T("库存"), LVCFMT_RIGHT, 50);     // 库存
    m_productList.InsertColumn(5, _T("折扣"), LVCFMT_CENTER, 50);    // 折扣
    m_productList.InsertColumn(6, _T("备注"), LVCFMT_CENTER, 180);    // 备注
}

void CCashierDialog::RefreshProductList()
{
    m_productList.DeleteAllItems();
    
    auto availableProducts = m_productManager.GetAvailableProducts();
    int index = 0;
    
    for (auto* product : availableProducts)
    {
        // 插入商品编码
        int nItem = m_productList.InsertItem(index, CString(product->GetCode().c_str()));
        
        // 设置商品名称
        m_productList.SetItemText(nItem, 1, CString(product->GetName().c_str()));
        
        // 设置原价
        CString originalPrice;
        originalPrice.Format(_T("%.2f"), product->GetPrice());
        m_productList.SetItemText(nItem, 2, originalPrice);
        
        // 设置现价（折扣后价格）
        CString currentPrice;
        currentPrice.Format(_T("%.2f"), product->GetDiscountedPrice());
        m_productList.SetItemText(nItem, 3, currentPrice);
        
        // 设置库存
        CString stock;
        stock.Format(_T("%d"), product->GetQuantity());
        m_productList.SetItemText(nItem, 4, stock);
        
        // 设置折扣信息
        CString discount;
        if (product->GetDiscount() > 0)
        {
            discount.Format(_T("%.0f%%"), product->GetDiscount() * 100);
        }
        else
        {
            discount = _T("无");
        }
        m_productList.SetItemText(nItem, 5, discount);
        
        // 存储产品指针到ItemData
        m_productList.SetItemData(nItem, (DWORD_PTR)product);
        
        // 如果有折扣，设置不同的文字颜色（可选）
        if (product->GetDiscount() > 0)
        {
            // 可以在这里设置特殊样式，比如红色文字表示打折商品
        }
        
        index++;
    }
}

void CCashierDialog::RefreshCartList()
{
    m_cartList.ResetContent();
    
    for (auto& item : m_shoppingCart.GetItems())
    {
        CProduct* product = item.GetProduct();
        CString itemText;
        
        // 格式化显示：商品名称 | 单价 | 数量 | 小计
        itemText.Format(_T("%-10s | %.2f元 | ×%d | %.2f元"),
            CString(product->GetName().c_str()),
            product->GetDiscountedPrice(),
            item.GetQuantity(),
            item.GetTotalPrice());
        
        m_cartList.AddString(itemText);
        m_cartList.SetItemDataPtr(m_cartList.GetCount() - 1, product);
    }
}


void CCashierDialog::UpdateTotalAmount()
{
    CString totalText;
    totalText.Format(_T("总计: %.2f元"), m_shoppingCart.GetTotalAmount());
    m_totalAmountStatic.SetWindowText(totalText);
}

void CCashierDialog::OnBnClickedAddToCart()
{
    int sel = m_productList.GetNextItem(-1, LVNI_SELECTED);
    if (sel == -1) 
    {
        MessageBox(_T("请先选择一个商品！"), _T("提示"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    CProduct* product = (CProduct*)m_productList.GetItemData(sel);
    if (!product) return;

    CString quantityStr;
    m_quantityEdit.GetWindowText(quantityStr);
    int quantity = _ttoi(quantityStr);
    
    if (quantity <= 0)
    {
        MessageBox(_T("请输入有效的数量！"), _T("错误"), MB_OK | MB_ICONERROR);
        return;
    }

    if (m_shoppingCart.AddItem(product, quantity))
    {
        RefreshCartList();
        UpdateTotalAmount();
        PlaySound(_T("ADD"));
    }
    else
    {
        MessageBox(_T("库存不足或添加失败！"), _T("错误"), MB_OK | MB_ICONERROR);
    }
}

void CCashierDialog::OnBnClickedRemoveFromCart()
{
    int sel = m_cartList.GetCurSel();
    if (sel == LB_ERR) return;

    CProduct* product = static_cast<CProduct*>(m_cartList.GetItemDataPtr(sel));
    if (!product) return;

    m_shoppingCart.RemoveItem(product->GetCode());
    RefreshCartList();
    UpdateTotalAmount();
}

void CCashierDialog::OnBnClickedClearCart()
{
    if (MessageBox(_T("确定要清空购物车吗？"), _T("确认"), MB_YESNO | MB_ICONQUESTION) == IDYES)
    {
        m_shoppingCart.Clear();
        RefreshCartList();
        UpdateTotalAmount();
    }
}

void CCashierDialog::OnBnClickedCheckout()
{
    if (m_shoppingCart.GetItems().empty())
    {
        MessageBox(_T("购物车为空，无法结账！"), _T("提示"), MB_OK | MB_ICONINFORMATION);
        return;
    }

    double totalAmount = m_shoppingCart.GetTotalAmount();
    
    // 显示付款对话框
    CPaymentDialog paymentDlg(totalAmount, this);
    if (paymentDlg.DoModal() == IDOK)
    {
        // 更新库存
        for (auto& item : m_shoppingCart.GetItems())
        {
            item.GetProduct()->ReduceQuantity(item.GetQuantity());
        }
        
        // 添加销售记录
        m_salesRecord.AddSale(m_shoppingCart);
        
        // 生成完整收据
        std::string receiptStr = m_shoppingCart.GenerateReceipt();
        
        // 添加付款信息 - 使用制表符对齐
        std::stringstream ss;
        ss << receiptStr;
        
        ss << "================================\n";
        ss << "\t\t    付款详情\t\t";
        ss << "\n收款金额: " << std::fixed << std::setprecision(2) << paymentDlg.GetPaidAmount() << "元\t\t";
        ss << "\n找零金额: " << std::fixed << std::setprecision(2) << paymentDlg.GetChangeAmount() << "元\n";
        ss << "================================\n";
        ss << "交易完成 - 感谢惠顾！\n";
        
        // 转换为CString时使用正确的编码
        std::string finalReceiptStr = ss.str();
        CString receipt;
        
        // 使用MultiByteToWideChar确保正确的编码转换
        int len = MultiByteToWideChar(CP_ACP, 0, finalReceiptStr.c_str(), -1, NULL, 0);
        if (len > 0)
        {
            WCHAR* wideStr = new WCHAR[len];
            MultiByteToWideChar(CP_ACP, 0, finalReceiptStr.c_str(), -1, wideStr, len);
            receipt = wideStr;
            delete[] wideStr;
        }
        else
        {
            // 备用方案：直接转换
            receipt = CString(finalReceiptStr.c_str());
        }
        
        // 显示收据
        CReceiptDialog receiptDlg(receipt, this);
        receiptDlg.DoModal();
        
        // 清空购物车
        m_shoppingCart.Clear();
        RefreshProductList();
        RefreshCartList();
        UpdateTotalAmount();
        
        PlaySound(_T("COMPLETE"));
        MessageBox(_T("交易完成！感谢您的惠顾！"), _T("交易成功"), MB_OK | MB_ICONINFORMATION);
    }
}

void CCashierDialog::OnBnClickedNewCustomer()
{
    if (!m_shoppingCart.GetItems().empty())
    {
        if (MessageBox(_T("当前购物车不为空，确定要开始新的顾客服务吗？"), 
                      _T("确认"), MB_YESNO | MB_ICONQUESTION) == IDNO)
            return;
    }
    
    m_shoppingCart.Clear();
    RefreshProductList();
    RefreshCartList();
    UpdateTotalAmount();
    m_quantityEdit.SetWindowText(_T("1"));
    
    PlaySound(_T("WELCOME"));
    MessageBox(_T("欢迎光临！"), _T("提示"), MB_OK | MB_ICONINFORMATION);
}

void CCashierDialog::OnBnClickedShowSalesReport()
{
    std::stringstream ss;
    
    // 获取当前日期
    time_t now = time(0);
    struct tm timeinfo;
    char dateStr[100];
    if (localtime_s(&timeinfo, &now) == 0)
    {
        strftime(dateStr, sizeof(dateStr), "%Y年%m月%d日", &timeinfo);
    }
    else
    {
        strcpy_s(dateStr, sizeof(dateStr), "今日");
    }
    
    // 优化后的紧凑格式
    ss << "统计日期: " << dateStr << "\r\n\r\n";
    
    // 直接从销售记录对象获取数据
    int transactionCount = m_salesRecord.GetTotalTransactions();
    double totalSales = m_salesRecord.GetTotalSales();
    double avgSales = (transactionCount > 0) ? (totalSales / transactionCount) : 0.0;
    
    ss << "交易笔数: " << transactionCount << " 笔\r\n";
    ss << "总销售额: " << std::fixed << std::setprecision(2) << totalSales << " 元\r\n";
    ss << "平均交易额: " << std::fixed << std::setprecision(2) << avgSales << " 元\r\n\r\n";
    
    // 业绩评价区域 - 简化评价语言
    if (transactionCount == 0)
    {
        ss << "今日暂无交易记录";
    }
    else if (totalSales < 100)
    {
        ss << "今日销售刚刚起步，继续加油！";
    }
    else if (totalSales < 500)
    {
        ss << "今日销售业绩良好，保持努力！";
    }
    else
    {
        ss << "今日销售业绩优秀，恭喜您！";
    }
    
    CString compactReport(ss.str().c_str());
    CSalesReportDialog dlg(compactReport, this);
    dlg.DoModal();
}

void CCashierDialog::OnBnClickedAddProduct()
{
    CAddProductDialog addProductDlg(this);
    if (addProductDlg.DoModal() == IDOK)
    {
        CProduct newProduct = addProductDlg.GetProduct();
        
        // 检查商品编码是否已存在
        if (m_productManager.FindProductByCode(newProduct.GetCode()))
        {
            MessageBox(_T("商品编码已存在，请使用不同的编码！"), _T("错误"), MB_OK | MB_ICONERROR);
            return;
        }
        
        // 添加新商品
        m_productManager.AddProduct(newProduct);
        RefreshProductList();
        
        MessageBox(_T("商品添加成功！"), _T("提示"), MB_OK | MB_ICONINFORMATION);
    }
}

void CCashierDialog::OnLbnSelchangeProductList()
{
    // 当选择商品表格中的项目时的处理
    int sel = m_productList.GetNextItem(-1, LVNI_SELECTED);
    if (sel != -1)
    {
        CProduct* product = (CProduct*)m_productList.GetItemData(sel);
        if (product)
        {
            // 可以在这里显示选中商品的详细信息
            // 例如在状态栏显示商品描述等
        }
    }
}

void CCashierDialog::OnLbnSelchangeCartList()
{
    // 可以在这里添加选择购物车项目时的逻辑
}

void CCashierDialog::PlaySound(const CString& soundType)
{
    // 简单的蜂鸣声实现，实际可以播放音频文件
    if (soundType == _T("ADD"))
        MessageBeep(MB_OK);
    else if (soundType == _T("COMPLETE"))
        MessageBeep(MB_ICONASTERISK);
    else if (soundType == _T("WELCOME"))
        MessageBeep(MB_ICONINFORMATION);
}

// CPaymentDialog
IMPLEMENT_DYNAMIC(CPaymentDialog, CDialogEx)

CPaymentDialog::CPaymentDialog(double totalAmount, CWnd* pParent)
    : CDialogEx(IDD_PAYMENT_DIALOG, pParent)
    , m_dTotalAmount(totalAmount)
    , m_dPaidAmount(0.0)
    , m_dChangeAmount(0.0)
    , m_bPaymentComplete(FALSE)
{
}

CPaymentDialog::~CPaymentDialog()
{
}

void CPaymentDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PAID_AMOUNT, m_paidAmountEdit);
    DDX_Control(pDX, IDC_TOTAL_AMOUNT, m_totalAmountStatic);
    DDX_Control(pDX, IDC_CHANGE_AMOUNT, m_changeAmountStatic);
}

BEGIN_MESSAGE_MAP(CPaymentDialog, CDialogEx)
    ON_BN_CLICKED(IDC_CALCULATE_CHANGE, &CPaymentDialog::OnBnClickedCalculateChange)
    ON_BN_CLICKED(IDC_COMPLETE_SALE, &CPaymentDialog::OnBnClickedCompleteSale)
END_MESSAGE_MAP()

BOOL CPaymentDialog::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    SetWindowText(_T("收银付款"));
    
    CString totalText;
    totalText.Format(_T("应付金额: %.2f元"), m_dTotalAmount);
    m_totalAmountStatic.SetWindowText(totalText);
    
    m_changeAmountStatic.SetWindowText(_T("找零: 0.00元"));
    
    GetDlgItem(IDC_COMPLETE_SALE)->EnableWindow(FALSE);

    return TRUE;
}

void CPaymentDialog::OnBnClickedCalculateChange()
{
    CString paidStr;
    m_paidAmountEdit.GetWindowText(paidStr);
    
    // 首先验证输入格式，检查是否超过分的精度
    if (!ValidateMoneyInput(paidStr))
    {
        return; // 验证失败，已显示错误提示
    }
    
    double paidAmount = _ttof(paidStr);
    m_dPaidAmount = paidAmount;
    
    if (m_dPaidAmount < m_dTotalAmount)
    {
        MessageBox(_T("付款金额不足！"), _T("错误"), MB_OK | MB_ICONERROR);
        return;
    }
    
    m_dChangeAmount = m_dPaidAmount - m_dTotalAmount;
    
    CString changeText;
    changeText.Format(_T("找零: %.2f元"), m_dChangeAmount);
    m_changeAmountStatic.SetWindowText(changeText);
    
    GetDlgItem(IDC_COMPLETE_SALE)->EnableWindow(TRUE);
    m_bPaymentComplete = TRUE;
}

void CPaymentDialog::OnBnClickedCompleteSale()
{
    if (!m_bPaymentComplete)
    {
        MessageBox(_T("请先计算找零！"), _T("提示"), MB_OK | MB_ICONINFORMATION);
        return;
    }
    
    EndDialog(IDOK);
}

bool CPaymentDialog::ValidateMoneyInput(const CString& amountStr)
{
    // 检查是否为空
    if (amountStr.IsEmpty())
    {
        MessageBox(_T("请输入付款金额！"), _T("输入错误"), MB_OK | MB_ICONWARNING);
        m_paidAmountEdit.SetFocus();
        return false;
    }
    
    // 检查是否包含非数字字符（除了小数点）
    for (int i = 0; i < amountStr.GetLength(); i++)
    {
        TCHAR ch = amountStr.GetAt(i);
        if (!_istdigit(ch) && ch != _T('.'))
        {
            MessageBox(_T("付款金额只能包含数字和小数点！\n请重新输入正确的金额。"), 
                      _T("输入格式错误"), MB_OK | MB_ICONWARNING);
            m_paidAmountEdit.SetFocus();
            m_paidAmountEdit.SetSel(0, -1); // 选中全部文本
            return false;
        }
    }
    
    // 检查小数点数量
    int dotCount = 0;
    int dotPos = -1;
    for (int i = 0; i < amountStr.GetLength(); i++)
    {
        if (amountStr.GetAt(i) == _T('.'))
        {
            dotCount++;
            if (dotPos == -1) dotPos = i;
        }
    }
    
    if (dotCount > 1)
    {
        MessageBox(_T("付款金额格式错误！\n一个金额只能包含一个小数点。"), 
                  _T("输入格式错误"), MB_OK | MB_ICONWARNING);
        m_paidAmountEdit.SetFocus();
        m_paidAmountEdit.SetSel(0, -1);
        return false;
    }
    
    // 检查小数位数是否超过2位（分的精度）
    if (dotPos != -1)
    {
        int decimalPlaces = amountStr.GetLength() - dotPos - 1;
        if (decimalPlaces > 2)
        {
            MessageBox(_T("付款金额的精度不能超过分（0.01元）！\n") \
                      _T("请输入最多2位小数的金额。\n") \
                      _T("例如：10.50、25.99、100.00"), 
                      _T("金额精度错误"), MB_OK | MB_ICONWARNING);
            m_paidAmountEdit.SetFocus();
            m_paidAmountEdit.SetSel(dotPos + 3, -1); // 选中多余的小数位
            return false;
        }
    }
    
    // 检查金额是否为负数或零
    double amount = _ttof(amountStr);
    if (amount <= 0)
    {
        MessageBox(_T("付款金额必须大于0！\n请输入有效的付款金额。"), 
                  _T("金额错误"), MB_OK | MB_ICONWARNING);
        m_paidAmountEdit.SetFocus();
        m_paidAmountEdit.SetSel(0, -1);
        return false;
    }
    
    // 检查金额是否过大（防止输入错误）
    if (amount > 99999.99)
    {
        MessageBox(_T("付款金额过大！\n请确认输入的金额是否正确。"), 
                  _T("金额异常"), MB_OK | MB_ICONWARNING);
        m_paidAmountEdit.SetFocus();
        m_paidAmountEdit.SetSel(0, -1);
        return false;
    }
    
    return true;
}

// CReceiptDialog
IMPLEMENT_DYNAMIC(CReceiptDialog, CDialogEx)

CReceiptDialog::CReceiptDialog(const CString& receipt, CWnd* pParent)
    : CDialogEx(IDD_RECEIPT_DIALOG, pParent)
    , m_strReceipt(receipt)
{
}

CReceiptDialog::~CReceiptDialog()
{
}

void CReceiptDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_RECEIPT_TEXT, m_receiptText);
}

BEGIN_MESSAGE_MAP(CReceiptDialog, CDialogEx)
END_MESSAGE_MAP()

BOOL CReceiptDialog::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    SetWindowText(_T("购物收据"));
    
    // 设置收据文本
    m_receiptText.SetWindowText(m_strReceipt);
   
    CFont* pFont = new CFont;
    pFont->CreateFont(
        22,                        // 大字体：22号
        0, 0, 0, FW_NORMAL, FALSE, FALSE, 0,
        GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, FIXED_PITCH | FF_MODERN,
        _T("Consolas"));
    
    m_receiptText.SetFont(pFont);
    
    // 计算所需的窗口大小 - 确保有足够宽度显示收据格式
    CDC* pDC = m_receiptText.GetDC();
    if (pDC)
    {
        CFont* pOldFont = pDC->SelectObject(pFont);
        
        // 计算单个字符的尺寸来估算所需宽度
        CSize charSize = pDC->GetTextExtent(_T("A"));
        
        // 为新的紧凑格式计算宽度 - 每行大约需要30个字符宽度
        int estimatedWidth = charSize.cx * 32 + 80;  // 32个字符宽度 + 更多边距
        
        // 计算文本高度
        CRect textRect;
        pDC->DrawText(m_strReceipt, &textRect, DT_CALCRECT | DT_WORDBREAK | DT_EXPANDTABS);
        int estimatedHeight = textRect.Height() + 150;
        
        pDC->SelectObject(pOldFont);
        m_receiptText.ReleaseDC(pDC);
        
        // 设置合适的窗口尺寸 - 适应新格式
        int windowWidth = estimatedWidth;
        int windowHeight = estimatedHeight;
        
        // 调整窗口尺寸范围 - 适合新的紧凑格式
        if (windowWidth < 450) windowWidth = 450;
        if (windowWidth > 650) windowWidth = 650;    // 减小最大宽度
        if (windowHeight < 400) windowHeight = 400;
        if (windowHeight > 800) windowHeight = 800;
        
        // 居中显示
        CWnd* pParent = GetParent();
        int x, y;
        if (pParent)
        {
            CRect parentRect;
            pParent->GetWindowRect(&parentRect);
            x = parentRect.left + (parentRect.Width() - windowWidth) / 2;
            y = parentRect.top + (parentRect.Height() - windowHeight) / 2;
        }
        else
        {
            x = (GetSystemMetrics(SM_CXSCREEN) - windowWidth) / 2;
            y = (GetSystemMetrics(SM_CYSCREEN) - windowHeight) / 2;
        }
        
        // 调整窗口大小和位置
        SetWindowPos(NULL, x, y, windowWidth, windowHeight, SWP_NOZORDER);
        
        // 调整文本控件大小 - 更大的边距
        m_receiptText.MoveWindow(25, 25, windowWidth - 50, windowHeight - 90);
        
        // 调整确定按钮位置
        CWnd* pOKButton = GetDlgItem(IDOK);
        if (pOKButton)
        {
            int buttonWidth = 80;
            int buttonHeight = 30;
            int buttonX = (windowWidth - buttonWidth) / 2;
            int buttonY = windowHeight - buttonHeight - 30;
            
            pOKButton->MoveWindow(buttonX, buttonY, buttonWidth, buttonHeight);
        }
    }
    else
    {
        // 如果无法计算文本尺寸，使用适合紧凑格式的默认尺寸
        int defaultWidth = 500;   // 减小默认宽度
        int defaultHeight = 600;  // 适中的默认高度
        
        CWnd* pParent = GetParent();
        int x, y;
        if (pParent)
        {
            CRect parentRect;
            pParent->GetWindowRect(&parentRect);
            x = parentRect.left + (parentRect.Width() - defaultWidth) / 2;
            y = parentRect.top + (parentRect.Height() - defaultHeight) / 2;
        }
        else
        {
            x = (GetSystemMetrics(SM_CXSCREEN) - defaultWidth) / 2;
            y = (GetSystemMetrics(SM_CYSCREEN) - defaultHeight) / 2;
        }
        
        SetWindowPos(NULL, x, y, defaultWidth, defaultHeight, SWP_NOZORDER);
        m_receiptText.MoveWindow(25, 25, defaultWidth - 50, defaultHeight - 90);
        
        CWnd* pOKButton = GetDlgItem(IDOK);
        if (pOKButton)
        {
            pOKButton->MoveWindow((defaultWidth - 80) / 2, defaultHeight - 60, 80, 30);
        }
    }

    return TRUE;
}

// CSalesReportDialog
IMPLEMENT_DYNAMIC(CSalesReportDialog, CDialogEx)

CSalesReportDialog::CSalesReportDialog(const CString& report, CWnd* pParent)
    : CDialogEx(IDD_SALES_REPORT_DIALOG, pParent)
    , m_strReport(report)
{
}

CSalesReportDialog::~CSalesReportDialog()
{
}

void CSalesReportDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SALES_REPORT_TEXT, m_salesReportText);
    DDX_Control(pDX, IDC_SALES_REPORT_TITLE, m_titleStatic);
}

BEGIN_MESSAGE_MAP(CSalesReportDialog, CDialogEx)
END_MESSAGE_MAP()

BOOL CSalesReportDialog::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    SetWindowText(_T("销售统计报告"));
    
    // 设置内容文本
    m_salesReportText.SetWindowText(m_strReport);
    
    // 标题字体 - 28号字体
    static CFont s_titleFont;
    if (!s_titleFont.m_hObject)
    {
        s_titleFont.CreateFont(
            28,                        // nHeight - 28号字体
            0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_MODERN,
            _T("Microsoft YaHei"));    // 使用微软雅黑，更适合中文显示
    }
    m_titleStatic.SetFont(&s_titleFont);

    // 内容字体 - 同样使用28号字体
    static CFont s_contentFont;
    if (!s_contentFont.m_hObject)
    {
        s_contentFont.CreateFont(
            28,                        // nHeight - 内容也使用28号字体
            0, 0, 0, FW_NORMAL, FALSE, FALSE, 0,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_MODERN,
            _T("Microsoft YaHei"));    // 使用微软雅黑
    }
    m_salesReportText.SetFont(&s_contentFont);

    // 根据内容优化窗口大小
    OptimizeWindowSize();

    return TRUE;
}

void CSalesReportDialog::OptimizeWindowSize()
{
    // 获取内容文本的实际大小
    CDC* pDC = m_salesReportText.GetDC();
    if (!pDC) return;
    
    CFont* pOldFont = pDC->SelectObject(m_salesReportText.GetFont());
    
    // 计算文本实际需要的尺寸
    CRect textRect;
    CString content;
    m_salesReportText.GetWindowText(content);
    
    // 估算文本区域大小 - 适应28号字体内容
    pDC->DrawText(content, &textRect, DT_CALCRECT | DT_WORDBREAK | DT_EXPANDTABS);
    
    pDC->SelectObject(pOldFont);
    m_salesReportText.ReleaseDC(pDC);
    
    // 计算所需窗口尺寸 - 为28号字体内容增加更多空间
    int contentWidth = max(450, min(600, textRect.Width() + 80));   // 宽度450-600之间，为28号字体增加更多空间
    int contentHeight = max(150, min(220, textRect.Height() + 50)); // 高度150-220之间，为28号字体增加更多空间
    
    int totalWidth = contentWidth + 20;    // 加上边距
    int totalHeight = contentHeight + 110; // 加上标题区域和按钮区域
    
    // 确保窗口尺寸合理 - 适应28号字体内容
    totalWidth = max(470, min(620, totalWidth));
    totalHeight = max(240, min(350, totalHeight));
    
    // 居中显示
    CRect screenRect;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &screenRect, 0);
    int x = (screenRect.Width() - totalWidth) / 2;
    int y = (screenRect.Height() - totalHeight) / 2;
    
    // 调整窗口大小和位置
    SetWindowPos(NULL, x, y, totalWidth, totalHeight, SWP_NOZORDER);
    
    // 调整控件大小 - 为28号字体预留充足空间
    m_titleStatic.MoveWindow(10, 8, totalWidth - 20, 35);  // 标题高度35
    m_salesReportText.MoveWindow(10, 45, totalWidth - 20, contentHeight); // 内容区域
    
    // 调整确定按钮位置
    CWnd* pOKButton = GetDlgItem(IDOK);
    if (pOKButton)
    {
        pOKButton->MoveWindow((totalWidth - 50) / 2, totalHeight - 30, 50, 20);
    }
}

// CAddProductDialog
IMPLEMENT_DYNAMIC(CAddProductDialog, CDialogEx)

CAddProductDialog::CAddProductDialog(CWnd* pParent)
    : CDialogEx(IDD_ADD_PRODUCT_DIALOG, pParent)
{
}

CAddProductDialog::~CAddProductDialog()
{
}

void CAddProductDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PRODUCT_CODE_EDIT, m_codeEdit);
    DDX_Control(pDX, IDC_PRODUCT_NAME_EDIT, m_nameEdit);
    DDX_Control(pDX, IDC_PRODUCT_PRICE_EDIT, m_priceEdit);
    DDX_Control(pDX, IDC_PRODUCT_STOCK_EDIT, m_stockEdit);
    DDX_Control(pDX, IDC_PRODUCT_DISCOUNT_EDIT, m_discountEdit);
}

BEGIN_MESSAGE_MAP(CAddProductDialog, CDialogEx)
END_MESSAGE_MAP()

BOOL CAddProductDialog::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    SetWindowText(_T("添加新商品"));
    
    // 设置默认折扣为0
    m_discountEdit.SetWindowText(_T("0"));
    
    return TRUE;
}

void CAddProductDialog::OnOK()
{
    if (ValidateInput())
    {
        CDialogEx::OnOK();
    }
}

bool CAddProductDialog::ValidateInput()
{
    CString code, name, priceStr, stockStr, discountStr;
    
    m_codeEdit.GetWindowText(code);
    m_nameEdit.GetWindowText(name);
    m_priceEdit.GetWindowText(priceStr);
    m_stockEdit.GetWindowText(stockStr);
    m_discountEdit.GetWindowText(discountStr);
    
    // 验证必填字段
    if (code.IsEmpty())
    {
        MessageBox(_T("请输入商品编码！"), _T("验证错误"), MB_OK | MB_ICONWARNING);
        m_codeEdit.SetFocus();
        return false;
    }
    
    if (name.IsEmpty())
    {
        MessageBox(_T("请输入商品名称！"), _T("验证错误"), MB_OK | MB_ICONWARNING);
        m_nameEdit.SetFocus();
        return false;
    }
    
    if (priceStr.IsEmpty())
    {
        MessageBox(_T("请输入商品价格！"), _T("验证错误"), MB_OK | MB_ICONWARNING);
        m_priceEdit.SetFocus();
        return false;
    }
    
    if (stockStr.IsEmpty())
    {
        MessageBox(_T("请输入库存数量！"), _T("验证错误"), MB_OK | MB_ICONWARNING);
        m_stockEdit.SetFocus();
        return false;
    }
    
    // 验证价格（精确到分）
    double price = ValidatePrice(priceStr);
    if (price < 0)
    {
        m_priceEdit.SetFocus();
        return false;
    }
    
    // 验证库存
    int stock = _ttoi(stockStr);
    if (stock < 0)
    {
        MessageBox(_T("库存数量不能为负数！"), _T("验证错误"), MB_OK | MB_ICONWARNING);
        m_stockEdit.SetFocus();
        return false;
    }
    
    // 验证折扣率
    double discount = 0.0;
    if (!discountStr.IsEmpty())
    {
        discount = ValidateDiscount(discountStr);
        if (discount < 0)
        {
            m_discountEdit.SetFocus();
            return false;
        }
    }
    
    // 创建商品对象
    std::string stdCode = CT2A(code);
    std::string stdName = CT2A(name);
    
    m_product = CProduct(stdCode, stdName, price, stock, discount / 100.0);
    
    return true;
}

double CAddProductDialog::ValidatePrice(const CString& priceStr)
{
    double price = _ttof(priceStr);
    
    if (price < 0)
    {
        MessageBox(_T("价格不能为负数！"), _T("验证错误"), MB_OK | MB_ICONWARNING);
        return -1;
    }
    
    // 检查小数位数不超过2位（精确到分）
    CString formattedPrice;
    formattedPrice.Format(_T("%.2f"), price);
    
    if (formattedPrice != priceStr && _ttof(formattedPrice) != price)
    {
        // 检查用户输入是否有超过2位小数
        int dotPos = priceStr.Find(_T('.'));
        if (dotPos != -1 && priceStr.GetLength() - dotPos - 1 > 2)
        {
            MessageBox(_T("价格精确度不能超过分（0.01元）！\n请输入最多2位小数的价格。"), 
                      _T("验证错误"), MB_OK | MB_ICONWARNING);
            return -1;
        }
    }
    
    // 舍入到分
    return floor(price * 100.0 + 0.5) / 100.0;
}

double CAddProductDialog::ValidateDiscount(const CString& discountStr)
{
    double discount = _ttof(discountStr);
    
    if (discount < 0 || discount > 100)
    {
        MessageBox(_T("折扣率必须在0-100之间！"), _T("验证错误"), MB_OK | MB_ICONWARNING);
        return -1;
    }
    
    return discount;
}