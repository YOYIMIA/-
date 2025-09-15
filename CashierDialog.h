//作者：黄羿翔
#pragma once
#include "afxdialogex.h"
#include "Product.h"
#include "ShoppingCart.h"

class CCashierDialog : public CDialogEx
{
    DECLARE_DYNAMIC(CCashierDialog)

public:
    CCashierDialog(CWnd* pParent = nullptr);
    virtual ~CCashierDialog();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_CASHIER_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();

    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnBnClickedAddToCart();
    afx_msg void OnBnClickedRemoveFromCart();
    afx_msg void OnBnClickedClearCart();
    afx_msg void OnBnClickedCheckout();
    afx_msg void OnBnClickedNewCustomer();
    afx_msg void OnBnClickedShowSalesReport();
    afx_msg void OnBnClickedAddProduct();
    afx_msg void OnLbnSelchangeProductList();
    afx_msg void OnLbnSelchangeCartList();

private:
    void RefreshProductList();
    void RefreshCartList();
    void UpdateTotalAmount();
    void PlaySound(const CString& soundType);
    void InitProductListCtrl();   // 新增方法

    CListCtrl m_productList;    // 改为 CListCtrl
    CListBox m_cartList;
    CEdit m_quantityEdit;
    CStatic m_totalAmountStatic;
    
    CProductManager m_productManager;
    CShoppingCart m_shoppingCart;
    CSalesRecord m_salesRecord;
};

class CPaymentDialog : public CDialogEx
{
    DECLARE_DYNAMIC(CPaymentDialog)

public:
    CPaymentDialog(double totalAmount, CWnd* pParent = nullptr);
    virtual ~CPaymentDialog();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_PAYMENT_DIALOG };
#endif

    double GetPaidAmount() const { return m_dPaidAmount; }
    double GetChangeAmount() const { return m_dChangeAmount; }

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();

    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnBnClickedCalculateChange();
    afx_msg void OnBnClickedCompleteSale();

private:
    CEdit m_paidAmountEdit;
    CStatic m_totalAmountStatic;
    CStatic m_changeAmountStatic;
    
    double m_dTotalAmount;
    double m_dPaidAmount;
    double m_dChangeAmount;
    BOOL m_bPaymentComplete;

    bool ValidateMoneyInput(const CString& amountStr);
};

class CReceiptDialog : public CDialogEx
{
    DECLARE_DYNAMIC(CReceiptDialog)

public:
    CReceiptDialog(const CString& receipt, CWnd* pParent = nullptr);
    virtual ~CReceiptDialog();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_RECEIPT_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();

    DECLARE_MESSAGE_MAP()

private:
    CEdit m_receiptText;
    CString m_strReceipt;
};

class CSalesReportDialog : public CDialogEx
{
    DECLARE_DYNAMIC(CSalesReportDialog)

public:
    CSalesReportDialog(const CString& report, CWnd* pParent = nullptr);
    virtual ~CSalesReportDialog();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_SALES_REPORT_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();

    DECLARE_MESSAGE_MAP()

private:
    CEdit m_salesReportText;
    CStatic m_titleStatic;
    CString m_strReport;

    void OptimizeWindowSize();  // 优化窗口尺寸的私有方法
};

class CAddProductDialog : public CDialogEx
{
    DECLARE_DYNAMIC(CAddProductDialog)

public:
    CAddProductDialog(CWnd* pParent = nullptr);
    virtual ~CAddProductDialog();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ADD_PRODUCT_DIALOG };
#endif

    CProduct GetProduct() const { return m_product; }

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    virtual void OnOK();

    DECLARE_MESSAGE_MAP()

private:
    CEdit m_codeEdit;
    CEdit m_nameEdit;
    CEdit m_priceEdit;
    CEdit m_stockEdit;
    CEdit m_discountEdit;
    
    CProduct m_product;
    
    bool ValidateInput();
    double ValidatePrice(const CString& priceStr);
    double ValidateDiscount(const CString& discountStr);
};