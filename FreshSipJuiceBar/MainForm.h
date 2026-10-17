#pragma once

#include "Models/Person.h"
#include "Models/Customer.h"
#include "Models/Staff.h"
#include "Models/Product.h"
#include "Models/Ingredient.h"
#include "Models/OrderItem.h"
#include "Models/Order.h"
#include "Models/PaymentMethod.h"
#include "Data/DatabaseManager.h"
#include "Services/ProductService.h"
#include "Services/CustomerService.h"
#include "Services/InventoryService.h"
#include "Services/OrderService.h"
#include "Services/ReportService.h"
#include "Forms/ReceiptForm.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using namespace System::Drawing;
using namespace FreshSipJuiceBar::Models;
using namespace FreshSipJuiceBar::Services;
using namespace FreshSipJuiceBar::Forms;

namespace FreshSipJuiceBar {

    /// <summary>
    /// MainForm: Central Master-Detail interface hosting the 6 screens:
    /// Screen A - Dashboard
    /// Screen B - Product Management (Full CRUD)
    /// Screen C - Orders & Billing POS (Polymorphic Payment & Cart Overloading)
    /// Screen D - Customer Management (Full CRUD)
    /// Screen E - Ingredient Inventory (Low-Stock Alerting)
    /// Screen F - Sales Reports (Financial Aggregations & Charts)
    /// </summary>
    public ref class MainForm : public System::Windows::Forms::Form {
    public:
        MainForm(void);
        MainForm(String^ initialRole, String^ initialUserName);

        property bool IsLoggingOut {
            bool get() { return m_isLoggingOut; }
        }

    protected:
        ~MainForm();

    private:
        System::ComponentModel::Container^ components;
        System::Windows::Forms::Timer^ timerClock;

        // Current Order Cart for POS
        Order^ m_currentPosOrder;
        String^ m_lastReceiptText;

        // --- Shell UI Controls ---
        Panel^ pnlSidebar;
        Panel^ pnlHeader;
        Panel^ pnlContentContainer;

        Label^ lblBrandLogo;
        Label^ lblBrandSubtitle;
        Label^ lblBreadcrumb;
        Label^ lblClock;
        Label^ lblSystemStatus;
        Label^ lblCurrentUserBadge;
        Button^ btnLogout;

        // Current Staff & Role State
        String^ m_currentRole;       // "Admin" or "Cashier"
        String^ m_currentUserName;   // Display name of logged-in staff
        bool m_isLoggingOut;

        Button^ btnNavDashboard;
        Button^ btnNavProducts;
        Button^ btnNavOrders;
        Button^ btnNavCustomers;
        Button^ btnNavInventory;
        Button^ btnNavReports;

        // --- Screen A: Dashboard Controls ---
        Panel^ pnlScreenDashboard;
        PictureBox^ picDashboardBanner;
        Panel^ cardKpiTodaySales;
        Label^ lblKpiSalesVal;
        Panel^ cardKpiTodayOrders;
        Label^ lblKpiOrdersVal;
        Panel^ cardKpiProducts;
        Label^ lblKpiProductsVal;
        Panel^ cardKpiLowStock;
        Label^ lblKpiLowStockVal;
        DataGridView^ dgvDashTopProducts;
        DataGridView^ dgvDashRecentOrders;
        Button^ btnDashQuickOrder;
        Button^ btnDashQuickProduct;
        Button^ btnDashQuickRestock;
        Label^ lblDashT1;
        Label^ lblDashT2;

        // --- Screen B: Products Controls ---
        Panel^ pnlScreenProducts;
        Panel^ pnlProdForm;
        Label^ lblSearchProd;
        Label^ lblFilterCat;
        TextBox^ txtProdId;
        TextBox^ txtProdName;
        ComboBox^ cmbProdCategory;
        TextBox^ txtProdDesc;
        TextBox^ txtProdPrice;
        NumericUpDown^ nudProdQty;
        TextBox^ txtProdImagePath;
        PictureBox^ picProdPreview;
        CheckBox^ chkProdActive;
        Button^ btnProdBrowseImg;
        Button^ btnProdAdd;
        Button^ btnProdUpdate;
        Button^ btnProdDelete;
        Button^ btnProdClear;
        TextBox^ txtProdSearch;
        ComboBox^ cmbProdFilter;
        DataGridView^ dgvProducts;

        // --- Screen C: POS Orders & Billing Controls ---
        Panel^ pnlScreenOrders;
        Panel^ pnlPosCatalog;
        Panel^ pnlPosCartPanel;
        Label^ lblPosQty;
        Label^ lblCustL;
        Label^ lblSubT;
        Label^ lblDiscT;
        Label^ lblNetT;
        Label^ lblPayL;
        TextBox^ txtPosSearch;
        ComboBox^ cmbPosCategory;
        DataGridView^ dgvPosProducts;
        NumericUpDown^ nudPosQty;
        Button^ btnPosAddToCart;
        Label^ lblPosOrderNumber;
        ComboBox^ cmbPosCustomer;
        Button^ btnPosNewCustomer;
        DataGridView^ dgvPosCart;
        Label^ lblPosSubtotal;
        NumericUpDown^ nudPosDiscount;
        Label^ lblPosFinalTotal;
        RadioButton^ rdoPayCash;
        RadioButton^ rdoPayCard;
        Panel^ pnlCashDetails;
        TextBox^ txtCashTendered;
        Label^ lblCashChange;
        Panel^ pnlCardDetails;
        ComboBox^ cmbCardType;
        TextBox^ txtCardLast4;
        Button^ btnPosCheckout;
        Button^ btnPosClearCart;
        Button^ btnPosViewReceipt;

        // --- Screen D: Customers Controls ---
        Panel^ pnlScreenCustomers;
        Panel^ pnlCustForm;
        Label^ lblSearchCust;
        TextBox^ txtCustId;
        TextBox^ txtCustName;
        TextBox^ txtCustPhone;
        TextBox^ txtCustEmail;
        TextBox^ txtCustAddress;
        Button^ btnCustAdd;
        Button^ btnCustUpdate;
        Button^ btnCustDelete;
        Button^ btnCustClear;
        TextBox^ txtCustSearch;
        DataGridView^ dgvCustomers;

        // --- Screen E: Inventory Controls ---
        Panel^ pnlScreenInventory;
        Panel^ pnlIngForm;
        Label^ lblSearchIng;
        TextBox^ txtIngId;
        TextBox^ txtIngName;
        NumericUpDown^ nudIngStock;
        ComboBox^ cmbIngUnit;
        NumericUpDown^ nudIngReorder;
        TextBox^ txtIngSupplier;
        TextBox^ txtIngSupplierPhone;
        Button^ btnIngAdd;
        Button^ btnIngUpdate;
        Button^ btnIngDelete;
        Button^ btnIngClear;
        TextBox^ txtIngSearch;
        CheckBox^ chkIngLowStockOnly;
        DataGridView^ dgvIngredients;

        // --- Screen F: Sales Reports Controls ---
        Panel^ pnlScreenReports;
        Panel^ pnlRepFilter;
        Panel^ cardRep1;
        Panel^ cardRep2;
        Panel^ cardRep3;
        Label^ lblR1;
        Label^ lblR2;
        DateTimePicker^ dtpReportStart;
        DateTimePicker^ dtpReportEnd;
        Button^ btnReportFilter;
        Button^ btnReportToday;
        Button^ btnReportMonth;
        Button^ btnPreviewReport;
        Button^ btnPrintReport;
        System::Drawing::Printing::PrintDocument^ printReportDoc;
        Label^ lblReportRevenueVal;
        Label^ lblReportOrdersVal;
        Label^ lblReportAvgOrderVal;
        DataGridView^ dgvReportTopProducts;
        DataGridView^ dgvReportDaily;
        Panel^ pnlReportChart;

        // --- UI Construction Helpers (Managed Member Methods) ---
        Button^ CreateNavButton(String^ text, int y, EventHandler^ handler);
        Panel^ CreateKpiCard(String^ title, Color borderCol, int x, int y, Label^% outValLabel);
        Button^ CreateActionButton(String^ text, Color bg, int x, int y, EventHandler^ handler);
        void AddFieldLabel(Panel^ parent, String^ text, int y);
        Panel^ CreateReportCard(String^ title, Color borderCol, int x, int y, Label^% outValLabel);

        // --- Initialization & Helper Routines ---
        void InitializeComponent(void);
        void InitializeCustomLayout(void);

        void SwitchScreen(Panel^ targetPanel, String^ screenTitle);
        void UpdateSystemStatusIndicator();
        void ResetPosOrder();
        Image^ SafeLoadImage(String^ relativeOrFullPath);

        // --- Role-Based Access Control ---
        void ApplyRolePermissions();
        void OnLogoutClick(Object^ sender, EventArgs^ e);

        // --- Grid Image & Layout Helpers ---
        void PopulateProductGridImages();
        void PopulatePosGridImages();
        void FormatPosCartGrid();
        void FormatCustomerGrid();
        void FormatInventoryGrid();
        void FormatReportGrids();
        void FormatDashboardGrids();
        void UpdateScreenLayout();
        void OnFormResize(Object^ sender, EventArgs^ e);

        // --- Event Handlers ---
        void OnNavClick(Object^ sender, EventArgs^ e);
        void OnClockTick(Object^ sender, EventArgs^ e);

        // Dashboard
        void LoadDashboardData();

        // Products
        void LoadProductsData();
        void OnProductSearchChanged(Object^ sender, EventArgs^ e);
        void OnProductSelectionChanged(Object^ sender, EventArgs^ e);
        void OnBrowseProductImage(Object^ sender, EventArgs^ e);
        void OnProductAdd(Object^ sender, EventArgs^ e);
        void OnProductUpdate(Object^ sender, EventArgs^ e);
        void OnProductDelete(Object^ sender, EventArgs^ e);
        void OnProductClear(Object^ sender, EventArgs^ e);

        // Orders & POS
        void LoadPosData();
        void OnPosFilterChanged(Object^ sender, EventArgs^ e);
        void OnPosAddToCart(Object^ sender, EventArgs^ e);
        void OnPosCartCellClick(Object^ sender, DataGridViewCellEventArgs^ e);
        void OnPosDiscountChanged(Object^ sender, EventArgs^ e);
        void OnPosPaymentModeChanged(Object^ sender, EventArgs^ e);
        void OnCashTenderedChanged(Object^ sender, EventArgs^ e);
        void OnPosCheckout(Object^ sender, EventArgs^ e);
        void OnPosClearCart(Object^ sender, EventArgs^ e);
        void OnPosViewReceipt(Object^ sender, EventArgs^ e);

        // Customers
        void LoadCustomersData();
        void OnCustomerSearchChanged(Object^ sender, EventArgs^ e);
        void OnCustomerSelectionChanged(Object^ sender, EventArgs^ e);
        void OnCustomerAdd(Object^ sender, EventArgs^ e);
        void OnCustomerUpdate(Object^ sender, EventArgs^ e);
        void OnCustomerDelete(Object^ sender, EventArgs^ e);
        void OnCustomerClear(Object^ sender, EventArgs^ e);

        // Inventory
        void LoadInventoryData();
        void OnInventorySearchChanged(Object^ sender, EventArgs^ e);
        void OnInventorySelectionChanged(Object^ sender, EventArgs^ e);
        void OnIngredientAdd(Object^ sender, EventArgs^ e);
        void OnIngredientUpdate(Object^ sender, EventArgs^ e);
        void OnIngredientDelete(Object^ sender, EventArgs^ e);
        void OnIngredientClear(Object^ sender, EventArgs^ e);
        void OnInventoryCellFormatting(Object^ sender, DataGridViewCellFormattingEventArgs^ e);

        // Reports
        void LoadReportsData();
        void OnReportFilter(Object^ sender, EventArgs^ e);
        void OnReportToday(Object^ sender, EventArgs^ e);
        void OnReportMonth(Object^ sender, EventArgs^ e);
        void OnPrintReportClick(Object^ sender, EventArgs^ e);
        void OnPreviewReportClick(Object^ sender, EventArgs^ e);
        void OnPrintReportPage(Object^ sender, System::Drawing::Printing::PrintPageEventArgs^ e);
        void OnPaintReportChart(Object^ sender, PaintEventArgs^ e);
    };

}
