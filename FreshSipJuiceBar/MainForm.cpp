#include "MainForm.h"
#include "Forms/LoginForm.h"

using namespace System;
using namespace System::IO;
using namespace System::Drawing;
using namespace System::Drawing::Drawing2D;
using namespace System::Windows::Forms;
using namespace FreshSipJuiceBar;
using namespace FreshSipJuiceBar::Models;
using namespace FreshSipJuiceBar::Services;

MainForm::MainForm(void) {
    m_currentRole = "Admin";
    m_currentUserName = "Chamara Bandara";
    m_isLoggingOut = false;
    InitializeComponent();
    InitializeCustomLayout();
    this->Resize += gcnew EventHandler(this, &MainForm::OnFormResize);
    UpdateScreenLayout();
    try {
        DatabaseManager::InitializeConnectionString();
        UpdateSystemStatusIndicator();
        ApplyRolePermissions();
        ResetPosOrder();
        LoadDashboardData();
        LoadPosData();
    }
    catch (Exception^) {
        UpdateSystemStatusIndicator();
        ApplyRolePermissions();
        ResetPosOrder();
    }
}

MainForm::MainForm(String^ initialRole, String^ initialUserName) {
    m_currentRole = String::IsNullOrEmpty(initialRole) ? "Admin" : initialRole;
    m_currentUserName = String::IsNullOrEmpty(initialUserName) ? (m_currentRole == "Cashier" ? "Anuki Samarasinghe" : "Chamara Bandara") : initialUserName;
    m_isLoggingOut = false;
    InitializeComponent();
    InitializeCustomLayout();
    this->Resize += gcnew EventHandler(this, &MainForm::OnFormResize);
    UpdateScreenLayout();
    try {
        DatabaseManager::InitializeConnectionString();
        UpdateSystemStatusIndicator();
        ApplyRolePermissions();
        ResetPosOrder();
        LoadDashboardData();
        LoadPosData();
    }
    catch (Exception^) {
        UpdateSystemStatusIndicator();
        ApplyRolePermissions();
        ResetPosOrder();
    }
}

MainForm::~MainForm() {
    if (components) {
        delete components;
    }
}

void MainForm::InitializeComponent(void) {
    this->components = gcnew System::ComponentModel::Container();
    this->timerClock = gcnew System::Windows::Forms::Timer(this->components);
    this->timerClock->Interval = 1000;
    this->timerClock->Tick += gcnew EventHandler(this, &MainForm::OnClockTick);
    this->timerClock->Start();

    this->SuspendLayout();
    this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::None;
    this->ClientSize = System::Drawing::Size(1024, 580);
    this->MinimumSize = System::Drawing::Size(980, 540);
    this->Text = "FreshSip Juice Bar Management System";
    this->StartPosition = FormStartPosition::CenterScreen;
    this->BackColor = Color::FromArgb(255, 249, 240);
    this->ResumeLayout(false);
}

Image^ MainForm::SafeLoadImage(String^ relativeOrFullPath) {
    if (String::IsNullOrWhiteSpace(relativeOrFullPath)) return nullptr;
    try {
        if (File::Exists(relativeOrFullPath)) {
            array<unsigned char>^ bytes = File::ReadAllBytes(relativeOrFullPath);
            MemoryStream^ ms = gcnew MemoryStream(bytes);
            return Image::FromStream(ms);
        }
        String^ appDir = AppDomain::CurrentDomain->BaseDirectory;
        String^ full = Path::Combine(appDir, relativeOrFullPath);
        if (File::Exists(full)) {
            array<unsigned char>^ bytes = File::ReadAllBytes(full);
            MemoryStream^ ms = gcnew MemoryStream(bytes);
            return Image::FromStream(ms);
        }
        String^ devPath = Path::Combine(appDir, "..\\..\\" + relativeOrFullPath);
        if (File::Exists(devPath)) {
            array<unsigned char>^ bytes = File::ReadAllBytes(devPath);
            MemoryStream^ ms = gcnew MemoryStream(bytes);
            return Image::FromStream(ms);
        }
    }
    catch (Exception^) {}
    return nullptr;
}

Button^ MainForm::CreateNavButton(String^ text, int y, EventHandler^ handler) {
    Button^ btn = gcnew Button();
    btn->UseMnemonic = false;
    btn->Text = text;
    btn->Location = Point(8, y);
    btn->Size = System::Drawing::Size(170, 36);
    btn->FlatStyle = FlatStyle::Flat;
    btn->FlatAppearance->BorderSize = 0;
    btn->BackColor = Color::FromArgb(45, 58, 65);
    btn->ForeColor = Color::White;
    btn->Font = gcnew System::Drawing::Font("Segoe UI", 9.0f, FontStyle::Bold);
    btn->TextAlign = ContentAlignment::MiddleLeft;
    btn->Padding = System::Windows::Forms::Padding(10, 0, 0, 0);
    btn->Cursor = Cursors::Hand;
    btn->Click += handler;
    this->pnlSidebar->Controls->Add(btn);
    return btn;
}

Panel^ MainForm::CreateKpiCard(String^ title, Color borderCol, int x, int y, Label^% outValLabel) {
    Panel^ card = gcnew Panel();
    card->Location = Point(x, y);
    card->Size = System::Drawing::Size(190, 70);
    card->BackColor = Color::White;
    card->BorderStyle = BorderStyle::FixedSingle;

    Label^ lblT = gcnew Label();
    lblT->Text = title;
    lblT->Font = gcnew System::Drawing::Font("Segoe UI", 7.5f, FontStyle::Bold);
    lblT->ForeColor = Color::FromArgb(110, 120, 130);
    lblT->Location = Point(10, 7);
    lblT->AutoSize = true;
    card->Controls->Add(lblT);

    outValLabel = gcnew Label();
    outValLabel->Text = "--";
    outValLabel->Font = gcnew System::Drawing::Font("Segoe UI", 13.5f, FontStyle::Bold);
    outValLabel->ForeColor = borderCol;
    outValLabel->Location = Point(8, 28);
    outValLabel->AutoSize = true;
    card->Controls->Add(outValLabel);

    this->pnlScreenDashboard->Controls->Add(card);
    return card;
}

Button^ MainForm::CreateActionButton(String^ text, Color bg, int x, int y, EventHandler^ handler) {
    Button^ btn = gcnew Button();
    btn->Text = text;
    btn->BackColor = bg;
    btn->ForeColor = Color::White;
    btn->Font = gcnew System::Drawing::Font("Segoe UI", 9.0f, FontStyle::Bold);
    btn->FlatStyle = FlatStyle::Flat;
    btn->FlatAppearance->BorderSize = 0;
    btn->Location = Point(x, y);
    btn->Size = System::Drawing::Size(135, 32);
    btn->Cursor = Cursors::Hand;
    btn->Click += handler;
    this->pnlScreenDashboard->Controls->Add(btn);
    return btn;
}

void MainForm::AddFieldLabel(Panel^ parent, String^ text, int y) {
    Label^ l = gcnew Label();
    l->Text = text;
    l->Font = gcnew System::Drawing::Font("Segoe UI", 9, FontStyle::Regular);
    l->ForeColor = Color::FromArgb(80, 90, 100);
    l->Location = Point(16, y);
    l->AutoSize = true;
    parent->Controls->Add(l);
}

Panel^ MainForm::CreateReportCard(String^ title, Color borderCol, int x, int y, Label^% outValLabel) {
    Panel^ card = gcnew Panel();
    card->Location = Point(x, y);
    card->Size = System::Drawing::Size(250, 72);
    card->BackColor = Color::White;
    card->BorderStyle = BorderStyle::FixedSingle;

    Label^ lblT = gcnew Label();
    lblT->Text = title;
    lblT->Font = gcnew System::Drawing::Font("Segoe UI", 8.5f, FontStyle::Bold);
    lblT->ForeColor = Color::FromArgb(110, 120, 130);
    lblT->Location = Point(12, 8);
    lblT->AutoSize = true;
    card->Controls->Add(lblT);

    outValLabel = gcnew Label();
    outValLabel->Text = "--";
    outValLabel->Font = gcnew System::Drawing::Font("Segoe UI", 14.0f, FontStyle::Bold);
    outValLabel->ForeColor = borderCol;
    outValLabel->Location = Point(10, 30);
    outValLabel->AutoSize = true;
    card->Controls->Add(outValLabel);

    this->pnlScreenReports->Controls->Add(card);
    return card;
}

void MainForm::InitializeCustomLayout(void) {
    // -------------------------------------------------------------
    // 1. Sidebar Panel (Left)
    // -------------------------------------------------------------
    pnlSidebar = gcnew Panel();
    pnlSidebar->Dock = DockStyle::Left;
    pnlSidebar->Width = 185;
    pnlSidebar->BackColor = Color::FromArgb(38, 50, 56);
    this->Controls->Add(pnlSidebar);

    // Brand Logo & Subtitle
    lblBrandLogo = gcnew Label();
    lblBrandLogo->Text = "FreshSip";
    lblBrandLogo->Font = gcnew System::Drawing::Font("Segoe UI", 17, FontStyle::Bold);
    lblBrandLogo->ForeColor = Color::FromArgb(255, 159, 67);
    lblBrandLogo->Location = Point(14, 14);
    lblBrandLogo->AutoSize = true;
    pnlSidebar->Controls->Add(lblBrandLogo);

    lblBrandSubtitle = gcnew Label();
    lblBrandSubtitle->Text = "Juice Bar POS & Management";
    lblBrandSubtitle->Font = gcnew System::Drawing::Font("Segoe UI", 8.0f, FontStyle::Regular);
    lblBrandSubtitle->ForeColor = Color::FromArgb(180, 200, 210);
    lblBrandSubtitle->Location = Point(16, 44);
    lblBrandSubtitle->AutoSize = true;
    pnlSidebar->Controls->Add(lblBrandSubtitle);

    int btnY = 78;
    int btnH = 36;
    int spacing = 5;

    btnNavDashboard = CreateNavButton("  Dashboard", btnY, gcnew EventHandler(this, &MainForm::OnNavClick)); btnY += btnH + spacing;
    btnNavProducts = CreateNavButton("  Products Menu", btnY, gcnew EventHandler(this, &MainForm::OnNavClick)); btnY += btnH + spacing;
    btnNavOrders = CreateNavButton("  Orders & POS", btnY, gcnew EventHandler(this, &MainForm::OnNavClick)); btnY += btnH + spacing;
    btnNavCustomers = CreateNavButton("  Customers", btnY, gcnew EventHandler(this, &MainForm::OnNavClick)); btnY += btnH + spacing;
    btnNavInventory = CreateNavButton("  Inventory", btnY, gcnew EventHandler(this, &MainForm::OnNavClick)); btnY += btnH + spacing;
    btnNavReports = CreateNavButton("  Sales Reports", btnY, gcnew EventHandler(this, &MainForm::OnNavClick)); btnY += btnH + spacing;

    // -------------------------------------------------------------
    // 2. Header Panel (Top)
    // -------------------------------------------------------------
    pnlHeader = gcnew Panel();
    pnlHeader->Dock = DockStyle::Top;
    pnlHeader->Height = 46;
    pnlHeader->BackColor = Color::White;
    this->Controls->Add(pnlHeader);

    lblBreadcrumb = gcnew Label();
    lblBreadcrumb->UseMnemonic = false;
    lblBreadcrumb->Text = "Dashboard Overview";
    lblBreadcrumb->Font = gcnew System::Drawing::Font("Segoe UI", 12.0f, FontStyle::Bold);
    lblBreadcrumb->ForeColor = Color::FromArgb(46, 139, 87);
    lblBreadcrumb->Location = Point(15, 12);
    lblBreadcrumb->AutoSize = true;
    pnlHeader->Controls->Add(lblBreadcrumb);

    Panel^ pnlHeaderRight = gcnew Panel();
    pnlHeaderRight->Dock = DockStyle::Right;
    pnlHeaderRight->Width = 590;
    pnlHeaderRight->BackColor = Color::Transparent;
    pnlHeader->Controls->Add(pnlHeaderRight);

    lblSystemStatus = gcnew Label();
    lblSystemStatus->Text = L"\x25CF System: Offline (No DB)";
    lblSystemStatus->Font = gcnew System::Drawing::Font("Segoe UI", 8.5f, FontStyle::Bold);
    lblSystemStatus->ForeColor = Color::FromArgb(220, 53, 69);
    lblSystemStatus->Location = Point(5, 15);
    lblSystemStatus->AutoSize = true;
    pnlHeaderRight->Controls->Add(lblSystemStatus);

    lblCurrentUserBadge = gcnew Label();
    lblCurrentUserBadge->Text = "User: [Admin] Chamara Bandara";
    lblCurrentUserBadge->Font = gcnew System::Drawing::Font("Segoe UI", 8.5f, FontStyle::Bold);
    lblCurrentUserBadge->ForeColor = Color::FromArgb(210, 105, 30);
    lblCurrentUserBadge->Location = Point(165, 15);
    lblCurrentUserBadge->AutoSize = true;
    pnlHeaderRight->Controls->Add(lblCurrentUserBadge);

    btnLogout = gcnew Button();
    btnLogout->Text = "Logout";
    btnLogout->Font = gcnew System::Drawing::Font("Segoe UI", 8.0f, FontStyle::Bold);
    btnLogout->ForeColor = Color::White;
    btnLogout->BackColor = Color::FromArgb(211, 47, 47);
    btnLogout->FlatStyle = FlatStyle::Flat;
    btnLogout->FlatAppearance->BorderSize = 0;
    btnLogout->Size = System::Drawing::Size(64, 26);
    btnLogout->Location = Point(365, 10);
    btnLogout->Cursor = Cursors::Hand;
    btnLogout->Click += gcnew EventHandler(this, &MainForm::OnLogoutClick);
    pnlHeaderRight->Controls->Add(btnLogout);

    lblClock = gcnew Label();
    lblClock->Font = gcnew System::Drawing::Font("Segoe UI", 8.5f, FontStyle::Regular);
    lblClock->ForeColor = Color::FromArgb(100, 115, 125);
    lblClock->Location = Point(440, 15);
    lblClock->AutoSize = true;
    lblClock->Text = DateTime::Now.ToString("yyyy-MM-dd HH:mm:ss");
    pnlHeaderRight->Controls->Add(lblClock);

    // -------------------------------------------------------------
    // 3. Central Content Container
    // -------------------------------------------------------------
    pnlContentContainer = gcnew Panel();
    pnlContentContainer->Dock = DockStyle::Fill;
    pnlContentContainer->BackColor = Color::FromArgb(255, 249, 240);
    pnlContentContainer->AutoScroll = false;
    this->Controls->Add(pnlContentContainer);
    pnlContentContainer->BringToFront();

    // =============================================================
    // BUILD SCREEN A: DASHBOARD
    // =============================================================
    pnlScreenDashboard = gcnew Panel();
    pnlScreenDashboard->Dock = DockStyle::Fill;
    pnlScreenDashboard->AutoScroll = false;
    pnlScreenDashboard->Padding = System::Windows::Forms::Padding(15);
    pnlContentContainer->Controls->Add(pnlScreenDashboard);

    picDashboardBanner = gcnew PictureBox();
    picDashboardBanner->Location = Point(10, 8);
    picDashboardBanner->Size = System::Drawing::Size(815, 95);
    picDashboardBanner->Anchor = AnchorStyles::Top | AnchorStyles::Left | AnchorStyles::Right;
    picDashboardBanner->SizeMode = PictureBoxSizeMode::StretchImage;
    picDashboardBanner->Image = SafeLoadImage("Assets/Banners/juice_bar_banner.png");
    picDashboardBanner->BorderStyle = BorderStyle::None;
    pnlScreenDashboard->Controls->Add(picDashboardBanner);

    int cardY = 112;
    cardKpiTodaySales = CreateKpiCard("TODAY'S REVENUE", Color::FromArgb(46, 139, 87), 10, cardY, lblKpiSalesVal);
    cardKpiTodayOrders = CreateKpiCard("TODAY'S ORDERS", Color::FromArgb(255, 159, 67), 215, cardY, lblKpiOrdersVal);
    cardKpiProducts = CreateKpiCard("ACTIVE PRODUCTS", Color::FromArgb(33, 150, 243), 420, cardY, lblKpiProductsVal);
    cardKpiLowStock = CreateKpiCard("LOW STOCK INGREDIENTS", Color::FromArgb(239, 83, 80), 625, cardY, lblKpiLowStockVal);

    int qaY = 188;
    Label^ lblQa = gcnew Label();
    lblQa->Text = "Quick Actions:";
    lblQa->Font = gcnew System::Drawing::Font("Segoe UI", 9.0f, FontStyle::Bold);
    lblQa->ForeColor = Color::FromArgb(38, 50, 56);
    lblQa->Location = Point(10, qaY + 5);
    lblQa->AutoSize = true;
    pnlScreenDashboard->Controls->Add(lblQa);

    btnDashQuickOrder = CreateActionButton("+ New POS Order", Color::FromArgb(46, 139, 87), 115, qaY, gcnew EventHandler(this, &MainForm::OnNavClick));
    btnDashQuickProduct = CreateActionButton("+ Add Product", Color::FromArgb(255, 159, 67), 260, qaY, gcnew EventHandler(this, &MainForm::OnNavClick));
    btnDashQuickRestock = CreateActionButton("Restock Alert", Color::FromArgb(38, 50, 56), 405, qaY, gcnew EventHandler(this, &MainForm::OnNavClick));

    int gridY = 226;
    int gridH = 280;

    lblDashT1 = gcnew Label();
    lblDashT1->Text = "Top Selling Juice Blends";
    lblDashT1->Font = gcnew System::Drawing::Font("Segoe UI", 10.0f, FontStyle::Bold);
    lblDashT1->ForeColor = Color::FromArgb(46, 139, 87);
    lblDashT1->Location = Point(10, gridY);
    lblDashT1->AutoSize = true;
    pnlScreenDashboard->Controls->Add(lblDashT1);

    dgvDashTopProducts = gcnew DataGridView();
    dgvDashTopProducts->Location = Point(10, gridY + 22);
    dgvDashTopProducts->Size = System::Drawing::Size(395, gridH);
    dgvDashTopProducts->BackgroundColor = Color::White;
    dgvDashTopProducts->ReadOnly = true;
    dgvDashTopProducts->AllowUserToAddRows = false;
    dgvDashTopProducts->RowHeadersVisible = false;
    dgvDashTopProducts->SelectionMode = DataGridViewSelectionMode::FullRowSelect;
    dgvDashTopProducts->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::None;
    dgvDashTopProducts->ScrollBars = ScrollBars::Both;
    pnlScreenDashboard->Controls->Add(dgvDashTopProducts);

    lblDashT2 = gcnew Label();
    lblDashT2->Text = "Recent Customer Orders";
    lblDashT2->Font = gcnew System::Drawing::Font("Segoe UI", 10.0f, FontStyle::Bold);
    lblDashT2->ForeColor = Color::FromArgb(46, 139, 87);
    lblDashT2->Location = Point(415, gridY);
    lblDashT2->AutoSize = true;
    pnlScreenDashboard->Controls->Add(lblDashT2);

    dgvDashRecentOrders = gcnew DataGridView();
    dgvDashRecentOrders->Location = Point(415, gridY + 22);
    dgvDashRecentOrders->Size = System::Drawing::Size(410, gridH);
    dgvDashRecentOrders->BackgroundColor = Color::White;
    dgvDashRecentOrders->ReadOnly = true;
    dgvDashRecentOrders->AllowUserToAddRows = false;
    dgvDashRecentOrders->RowHeadersVisible = false;
    dgvDashRecentOrders->SelectionMode = DataGridViewSelectionMode::FullRowSelect;
    dgvDashRecentOrders->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::None;
    dgvDashRecentOrders->ScrollBars = ScrollBars::Both;
    pnlScreenDashboard->Controls->Add(dgvDashRecentOrders);

    // =============================================================
    // BUILD SCREEN B: PRODUCT MANAGEMENT (Full CRUD)
    // =============================================================
    pnlScreenProducts = gcnew Panel();
    pnlScreenProducts->Dock = DockStyle::Fill;
    pnlScreenProducts->AutoScroll = false;
    pnlScreenProducts->Visible = false;
    pnlContentContainer->Controls->Add(pnlScreenProducts);

    pnlProdForm = gcnew Panel();
    pnlProdForm->Location = Point(10, 10);
    pnlProdForm->Size = System::Drawing::Size(310, 510);
    pnlProdForm->BackColor = Color::White;
    pnlProdForm->BorderStyle = BorderStyle::FixedSingle;
    pnlProdForm->AutoScroll = false;
    pnlScreenProducts->Controls->Add(pnlProdForm);

    Label^ lblProdFormTitle = gcnew Label();
    lblProdFormTitle->Text = "Product Editor";
    lblProdFormTitle->Font = gcnew System::Drawing::Font("Segoe UI", 12, FontStyle::Bold);
    lblProdFormTitle->ForeColor = Color::FromArgb(46, 139, 87);
    lblProdFormTitle->Location = Point(14, 10);
    lblProdFormTitle->AutoSize = true;
    pnlProdForm->Controls->Add(lblProdFormTitle);

    int pfY = 38;
    AddFieldLabel(pnlProdForm, "Product ID (Auto):", pfY);
    txtProdId = gcnew TextBox();
    txtProdId->Location = Point(14, pfY + 18);
    txtProdId->Size = System::Drawing::Size(100, 22);
    txtProdId->ReadOnly = true;
    txtProdId->BackColor = Color::FromArgb(240, 244, 248);
    pnlProdForm->Controls->Add(txtProdId);
    pfY += 44;

    AddFieldLabel(pnlProdForm, "Product Name *:", pfY);
    txtProdName = gcnew TextBox();
    txtProdName->Location = Point(14, pfY + 18);
    txtProdName->Size = System::Drawing::Size(285, 22);
    pnlProdForm->Controls->Add(txtProdName);
    pfY += 44;

    AddFieldLabel(pnlProdForm, "Category *:", pfY);
    cmbProdCategory = gcnew ComboBox();
    cmbProdCategory->Location = Point(14, pfY + 18);
    cmbProdCategory->Size = System::Drawing::Size(285, 22);
    cmbProdCategory->DropDownStyle = ComboBoxStyle::DropDownList;
    cmbProdCategory->Items->AddRange(gcnew array<Object^> { "Fresh Juices", "Smoothies", "Detox Blends", "Specialty", "Fruit Bowls" });
    cmbProdCategory->SelectedIndex = 0;
    pnlProdForm->Controls->Add(cmbProdCategory);
    pfY += 44;

    AddFieldLabel(pnlProdForm, "Description:", pfY);
    txtProdDesc = gcnew TextBox();
    txtProdDesc->Location = Point(14, pfY + 18);
    txtProdDesc->Size = System::Drawing::Size(285, 36);
    txtProdDesc->Multiline = true;
    pnlProdForm->Controls->Add(txtProdDesc);
    pfY += 58;

    AddFieldLabel(pnlProdForm, "Price (LKR) *:", pfY);
    txtProdPrice = gcnew TextBox();
    txtProdPrice->Location = Point(14, pfY + 18);
    txtProdPrice->Size = System::Drawing::Size(125, 22);
    txtProdPrice->Text = "350.00";
    pnlProdForm->Controls->Add(txtProdPrice);

    Label^ lblQtyL = gcnew Label();
    lblQtyL->Text = "Stock Quantity *:";
    lblQtyL->Font = gcnew System::Drawing::Font("Segoe UI", 8.5f, FontStyle::Regular);
    lblQtyL->ForeColor = Color::FromArgb(80, 90, 100);
    lblQtyL->Location = Point(155, pfY);
    lblQtyL->AutoSize = true;
    pnlProdForm->Controls->Add(lblQtyL);

    nudProdQty = gcnew NumericUpDown();
    nudProdQty->Location = Point(155, pfY + 18);
    nudProdQty->Size = System::Drawing::Size(144, 22);
    nudProdQty->Maximum = 9999;
    nudProdQty->Value = 50;
    pnlProdForm->Controls->Add(nudProdQty);
    pfY += 44;

    // Image Photo Picker (Image Path string is completely HIDDEN - only actual photo is displayed)
    AddFieldLabel(pnlProdForm, "Product Photo:", pfY);
    btnProdBrowseImg = gcnew Button();
    btnProdBrowseImg->UseMnemonic = false;
    btnProdBrowseImg->Text = "Browse Photo...";
    btnProdBrowseImg->Location = Point(165, pfY - 3);
    btnProdBrowseImg->Size = System::Drawing::Size(134, 24);
    btnProdBrowseImg->Font = gcnew System::Drawing::Font("Segoe UI", 8.0f, FontStyle::Bold);
    btnProdBrowseImg->BackColor = Color::FromArgb(235, 245, 240);
    btnProdBrowseImg->FlatStyle = FlatStyle::Flat;
    btnProdBrowseImg->Click += gcnew EventHandler(this, &MainForm::OnBrowseProductImage);
    pnlProdForm->Controls->Add(btnProdBrowseImg);

    txtProdImagePath = gcnew TextBox();
    txtProdImagePath->Visible = false; // Never show the raw path string to users
    pnlProdForm->Controls->Add(txtProdImagePath);
    pfY += 24;

    picProdPreview = gcnew PictureBox();
    picProdPreview->Location = Point(14, pfY);
    picProdPreview->Size = System::Drawing::Size(285, 90);
    picProdPreview->BorderStyle = BorderStyle::FixedSingle;
    picProdPreview->SizeMode = PictureBoxSizeMode::Zoom;
    picProdPreview->BackColor = Color::FromArgb(250, 252, 255);
    pnlProdForm->Controls->Add(picProdPreview);
    pfY += 96;

    chkProdActive = gcnew CheckBox();
    chkProdActive->Text = "Active & Available for Sale";
    chkProdActive->Location = Point(14, pfY);
    chkProdActive->AutoSize = true;
    chkProdActive->Checked = true;
    pnlProdForm->Controls->Add(chkProdActive);
    pfY += 28;

    btnProdAdd = gcnew Button();
    btnProdAdd->Text = "Add New";
    btnProdAdd->Location = Point(14, pfY);
    btnProdAdd->Size = System::Drawing::Size(68, 30);
    btnProdAdd->BackColor = Color::FromArgb(46, 139, 87);
    btnProdAdd->ForeColor = Color::White;
    btnProdAdd->FlatStyle = FlatStyle::Flat;
    btnProdAdd->FlatAppearance->BorderSize = 0;
    btnProdAdd->Click += gcnew EventHandler(this, &MainForm::OnProductAdd);
    pnlProdForm->Controls->Add(btnProdAdd);

    btnProdUpdate = gcnew Button();
    btnProdUpdate->Text = "Update";
    btnProdUpdate->Location = Point(88, pfY);
    btnProdUpdate->Size = System::Drawing::Size(68, 30);
    btnProdUpdate->BackColor = Color::FromArgb(255, 159, 67);
    btnProdUpdate->ForeColor = Color::White;
    btnProdUpdate->FlatStyle = FlatStyle::Flat;
    btnProdUpdate->FlatAppearance->BorderSize = 0;
    btnProdUpdate->Click += gcnew EventHandler(this, &MainForm::OnProductUpdate);
    pnlProdForm->Controls->Add(btnProdUpdate);

    btnProdDelete = gcnew Button();
    btnProdDelete->Text = "Delete";
    btnProdDelete->Location = Point(162, pfY);
    btnProdDelete->Size = System::Drawing::Size(68, 30);
    btnProdDelete->BackColor = Color::FromArgb(220, 53, 69);
    btnProdDelete->ForeColor = Color::White;
    btnProdDelete->FlatStyle = FlatStyle::Flat;
    btnProdDelete->FlatAppearance->BorderSize = 0;
    btnProdDelete->Click += gcnew EventHandler(this, &MainForm::OnProductDelete);
    pnlProdForm->Controls->Add(btnProdDelete);

    btnProdClear = gcnew Button();
    btnProdClear->Text = "Clear";
    btnProdClear->Location = Point(236, pfY);
    btnProdClear->Size = System::Drawing::Size(63, 30);
    btnProdClear->BackColor = Color::FromArgb(108, 117, 125);
    btnProdClear->ForeColor = Color::White;
    btnProdClear->FlatStyle = FlatStyle::Flat;
    btnProdClear->FlatAppearance->BorderSize = 0;
    btnProdClear->Click += gcnew EventHandler(this, &MainForm::OnProductClear);
    pnlProdForm->Controls->Add(btnProdClear);

    lblSearchProd = gcnew Label();
    lblSearchProd->Text = "Search:";
    lblSearchProd->Location = Point(330, 14);
    lblSearchProd->AutoSize = true;
    lblSearchProd->Font = gcnew System::Drawing::Font("Segoe UI", 9.0f, FontStyle::Bold);
    pnlScreenProducts->Controls->Add(lblSearchProd);

    txtProdSearch = gcnew TextBox();
    txtProdSearch->Location = Point(385, 11);
    txtProdSearch->Size = System::Drawing::Size(140, 22);
    txtProdSearch->TextChanged += gcnew EventHandler(this, &MainForm::OnProductSearchChanged);
    pnlScreenProducts->Controls->Add(txtProdSearch);

    lblFilterCat = gcnew Label();
    lblFilterCat->Text = "Category:";
    lblFilterCat->Location = Point(535, 14);
    lblFilterCat->AutoSize = true;
    lblFilterCat->Font = gcnew System::Drawing::Font("Segoe UI", 9.0f, FontStyle::Bold);
    pnlScreenProducts->Controls->Add(lblFilterCat);

    cmbProdFilter = gcnew ComboBox();
    cmbProdFilter->Location = Point(600, 11);
    cmbProdFilter->Size = System::Drawing::Size(165, 22);
    cmbProdFilter->DropDownStyle = ComboBoxStyle::DropDownList;
    cmbProdFilter->Items->AddRange(gcnew array<Object^> { "All Categories", "Fresh Juices", "Smoothies", "Detox Blends", "Specialty", "Fruit Bowls" });
    cmbProdFilter->SelectedIndex = 0;
    cmbProdFilter->SelectedIndexChanged += gcnew EventHandler(this, &MainForm::OnProductSearchChanged);
    pnlScreenProducts->Controls->Add(cmbProdFilter);

    dgvProducts = gcnew DataGridView();
    dgvProducts->Location = Point(330, 40);
    dgvProducts->Size = System::Drawing::Size(500, 480);
    dgvProducts->BackgroundColor = Color::White;
    dgvProducts->ReadOnly = true;
    dgvProducts->AllowUserToAddRows = false;
    dgvProducts->RowHeadersVisible = false;
    dgvProducts->SelectionMode = DataGridViewSelectionMode::FullRowSelect;
    dgvProducts->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::None;
    dgvProducts->ScrollBars = ScrollBars::Both;
    dgvProducts->RowTemplate->Height = 48;
    dgvProducts->SelectionChanged += gcnew EventHandler(this, &MainForm::OnProductSelectionChanged);
    pnlScreenProducts->Controls->Add(dgvProducts);

    // =============================================================
    // BUILD SCREEN C: ORDERS & POS BILLING
    // =============================================================
    pnlScreenOrders = gcnew Panel();
    pnlScreenOrders->Dock = DockStyle::Fill;
    pnlScreenOrders->AutoScroll = false;
    pnlScreenOrders->Visible = false;
    pnlContentContainer->Controls->Add(pnlScreenOrders);

    pnlPosCatalog = gcnew Panel();
    pnlPosCatalog->Location = Point(10, 10);
    pnlPosCatalog->Size = System::Drawing::Size(440, 510);
    pnlPosCatalog->BackColor = Color::White;
    pnlPosCatalog->BorderStyle = BorderStyle::FixedSingle;
    pnlPosCatalog->AutoScroll = false;
    pnlScreenOrders->Controls->Add(pnlPosCatalog);

    Label^ lblPosCatTitle = gcnew Label();
    lblPosCatTitle->Text = "Juice Catalog";
    lblPosCatTitle->Font = gcnew System::Drawing::Font("Segoe UI", 11.5f, FontStyle::Bold);
    lblPosCatTitle->ForeColor = Color::FromArgb(46, 139, 87);
    lblPosCatTitle->Location = Point(10, 10);
    lblPosCatTitle->AutoSize = true;
    pnlPosCatalog->Controls->Add(lblPosCatTitle);

    txtPosSearch = gcnew TextBox();
    txtPosSearch->Location = Point(10, 36);
    txtPosSearch->Size = System::Drawing::Size(175, 24);
    txtPosSearch->TextChanged += gcnew EventHandler(this, &MainForm::OnPosFilterChanged);
    pnlPosCatalog->Controls->Add(txtPosSearch);

    cmbPosCategory = gcnew ComboBox();
    cmbPosCategory->Location = Point(190, 36);
    cmbPosCategory->Size = System::Drawing::Size(180, 24);
    cmbPosCategory->DropDownStyle = ComboBoxStyle::DropDownList;
    cmbPosCategory->Items->AddRange(gcnew array<Object^> { "All Categories", "Fresh Juices", "Smoothies", "Detox Blends", "Specialty", "Fruit Bowls" });
    cmbPosCategory->SelectedIndex = 0;
    cmbPosCategory->SelectedIndexChanged += gcnew EventHandler(this, &MainForm::OnPosFilterChanged);
    pnlPosCatalog->Controls->Add(cmbPosCategory);

    dgvPosProducts = gcnew DataGridView();
    dgvPosProducts->Location = Point(10, 68);
    dgvPosProducts->Size = System::Drawing::Size(420, 388);
    dgvPosProducts->BackgroundColor = Color::FromArgb(250, 252, 255);
    dgvPosProducts->ReadOnly = true;
    dgvPosProducts->AllowUserToAddRows = false;
    dgvPosProducts->RowHeadersVisible = false;
    dgvPosProducts->SelectionMode = DataGridViewSelectionMode::FullRowSelect;
    dgvPosProducts->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::None;
    dgvPosProducts->ScrollBars = ScrollBars::Both;
    pnlPosCatalog->Controls->Add(dgvPosProducts);

    lblPosQty = gcnew Label();
    lblPosQty->Text = "Qty:";
    lblPosQty->Location = Point(10, 468);
    lblPosQty->AutoSize = true;
    lblPosQty->Font = gcnew System::Drawing::Font("Segoe UI", 9.0f, FontStyle::Bold);
    pnlPosCatalog->Controls->Add(lblPosQty);

    nudPosQty = gcnew NumericUpDown();
    nudPosQty->Location = Point(44, 466);
    nudPosQty->Size = System::Drawing::Size(58, 24);
    nudPosQty->Minimum = 1;
    nudPosQty->Maximum = 100;
    nudPosQty->Value = 1;
    pnlPosCatalog->Controls->Add(nudPosQty);

    btnPosAddToCart = gcnew Button();
    btnPosAddToCart->Text = "+ Add to Cart";
    btnPosAddToCart->Location = Point(110, 460);
    btnPosAddToCart->Size = System::Drawing::Size(260, 36);
    btnPosAddToCart->BackColor = Color::FromArgb(46, 139, 87);
    btnPosAddToCart->ForeColor = Color::White;
    btnPosAddToCart->Font = gcnew System::Drawing::Font("Segoe UI", 9.5f, FontStyle::Bold);
    btnPosAddToCart->FlatStyle = FlatStyle::Flat;
    btnPosAddToCart->FlatAppearance->BorderSize = 0;
    btnPosAddToCart->Cursor = Cursors::Hand;
    btnPosAddToCart->Click += gcnew EventHandler(this, &MainForm::OnPosAddToCart);
    pnlPosCatalog->Controls->Add(btnPosAddToCart);

    pnlPosCartPanel = gcnew Panel();
    pnlPosCartPanel->Location = Point(460, 10);
    pnlPosCartPanel->Size = System::Drawing::Size(370, 510);
    pnlPosCartPanel->BackColor = Color::White;
    pnlPosCartPanel->BorderStyle = BorderStyle::FixedSingle;
    pnlPosCartPanel->AutoScroll = false;
    pnlScreenOrders->Controls->Add(pnlPosCartPanel);

    lblPosOrderNumber = gcnew Label();
    lblPosOrderNumber->Text = "Order: ORD-0000";
    lblPosOrderNumber->Font = gcnew System::Drawing::Font("Segoe UI", 11.5f, FontStyle::Bold);
    lblPosOrderNumber->ForeColor = Color::FromArgb(38, 50, 56);
    lblPosOrderNumber->Location = Point(12, 10);
    lblPosOrderNumber->AutoSize = true;
    pnlPosCartPanel->Controls->Add(lblPosOrderNumber);

    lblCustL = gcnew Label();
    lblCustL->Text = "Customer:";
    lblCustL->Location = Point(12, 38);
    lblCustL->AutoSize = true;
    lblCustL->Font = gcnew System::Drawing::Font("Segoe UI", 8.5f, FontStyle::Regular);
    pnlPosCartPanel->Controls->Add(lblCustL);

    cmbPosCustomer = gcnew ComboBox();
    cmbPosCustomer->Location = Point(78, 34);
    cmbPosCustomer->Size = System::Drawing::Size(250, 24);
    cmbPosCustomer->DropDownStyle = ComboBoxStyle::DropDownList;
    pnlPosCartPanel->Controls->Add(cmbPosCustomer);

    btnPosNewCustomer = gcnew Button();
    btnPosNewCustomer->Text = "+ New";
    btnPosNewCustomer->Location = Point(338, 34);
    btnPosNewCustomer->Size = System::Drawing::Size(75, 25);
    btnPosNewCustomer->Click += gcnew EventHandler(this, &MainForm::OnNavClick);
    pnlPosCartPanel->Controls->Add(btnPosNewCustomer);

    dgvPosCart = gcnew DataGridView();
    dgvPosCart->Location = Point(12, 68);
    dgvPosCart->Size = System::Drawing::Size(346, 185);
    dgvPosCart->BackgroundColor = Color::FromArgb(255, 253, 248);
    dgvPosCart->ReadOnly = true;
    dgvPosCart->AllowUserToAddRows = false;
    dgvPosCart->RowHeadersVisible = false;
    dgvPosCart->SelectionMode = DataGridViewSelectionMode::FullRowSelect;
    dgvPosCart->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::None;
    dgvPosCart->ScrollBars = ScrollBars::Both;
    dgvPosCart->CellClick += gcnew DataGridViewCellEventHandler(this, &MainForm::OnPosCartCellClick);
    pnlPosCartPanel->Controls->Add(dgvPosCart);

    int totY = 260;
    lblSubT = gcnew Label();
    lblSubT->Text = "Subtotal:";
    lblSubT->Location = Point(12, totY);
    lblSubT->AutoSize = true;
    lblSubT->Font = gcnew System::Drawing::Font("Segoe UI", 9.0f);
    pnlPosCartPanel->Controls->Add(lblSubT);

    lblPosSubtotal = gcnew Label();
    lblPosSubtotal->Text = "LKR 0.00";
    lblPosSubtotal->Location = Point(285, totY);
    lblPosSubtotal->AutoSize = true;
    lblPosSubtotal->Font = gcnew System::Drawing::Font("Segoe UI", 9.0f, FontStyle::Bold);
    pnlPosCartPanel->Controls->Add(lblPosSubtotal);
    totY += 24;

    lblDiscT = gcnew Label();
    lblDiscT->Text = "Discount (LKR):";
    lblDiscT->Location = Point(12, totY);
    lblDiscT->AutoSize = true;
    lblDiscT->Font = gcnew System::Drawing::Font("Segoe UI", 9.0f);
    pnlPosCartPanel->Controls->Add(lblDiscT);

    nudPosDiscount = gcnew NumericUpDown();
    nudPosDiscount->Location = Point(285, totY - 2);
    nudPosDiscount->Size = System::Drawing::Size(115, 22);
    nudPosDiscount->Maximum = 99999;
    nudPosDiscount->DecimalPlaces = 2;
    nudPosDiscount->ValueChanged += gcnew EventHandler(this, &MainForm::OnPosDiscountChanged);
    pnlPosCartPanel->Controls->Add(nudPosDiscount);
    totY += 26;

    lblNetT = gcnew Label();
    lblNetT->Text = "FINAL TOTAL:";
    lblNetT->Location = Point(12, totY);
    lblNetT->AutoSize = true;
    lblNetT->Font = gcnew System::Drawing::Font("Segoe UI", 11.0f, FontStyle::Bold);
    lblNetT->ForeColor = Color::FromArgb(46, 139, 87);
    pnlPosCartPanel->Controls->Add(lblNetT);

    lblPosFinalTotal = gcnew Label();
    lblPosFinalTotal->Text = "LKR 0.00";
    lblPosFinalTotal->Location = Point(275, totY);
    lblPosFinalTotal->AutoSize = true;
    lblPosFinalTotal->Font = gcnew System::Drawing::Font("Segoe UI", 12.5f, FontStyle::Bold);
    lblPosFinalTotal->ForeColor = Color::FromArgb(46, 139, 87);
    pnlPosCartPanel->Controls->Add(lblPosFinalTotal);
    totY += 32;

    lblPayL = gcnew Label();
    lblPayL->Text = "Payment:";
    lblPayL->Location = Point(12, totY);
    lblPayL->AutoSize = true;
    lblPayL->Font = gcnew System::Drawing::Font("Segoe UI", 8.5f, FontStyle::Bold);
    pnlPosCartPanel->Controls->Add(lblPayL);

    rdoPayCash = gcnew RadioButton();
    rdoPayCash->Text = "Cash";
    rdoPayCash->Location = Point(100, totY - 2);
    rdoPayCash->AutoSize = true;
    rdoPayCash->Checked = true;
    rdoPayCash->CheckedChanged += gcnew EventHandler(this, &MainForm::OnPosPaymentModeChanged);
    pnlPosCartPanel->Controls->Add(rdoPayCash);

    rdoPayCard = gcnew RadioButton();
    rdoPayCard->Text = "Card";
    rdoPayCard->Location = Point(180, totY - 2);
    rdoPayCard->AutoSize = true;
    rdoPayCard->CheckedChanged += gcnew EventHandler(this, &MainForm::OnPosPaymentModeChanged);
    pnlPosCartPanel->Controls->Add(rdoPayCard);
    totY += 28;

    pnlCashDetails = gcnew Panel();
    pnlCashDetails->Location = Point(12, totY);
    pnlCashDetails->Size = System::Drawing::Size(396, 36);
    pnlPosCartPanel->Controls->Add(pnlCashDetails);

    Label^ lblCashTendL = gcnew Label();
    lblCashTendL->Text = "Tendered (LKR):";
    lblCashTendL->Location = Point(0, 6);
    lblCashTendL->AutoSize = true;
    pnlCashDetails->Controls->Add(lblCashTendL);

    txtCashTendered = gcnew TextBox();
    txtCashTendered->Location = Point(105, 4);
    txtCashTendered->Size = System::Drawing::Size(90, 22);
    txtCashTendered->Text = "0.00";
    txtCashTendered->TextChanged += gcnew EventHandler(this, &MainForm::OnCashTenderedChanged);
    pnlCashDetails->Controls->Add(txtCashTendered);

    lblCashChange = gcnew Label();
    lblCashChange->Text = "Change: LKR 0.00";
    lblCashChange->Font = gcnew System::Drawing::Font("Segoe UI", 9.0f, FontStyle::Bold);
    lblCashChange->ForeColor = Color::FromArgb(46, 139, 87);
    lblCashChange->Location = Point(205, 6);
    lblCashChange->AutoSize = true;
    pnlCashDetails->Controls->Add(lblCashChange);

    pnlCardDetails = gcnew Panel();
    pnlCardDetails->Location = Point(12, totY);
    pnlCardDetails->Size = System::Drawing::Size(396, 36);
    pnlCardDetails->Visible = false;
    pnlPosCartPanel->Controls->Add(pnlCardDetails);

    Label^ lblCardTypeL = gcnew Label();
    lblCardTypeL->Text = "Network:";
    lblCardTypeL->Location = Point(0, 6);
    lblCardTypeL->AutoSize = true;
    pnlCardDetails->Controls->Add(lblCardTypeL);

    cmbCardType = gcnew ComboBox();
    cmbCardType->Location = Point(65, 4);
    cmbCardType->Size = System::Drawing::Size(95, 22);
    cmbCardType->DropDownStyle = ComboBoxStyle::DropDownList;
    cmbCardType->Items->AddRange(gcnew array<Object^> { "Visa", "Mastercard", "Amex" });
    cmbCardType->SelectedIndex = 0;
    pnlCardDetails->Controls->Add(cmbCardType);

    Label^ lblLast4L = gcnew Label();
    lblLast4L->Text = "Last 4:";
    lblLast4L->Location = Point(175, 6);
    lblLast4L->AutoSize = true;
    pnlCardDetails->Controls->Add(lblLast4L);

    txtCardLast4 = gcnew TextBox();
    txtCardLast4->Location = Point(225, 4);
    txtCardLast4->Size = System::Drawing::Size(55, 22);
    txtCardLast4->MaxLength = 4;
    txtCardLast4->Text = "1234";
    pnlCardDetails->Controls->Add(txtCardLast4);

    totY += 46;

    btnPosCheckout = gcnew Button();
    btnPosCheckout->Text = "Confirm & Checkout";
    btnPosCheckout->Location = Point(12, totY);
    btnPosCheckout->Size = System::Drawing::Size(200, 38);
    btnPosCheckout->BackColor = Color::FromArgb(46, 139, 87);
    btnPosCheckout->ForeColor = Color::White;
    btnPosCheckout->Font = gcnew System::Drawing::Font("Segoe UI", 9.5f, FontStyle::Bold);
    btnPosCheckout->FlatStyle = FlatStyle::Flat;
    btnPosCheckout->FlatAppearance->BorderSize = 0;
    btnPosCheckout->Cursor = Cursors::Hand;
    btnPosCheckout->Click += gcnew EventHandler(this, &MainForm::OnPosCheckout);
    pnlPosCartPanel->Controls->Add(btnPosCheckout);

    btnPosClearCart = gcnew Button();
    btnPosClearCart->Text = "Clear Cart";
    btnPosClearCart->Location = Point(220, totY);
    btnPosClearCart->Size = System::Drawing::Size(88, 38);
    btnPosClearCart->BackColor = Color::FromArgb(108, 117, 125);
    btnPosClearCart->ForeColor = Color::White;
    btnPosClearCart->FlatStyle = FlatStyle::Flat;
    btnPosClearCart->FlatAppearance->BorderSize = 0;
    btnPosClearCart->Cursor = Cursors::Hand;
    btnPosClearCart->Click += gcnew EventHandler(this, &MainForm::OnPosClearCart);
    pnlPosCartPanel->Controls->Add(btnPosClearCart);

    btnPosViewReceipt = gcnew Button();
    btnPosViewReceipt->Text = "Receipt";
    btnPosViewReceipt->Location = Point(316, totY);
    btnPosViewReceipt->Size = System::Drawing::Size(90, 38);
    btnPosViewReceipt->BackColor = Color::FromArgb(38, 50, 56);
    btnPosViewReceipt->ForeColor = Color::White;
    btnPosViewReceipt->FlatStyle = FlatStyle::Flat;
    btnPosViewReceipt->FlatAppearance->BorderSize = 0;
    btnPosViewReceipt->Cursor = Cursors::Hand;
    btnPosViewReceipt->Click += gcnew EventHandler(this, &MainForm::OnPosViewReceipt);
    pnlPosCartPanel->Controls->Add(btnPosViewReceipt);

    // =============================================================
    // BUILD SCREEN D: CUSTOMER MANAGEMENT (Full CRUD)
    // =============================================================
    pnlScreenCustomers = gcnew Panel();
    pnlScreenCustomers->Dock = DockStyle::Fill;
    pnlScreenCustomers->Visible = false;
    pnlScreenCustomers->AutoScroll = false;
    pnlContentContainer->Controls->Add(pnlScreenCustomers);

    pnlCustForm = gcnew Panel();
    pnlCustForm->Location = Point(10, 10);
    pnlCustForm->Size = System::Drawing::Size(310, 510);
    pnlCustForm->BackColor = Color::White;
    pnlCustForm->BorderStyle = BorderStyle::FixedSingle;
    pnlCustForm->AutoScroll = false;
    pnlScreenCustomers->Controls->Add(pnlCustForm);

    Label^ lblCustFormTitle = gcnew Label();
    lblCustFormTitle->Text = "Customer Registration";
    lblCustFormTitle->Font = gcnew System::Drawing::Font("Segoe UI", 12, FontStyle::Bold);
    lblCustFormTitle->ForeColor = Color::FromArgb(46, 139, 87);
    lblCustFormTitle->Location = Point(12, 10);
    lblCustFormTitle->AutoSize = true;
    pnlCustForm->Controls->Add(lblCustFormTitle);

    int cfY = 40;
    AddFieldLabel(pnlCustForm, "Customer ID (Auto):", cfY);
    txtCustId = gcnew TextBox();
    txtCustId->Location = Point(12, cfY + 18);
    txtCustId->Size = System::Drawing::Size(100, 22);
    txtCustId->ReadOnly = true;
    txtCustId->BackColor = Color::FromArgb(240, 244, 248);
    pnlCustForm->Controls->Add(txtCustId);
    cfY += 46;

    AddFieldLabel(pnlCustForm, "Full Name *:", cfY);
    txtCustName = gcnew TextBox();
    txtCustName->Location = Point(12, cfY + 18);
    txtCustName->Size = System::Drawing::Size(272, 22);
    pnlCustForm->Controls->Add(txtCustName);
    cfY += 46;

    AddFieldLabel(pnlCustForm, "Phone Number * (Unique):", cfY);
    txtCustPhone = gcnew TextBox();
    txtCustPhone->Location = Point(12, cfY + 18);
    txtCustPhone->Size = System::Drawing::Size(272, 22);
    pnlCustForm->Controls->Add(txtCustPhone);
    cfY += 46;

    AddFieldLabel(pnlCustForm, "Email Address (Optional):", cfY);
    txtCustEmail = gcnew TextBox();
    txtCustEmail->Location = Point(12, cfY + 18);
    txtCustEmail->Size = System::Drawing::Size(272, 22);
    pnlCustForm->Controls->Add(txtCustEmail);
    cfY += 46;

    AddFieldLabel(pnlCustForm, "Residential Address:", cfY);
    txtCustAddress = gcnew TextBox();
    txtCustAddress->Location = Point(12, cfY + 18);
    txtCustAddress->Size = System::Drawing::Size(272, 52);
    txtCustAddress->Multiline = true;
    pnlCustForm->Controls->Add(txtCustAddress);
    cfY += 78;

    btnCustAdd = gcnew Button();
    btnCustAdd->Text = "Register";
    btnCustAdd->Location = Point(12, cfY);
    btnCustAdd->Size = System::Drawing::Size(65, 32);
    btnCustAdd->BackColor = Color::FromArgb(46, 139, 87);
    btnCustAdd->ForeColor = Color::White;
    btnCustAdd->FlatStyle = FlatStyle::Flat;
    btnCustAdd->FlatAppearance->BorderSize = 0;
    btnCustAdd->Click += gcnew EventHandler(this, &MainForm::OnCustomerAdd);
    pnlCustForm->Controls->Add(btnCustAdd);

    btnCustUpdate = gcnew Button();
    btnCustUpdate->Text = "Update";
    btnCustUpdate->Location = Point(82, cfY);
    btnCustUpdate->Size = System::Drawing::Size(65, 32);
    btnCustUpdate->BackColor = Color::FromArgb(255, 159, 67);
    btnCustUpdate->ForeColor = Color::White;
    btnCustUpdate->FlatStyle = FlatStyle::Flat;
    btnCustUpdate->FlatAppearance->BorderSize = 0;
    btnCustUpdate->Click += gcnew EventHandler(this, &MainForm::OnCustomerUpdate);
    pnlCustForm->Controls->Add(btnCustUpdate);

    btnCustDelete = gcnew Button();
    btnCustDelete->Text = "Delete";
    btnCustDelete->Location = Point(152, cfY);
    btnCustDelete->Size = System::Drawing::Size(65, 32);
    btnCustDelete->BackColor = Color::FromArgb(220, 53, 69);
    btnCustDelete->ForeColor = Color::White;
    btnCustDelete->FlatStyle = FlatStyle::Flat;
    btnCustDelete->FlatAppearance->BorderSize = 0;
    btnCustDelete->Click += gcnew EventHandler(this, &MainForm::OnCustomerDelete);
    pnlCustForm->Controls->Add(btnCustDelete);

    btnCustClear = gcnew Button();
    btnCustClear->Text = "Clear";
    btnCustClear->Location = Point(222, cfY);
    btnCustClear->Size = System::Drawing::Size(65, 32);
    btnCustClear->BackColor = Color::FromArgb(108, 117, 125);
    btnCustClear->ForeColor = Color::White;
    btnCustClear->FlatStyle = FlatStyle::Flat;
    btnCustClear->FlatAppearance->BorderSize = 0;
    btnCustClear->Click += gcnew EventHandler(this, &MainForm::OnCustomerClear);
    pnlCustForm->Controls->Add(btnCustClear);

    lblSearchCust = gcnew Label();
    lblSearchCust->Text = "Search Customers:";
    lblSearchCust->Location = Point(330, 14);
    lblSearchCust->AutoSize = true;
    lblSearchCust->Font = gcnew System::Drawing::Font("Segoe UI", 9.5f, FontStyle::Bold);
    pnlScreenCustomers->Controls->Add(lblSearchCust);

    txtCustSearch = gcnew TextBox();
    txtCustSearch->Location = Point(460, 11);
    txtCustSearch->Size = System::Drawing::Size(240, 24);
    txtCustSearch->TextChanged += gcnew EventHandler(this, &MainForm::OnCustomerSearchChanged);
    pnlScreenCustomers->Controls->Add(txtCustSearch);

    dgvCustomers = gcnew DataGridView();
    dgvCustomers->Location = Point(330, 40);
    dgvCustomers->Size = System::Drawing::Size(500, 480);
    dgvCustomers->BackgroundColor = Color::White;
    dgvCustomers->ReadOnly = true;
    dgvCustomers->AllowUserToAddRows = false;
    dgvCustomers->RowHeadersVisible = false;
    dgvCustomers->SelectionMode = DataGridViewSelectionMode::FullRowSelect;
    dgvCustomers->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::None;
    dgvCustomers->ScrollBars = ScrollBars::Both;
    dgvCustomers->SelectionChanged += gcnew EventHandler(this, &MainForm::OnCustomerSelectionChanged);
    pnlScreenCustomers->Controls->Add(dgvCustomers);

    // =============================================================
    // BUILD SCREEN E: INGREDIENT INVENTORY (Low-Stock Alerting)
    // =============================================================
    pnlScreenInventory = gcnew Panel();
    pnlScreenInventory->Dock = DockStyle::Fill;
    pnlScreenInventory->Visible = false;
    pnlScreenInventory->AutoScroll = false;
    pnlContentContainer->Controls->Add(pnlScreenInventory);

    pnlIngForm = gcnew Panel();
    pnlIngForm->Location = Point(10, 10);
    pnlIngForm->Size = System::Drawing::Size(310, 510);
    pnlIngForm->BackColor = Color::White;
    pnlIngForm->BorderStyle = BorderStyle::FixedSingle;
    pnlIngForm->AutoScroll = false;
    pnlScreenInventory->Controls->Add(pnlIngForm);

    Label^ lblIngFormTitle = gcnew Label();
    lblIngFormTitle->Text = "Ingredient Inventory";
    lblIngFormTitle->Font = gcnew System::Drawing::Font("Segoe UI", 12, FontStyle::Bold);
    lblIngFormTitle->ForeColor = Color::FromArgb(46, 139, 87);
    lblIngFormTitle->Location = Point(12, 10);
    lblIngFormTitle->AutoSize = true;
    pnlIngForm->Controls->Add(lblIngFormTitle);

    int ifY = 40;
    AddFieldLabel(pnlIngForm, "Ingredient ID (Auto):", ifY);
    txtIngId = gcnew TextBox();
    txtIngId->Location = Point(12, ifY + 18);
    txtIngId->Size = System::Drawing::Size(100, 22);
    txtIngId->ReadOnly = true;
    txtIngId->BackColor = Color::FromArgb(240, 244, 248);
    pnlIngForm->Controls->Add(txtIngId);
    ifY += 46;

    AddFieldLabel(pnlIngForm, "Ingredient Name *:", ifY);
    txtIngName = gcnew TextBox();
    txtIngName->Location = Point(12, ifY + 18);
    txtIngName->Size = System::Drawing::Size(272, 22);
    pnlIngForm->Controls->Add(txtIngName);
    ifY += 46;

    AddFieldLabel(pnlIngForm, "Current Stock *:", ifY);
    nudIngStock = gcnew NumericUpDown();
    nudIngStock->Location = Point(12, ifY + 18);
    nudIngStock->Size = System::Drawing::Size(125, 22);
    nudIngStock->DecimalPlaces = 2;
    nudIngStock->Maximum = 99999;
    nudIngStock->Value = 10;
    pnlIngForm->Controls->Add(nudIngStock);

    Label^ lblUnitL = gcnew Label();
    lblUnitL->Text = "Unit *:";
    lblUnitL->Location = Point(148, ifY);
    lblUnitL->AutoSize = true;
    lblUnitL->Font = gcnew System::Drawing::Font("Segoe UI", 8.5f);
    pnlIngForm->Controls->Add(lblUnitL);

    cmbIngUnit = gcnew ComboBox();
    cmbIngUnit->Location = Point(148, ifY + 18);
    cmbIngUnit->Size = System::Drawing::Size(136, 22);
    cmbIngUnit->DropDownStyle = ComboBoxStyle::DropDownList;
    cmbIngUnit->Items->AddRange(gcnew array<Object^> { "kg", "L", "pcs", "g", "ml" });
    cmbIngUnit->SelectedIndex = 0;
    pnlIngForm->Controls->Add(cmbIngUnit);
    ifY += 46;

    AddFieldLabel(pnlIngForm, "Reorder Level Threshold *:", ifY);
    nudIngReorder = gcnew NumericUpDown();
    nudIngReorder->Location = Point(12, ifY + 18);
    nudIngReorder->Size = System::Drawing::Size(125, 22);
    nudIngReorder->DecimalPlaces = 2;
    nudIngReorder->Maximum = 9999;
    nudIngReorder->Value = 5;
    pnlIngForm->Controls->Add(nudIngReorder);
    ifY += 46;

    AddFieldLabel(pnlIngForm, "Supplier Name:", ifY);
    txtIngSupplier = gcnew TextBox();
    txtIngSupplier->Location = Point(12, ifY + 18);
    txtIngSupplier->Size = System::Drawing::Size(272, 22);
    pnlIngForm->Controls->Add(txtIngSupplier);
    ifY += 46;

    AddFieldLabel(pnlIngForm, "Supplier Contact Phone:", ifY);
    txtIngSupplierPhone = gcnew TextBox();
    txtIngSupplierPhone->Location = Point(12, ifY + 18);
    txtIngSupplierPhone->Size = System::Drawing::Size(272, 22);
    pnlIngForm->Controls->Add(txtIngSupplierPhone);
    ifY += 56;

    btnIngAdd = gcnew Button();
    btnIngAdd->Text = "Add Item";
    btnIngAdd->Location = Point(12, ifY);
    btnIngAdd->Size = System::Drawing::Size(65, 32);
    btnIngAdd->BackColor = Color::FromArgb(46, 139, 87);
    btnIngAdd->ForeColor = Color::White;
    btnIngAdd->FlatStyle = FlatStyle::Flat;
    btnIngAdd->FlatAppearance->BorderSize = 0;
    btnIngAdd->Click += gcnew EventHandler(this, &MainForm::OnIngredientAdd);
    pnlIngForm->Controls->Add(btnIngAdd);

    btnIngUpdate = gcnew Button();
    btnIngUpdate->Text = "Update";
    btnIngUpdate->Location = Point(82, ifY);
    btnIngUpdate->Size = System::Drawing::Size(65, 32);
    btnIngUpdate->BackColor = Color::FromArgb(255, 159, 67);
    btnIngUpdate->ForeColor = Color::White;
    btnIngUpdate->FlatStyle = FlatStyle::Flat;
    btnIngUpdate->FlatAppearance->BorderSize = 0;
    btnIngUpdate->Click += gcnew EventHandler(this, &MainForm::OnIngredientUpdate);
    pnlIngForm->Controls->Add(btnIngUpdate);

    btnIngDelete = gcnew Button();
    btnIngDelete->Text = "Delete";
    btnIngDelete->Location = Point(152, ifY);
    btnIngDelete->Size = System::Drawing::Size(65, 32);
    btnIngDelete->BackColor = Color::FromArgb(220, 53, 69);
    btnIngDelete->ForeColor = Color::White;
    btnIngDelete->FlatStyle = FlatStyle::Flat;
    btnIngDelete->FlatAppearance->BorderSize = 0;
    btnIngDelete->Click += gcnew EventHandler(this, &MainForm::OnIngredientDelete);
    pnlIngForm->Controls->Add(btnIngDelete);

    btnIngClear = gcnew Button();
    btnIngClear->Text = "Clear";
    btnIngClear->Location = Point(222, ifY);
    btnIngClear->Size = System::Drawing::Size(65, 32);
    btnIngClear->BackColor = Color::FromArgb(108, 117, 125);
    btnIngClear->ForeColor = Color::White;
    btnIngClear->FlatStyle = FlatStyle::Flat;
    btnIngClear->FlatAppearance->BorderSize = 0;
    btnIngClear->Click += gcnew EventHandler(this, &MainForm::OnIngredientClear);
    pnlIngForm->Controls->Add(btnIngClear);

    lblSearchIng = gcnew Label();
    lblSearchIng->Text = "Search Inventory:";
    lblSearchIng->Location = Point(330, 14);
    lblSearchIng->AutoSize = true;
    lblSearchIng->Font = gcnew System::Drawing::Font("Segoe UI", 9.5f, FontStyle::Bold);
    pnlScreenInventory->Controls->Add(lblSearchIng);

    txtIngSearch = gcnew TextBox();
    txtIngSearch->Location = Point(455, 11);
    txtIngSearch->Size = System::Drawing::Size(160, 24);
    txtIngSearch->TextChanged += gcnew EventHandler(this, &MainForm::OnInventorySearchChanged);
    pnlScreenInventory->Controls->Add(txtIngSearch);

    chkIngLowStockOnly = gcnew CheckBox();
    chkIngLowStockOnly->Text = "Low Stock Alerts Only";
    chkIngLowStockOnly->Location = Point(625, 13);
    chkIngLowStockOnly->AutoSize = true;
    chkIngLowStockOnly->Font = gcnew System::Drawing::Font("Segoe UI", 9.0f, FontStyle::Bold);
    chkIngLowStockOnly->ForeColor = Color::FromArgb(220, 53, 69);
    chkIngLowStockOnly->CheckedChanged += gcnew EventHandler(this, &MainForm::OnInventorySearchChanged);
    pnlScreenInventory->Controls->Add(chkIngLowStockOnly);

    dgvIngredients = gcnew DataGridView();
    dgvIngredients->Location = Point(330, 40);
    dgvIngredients->Size = System::Drawing::Size(500, 480);
    dgvIngredients->BackgroundColor = Color::White;
    dgvIngredients->ReadOnly = true;
    dgvIngredients->AllowUserToAddRows = false;
    dgvIngredients->RowHeadersVisible = false;
    dgvIngredients->SelectionMode = DataGridViewSelectionMode::FullRowSelect;
    dgvIngredients->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::None;
    dgvIngredients->ScrollBars = ScrollBars::Both;
    dgvIngredients->SelectionChanged += gcnew EventHandler(this, &MainForm::OnInventorySelectionChanged);
    dgvIngredients->CellFormatting += gcnew DataGridViewCellFormattingEventHandler(this, &MainForm::OnInventoryCellFormatting);
    pnlScreenInventory->Controls->Add(dgvIngredients);

    // =============================================================
    // BUILD SCREEN F: SALES REPORTS & CHARTS + PRINTING
    // =============================================================
    pnlScreenReports = gcnew Panel();
    pnlScreenReports->Dock = DockStyle::Fill;
    pnlScreenReports->Visible = false;
    pnlScreenReports->AutoScroll = false;
    pnlContentContainer->Controls->Add(pnlScreenReports);

    printReportDoc = gcnew Drawing::Printing::PrintDocument();
    printReportDoc->DocumentName = "FreshSip_Sales_Report";
    printReportDoc->PrintPage += gcnew Drawing::Printing::PrintPageEventHandler(this, &MainForm::OnPrintReportPage);

    pnlRepFilter = gcnew Panel();
    pnlRepFilter->Location = Point(10, 8);
    pnlRepFilter->Size = System::Drawing::Size(815, 44);
    pnlRepFilter->BackColor = Color::White;
    pnlRepFilter->BorderStyle = BorderStyle::FixedSingle;
    pnlScreenReports->Controls->Add(pnlRepFilter);

    Label^ lblFrom = gcnew Label();
    lblFrom->Text = "From:";
    lblFrom->Location = Point(8, 12);
    lblFrom->AutoSize = true;
    lblFrom->Font = gcnew System::Drawing::Font("Segoe UI", 9.0f, FontStyle::Bold);
    pnlRepFilter->Controls->Add(lblFrom);

    dtpReportStart = gcnew DateTimePicker();
    dtpReportStart->Location = Point(48, 10);
    dtpReportStart->Size = System::Drawing::Size(100, 22);
    dtpReportStart->Format = DateTimePickerFormat::Short;
    dtpReportStart->Value = DateTime::Now.AddDays(-7);
    pnlRepFilter->Controls->Add(dtpReportStart);

    Label^ lblTo = gcnew Label();
    lblTo->Text = "To:";
    lblTo->Location = Point(155, 12);
    lblTo->AutoSize = true;
    lblTo->Font = gcnew System::Drawing::Font("Segoe UI", 9.0f, FontStyle::Bold);
    pnlRepFilter->Controls->Add(lblTo);

    dtpReportEnd = gcnew DateTimePicker();
    dtpReportEnd->Location = Point(180, 10);
    dtpReportEnd->Size = System::Drawing::Size(100, 22);
    dtpReportEnd->Format = DateTimePickerFormat::Short;
    dtpReportEnd->Value = DateTime::Now;
    pnlRepFilter->Controls->Add(dtpReportEnd);

    btnReportFilter = gcnew Button();
    btnReportFilter->Text = "Filter";
    btnReportFilter->Location = Point(288, 8);
    btnReportFilter->Size = System::Drawing::Size(60, 26);
    btnReportFilter->BackColor = Color::FromArgb(46, 139, 87);
    btnReportFilter->ForeColor = Color::White;
    btnReportFilter->FlatStyle = FlatStyle::Flat;
    btnReportFilter->FlatAppearance->BorderSize = 0;
    btnReportFilter->Click += gcnew EventHandler(this, &MainForm::OnReportFilter);
    pnlRepFilter->Controls->Add(btnReportFilter);

    btnReportToday = gcnew Button();
    btnReportToday->Text = "Today";
    btnReportToday->Location = Point(354, 8);
    btnReportToday->Size = System::Drawing::Size(62, 26);
    btnReportToday->Click += gcnew EventHandler(this, &MainForm::OnReportToday);
    pnlRepFilter->Controls->Add(btnReportToday);

    btnReportMonth = gcnew Button();
    btnReportMonth->Text = "This Month";
    btnReportMonth->Location = Point(422, 8);
    btnReportMonth->Size = System::Drawing::Size(85, 26);
    btnReportMonth->Click += gcnew EventHandler(this, &MainForm::OnReportMonth);
    pnlRepFilter->Controls->Add(btnReportMonth);

    btnPreviewReport = gcnew Button();
    btnPreviewReport->Text = "Preview Print";
    btnPreviewReport->Location = Point(515, 8);
    btnPreviewReport->Size = System::Drawing::Size(130, 26);
    btnPreviewReport->BackColor = Color::FromArgb(38, 50, 56);
    btnPreviewReport->ForeColor = Color::White;
    btnPreviewReport->FlatStyle = FlatStyle::Flat;
    btnPreviewReport->FlatAppearance->BorderSize = 0;
    btnPreviewReport->Cursor = Cursors::Hand;
    btnPreviewReport->Click += gcnew EventHandler(this, &MainForm::OnPreviewReportClick);
    pnlRepFilter->Controls->Add(btnPreviewReport);

    btnPrintReport = gcnew Button();
    btnPrintReport->Text = "Print Report";
    btnPrintReport->Location = Point(652, 8);
    btnPrintReport->Size = System::Drawing::Size(125, 26);
    btnPrintReport->BackColor = Color::FromArgb(46, 139, 87);
    btnPrintReport->ForeColor = Color::White;
    btnPrintReport->FlatStyle = FlatStyle::Flat;
    btnPrintReport->FlatAppearance->BorderSize = 0;
    btnPrintReport->Cursor = Cursors::Hand;
    btnPrintReport->Click += gcnew EventHandler(this, &MainForm::OnPrintReportClick);
    pnlRepFilter->Controls->Add(btnPrintReport);

    int rcardY = 58;
    cardRep1 = CreateReportCard("TOTAL PERIOD REVENUE", Color::FromArgb(46, 139, 87), 10, rcardY, lblReportRevenueVal);
    cardRep2 = CreateReportCard("TOTAL ORDERS PROCESSED", Color::FromArgb(255, 159, 67), 280, rcardY, lblReportOrdersVal);
    cardRep3 = CreateReportCard("AVERAGE ORDER VALUE", Color::FromArgb(33, 150, 243), 550, rcardY, lblReportAvgOrderVal);

    int chartY = 135;
    pnlReportChart = gcnew Panel();
    pnlReportChart->Location = Point(10, chartY);
    pnlReportChart->Size = System::Drawing::Size(815, 160);
    pnlReportChart->BackColor = Color::White;
    pnlReportChart->BorderStyle = BorderStyle::FixedSingle;
    pnlReportChart->Paint += gcnew PaintEventHandler(this, &MainForm::OnPaintReportChart);
    pnlScreenReports->Controls->Add(pnlReportChart);

    int rtblY = 302;
    lblR1 = gcnew Label();
    lblR1->Text = "Best-Selling Products Breakdown:";
    lblR1->Location = Point(10, rtblY);
    lblR1->AutoSize = true;
    lblR1->Font = gcnew System::Drawing::Font("Segoe UI", 9.5f, FontStyle::Bold);
    pnlScreenReports->Controls->Add(lblR1);

    dgvReportTopProducts = gcnew DataGridView();
    dgvReportTopProducts->Location = Point(10, rtblY + 22);
    dgvReportTopProducts->Size = System::Drawing::Size(395, 190);
    dgvReportTopProducts->BackgroundColor = Color::White;
    dgvReportTopProducts->ReadOnly = true;
    dgvReportTopProducts->AllowUserToAddRows = false;
    dgvReportTopProducts->RowHeadersVisible = false;
    dgvReportTopProducts->SelectionMode = DataGridViewSelectionMode::FullRowSelect;
    dgvReportTopProducts->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::None;
    dgvReportTopProducts->ScrollBars = ScrollBars::Both;
    pnlScreenReports->Controls->Add(dgvReportTopProducts);

    lblR2 = gcnew Label();
    lblR2->Text = "Daily Sales History:";
    lblR2->Location = Point(415, rtblY);
    lblR2->AutoSize = true;
    lblR2->Font = gcnew System::Drawing::Font("Segoe UI", 9.5f, FontStyle::Bold);
    pnlScreenReports->Controls->Add(lblR2);

    dgvReportDaily = gcnew DataGridView();
    dgvReportDaily->Location = Point(415, rtblY + 22);
    dgvReportDaily->Size = System::Drawing::Size(410, 190);
    dgvReportDaily->BackgroundColor = Color::White;
    dgvReportDaily->ReadOnly = true;
    dgvReportDaily->AllowUserToAddRows = false;
    dgvReportDaily->RowHeadersVisible = false;
    dgvReportDaily->SelectionMode = DataGridViewSelectionMode::FullRowSelect;
    dgvReportDaily->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::None;
    dgvReportDaily->ScrollBars = ScrollBars::Both;
    pnlScreenReports->Controls->Add(dgvReportDaily);

    // ----------------------------------------------------------------------------
    // Navigation and Screen Switching (6 Business Screens Only - DB Encapsulated in Backend)
    // ----------------------------------------------------------------------------
}

void MainForm::SwitchScreen(Panel^ targetPanel, String^ screenTitle) {
    pnlScreenDashboard->Visible = false;
    pnlScreenProducts->Visible = false;
    pnlScreenOrders->Visible = false;
    pnlScreenCustomers->Visible = false;
    pnlScreenInventory->Visible = false;
    pnlScreenReports->Visible = false;

    targetPanel->Visible = true;
    targetPanel->BringToFront();
    lblBreadcrumb->Text = screenTitle;

    array<Button^>^ navButtons = { btnNavDashboard, btnNavProducts, btnNavOrders, btnNavCustomers, btnNavInventory, btnNavReports };
    for each (Button^ b in navButtons) {
        b->BackColor = Color::FromArgb(45, 58, 65);
    }

    if (targetPanel == pnlScreenDashboard) btnNavDashboard->BackColor = Color::FromArgb(46, 139, 87);
    else if (targetPanel == pnlScreenProducts) btnNavProducts->BackColor = Color::FromArgb(46, 139, 87);
    else if (targetPanel == pnlScreenOrders) btnNavOrders->BackColor = Color::FromArgb(46, 139, 87);
    else if (targetPanel == pnlScreenCustomers) btnNavCustomers->BackColor = Color::FromArgb(46, 139, 87);
    else if (targetPanel == pnlScreenInventory) btnNavInventory->BackColor = Color::FromArgb(46, 139, 87);
    else if (targetPanel == pnlScreenReports) btnNavReports->BackColor = Color::FromArgb(46, 139, 87);

    UpdateScreenLayout();
}

void MainForm::ApplyRolePermissions() {
    if (m_currentRole == "Cashier") {
        m_currentUserName = "Anuki Samarasinghe";
        lblCurrentUserBadge->Text = "User: [Cashier] Anuki Samarasinghe";
        lblCurrentUserBadge->ForeColor = Color::FromArgb(46, 139, 87);

        // Cashier permissions:
        // ALLOWED: Dashboard, Orders & POS, Customers
        // RESTRICTED: Products Menu (CRUD), Inventory, Sales Reports
        btnNavProducts->Enabled = false;
        btnNavProducts->BackColor = Color::FromArgb(35, 42, 46);
        btnNavProducts->ForeColor = Color::FromArgb(100, 110, 115);

        btnNavInventory->Enabled = false;
        btnNavInventory->BackColor = Color::FromArgb(35, 42, 46);
        btnNavInventory->ForeColor = Color::FromArgb(100, 110, 115);

        btnNavReports->Enabled = false;
        btnNavReports->BackColor = Color::FromArgb(35, 42, 46);
        btnNavReports->ForeColor = Color::FromArgb(100, 110, 115);

        btnDashQuickProduct->Enabled = false;
        btnDashQuickRestock->Enabled = false;

        // If currently on a restricted screen, return to POS Billing
        if (pnlScreenProducts->Visible || pnlScreenInventory->Visible || pnlScreenReports->Visible) {
            SwitchScreen(pnlScreenOrders, "Orders & POS Billing");
            LoadPosData();
        }
    }
    else {
        // Admin
        m_currentRole = "Admin";
        m_currentUserName = "Chamara Bandara";
        lblCurrentUserBadge->Text = "User: [Admin] Chamara Bandara";
        lblCurrentUserBadge->ForeColor = Color::FromArgb(210, 105, 30);

        btnNavProducts->Enabled = true;
        btnNavProducts->BackColor = Color::FromArgb(45, 58, 65);
        btnNavProducts->ForeColor = Color::White;

        btnNavInventory->Enabled = true;
        btnNavInventory->BackColor = Color::FromArgb(45, 58, 65);
        btnNavInventory->ForeColor = Color::White;

        btnNavReports->Enabled = true;
        btnNavReports->BackColor = Color::FromArgb(45, 58, 65);
        btnNavReports->ForeColor = Color::White;

        btnDashQuickProduct->Enabled = true;
        btnDashQuickRestock->Enabled = true;
    }
}

void MainForm::OnLogoutClick(Object^ sender, EventArgs^ e) {
    if (MessageBox::Show("Are you sure you want to log out from FreshSip Juice Bar?",
                         "Confirm Logout", MessageBoxButtons::YesNo, MessageBoxIcon::Question) == System::Windows::Forms::DialogResult::Yes) {
        m_isLoggingOut = true;
        this->Close();
    }
}

void MainForm::OnNavClick(Object^ sender, EventArgs^ e) {
    if (sender == btnNavDashboard) {
        SwitchScreen(pnlScreenDashboard, "Dashboard Overview");
        LoadDashboardData();
    }
    else if (sender == btnNavProducts || sender == btnDashQuickProduct) {
        if (m_currentRole != "Admin") {
            MessageBox::Show("Access Denied: Product catalog management requires Admin role privileges.", "Security Restriction", MessageBoxButtons::OK, MessageBoxIcon::Warning);
            return;
        }
        SwitchScreen(pnlScreenProducts, "Product Management");
        LoadProductsData();
    }
    else if (sender == btnNavOrders || sender == btnDashQuickOrder) {
        SwitchScreen(pnlScreenOrders, "Orders & POS Billing");
        LoadPosData();
    }
    else if (sender == btnNavCustomers || sender == btnPosNewCustomer) {
        SwitchScreen(pnlScreenCustomers, "Customer Management");
        LoadCustomersData();
    }
    else if (sender == btnNavInventory || sender == btnDashQuickRestock) {
        if (m_currentRole != "Admin") {
            MessageBox::Show("Access Denied: Inventory management requires Admin role privileges.", "Security Restriction", MessageBoxButtons::OK, MessageBoxIcon::Warning);
            return;
        }
        SwitchScreen(pnlScreenInventory, "Ingredient Inventory");
        LoadInventoryData();
    }
    else if (sender == btnNavReports) {
        if (m_currentRole != "Admin") {
            MessageBox::Show("Access Denied: Financial sales reports require Admin role privileges.", "Security Restriction", MessageBoxButtons::OK, MessageBoxIcon::Warning);
            return;
        }
        SwitchScreen(pnlScreenReports, "Sales Reports & Analytics");
        LoadReportsData();
    }
}

void MainForm::OnClockTick(Object^ sender, EventArgs^ e) {
    lblClock->Text = DateTime::Now.ToString("yyyy-MM-dd HH:mm:ss");
    static int s_dbCheckTimer = 0;
    s_dbCheckTimer++;
    if (s_dbCheckTimer >= 4) {
        s_dbCheckTimer = 0;
        UpdateSystemStatusIndicator();
    }
}

void MainForm::UpdateSystemStatusIndicator() {
    String^ err = "";
    bool ok = DatabaseManager::TestConnection(err);
    if (ok) {
        lblSystemStatus->Text = L"\x25CF System: Online";
        lblSystemStatus->ForeColor = Color::FromArgb(46, 139, 87);
    } else {
        lblSystemStatus->Text = L"\x25CF System: Offline (No DB)";
        lblSystemStatus->ForeColor = Color::FromArgb(220, 53, 69);
    }
}

void MainForm::PopulateProductGridImages() {
    dgvProducts->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::None;
    dgvProducts->ScrollBars = ScrollBars::Both;
    dgvProducts->RowHeadersVisible = false;
    dgvProducts->EnableHeadersVisualStyles = false;
    dgvProducts->ColumnHeadersDefaultCellStyle->BackColor = Color::FromArgb(240, 244, 248);
    dgvProducts->ColumnHeadersDefaultCellStyle->ForeColor = Color::FromArgb(38, 50, 56);
    dgvProducts->ColumnHeadersDefaultCellStyle->Font = gcnew Drawing::Font("Segoe UI", 9.0f, FontStyle::Bold);
    dgvProducts->ColumnHeadersHeight = 32;

    if (dgvProducts->Columns["ImagePath"] != nullptr) {
        dgvProducts->Columns["ImagePath"]->Visible = false; // Never show raw text path to user
    }
    if (dgvProducts->Columns["Description"] != nullptr) {
        dgvProducts->Columns["Description"]->Visible = false; // Hide from compact table - displayed in left editor panel
    }
    if (dgvProducts->Columns["ColProdPhoto"] == nullptr) {
        DataGridViewImageColumn^ imgCol = gcnew DataGridViewImageColumn();
        imgCol->Name = "ColProdPhoto";
        imgCol->HeaderText = "Photo";
        imgCol->ImageLayout = DataGridViewImageCellLayout::Zoom;
        imgCol->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
        imgCol->Width = 65;
        dgvProducts->Columns->Insert(0, imgCol);
    }
    for each (DataGridViewRow^ row in dgvProducts->Rows) {
        row->Height = 48;
        Product^ p = dynamic_cast<Product^>(row->DataBoundItem);
        if (p != nullptr && !String::IsNullOrEmpty(p->ImagePath)) {
            row->Cells["ColProdPhoto"]->Value = SafeLoadImage(p->ImagePath);
        }
    }

    // Explicit human-readable column arrangement from left to right:
    // 0: Photo, 1: ID, 2: Juice Product Name, 3: Category, 4: Price, 5: Stock, 6: Active
    if (dgvProducts->Columns["ColProdPhoto"] != nullptr) {
        dgvProducts->Columns["ColProdPhoto"]->DisplayIndex = 0;
        dgvProducts->Columns["ColProdPhoto"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
        dgvProducts->Columns["ColProdPhoto"]->Width = 65;
    }
    if (dgvProducts->Columns["ProductId"] != nullptr) {
        dgvProducts->Columns["ProductId"]->DisplayIndex = 1;
        dgvProducts->Columns["ProductId"]->HeaderText = "ID";
        dgvProducts->Columns["ProductId"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
        dgvProducts->Columns["ProductId"]->Width = 50;
        dgvProducts->Columns["ProductId"]->DefaultCellStyle->Alignment = DataGridViewContentAlignment::MiddleCenter;
    }
    if (dgvProducts->Columns["ProductName"] != nullptr) {
        dgvProducts->Columns["ProductName"]->DisplayIndex = 2;
        dgvProducts->Columns["ProductName"]->HeaderText = "Juice Product Name";
        dgvProducts->Columns["ProductName"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::Fill;
        dgvProducts->Columns["ProductName"]->MinimumWidth = 160;
    }
    if (dgvProducts->Columns["Category"] != nullptr) {
        dgvProducts->Columns["Category"]->DisplayIndex = 3;
        dgvProducts->Columns["Category"]->HeaderText = "Category";
        dgvProducts->Columns["Category"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
        dgvProducts->Columns["Category"]->Width = 115;
    }
    if (dgvProducts->Columns["Price"] != nullptr) {
        dgvProducts->Columns["Price"]->DisplayIndex = 4;
        dgvProducts->Columns["Price"]->HeaderText = "Price (LKR)";
        dgvProducts->Columns["Price"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
        dgvProducts->Columns["Price"]->DefaultCellStyle->Format = "N2";
        dgvProducts->Columns["Price"]->DefaultCellStyle->Alignment = DataGridViewContentAlignment::MiddleRight;
        dgvProducts->Columns["Price"]->Width = 95;
    }
    if (dgvProducts->Columns["Quantity"] != nullptr) {
        dgvProducts->Columns["Quantity"]->DisplayIndex = 5;
        dgvProducts->Columns["Quantity"]->HeaderText = "Stock";
        dgvProducts->Columns["Quantity"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
        dgvProducts->Columns["Quantity"]->DefaultCellStyle->Alignment = DataGridViewContentAlignment::MiddleCenter;
        dgvProducts->Columns["Quantity"]->Width = 65;
    }
    if (dgvProducts->Columns["IsActive"] != nullptr) {
        dgvProducts->Columns["IsActive"]->DisplayIndex = 6;
        dgvProducts->Columns["IsActive"]->HeaderText = "Active";
        dgvProducts->Columns["IsActive"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
        dgvProducts->Columns["IsActive"]->DefaultCellStyle->Alignment = DataGridViewContentAlignment::MiddleCenter;
        dgvProducts->Columns["IsActive"]->Width = 60;
    }
}

void MainForm::PopulatePosGridImages() {
    dgvPosProducts->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::None;
    dgvPosProducts->ScrollBars = ScrollBars::Both;
    dgvPosProducts->RowHeadersVisible = false;
    dgvPosProducts->EnableHeadersVisualStyles = false;
    dgvPosProducts->ColumnHeadersDefaultCellStyle->BackColor = Color::FromArgb(240, 244, 248);
    dgvPosProducts->ColumnHeadersDefaultCellStyle->ForeColor = Color::FromArgb(38, 50, 56);
    dgvPosProducts->ColumnHeadersDefaultCellStyle->Font = gcnew Drawing::Font("Segoe UI", 9.0f, FontStyle::Bold);
    dgvPosProducts->ColumnHeadersHeight = 30;

    if (dgvPosProducts->Columns["ProductId"] != nullptr) dgvPosProducts->Columns["ProductId"]->Visible = false;
    if (dgvPosProducts->Columns["Category"] != nullptr) dgvPosProducts->Columns["Category"]->Visible = false;
    if (dgvPosProducts->Columns["Description"] != nullptr) dgvPosProducts->Columns["Description"]->Visible = false;
    if (dgvPosProducts->Columns["ImagePath"] != nullptr) dgvPosProducts->Columns["ImagePath"]->Visible = false;
    if (dgvPosProducts->Columns["IsActive"] != nullptr) dgvPosProducts->Columns["IsActive"]->Visible = false;

    if (dgvPosProducts->Columns["ColPosPhoto"] == nullptr) {
        DataGridViewImageColumn^ imgCol = gcnew DataGridViewImageColumn();
        imgCol->Name = "ColPosPhoto";
        imgCol->HeaderText = "Drink";
        imgCol->ImageLayout = DataGridViewImageCellLayout::Zoom;
        imgCol->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
        imgCol->Width = 55;
        dgvPosProducts->Columns->Insert(0, imgCol);
    }
    for each (DataGridViewRow^ row in dgvPosProducts->Rows) {
        row->Height = 44;
        Product^ p = dynamic_cast<Product^>(row->DataBoundItem);
        if (p != nullptr && !String::IsNullOrEmpty(p->ImagePath)) {
            row->Cells["ColPosPhoto"]->Value = SafeLoadImage(p->ImagePath);
        }
    }

    if (dgvPosProducts->Columns["ColPosPhoto"] != nullptr) {
        dgvPosProducts->Columns["ColPosPhoto"]->DisplayIndex = 0;
        dgvPosProducts->Columns["ColPosPhoto"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
        dgvPosProducts->Columns["ColPosPhoto"]->Width = 55;
    }
    if (dgvPosProducts->Columns["ProductName"] != nullptr) {
        dgvPosProducts->Columns["ProductName"]->DisplayIndex = 1;
        dgvPosProducts->Columns["ProductName"]->HeaderText = "Juice Item";
        dgvPosProducts->Columns["ProductName"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::Fill;
        dgvPosProducts->Columns["ProductName"]->MinimumWidth = 140;
    }
    if (dgvPosProducts->Columns["Price"] != nullptr) {
        dgvPosProducts->Columns["Price"]->DisplayIndex = 2;
        dgvPosProducts->Columns["Price"]->HeaderText = "Price (LKR)";
        dgvPosProducts->Columns["Price"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
        dgvPosProducts->Columns["Price"]->DefaultCellStyle->Format = "N2";
        dgvPosProducts->Columns["Price"]->DefaultCellStyle->Alignment = DataGridViewContentAlignment::MiddleRight;
        dgvPosProducts->Columns["Price"]->Width = 90;
    }
    if (dgvPosProducts->Columns["Quantity"] != nullptr) {
        dgvPosProducts->Columns["Quantity"]->DisplayIndex = 3;
        dgvPosProducts->Columns["Quantity"]->HeaderText = "Stock";
        dgvPosProducts->Columns["Quantity"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
        dgvPosProducts->Columns["Quantity"]->DefaultCellStyle->Alignment = DataGridViewContentAlignment::MiddleCenter;
        dgvPosProducts->Columns["Quantity"]->Width = 60;
    }
}

void MainForm::FormatPosCartGrid() {
    dgvPosCart->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::None;
    dgvPosCart->ScrollBars = ScrollBars::Both;
    dgvPosCart->RowHeadersVisible = false;
    dgvPosCart->EnableHeadersVisualStyles = false;
    dgvPosCart->ColumnHeadersDefaultCellStyle->BackColor = Color::FromArgb(240, 244, 248);
    dgvPosCart->ColumnHeadersDefaultCellStyle->ForeColor = Color::FromArgb(38, 50, 56);
    dgvPosCart->ColumnHeadersDefaultCellStyle->Font = gcnew Drawing::Font("Segoe UI", 9.0f, FontStyle::Bold);
    dgvPosCart->ColumnHeadersHeight = 30;

    if (dgvPosCart->Columns["OrderItemId"] != nullptr) dgvPosCart->Columns["OrderItemId"]->Visible = false;
    if (dgvPosCart->Columns["OrderId"] != nullptr) dgvPosCart->Columns["OrderId"]->Visible = false;
    if (dgvPosCart->Columns["ProductId"] != nullptr) dgvPosCart->Columns["ProductId"]->Visible = false;

    if (dgvPosCart->Columns["ProductName"] != nullptr) {
        dgvPosCart->Columns["ProductName"]->DisplayIndex = 0;
        dgvPosCart->Columns["ProductName"]->HeaderText = "Juice Item";
        dgvPosCart->Columns["ProductName"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::Fill;
        dgvPosCart->Columns["ProductName"]->MinimumWidth = 120;
    }
    if (dgvPosCart->Columns["UnitPrice"] != nullptr) {
        dgvPosCart->Columns["UnitPrice"]->DisplayIndex = 1;
        dgvPosCart->Columns["UnitPrice"]->HeaderText = "Price (LKR)";
        dgvPosCart->Columns["UnitPrice"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
        dgvPosCart->Columns["UnitPrice"]->Width = 85;
        dgvPosCart->Columns["UnitPrice"]->DefaultCellStyle->Format = "N2";
        dgvPosCart->Columns["UnitPrice"]->DefaultCellStyle->Alignment = DataGridViewContentAlignment::MiddleRight;
    }
    if (dgvPosCart->Columns["Quantity"] != nullptr) {
        dgvPosCart->Columns["Quantity"]->DisplayIndex = 2;
        dgvPosCart->Columns["Quantity"]->HeaderText = "Qty";
        dgvPosCart->Columns["Quantity"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
        dgvPosCart->Columns["Quantity"]->Width = 50;
        dgvPosCart->Columns["Quantity"]->DefaultCellStyle->Alignment = DataGridViewContentAlignment::MiddleCenter;
    }
    if (dgvPosCart->Columns["Subtotal"] != nullptr) {
        dgvPosCart->Columns["Subtotal"]->DisplayIndex = 3;
        dgvPosCart->Columns["Subtotal"]->HeaderText = "Total (LKR)";
        dgvPosCart->Columns["Subtotal"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
        dgvPosCart->Columns["Subtotal"]->Width = 90;
        dgvPosCart->Columns["Subtotal"]->DefaultCellStyle->Format = "N2";
        dgvPosCart->Columns["Subtotal"]->DefaultCellStyle->Alignment = DataGridViewContentAlignment::MiddleRight;
    }
}

void MainForm::FormatCustomerGrid() {
    dgvCustomers->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::None;
    dgvCustomers->ScrollBars = ScrollBars::Both;
    dgvCustomers->RowHeadersVisible = false;
    dgvCustomers->EnableHeadersVisualStyles = false;
    dgvCustomers->ColumnHeadersDefaultCellStyle->BackColor = Color::FromArgb(240, 244, 248);
    dgvCustomers->ColumnHeadersDefaultCellStyle->ForeColor = Color::FromArgb(38, 50, 56);
    dgvCustomers->ColumnHeadersDefaultCellStyle->Font = gcnew Drawing::Font("Segoe UI", 9.0f, FontStyle::Bold);
    dgvCustomers->ColumnHeadersHeight = 32;

    if (dgvCustomers->Columns["Id"] != nullptr) {
        dgvCustomers->Columns["Id"]->DisplayIndex = 0;
        dgvCustomers->Columns["Id"]->HeaderText = "ID";
        dgvCustomers->Columns["Id"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
        dgvCustomers->Columns["Id"]->Width = 50;
        dgvCustomers->Columns["Id"]->DefaultCellStyle->Alignment = DataGridViewContentAlignment::MiddleCenter;
    }
    if (dgvCustomers->Columns["Name"] != nullptr) {
        dgvCustomers->Columns["Name"]->DisplayIndex = 1;
        dgvCustomers->Columns["Name"]->HeaderText = "Customer Name";
        dgvCustomers->Columns["Name"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
        dgvCustomers->Columns["Name"]->Width = 160;
    }
    if (dgvCustomers->Columns["Phone"] != nullptr) {
        dgvCustomers->Columns["Phone"]->DisplayIndex = 2;
        dgvCustomers->Columns["Phone"]->HeaderText = "Phone";
        dgvCustomers->Columns["Phone"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
        dgvCustomers->Columns["Phone"]->Width = 115;
    }
    if (dgvCustomers->Columns["Email"] != nullptr) {
        dgvCustomers->Columns["Email"]->DisplayIndex = 3;
        dgvCustomers->Columns["Email"]->HeaderText = "Email Address";
        dgvCustomers->Columns["Email"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
        dgvCustomers->Columns["Email"]->Width = 170;
    }
    if (dgvCustomers->Columns["Address"] != nullptr) {
        dgvCustomers->Columns["Address"]->DisplayIndex = 4;
        dgvCustomers->Columns["Address"]->HeaderText = "Address";
        dgvCustomers->Columns["Address"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::Fill;
        dgvCustomers->Columns["Address"]->MinimumWidth = 140;
    }
    if (dgvCustomers->Columns["RegisteredDate"] != nullptr) {
        dgvCustomers->Columns["RegisteredDate"]->DisplayIndex = 5;
        dgvCustomers->Columns["RegisteredDate"]->HeaderText = "Registered Date";
        dgvCustomers->Columns["RegisteredDate"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
        dgvCustomers->Columns["RegisteredDate"]->Width = 120;
        dgvCustomers->Columns["RegisteredDate"]->DefaultCellStyle->Format = "yyyy-MM-dd";
        dgvCustomers->Columns["RegisteredDate"]->DefaultCellStyle->Alignment = DataGridViewContentAlignment::MiddleCenter;
    }
}

void MainForm::FormatInventoryGrid() {
    dgvIngredients->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::None;
    dgvIngredients->ScrollBars = ScrollBars::Both;
    dgvIngredients->RowHeadersVisible = false;
    dgvIngredients->EnableHeadersVisualStyles = false;
    dgvIngredients->ColumnHeadersDefaultCellStyle->BackColor = Color::FromArgb(240, 244, 248);
    dgvIngredients->ColumnHeadersDefaultCellStyle->ForeColor = Color::FromArgb(38, 50, 56);
    dgvIngredients->ColumnHeadersDefaultCellStyle->Font = gcnew Drawing::Font("Segoe UI", 9.0f, FontStyle::Bold);
    dgvIngredients->ColumnHeadersHeight = 32;

    if (dgvIngredients->Columns["IngredientId"] != nullptr) {
        dgvIngredients->Columns["IngredientId"]->DisplayIndex = 0;
        dgvIngredients->Columns["IngredientId"]->HeaderText = "ID";
        dgvIngredients->Columns["IngredientId"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
        dgvIngredients->Columns["IngredientId"]->Width = 45;
        dgvIngredients->Columns["IngredientId"]->DefaultCellStyle->Alignment = DataGridViewContentAlignment::MiddleCenter;
    }
    if (dgvIngredients->Columns["IngredientName"] != nullptr) {
        dgvIngredients->Columns["IngredientName"]->DisplayIndex = 1;
        dgvIngredients->Columns["IngredientName"]->HeaderText = "Ingredient Name";
        dgvIngredients->Columns["IngredientName"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::Fill;
        dgvIngredients->Columns["IngredientName"]->MinimumWidth = 150;
    }
    if (dgvIngredients->Columns["QuantityInStock"] != nullptr) {
        dgvIngredients->Columns["QuantityInStock"]->DisplayIndex = 2;
        dgvIngredients->Columns["QuantityInStock"]->HeaderText = "Stock";
        dgvIngredients->Columns["QuantityInStock"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
        dgvIngredients->Columns["QuantityInStock"]->Width = 70;
        dgvIngredients->Columns["QuantityInStock"]->DefaultCellStyle->Format = "N2";
        dgvIngredients->Columns["QuantityInStock"]->DefaultCellStyle->Alignment = DataGridViewContentAlignment::MiddleRight;
    }
    if (dgvIngredients->Columns["UnitOfMeasure"] != nullptr) {
        dgvIngredients->Columns["UnitOfMeasure"]->DisplayIndex = 3;
        dgvIngredients->Columns["UnitOfMeasure"]->HeaderText = "Unit";
        dgvIngredients->Columns["UnitOfMeasure"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
        dgvIngredients->Columns["UnitOfMeasure"]->Width = 55;
        dgvIngredients->Columns["UnitOfMeasure"]->DefaultCellStyle->Alignment = DataGridViewContentAlignment::MiddleCenter;
    }
    if (dgvIngredients->Columns["ReorderLevel"] != nullptr) {
        dgvIngredients->Columns["ReorderLevel"]->DisplayIndex = 4;
        dgvIngredients->Columns["ReorderLevel"]->HeaderText = "Reorder At";
        dgvIngredients->Columns["ReorderLevel"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
        dgvIngredients->Columns["ReorderLevel"]->Width = 80;
        dgvIngredients->Columns["ReorderLevel"]->DefaultCellStyle->Format = "N2";
        dgvIngredients->Columns["ReorderLevel"]->DefaultCellStyle->Alignment = DataGridViewContentAlignment::MiddleRight;
    }
    if (dgvIngredients->Columns["IsLowStock"] != nullptr) {
        dgvIngredients->Columns["IsLowStock"]->DisplayIndex = 5;
        dgvIngredients->Columns["IsLowStock"]->HeaderText = "Low Stock";
        dgvIngredients->Columns["IsLowStock"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
        dgvIngredients->Columns["IsLowStock"]->Width = 75;
        dgvIngredients->Columns["IsLowStock"]->DefaultCellStyle->Alignment = DataGridViewContentAlignment::MiddleCenter;
    }
    if (dgvIngredients->Columns["SupplierName"] != nullptr) {
        dgvIngredients->Columns["SupplierName"]->DisplayIndex = 6;
        dgvIngredients->Columns["SupplierName"]->HeaderText = "Supplier";
        dgvIngredients->Columns["SupplierName"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
        dgvIngredients->Columns["SupplierName"]->Width = 130;
    }
    if (dgvIngredients->Columns["SupplierPhone"] != nullptr) {
        dgvIngredients->Columns["SupplierPhone"]->DisplayIndex = 7;
        dgvIngredients->Columns["SupplierPhone"]->HeaderText = "Supplier Phone";
        dgvIngredients->Columns["SupplierPhone"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
        dgvIngredients->Columns["SupplierPhone"]->Width = 115;
    }
    if (dgvIngredients->Columns["LastRestocked"] != nullptr) {
        dgvIngredients->Columns["LastRestocked"]->DisplayIndex = 8;
        dgvIngredients->Columns["LastRestocked"]->HeaderText = "Last Restocked";
        dgvIngredients->Columns["LastRestocked"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
        dgvIngredients->Columns["LastRestocked"]->Width = 105;
        dgvIngredients->Columns["LastRestocked"]->DefaultCellStyle->Format = "yyyy-MM-dd";
        dgvIngredients->Columns["LastRestocked"]->DefaultCellStyle->Alignment = DataGridViewContentAlignment::MiddleCenter;
    }
}

void MainForm::FormatDashboardGrids() {
    if (dgvDashTopProducts != nullptr && dgvDashTopProducts->Columns->Count > 0) {
        dgvDashTopProducts->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::None;
        dgvDashTopProducts->ScrollBars = ScrollBars::Both;
        dgvDashTopProducts->RowHeadersVisible = false;
        dgvDashTopProducts->EnableHeadersVisualStyles = false;
        dgvDashTopProducts->ColumnHeadersDefaultCellStyle->BackColor = Color::FromArgb(240, 244, 248);
        dgvDashTopProducts->ColumnHeadersDefaultCellStyle->ForeColor = Color::FromArgb(38, 50, 56);
        dgvDashTopProducts->ColumnHeadersDefaultCellStyle->Font = gcnew Drawing::Font("Segoe UI", 9.0f, FontStyle::Bold);
        dgvDashTopProducts->ColumnHeadersHeight = 30;

        if (dgvDashTopProducts->Columns["ProductName"] != nullptr) {
            dgvDashTopProducts->Columns["ProductName"]->HeaderText = "Juice Product";
            dgvDashTopProducts->Columns["ProductName"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::Fill;
            dgvDashTopProducts->Columns["ProductName"]->MinimumWidth = 140;
        }
        if (dgvDashTopProducts->Columns["TotalQuantitySold"] != nullptr) {
            dgvDashTopProducts->Columns["TotalQuantitySold"]->HeaderText = "Sold";
            dgvDashTopProducts->Columns["TotalQuantitySold"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
            dgvDashTopProducts->Columns["TotalQuantitySold"]->Width = 75;
            dgvDashTopProducts->Columns["TotalQuantitySold"]->DefaultCellStyle->Alignment = DataGridViewContentAlignment::MiddleCenter;
        }
        if (dgvDashTopProducts->Columns["TotalRevenue"] != nullptr) {
            dgvDashTopProducts->Columns["TotalRevenue"]->HeaderText = "Revenue (LKR)";
            dgvDashTopProducts->Columns["TotalRevenue"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
            dgvDashTopProducts->Columns["TotalRevenue"]->Width = 110;
            dgvDashTopProducts->Columns["TotalRevenue"]->DefaultCellStyle->Format = "N2";
            dgvDashTopProducts->Columns["TotalRevenue"]->DefaultCellStyle->Alignment = DataGridViewContentAlignment::MiddleRight;
        }
    }

    if (dgvDashRecentOrders != nullptr && dgvDashRecentOrders->Columns->Count > 0) {
        dgvDashRecentOrders->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::None;
        dgvDashRecentOrders->ScrollBars = ScrollBars::Both;
        dgvDashRecentOrders->RowHeadersVisible = false;
        dgvDashRecentOrders->EnableHeadersVisualStyles = false;
        dgvDashRecentOrders->ColumnHeadersDefaultCellStyle->BackColor = Color::FromArgb(240, 244, 248);
        dgvDashRecentOrders->ColumnHeadersDefaultCellStyle->ForeColor = Color::FromArgb(38, 50, 56);
        dgvDashRecentOrders->ColumnHeadersDefaultCellStyle->Font = gcnew Drawing::Font("Segoe UI", 9.0f, FontStyle::Bold);
        dgvDashRecentOrders->ColumnHeadersHeight = 30;

        if (dgvDashRecentOrders->Columns["OrderId"] != nullptr) dgvDashRecentOrders->Columns["OrderId"]->Visible = false;
        if (dgvDashRecentOrders->Columns["CustomerId"] != nullptr) dgvDashRecentOrders->Columns["CustomerId"]->Visible = false;
        if (dgvDashRecentOrders->Columns["Subtotal"] != nullptr) dgvDashRecentOrders->Columns["Subtotal"]->Visible = false;
        if (dgvDashRecentOrders->Columns["Discount"] != nullptr) dgvDashRecentOrders->Columns["Discount"]->Visible = false;
        if (dgvDashRecentOrders->Columns["Notes"] != nullptr) dgvDashRecentOrders->Columns["Notes"]->Visible = false;
        if (dgvDashRecentOrders->Columns["Items"] != nullptr) dgvDashRecentOrders->Columns["Items"]->Visible = false;

        if (dgvDashRecentOrders->Columns["OrderNumber"] != nullptr) {
            dgvDashRecentOrders->Columns["OrderNumber"]->DisplayIndex = 0;
            dgvDashRecentOrders->Columns["OrderNumber"]->HeaderText = "Order #";
            dgvDashRecentOrders->Columns["OrderNumber"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
            dgvDashRecentOrders->Columns["OrderNumber"]->Width = 105;
        }
        if (dgvDashRecentOrders->Columns["CustomerName"] != nullptr) {
            dgvDashRecentOrders->Columns["CustomerName"]->DisplayIndex = 1;
            dgvDashRecentOrders->Columns["CustomerName"]->HeaderText = "Customer";
            dgvDashRecentOrders->Columns["CustomerName"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::Fill;
            dgvDashRecentOrders->Columns["CustomerName"]->MinimumWidth = 120;
        }
        if (dgvDashRecentOrders->Columns["OrderDate"] != nullptr) {
            dgvDashRecentOrders->Columns["OrderDate"]->DisplayIndex = 2;
            dgvDashRecentOrders->Columns["OrderDate"]->HeaderText = "Time";
            dgvDashRecentOrders->Columns["OrderDate"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
            dgvDashRecentOrders->Columns["OrderDate"]->Width = 70;
            dgvDashRecentOrders->Columns["OrderDate"]->DefaultCellStyle->Format = "HH:mm";
            dgvDashRecentOrders->Columns["OrderDate"]->DefaultCellStyle->Alignment = DataGridViewContentAlignment::MiddleCenter;
        }
        if (dgvDashRecentOrders->Columns["FinalTotal"] != nullptr) {
            dgvDashRecentOrders->Columns["FinalTotal"]->DisplayIndex = 3;
            dgvDashRecentOrders->Columns["FinalTotal"]->HeaderText = "Total (LKR)";
            dgvDashRecentOrders->Columns["FinalTotal"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
            dgvDashRecentOrders->Columns["FinalTotal"]->Width = 95;
            dgvDashRecentOrders->Columns["FinalTotal"]->DefaultCellStyle->Format = "N2";
            dgvDashRecentOrders->Columns["FinalTotal"]->DefaultCellStyle->Alignment = DataGridViewContentAlignment::MiddleRight;
        }
        if (dgvDashRecentOrders->Columns["PaymentMethod"] != nullptr) {
            dgvDashRecentOrders->Columns["PaymentMethod"]->DisplayIndex = 4;
            dgvDashRecentOrders->Columns["PaymentMethod"]->HeaderText = "Payment";
            dgvDashRecentOrders->Columns["PaymentMethod"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
            dgvDashRecentOrders->Columns["PaymentMethod"]->Width = 75;
            dgvDashRecentOrders->Columns["PaymentMethod"]->DefaultCellStyle->Alignment = DataGridViewContentAlignment::MiddleCenter;
        }
        if (dgvDashRecentOrders->Columns["Status"] != nullptr) {
            dgvDashRecentOrders->Columns["Status"]->DisplayIndex = 5;
            dgvDashRecentOrders->Columns["Status"]->HeaderText = "Status";
            dgvDashRecentOrders->Columns["Status"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
            dgvDashRecentOrders->Columns["Status"]->Width = 80;
            dgvDashRecentOrders->Columns["Status"]->DefaultCellStyle->Alignment = DataGridViewContentAlignment::MiddleCenter;
        }
    }
}

void MainForm::FormatReportGrids() {
    if (dgvReportTopProducts != nullptr && dgvReportTopProducts->Columns->Count > 0) {
        dgvReportTopProducts->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::None;
        dgvReportTopProducts->ScrollBars = ScrollBars::Both;
        dgvReportTopProducts->RowHeadersVisible = false;
        dgvReportTopProducts->EnableHeadersVisualStyles = false;
        dgvReportTopProducts->ColumnHeadersDefaultCellStyle->BackColor = Color::FromArgb(240, 244, 248);
        dgvReportTopProducts->ColumnHeadersDefaultCellStyle->ForeColor = Color::FromArgb(38, 50, 56);
        dgvReportTopProducts->ColumnHeadersDefaultCellStyle->Font = gcnew Drawing::Font("Segoe UI", 9.0f, FontStyle::Bold);
        dgvReportTopProducts->ColumnHeadersHeight = 30;

        if (dgvReportTopProducts->Columns["ProductName"] != nullptr) {
            dgvReportTopProducts->Columns["ProductName"]->HeaderText = "Juice Product";
            dgvReportTopProducts->Columns["ProductName"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::Fill;
            dgvReportTopProducts->Columns["ProductName"]->MinimumWidth = 140;
        }
        if (dgvReportTopProducts->Columns["TotalQuantitySold"] != nullptr) {
            dgvReportTopProducts->Columns["TotalQuantitySold"]->HeaderText = "Units Sold";
            dgvReportTopProducts->Columns["TotalQuantitySold"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
            dgvReportTopProducts->Columns["TotalQuantitySold"]->Width = 90;
            dgvReportTopProducts->Columns["TotalQuantitySold"]->DefaultCellStyle->Alignment = DataGridViewContentAlignment::MiddleCenter;
        }
        if (dgvReportTopProducts->Columns["TotalRevenue"] != nullptr) {
            dgvReportTopProducts->Columns["TotalRevenue"]->HeaderText = "Revenue (LKR)";
            dgvReportTopProducts->Columns["TotalRevenue"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
            dgvReportTopProducts->Columns["TotalRevenue"]->Width = 120;
            dgvReportTopProducts->Columns["TotalRevenue"]->DefaultCellStyle->Format = "N2";
            dgvReportTopProducts->Columns["TotalRevenue"]->DefaultCellStyle->Alignment = DataGridViewContentAlignment::MiddleRight;
        }
    }

    if (dgvReportDaily != nullptr && dgvReportDaily->Columns->Count > 0) {
        dgvReportDaily->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::None;
        dgvReportDaily->ScrollBars = ScrollBars::Both;
        dgvReportDaily->RowHeadersVisible = false;
        dgvReportDaily->EnableHeadersVisualStyles = false;
        dgvReportDaily->ColumnHeadersDefaultCellStyle->BackColor = Color::FromArgb(240, 244, 248);
        dgvReportDaily->ColumnHeadersDefaultCellStyle->ForeColor = Color::FromArgb(38, 50, 56);
        dgvReportDaily->ColumnHeadersDefaultCellStyle->Font = gcnew Drawing::Font("Segoe UI", 9.0f, FontStyle::Bold);
        dgvReportDaily->ColumnHeadersHeight = 30;

        if (dgvReportDaily->Columns["DateString"] != nullptr) {
            dgvReportDaily->Columns["DateString"]->HeaderText = "Date";
            dgvReportDaily->Columns["DateString"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
            dgvReportDaily->Columns["DateString"]->Width = 110;
            dgvReportDaily->Columns["DateString"]->DefaultCellStyle->Alignment = DataGridViewContentAlignment::MiddleCenter;
        }
        if (dgvReportDaily->Columns["OrderCount"] != nullptr) {
            dgvReportDaily->Columns["OrderCount"]->HeaderText = "Orders";
            dgvReportDaily->Columns["OrderCount"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::None;
            dgvReportDaily->Columns["OrderCount"]->Width = 85;
            dgvReportDaily->Columns["OrderCount"]->DefaultCellStyle->Alignment = DataGridViewContentAlignment::MiddleCenter;
        }
        if (dgvReportDaily->Columns["TotalRevenue"] != nullptr) {
            dgvReportDaily->Columns["TotalRevenue"]->HeaderText = "Daily Revenue (LKR)";
            dgvReportDaily->Columns["TotalRevenue"]->AutoSizeMode = DataGridViewAutoSizeColumnMode::Fill;
            dgvReportDaily->Columns["TotalRevenue"]->MinimumWidth = 120;
            dgvReportDaily->Columns["TotalRevenue"]->DefaultCellStyle->Format = "N2";
            dgvReportDaily->Columns["TotalRevenue"]->DefaultCellStyle->Alignment = DataGridViewContentAlignment::MiddleRight;
        }
    }
}

void MainForm::OnFormResize(Object^ sender, EventArgs^ e) {
    UpdateScreenLayout();
}

void MainForm::UpdateScreenLayout() {
    if (pnlContentContainer == nullptr) return;

    int totalW = pnlContentContainer->ClientSize.Width;
    int totalH = pnlContentContainer->ClientSize.Height;
    if (totalW < 800) totalW = 800;
    if (totalH < 500) totalH = 500;

    // 1. Dashboard Layout
    if (pnlScreenDashboard != nullptr) {
        if (picDashboardBanner != nullptr) {
            picDashboardBanner->Location = Point(10, 8);
            picDashboardBanner->Size = System::Drawing::Size(totalW - 20, 95);
        }

        int cardGap = 12;
        int cardW = Math::Max(180, (totalW - 20 - 3 * cardGap) / 4);
        int cardY = 112;
        if (cardKpiTodaySales != nullptr) {
            cardKpiTodaySales->Location = Point(10, cardY);
            cardKpiTodaySales->Width = cardW;
        }
        if (cardKpiTodayOrders != nullptr) {
            cardKpiTodayOrders->Location = Point(10 + (cardW + cardGap), cardY);
            cardKpiTodayOrders->Width = cardW;
        }
        if (cardKpiProducts != nullptr) {
            cardKpiProducts->Location = Point(10 + 2 * (cardW + cardGap), cardY);
            cardKpiProducts->Width = cardW;
        }
        if (cardKpiLowStock != nullptr) {
            cardKpiLowStock->Location = Point(10 + 3 * (cardW + cardGap), cardY);
            cardKpiLowStock->Width = cardW;
        }

        int gridY = 226;
        int gridH = Math::Max(200, totalH - gridY - 30);
        int dashGap = 15;
        int topProdW = Math::Max(320, (totalW - 20 - dashGap) * 44 / 100);
        int recentOrdW = (totalW - 20 - dashGap) - topProdW;
        int recentOrdX = 10 + topProdW + dashGap;

        if (lblDashT1 != nullptr) lblDashT1->Location = Point(10, gridY);
        if (dgvDashTopProducts != nullptr) {
            dgvDashTopProducts->Location = Point(10, gridY + 22);
            dgvDashTopProducts->Size = System::Drawing::Size(topProdW, gridH);
        }

        if (lblDashT2 != nullptr) lblDashT2->Location = Point(recentOrdX, gridY);
        if (dgvDashRecentOrders != nullptr) {
            dgvDashRecentOrders->Location = Point(recentOrdX, gridY + 22);
            dgvDashRecentOrders->Size = System::Drawing::Size(recentOrdW, gridH);
        }
    }

    // 2. Product Management Layout
    if (pnlScreenProducts != nullptr) {
        if (pnlProdForm != nullptr) {
            pnlProdForm->Location = Point(10, 10);
            pnlProdForm->Size = System::Drawing::Size(310, totalH - 20);
        }
        int prodGridX = 330;
        int prodGridW = Math::Max(450, totalW - prodGridX - 10);
        int prodGridH = Math::Max(300, totalH - 50);

        if (lblSearchProd != nullptr) lblSearchProd->Location = Point(prodGridX, 14);
        if (txtProdSearch != nullptr) {
            txtProdSearch->Location = Point(prodGridX + 55, 11);
            txtProdSearch->Width = Math::Min(220, (prodGridW / 2) - 80);
        }
        if (lblFilterCat != nullptr && txtProdSearch != nullptr) {
            lblFilterCat->Location = Point(txtProdSearch->Right + 15, 14);
        }
        if (cmbProdFilter != nullptr && lblFilterCat != nullptr) {
            cmbProdFilter->Location = Point(lblFilterCat->Right + 10, 11);
            cmbProdFilter->Width = 170;
        }
        if (dgvProducts != nullptr) {
            dgvProducts->Location = Point(prodGridX, 40);
            dgvProducts->Size = System::Drawing::Size(prodGridW, prodGridH);
        }
    }

    // 3. Orders & POS Layout
    if (pnlScreenOrders != nullptr) {
        int posGap = 12;
        int catalogW = Math::Max(440, (totalW - 20 - posGap) * 48 / 100);
        int cartPanelW = (totalW - 20 - posGap) - catalogW;
        int cartPanelX = 10 + catalogW + posGap;
        int panelH = totalH - 20;

        if (pnlPosCatalog != nullptr) {
            pnlPosCatalog->Location = Point(10, 10);
            pnlPosCatalog->Size = System::Drawing::Size(catalogW, panelH);

            if (txtPosSearch != nullptr && cmbPosCategory != nullptr) {
                int searchW = Math::Max(150, (catalogW - 25) / 2);
                txtPosSearch->Location = Point(10, 36);
                txtPosSearch->Width = searchW;
                cmbPosCategory->Location = Point(10 + searchW + 5, 36);
                cmbPosCategory->Width = catalogW - 10 - cmbPosCategory->Left;
            }

            int posBotY = panelH - 46;
            if (lblPosQty != nullptr) lblPosQty->Location = Point(10, posBotY + 6);
            if (nudPosQty != nullptr) nudPosQty->Location = Point(44, posBotY + 4);
            if (btnPosAddToCart != nullptr) {
                btnPosAddToCart->Location = Point(110, posBotY);
                btnPosAddToCart->Size = System::Drawing::Size(Math::Max(180, catalogW - 120), 34);
            }

            if (dgvPosProducts != nullptr) {
                dgvPosProducts->Location = Point(10, 68);
                dgvPosProducts->Size = System::Drawing::Size(catalogW - 20, Math::Max(200, posBotY - 76));
            }
        }

        if (pnlPosCartPanel != nullptr) {
            pnlPosCartPanel->Location = Point(cartPanelX, 10);
            pnlPosCartPanel->Size = System::Drawing::Size(cartPanelW, panelH);

            if (btnPosNewCustomer != nullptr) {
                btnPosNewCustomer->Location = Point(cartPanelW - 85, 34);
            }
            if (cmbPosCustomer != nullptr && btnPosNewCustomer != nullptr) {
                cmbPosCustomer->Location = Point(78, 34);
                cmbPosCustomer->Width = Math::Max(120, btnPosNewCustomer->Left - 85);
            }

            int checkoutSectionH = 260;
            int cartGridH = Math::Max(140, panelH - 68 - checkoutSectionH);
            if (dgvPosCart != nullptr) {
                dgvPosCart->Location = Point(12, 68);
                dgvPosCart->Size = System::Drawing::Size(cartPanelW - 24, cartGridH);
            }

            int totY = 68 + cartGridH + 8;
            if (lblSubT != nullptr) lblSubT->Location = Point(12, totY);
            if (lblPosSubtotal != nullptr) lblPosSubtotal->Location = Point(Math::Max(160, cartPanelW - 140), totY);
            totY += 24;

            if (lblDiscT != nullptr) lblDiscT->Location = Point(12, totY);
            if (nudPosDiscount != nullptr) {
                nudPosDiscount->Location = Point(Math::Max(160, cartPanelW - 140), totY - 2);
                nudPosDiscount->Width = 120;
            }
            totY += 26;

            if (lblNetT != nullptr) lblNetT->Location = Point(12, totY);
            if (lblPosFinalTotal != nullptr) lblPosFinalTotal->Location = Point(Math::Max(150, cartPanelW - 150), totY);
            totY += 30;

            if (lblPayL != nullptr) lblPayL->Location = Point(12, totY);
            if (rdoPayCash != nullptr) rdoPayCash->Location = Point(90, totY - 2);
            if (rdoPayCard != nullptr) rdoPayCard->Location = Point(160, totY - 2);
            totY += 26;

            if (pnlCashDetails != nullptr) {
                pnlCashDetails->Location = Point(12, totY);
                pnlCashDetails->Width = cartPanelW - 24;
            }
            if (pnlCardDetails != nullptr) {
                pnlCardDetails->Location = Point(12, totY);
                pnlCardDetails->Width = cartPanelW - 24;
            }
            totY += 38;

            if (btnPosCheckout != nullptr) {
                int btnW = Math::Max(120, (cartPanelW - 40) / 2);
                btnPosCheckout->Location = Point(12, totY);
                btnPosCheckout->Size = System::Drawing::Size(btnW, 36);
                if (btnPosClearCart != nullptr) {
                    btnPosClearCart->Location = Point(btnPosCheckout->Right + 8, totY);
                    btnPosClearCart->Size = System::Drawing::Size(Math::Max(75, (cartPanelW - 36 - btnW) / 2), 36);
                }
                if (btnPosViewReceipt != nullptr && btnPosClearCart != nullptr) {
                    btnPosViewReceipt->Location = Point(btnPosClearCart->Right + 8, totY);
                    btnPosViewReceipt->Size = System::Drawing::Size(Math::Max(75, cartPanelW - 12 - (btnPosClearCart->Right + 8)), 36);
                }
            }
        }
    }

    // 4. Customer Management Layout
    if (pnlScreenCustomers != nullptr) {
        if (pnlCustForm != nullptr) {
            pnlCustForm->Location = Point(10, 10);
            pnlCustForm->Size = System::Drawing::Size(310, totalH - 20);
        }
        int custGridX = 330;
        int custGridW = Math::Max(450, totalW - custGridX - 10);
        int custGridH = Math::Max(300, totalH - 50);

        if (lblSearchCust != nullptr) lblSearchCust->Location = Point(custGridX, 14);
        if (txtCustSearch != nullptr) {
            txtCustSearch->Location = Point(custGridX + 130, 11);
            txtCustSearch->Width = Math::Min(320, custGridW - 140);
        }
        if (dgvCustomers != nullptr) {
            dgvCustomers->Location = Point(custGridX, 40);
            dgvCustomers->Size = System::Drawing::Size(custGridW, custGridH);
        }
    }

    // 5. Ingredient Inventory Layout
    if (pnlScreenInventory != nullptr) {
        if (pnlIngForm != nullptr) {
            pnlIngForm->Location = Point(10, 10);
            pnlIngForm->Size = System::Drawing::Size(310, totalH - 20);
        }
        int ingGridX = 330;
        int ingGridW = Math::Max(450, totalW - ingGridX - 10);
        int ingGridH = Math::Max(300, totalH - 50);

        if (lblSearchIng != nullptr) lblSearchIng->Location = Point(ingGridX, 14);
        if (txtIngSearch != nullptr) {
            txtIngSearch->Location = Point(ingGridX + 125, 11);
            txtIngSearch->Width = Math::Min(220, (ingGridW / 2) - 100);
        }
        if (chkIngLowStockOnly != nullptr && txtIngSearch != nullptr) {
            chkIngLowStockOnly->Location = Point(txtIngSearch->Right + 15, 13);
        }
        if (dgvIngredients != nullptr) {
            dgvIngredients->Location = Point(ingGridX, 40);
            dgvIngredients->Size = System::Drawing::Size(ingGridW, ingGridH);
        }
    }

    // 6. Sales Reports Layout
    if (pnlScreenReports != nullptr) {
        if (pnlRepFilter != nullptr) {
            pnlRepFilter->Location = Point(10, 8);
            pnlRepFilter->Size = System::Drawing::Size(totalW - 20, 44);
        }

        int rGap = 12;
        int rCardW = Math::Max(220, (totalW - 20 - 2 * rGap) / 3);
        int rcardY = 58;
        if (cardRep1 != nullptr) {
            cardRep1->Location = Point(10, rcardY);
            cardRep1->Width = rCardW;
        }
        if (cardRep2 != nullptr) {
            cardRep2->Location = Point(10 + (rCardW + rGap), rcardY);
            cardRep2->Width = rCardW;
        }
        if (cardRep3 != nullptr) {
            cardRep3->Location = Point(10 + 2 * (rCardW + rGap), rcardY);
            cardRep3->Width = rCardW;
        }

        int chartY = 135;
        int chartH = 155;
        if (pnlReportChart != nullptr) {
            pnlReportChart->Location = Point(10, chartY);
            pnlReportChart->Size = System::Drawing::Size(totalW - 20, chartH);
        }

        int rtblY = chartY + chartH + 10;
        int rtblH = Math::Max(160, totalH - rtblY - 32);
        int repGap = 15;
        int repTopW = Math::Max(320, (totalW - 20 - repGap) * 48 / 100);
        int repDailyW = (totalW - 20 - repGap) - repTopW;
        int repDailyX = 10 + repTopW + repGap;

        if (lblR1 != nullptr) lblR1->Location = Point(10, rtblY);
        if (dgvReportTopProducts != nullptr) {
            dgvReportTopProducts->Location = Point(10, rtblY + 22);
            dgvReportTopProducts->Size = System::Drawing::Size(repTopW, rtblH);
        }

        if (lblR2 != nullptr) lblR2->Location = Point(repDailyX, rtblY);
        if (dgvReportDaily != nullptr) {
            dgvReportDaily->Location = Point(repDailyX, rtblY + 22);
            dgvReportDaily->Size = System::Drawing::Size(repDailyW, rtblH);
        }
    }
}

void MainForm::LoadDashboardData() {
    UpdateSystemStatusIndicator();

    Decimal todayRev = ReportService::GetTodaySalesTotal();
    int todayOrders = ReportService::GetTodayOrderCount();
    int activeProds = ReportService::GetAvailableProductsCount();
    int lowStockCount = InventoryService::GetLowStockCount();

    lblKpiSalesVal->Text = String::Format("LKR {0:N2}", todayRev);
    lblKpiOrdersVal->Text = todayOrders.ToString();
    lblKpiProductsVal->Text = activeProds.ToString();
    lblKpiLowStockVal->Text = lowStockCount.ToString() + (lowStockCount > 0 ? " Items!" : " Items");
    lblKpiLowStockVal->ForeColor = (lowStockCount > 0) ? Color::FromArgb(220, 53, 69) : Color::FromArgb(46, 139, 87);

    List<ProductSalesSummary^>^ top = ReportService::GetTopSellingProducts(5);
    dgvDashTopProducts->DataSource = top;

    List<Order^>^ recent = OrderService::GetRecentOrders(6);
    dgvDashRecentOrders->DataSource = recent;

    FormatDashboardGrids();
}

void MainForm::LoadProductsData() {
    List<Product^>^ list = ProductService::GetAllProducts();
    dgvProducts->DataSource = list;
    PopulateProductGridImages();
}

void MainForm::OnProductSearchChanged(Object^ sender, EventArgs^ e) {
    String^ kw = txtProdSearch->Text;
    String^ cat = cmbProdFilter->SelectedItem ? cmbProdFilter->SelectedItem->ToString() : "All Categories";
    dgvProducts->DataSource = ProductService::SearchProducts(kw, cat);
    PopulateProductGridImages();
}

void MainForm::OnProductSelectionChanged(Object^ sender, EventArgs^ e) {
    if (dgvProducts->SelectedRows->Count > 0) {
        DataGridViewRow^ row = dgvProducts->SelectedRows[0];
        Product^ p = dynamic_cast<Product^>(row->DataBoundItem);
        if (p != nullptr) {
            txtProdId->Text = p->ProductId.ToString();
            txtProdName->Text = p->ProductName;
            cmbProdCategory->SelectedItem = p->Category;
            txtProdDesc->Text = p->Description;
            txtProdPrice->Text = p->Price.ToString("F2");
            nudProdQty->Value = Math::Min(Math::Max(0, p->Quantity), 9999);
            txtProdImagePath->Text = p->ImagePath;
            chkProdActive->Checked = p->IsActive;

            picProdPreview->Image = SafeLoadImage(p->ImagePath);
        }
    }
}

void MainForm::OnBrowseProductImage(Object^ sender, EventArgs^ e) {
    OpenFileDialog^ dlg = gcnew OpenFileDialog();
    dlg->Filter = "Image Files (*.png;*.jpg;*.jpeg;*.bmp)|*.png;*.jpg;*.jpeg;*.bmp|All Files (*.*)|*.*";
    if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
        txtProdImagePath->Text = dlg->FileName;
        picProdPreview->Image = SafeLoadImage(dlg->FileName);
    }
}

void MainForm::OnProductAdd(Object^ sender, EventArgs^ e) {
    try {
        Decimal price = Decimal::Parse(txtProdPrice->Text);
        Product^ p = gcnew Product();
        p->ProductName = txtProdName->Text;
        p->Category = cmbProdCategory->SelectedItem->ToString();
        p->Description = txtProdDesc->Text;
        p->Price = price;
        p->Quantity = Convert::ToInt32(nudProdQty->Value);
        p->ImagePath = txtProdImagePath->Text;
        p->IsActive = chkProdActive->Checked;

        String^ err = "";
        bool ok = ProductService::AddProduct(p, err);
        if (ok) {
            MessageBox::Show("Product added successfully!", "Success", MessageBoxButtons::OK, MessageBoxIcon::Information);
            OnProductClear(sender, e);
            LoadProductsData();
        } else {
            MessageBox::Show("Failed to add product: " + err, "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
        }
    }
    catch (Exception^ ex) {
        MessageBox::Show("Validation error: " + ex->Message, "Invalid Input", MessageBoxButtons::OK, MessageBoxIcon::Warning);
    }
}

void MainForm::OnProductUpdate(Object^ sender, EventArgs^ e) {
    if (String::IsNullOrWhiteSpace(txtProdId->Text)) {
        MessageBox::Show("Please select a product from the list to update.", "Selection Required", MessageBoxButtons::OK, MessageBoxIcon::Information);
        return;
    }

    try {
        Decimal price = Decimal::Parse(txtProdPrice->Text);
        Product^ p = gcnew Product();
        p->ProductId = Convert::ToInt32(txtProdId->Text);
        p->ProductName = txtProdName->Text;
        p->Category = cmbProdCategory->SelectedItem->ToString();
        p->Description = txtProdDesc->Text;
        p->Price = price;
        p->Quantity = Convert::ToInt32(nudProdQty->Value);
        p->ImagePath = txtProdImagePath->Text;
        p->IsActive = chkProdActive->Checked;

        String^ err = "";
        bool ok = ProductService::UpdateProduct(p, err);
        if (ok) {
            MessageBox::Show("Product updated successfully!", "Success", MessageBoxButtons::OK, MessageBoxIcon::Information);
            LoadProductsData();
        } else {
            MessageBox::Show("Failed to update product: " + err, "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
        }
    }
    catch (Exception^ ex) {
        MessageBox::Show("Validation error: " + ex->Message, "Invalid Input", MessageBoxButtons::OK, MessageBoxIcon::Warning);
    }
}

void MainForm::OnProductDelete(Object^ sender, EventArgs^ e) {
    if (String::IsNullOrWhiteSpace(txtProdId->Text)) {
        MessageBox::Show("Please select a product to delete.", "Selection Required", MessageBoxButtons::OK, MessageBoxIcon::Information);
        return;
    }

    int pid = Convert::ToInt32(txtProdId->Text);
    String^ name = txtProdName->Text;

    if (MessageBox::Show("Are you sure you want to delete product '" + name + "'?", "Confirm Deletion",
        MessageBoxButtons::YesNo, MessageBoxIcon::Question) == System::Windows::Forms::DialogResult::Yes) {
        String^ err = "";
        bool ok = ProductService::DeleteProduct(pid, err);
        if (ok) {
            MessageBox::Show(String::IsNullOrEmpty(err) ? "Product deleted successfully." : err, "Result", MessageBoxButtons::OK, MessageBoxIcon::Information);
            OnProductClear(sender, e);
            LoadProductsData();
        } else {
            MessageBox::Show("Delete failed: " + err, "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
        }
    }
}

void MainForm::OnProductClear(Object^ sender, EventArgs^ e) {
    txtProdId->Text = "";
    txtProdName->Text = "";
    txtProdDesc->Text = "";
    txtProdPrice->Text = "350.00";
    nudProdQty->Value = 20;
    txtProdImagePath->Text = "";
    chkProdActive->Checked = true;
    picProdPreview->Image = nullptr;
    if (cmbProdCategory->Items->Count > 0) cmbProdCategory->SelectedIndex = 0;
}

void MainForm::ResetPosOrder() {
    m_currentPosOrder = gcnew Order();
    lblPosOrderNumber->Text = "Order: " + m_currentPosOrder->OrderNumber;
    nudPosDiscount->Value = Decimal(0);
    txtCashTendered->Text = "0.00";
    lblCashChange->Text = "Change: LKR 0.00";
    dgvPosCart->DataSource = nullptr;
    dgvPosCart->DataSource = m_currentPosOrder->Items;
    FormatPosCartGrid();
    lblPosSubtotal->Text = "LKR 0.00";
    lblPosFinalTotal->Text = "LKR 0.00";
}

void MainForm::LoadPosData() {
    List<Product^>^ prods = ProductService::GetActiveProducts();
    dgvPosProducts->DataSource = prods;
    PopulatePosGridImages();

    List<Customer^>^ custs = CustomerService::GetAllCustomers();
    cmbPosCustomer->Items->Clear();
    cmbPosCustomer->Items->Add("Walk-in Customer");
    for each (Customer^ c in custs) {
        cmbPosCustomer->Items->Add(c->Id.ToString() + " - " + c->Name + " (" + c->Phone + ")");
    }
    cmbPosCustomer->SelectedIndex = 0;
}

void MainForm::OnPosFilterChanged(Object^ sender, EventArgs^ e) {
    String^ kw = txtPosSearch->Text;
    String^ cat = cmbPosCategory->SelectedItem ? cmbPosCategory->SelectedItem->ToString() : "All Categories";
    dgvPosProducts->DataSource = ProductService::SearchProducts(kw, cat);
    PopulatePosGridImages();
}

void MainForm::OnPosAddToCart(Object^ sender, EventArgs^ e) {
    if (dgvPosProducts->SelectedRows->Count == 0) {
        MessageBox::Show("Please select a juice blend from the catalog.", "Item Required", MessageBoxButtons::OK, MessageBoxIcon::Information);
        return;
    }

    Product^ p = dynamic_cast<Product^>(dgvPosProducts->SelectedRows[0]->DataBoundItem);
    if (p == nullptr) return;

    int qty = Convert::ToInt32(nudPosQty->Value);
    if (qty > p->Quantity) {
        MessageBox::Show(String::Format("Only {0} units available in stock.", p->Quantity), "Stock Limit", MessageBoxButtons::OK, MessageBoxIcon::Warning);
        return;
    }

    bool found = false;
    for each (OrderItem^ it in m_currentPosOrder->Items) {
        if (it->ProductId == p->ProductId) {
            it->Quantity += qty;
            found = true;
            break;
        }
    }

    if (!found) {
        OrderItem^ newItem = gcnew OrderItem(p->ProductId, p->ProductName, qty, p->Price);
        m_currentPosOrder->Items->Add(newItem);
    }

    m_currentPosOrder->RecalculateTotals();
    lblPosSubtotal->Text = String::Format("LKR {0:N2}", m_currentPosOrder->Subtotal);
    lblPosFinalTotal->Text = String::Format("LKR {0:N2}", m_currentPosOrder->FinalTotal);

    dgvPosCart->DataSource = nullptr;
    dgvPosCart->DataSource = m_currentPosOrder->Items;
    FormatPosCartGrid();

    OnCashTenderedChanged(sender, e);
}

void MainForm::OnPosCartCellClick(Object^ sender, DataGridViewCellEventArgs^ e) {
    if (e->RowIndex >= 0 && e->RowIndex < m_currentPosOrder->Items->Count) {
        OrderItem^ it = m_currentPosOrder->Items[e->RowIndex];
        if (MessageBox::Show("Remove '" + it->ProductName + "' from cart?", "Remove Item", MessageBoxButtons::YesNo, MessageBoxIcon::Question) == System::Windows::Forms::DialogResult::Yes) {
            m_currentPosOrder->Items->RemoveAt(e->RowIndex);
            m_currentPosOrder->RecalculateTotals();
            lblPosSubtotal->Text = String::Format("LKR {0:N2}", m_currentPosOrder->Subtotal);
            lblPosFinalTotal->Text = String::Format("LKR {0:N2}", m_currentPosOrder->FinalTotal);
            dgvPosCart->DataSource = nullptr;
            dgvPosCart->DataSource = m_currentPosOrder->Items;
            FormatPosCartGrid();
            OnCashTenderedChanged(sender, e);
        }
    }
}

void MainForm::OnPosDiscountChanged(Object^ sender, EventArgs^ e) {
    if (m_currentPosOrder != nullptr) {
        m_currentPosOrder->Discount = nudPosDiscount->Value;
        m_currentPosOrder->RecalculateTotals();
        lblPosFinalTotal->Text = String::Format("LKR {0:N2}", m_currentPosOrder->FinalTotal);
        OnCashTenderedChanged(sender, e);
    }
}

void MainForm::OnPosPaymentModeChanged(Object^ sender, EventArgs^ e) {
    if (rdoPayCash->Checked) {
        pnlCashDetails->Visible = true;
        pnlCardDetails->Visible = false;
    } else {
        pnlCashDetails->Visible = false;
        pnlCardDetails->Visible = true;
    }
}

void MainForm::OnCashTenderedChanged(Object^ sender, EventArgs^ e) {
    try {
        Decimal tendered = Decimal::Parse(txtCashTendered->Text);
        Decimal finalTot = m_currentPosOrder->FinalTotal;
        Decimal diff = tendered - finalTot;
        if (diff >= Decimal(0)) {
            lblCashChange->Text = String::Format("Change: LKR {0:N2}", diff);
            lblCashChange->ForeColor = Color::FromArgb(46, 139, 87);
        } else {
            lblCashChange->Text = String::Format("Shortage: LKR {0:N2}", -diff);
            lblCashChange->ForeColor = Color::FromArgb(220, 53, 69);
        }
    }
    catch (Exception^) {
        lblCashChange->Text = "Change: --";
    }
}

void MainForm::OnPosCheckout(Object^ sender, EventArgs^ e) {
    if (m_currentPosOrder->Items->Count == 0) {
        MessageBox::Show("Cart is empty! Please add products before checking out.", "Empty Order", MessageBoxButtons::OK, MessageBoxIcon::Warning);
        return;
    }

    if (cmbPosCustomer->SelectedIndex > 0) {
        String^ selectedStr = cmbPosCustomer->SelectedItem->ToString();
        int dashIdx = selectedStr->IndexOf(" - ");
        if (dashIdx > 0) {
            int cid = Convert::ToInt32(selectedStr->Substring(0, dashIdx));
            m_currentPosOrder->CustomerId = Nullable<int>(cid);
            m_currentPosOrder->CustomerName = selectedStr->Substring(dashIdx + 3);
        }
    } else {
        m_currentPosOrder->CustomerId = Nullable<int>();
        m_currentPosOrder->CustomerName = "Walk-in Customer";
    }

    PaymentMethod^ payment = nullptr;
    if (rdoPayCash->Checked) {
        Decimal tendered = Decimal(0);
        if (!Decimal::TryParse(txtCashTendered->Text, tendered)) {
            MessageBox::Show("Please enter a valid cash amount tendered.", "Invalid Tendered Cash", MessageBoxButtons::OK, MessageBoxIcon::Warning);
            return;
        }
        payment = gcnew CashPayment(tendered);
    } else {
        String^ ctype = cmbCardType->SelectedItem ? cmbCardType->SelectedItem->ToString() : "Visa";
        String^ l4 = txtCardLast4->Text;
        payment = gcnew CardPayment(ctype, l4);
    }

    String^ receipt = "";
    String^ errorMsg = "";
    bool success = OrderService::ProcessAndSaveOrder(m_currentPosOrder, payment, receipt, errorMsg);

    if (success) {
        m_lastReceiptText = receipt;
        MessageBox::Show("Order placed and saved successfully!", "Checkout Completed", MessageBoxButtons::OK, MessageBoxIcon::Information);

        ReceiptForm^ dlg = gcnew ReceiptForm(receipt);
        dlg->ShowDialog(this);

        ResetPosOrder();
        LoadPosData();
    } else {
        MessageBox::Show("Checkout failed: " + errorMsg, "Transaction Failed", MessageBoxButtons::OK, MessageBoxIcon::Error);
    }
}

void MainForm::OnPosClearCart(Object^ sender, EventArgs^ e) {
    if (MessageBox::Show("Are you sure you want to clear the shopping cart?", "Clear Cart", MessageBoxButtons::YesNo, MessageBoxIcon::Question) == System::Windows::Forms::DialogResult::Yes) {
        ResetPosOrder();
    }
}

void MainForm::OnPosViewReceipt(Object^ sender, EventArgs^ e) {
    if (String::IsNullOrEmpty(m_lastReceiptText)) {
        MessageBox::Show("No completed order receipt available for preview.", "Receipt Preview", MessageBoxButtons::OK, MessageBoxIcon::Information);
        return;
    }
    ReceiptForm^ dlg = gcnew ReceiptForm(m_lastReceiptText);
    dlg->ShowDialog(this);
}

void MainForm::LoadCustomersData() {
    dgvCustomers->DataSource = CustomerService::GetAllCustomers();
    FormatCustomerGrid();
}

void MainForm::OnCustomerSearchChanged(Object^ sender, EventArgs^ e) {
    dgvCustomers->DataSource = CustomerService::SearchCustomers(txtCustSearch->Text);
    FormatCustomerGrid();
}

void MainForm::OnCustomerSelectionChanged(Object^ sender, EventArgs^ e) {
    if (dgvCustomers->SelectedRows->Count > 0) {
        Customer^ c = dynamic_cast<Customer^>(dgvCustomers->SelectedRows[0]->DataBoundItem);
        if (c != nullptr) {
            txtCustId->Text = c->Id.ToString();
            txtCustName->Text = c->Name;
            txtCustPhone->Text = c->Phone;
            txtCustEmail->Text = c->Email;
            txtCustAddress->Text = c->Address;
        }
    }
}

void MainForm::OnCustomerAdd(Object^ sender, EventArgs^ e) {
    try {
        Customer^ c = gcnew Customer();
        c->Name = txtCustName->Text;
        c->Phone = txtCustPhone->Text;
        c->Email = txtCustEmail->Text;
        c->Address = txtCustAddress->Text;

        String^ err = "";
        bool ok = CustomerService::AddCustomer(c, err);
        if (ok) {
            MessageBox::Show("Customer registered successfully!", "Success", MessageBoxButtons::OK, MessageBoxIcon::Information);
            OnCustomerClear(sender, e);
            LoadCustomersData();
        } else {
            MessageBox::Show("Failed to register customer: " + err, "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
        }
    }
    catch (Exception^ ex) {
        MessageBox::Show("Validation error: " + ex->Message, "Invalid Input", MessageBoxButtons::OK, MessageBoxIcon::Warning);
    }
}

void MainForm::OnCustomerUpdate(Object^ sender, EventArgs^ e) {
    if (String::IsNullOrWhiteSpace(txtCustId->Text)) {
        MessageBox::Show("Select a customer from the table to update.", "Selection Required", MessageBoxButtons::OK, MessageBoxIcon::Information);
        return;
    }

    try {
        Customer^ c = gcnew Customer();
        c->Id = Convert::ToInt32(txtCustId->Text);
        c->Name = txtCustName->Text;
        c->Phone = txtCustPhone->Text;
        c->Email = txtCustEmail->Text;
        c->Address = txtCustAddress->Text;

        String^ err = "";
        bool ok = CustomerService::UpdateCustomer(c, err);
        if (ok) {
            MessageBox::Show("Customer updated successfully!", "Success", MessageBoxButtons::OK, MessageBoxIcon::Information);
            LoadCustomersData();
        } else {
            MessageBox::Show("Failed to update customer: " + err, "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
        }
    }
    catch (Exception^ ex) {
        MessageBox::Show("Validation error: " + ex->Message, "Invalid Input", MessageBoxButtons::OK, MessageBoxIcon::Warning);
    }
}

void MainForm::OnCustomerDelete(Object^ sender, EventArgs^ e) {
    if (String::IsNullOrWhiteSpace(txtCustId->Text)) {
        MessageBox::Show("Select a customer from the table to delete.", "Selection Required", MessageBoxButtons::OK, MessageBoxIcon::Information);
        return;
    }

    int cid = Convert::ToInt32(txtCustId->Text);
    String^ name = txtCustName->Text;

    if (MessageBox::Show("Delete customer '" + name + "'?", "Confirm Delete", MessageBoxButtons::YesNo, MessageBoxIcon::Question) == System::Windows::Forms::DialogResult::Yes) {
        String^ err = "";
        bool ok = CustomerService::DeleteCustomer(cid, err);
        if (ok) {
            MessageBox::Show("Customer deleted successfully.", "Result", MessageBoxButtons::OK, MessageBoxIcon::Information);
            OnCustomerClear(sender, e);
            LoadCustomersData();
        } else {
            MessageBox::Show("Delete failed: " + err, "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
        }
    }
}

void MainForm::OnCustomerClear(Object^ sender, EventArgs^ e) {
    txtCustId->Text = "";
    txtCustName->Text = "";
    txtCustPhone->Text = "";
    txtCustEmail->Text = "";
    txtCustAddress->Text = "";
}

void MainForm::LoadInventoryData() {
    if (chkIngLowStockOnly->Checked) {
        dgvIngredients->DataSource = InventoryService::GetLowStockIngredients();
    } else {
        dgvIngredients->DataSource = InventoryService::GetAllIngredients();
    }
    FormatInventoryGrid();
}

void MainForm::OnInventorySearchChanged(Object^ sender, EventArgs^ e) {
    if (chkIngLowStockOnly->Checked) {
        dgvIngredients->DataSource = InventoryService::GetLowStockIngredients();
    } else {
        dgvIngredients->DataSource = InventoryService::SearchIngredients(txtIngSearch->Text);
    }
    FormatInventoryGrid();
}

void MainForm::OnInventorySelectionChanged(Object^ sender, EventArgs^ e) {
    if (dgvIngredients->SelectedRows->Count > 0) {
        Ingredient^ ing = dynamic_cast<Ingredient^>(dgvIngredients->SelectedRows[0]->DataBoundItem);
        if (ing != nullptr) {
            txtIngId->Text = ing->IngredientId.ToString();
            txtIngName->Text = ing->IngredientName;
            nudIngStock->Value = Math::Min(Math::Max(Decimal(0), ing->QuantityInStock), Decimal(99999));
            cmbIngUnit->SelectedItem = ing->UnitOfMeasure;
            nudIngReorder->Value = Math::Min(Math::Max(Decimal(0), ing->ReorderLevel), Decimal(9999));
            txtIngSupplier->Text = ing->SupplierName;
            txtIngSupplierPhone->Text = ing->SupplierPhone;
        }
    }
}

void MainForm::OnIngredientAdd(Object^ sender, EventArgs^ e) {
    try {
        Ingredient^ ing = gcnew Ingredient();
        ing->IngredientName = txtIngName->Text;
        ing->QuantityInStock = nudIngStock->Value;
        ing->UnitOfMeasure = cmbIngUnit->SelectedItem->ToString();
        ing->ReorderLevel = nudIngReorder->Value;
        ing->SupplierName = txtIngSupplier->Text;
        ing->SupplierPhone = txtIngSupplierPhone->Text;

        String^ err = "";
        bool ok = InventoryService::AddIngredient(ing, err);
        if (ok) {
            MessageBox::Show("Ingredient added to inventory!", "Success", MessageBoxButtons::OK, MessageBoxIcon::Information);
            OnIngredientClear(sender, e);
            LoadInventoryData();
        } else {
            MessageBox::Show("Failed to add ingredient: " + err, "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
        }
    }
    catch (Exception^ ex) {
        MessageBox::Show("Validation error: " + ex->Message, "Invalid Input", MessageBoxButtons::OK, MessageBoxIcon::Warning);
    }
}

void MainForm::OnIngredientUpdate(Object^ sender, EventArgs^ e) {
    if (String::IsNullOrWhiteSpace(txtIngId->Text)) {
        MessageBox::Show("Select an ingredient from the table to update.", "Selection Required", MessageBoxButtons::OK, MessageBoxIcon::Information);
        return;
    }

    try {
        Ingredient^ ing = gcnew Ingredient();
        ing->IngredientId = Convert::ToInt32(txtIngId->Text);
        ing->IngredientName = txtIngName->Text;
        ing->QuantityInStock = nudIngStock->Value;
        ing->UnitOfMeasure = cmbIngUnit->SelectedItem->ToString();
        ing->ReorderLevel = nudIngReorder->Value;
        ing->SupplierName = txtIngSupplier->Text;
        ing->SupplierPhone = txtIngSupplierPhone->Text;

        String^ err = "";
        bool ok = InventoryService::UpdateIngredient(ing, err);
        if (ok) {
            MessageBox::Show("Ingredient inventory updated!", "Success", MessageBoxButtons::OK, MessageBoxIcon::Information);
            LoadInventoryData();
        } else {
            MessageBox::Show("Failed to update ingredient: " + err, "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
        }
    }
    catch (Exception^ ex) {
        MessageBox::Show("Validation error: " + ex->Message, "Invalid Input", MessageBoxButtons::OK, MessageBoxIcon::Warning);
    }
}

void MainForm::OnIngredientDelete(Object^ sender, EventArgs^ e) {
    if (String::IsNullOrWhiteSpace(txtIngId->Text)) {
        MessageBox::Show("Select an ingredient to delete.", "Selection Required", MessageBoxButtons::OK, MessageBoxIcon::Information);
        return;
    }

    int id = Convert::ToInt32(txtIngId->Text);
    String^ name = txtIngName->Text;

    if (MessageBox::Show("Delete ingredient '" + name + "'?", "Confirm Delete", MessageBoxButtons::YesNo, MessageBoxIcon::Question) == System::Windows::Forms::DialogResult::Yes) {
        String^ err = "";
        bool ok = InventoryService::DeleteIngredient(id, err);
        if (ok) {
            MessageBox::Show("Ingredient removed from inventory.", "Result", MessageBoxButtons::OK, MessageBoxIcon::Information);
            OnIngredientClear(sender, e);
            LoadInventoryData();
        } else {
            MessageBox::Show("Delete failed: " + err, "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
        }
    }
}

void MainForm::OnIngredientClear(Object^ sender, EventArgs^ e) {
    txtIngId->Text = "";
    txtIngName->Text = "";
    nudIngStock->Value = Decimal(10);
    nudIngReorder->Value = Decimal(5);
    txtIngSupplier->Text = "";
    txtIngSupplierPhone->Text = "";
}

void MainForm::OnInventoryCellFormatting(Object^ sender, DataGridViewCellFormattingEventArgs^ e) {
    if (e->RowIndex >= 0 && e->RowIndex < dgvIngredients->Rows->Count) {
        Ingredient^ ing = dynamic_cast<Ingredient^>(dgvIngredients->Rows[e->RowIndex]->DataBoundItem);
        if (ing != nullptr && ing->IsLowStock) {
            dgvIngredients->Rows[e->RowIndex]->DefaultCellStyle->BackColor = Color::FromArgb(255, 235, 235);
            dgvIngredients->Rows[e->RowIndex]->DefaultCellStyle->ForeColor = Color::FromArgb(180, 20, 20);
        }
    }
}

void MainForm::LoadReportsData() {
    DateTime s = dtpReportStart->Value.Date;
    DateTime e = dtpReportEnd->Value.Date.AddDays(1).AddTicks(-1);

    List<DailyRevenueSummary^>^ rev = ReportService::GetRevenueSummary(s, e);
    dgvReportDaily->DataSource = rev;

    List<ProductSalesSummary^>^ top = ReportService::GetTopSellingProducts(10);
    dgvReportTopProducts->DataSource = top;

    FormatReportGrids();

    Decimal totalRev = Decimal(0);
    int totalOrds = 0;
    for each (DailyRevenueSummary^ d in rev) {
        totalRev += d->TotalRevenue;
        totalOrds += d->OrderCount;
    }

    lblReportRevenueVal->Text = String::Format("LKR {0:N2}", totalRev);
    lblReportOrdersVal->Text = totalOrds.ToString();
    Decimal avg = (totalOrds > 0) ? (totalRev / Decimal(totalOrds)) : Decimal(0);
    lblReportAvgOrderVal->Text = String::Format("LKR {0:N2}", avg);

    pnlReportChart->Invalidate();
}

void MainForm::OnReportFilter(Object^ sender, EventArgs^ e) {
    LoadReportsData();
}

void MainForm::OnReportToday(Object^ sender, EventArgs^ e) {
    dtpReportStart->Value = DateTime::Now;
    dtpReportEnd->Value = DateTime::Now;
    LoadReportsData();
}

void MainForm::OnReportMonth(Object^ sender, EventArgs^ e) {
    DateTime now = DateTime::Now;
    dtpReportStart->Value = DateTime(now.Year, now.Month, 1);
    dtpReportEnd->Value = now;
    LoadReportsData();
}

void MainForm::OnPaintReportChart(Object^ sender, PaintEventArgs^ e) {
    Graphics^ g = e->Graphics;
    g->SmoothingMode = SmoothingMode::AntiAlias;

    List<DailyRevenueSummary^>^ data = dynamic_cast<List<DailyRevenueSummary^>^>(dgvReportDaily->DataSource);
    int w = pnlReportChart->Width;
    int h = pnlReportChart->Height;

    System::Drawing::Font^ titleFont = gcnew System::Drawing::Font("Segoe UI", 10.5f, FontStyle::Bold);
    g->DrawString("Revenue Trend by Date", titleFont, Brushes::DimGray, 15, 12);

    if (data == nullptr || data->Count == 0) {
        System::Drawing::Font^ f = gcnew System::Drawing::Font("Segoe UI", 11);
        g->DrawString("No sales records available for the selected period.", f, Brushes::Gray, 15, 80);
        return;
    }

    Decimal maxVal = Decimal(100);
    for each (DailyRevenueSummary^ d in data) {
        if (d->TotalRevenue > maxVal) maxVal = d->TotalRevenue;
    }

    int leftMargin = 70;
    int bottomMargin = 40;
    int topMargin = 45;
    int chartW = w - leftMargin - 30;
    int chartH = h - bottomMargin - topMargin;

    Pen^ axisPen = gcnew Pen(Color::FromArgb(200, 210, 220), 1.5f);
    g->DrawLine(axisPen, leftMargin, topMargin, leftMargin, topMargin + chartH);
    g->DrawLine(axisPen, leftMargin, topMargin + chartH, leftMargin + chartW, topMargin + chartH);

    int count = data->Count;
    int barWidth = Math::Max(15, Math::Min(65, (chartW / count) - 15));
    int slotWidth = chartW / count;

    SolidBrush^ barBrush = gcnew SolidBrush(Color::FromArgb(46, 139, 87));
    System::Drawing::Font^ labelFont = gcnew System::Drawing::Font("Segoe UI", 8);

    for (int i = 0; i < count; i++) {
        DailyRevenueSummary^ d = data[i];
        float pct = (float)(d->TotalRevenue / maxVal);
        int barH = (int)(chartH * pct);
        int barX = leftMargin + (i * slotWidth) + ((slotWidth - barWidth) / 2);
        int barY = topMargin + chartH - barH;

        g->FillRectangle(barBrush, barX, barY, barWidth, barH);

        String^ valStr = String::Format("{0:N0}", d->TotalRevenue);
        g->DrawString(valStr, labelFont, Brushes::DarkSlateGray, (float)(barX - 5), (float)(barY - 18));

        String^ dtStr = d->DateString;
        if (dtStr->Length >= 5) dtStr = dtStr->Substring(5);
        g->DrawString(dtStr, labelFont, Brushes::Gray, (float)barX, (float)(topMargin + chartH + 8));
    }
}

void MainForm::OnPreviewReportClick(Object^ sender, EventArgs^ e) {
    try {
        PrintPreviewDialog^ dlg = gcnew PrintPreviewDialog();
        dlg->Document = printReportDoc;
        dlg->WindowState = FormWindowState::Maximized;
        dlg->ShowDialog(this);
    }
    catch (Exception^ ex) {
        MessageBox::Show("Unable to open Print Preview: " + ex->Message, "Printing Notice", MessageBoxButtons::OK, MessageBoxIcon::Error);
    }
}

void MainForm::OnPrintReportClick(Object^ sender, EventArgs^ e) {
    try {
        PrintDialog^ dlg = gcnew PrintDialog();
        dlg->Document = printReportDoc;
        dlg->UseEXDialog = true;
        if (dlg->ShowDialog(this) == System::Windows::Forms::DialogResult::OK) {
            printReportDoc->Print();
        }
    }
    catch (Exception^ ex) {
        MessageBox::Show("Unable to send document to printer: " + ex->Message, "Printing Notice", MessageBoxButtons::OK, MessageBoxIcon::Error);
    }
}

void MainForm::OnPrintReportPage(Object^ sender, Drawing::Printing::PrintPageEventArgs^ e) {
    Graphics^ g = e->Graphics;
    g->SmoothingMode = SmoothingMode::AntiAlias;

    // Standard printable bounds
    int left = 50;
    int top = 50;
    int width = e->PageBounds.Width - 100;
    int y = top;

    // Fonts & Brushes
    System::Drawing::Font^ titleFont = gcnew System::Drawing::Font("Segoe UI", 18, FontStyle::Bold);
    System::Drawing::Font^ subTitleFont = gcnew System::Drawing::Font("Segoe UI", 11, FontStyle::Regular);
    System::Drawing::Font^ boldFont = gcnew System::Drawing::Font("Segoe UI", 10, FontStyle::Bold);
    System::Drawing::Font^ regularFont = gcnew System::Drawing::Font("Segoe UI", 9.5f, FontStyle::Regular);
    System::Drawing::Font^ smallFont = gcnew System::Drawing::Font("Segoe UI", 8.5f, FontStyle::Italic);

    Brush^ brandBrush = gcnew SolidBrush(Color::FromArgb(46, 139, 87));
    Brush^ textBrush = gcnew SolidBrush(Color::FromArgb(38, 50, 56));
    Pen^ linePen = gcnew Pen(Color::FromArgb(46, 139, 87), 2.0f);
    Pen^ thinPen = gcnew Pen(Color::FromArgb(220, 220, 220), 1.0f);

    // Business Header
    g->DrawString("FRESHSIP JUICE BAR", titleFont, brandBrush, (float)left, (float)y);
    y += 32;
    g->DrawString("Executive Management & Sales Analytics Summary", subTitleFont, textBrush, (float)left, (float)y);
    y += 24;

    g->DrawLine(linePen, left, y, left + width, y);
    y += 15;

    // Report Metadata
    String^ periodStr = String::Format("Reporting Period: {0:yyyy-MM-dd} to {1:yyyy-MM-dd}", dtpReportStart->Value, dtpReportEnd->Value);
    String^ generatedStr = String::Format("Generated: {0:yyyy-MM-dd HH:mm:ss}  |  Operator: {1} ({2})", DateTime::Now, m_currentUserName, m_currentRole);
    g->DrawString(periodStr, boldFont, textBrush, (float)left, (float)y);
    y += 18;
    g->DrawString(generatedStr, regularFont, Brushes::Gray, (float)left, (float)y);
    y += 26;

    // 3 KPI Summary Boxes
    int kpiW = (width - 20) / 3;
    int kpiH = 55;
    
    // Box 1: Total Revenue
    g->FillRectangle(gcnew SolidBrush(Color::FromArgb(235, 247, 238)), left, y, kpiW, kpiH);
    g->DrawRectangle(gcnew Pen(Color::FromArgb(46, 139, 87), 1.0f), left, y, kpiW, kpiH);
    g->DrawString("TOTAL REVENUE", smallFont, Brushes::Gray, (float)(left + 10), (float)(y + 8));
    g->DrawString(lblReportRevenueVal->Text, boldFont, brandBrush, (float)(left + 10), (float)(y + 26));

    // Box 2: Total Orders
    int kpi2X = left + kpiW + 10;
    g->FillRectangle(gcnew SolidBrush(Color::FromArgb(255, 245, 235)), kpi2X, y, kpiW, kpiH);
    g->DrawRectangle(gcnew Pen(Color::FromArgb(255, 159, 67), 1.0f), kpi2X, y, kpiW, kpiH);
    g->DrawString("ORDERS PROCESSED", smallFont, Brushes::Gray, (float)(kpi2X + 10), (float)(y + 8));
    g->DrawString(lblReportOrdersVal->Text, boldFont, gcnew SolidBrush(Color::FromArgb(230, 110, 20)), (float)(kpi2X + 10), (float)(y + 26));

    // Box 3: Avg Order Value
    int kpi3X = kpi2X + kpiW + 10;
    g->FillRectangle(gcnew SolidBrush(Color::FromArgb(235, 245, 255)), kpi3X, y, kpiW, kpiH);
    g->DrawRectangle(gcnew Pen(Color::FromArgb(33, 150, 243), 1.0f), kpi3X, y, kpiW, kpiH);
    g->DrawString("AVERAGE ORDER VALUE", smallFont, Brushes::Gray, (float)(kpi3X + 10), (float)(y + 8));
    g->DrawString(lblReportAvgOrderVal->Text, boldFont, gcnew SolidBrush(Color::FromArgb(25, 118, 210)), (float)(kpi3X + 10), (float)(y + 26));

    y += kpiH + 22;

    // SECTION 1: Top Selling Products
    g->DrawString("Best-Selling Products Breakdown", boldFont, brandBrush, (float)left, (float)y);
    y += 20;

    // Table Header
    g->FillRectangle(gcnew SolidBrush(Color::FromArgb(245, 245, 245)), left, y, width, 24);
    g->DrawRectangle(thinPen, left, y, width, 24);
    g->DrawString("#", boldFont, textBrush, (float)(left + 8), (float)(y + 4));
    g->DrawString("Product Name", boldFont, textBrush, (float)(left + 40), (float)(y + 4));
    g->DrawString("Quantity Sold", boldFont, textBrush, (float)(left + 380), (float)(y + 4));
    g->DrawString("Total Revenue (LKR)", boldFont, textBrush, (float)(left + 520), (float)(y + 4));
    y += 24;

    List<ProductSalesSummary^>^ topProducts = dynamic_cast<List<ProductSalesSummary^>^>(dgvReportTopProducts->DataSource);
    if (topProducts != nullptr && topProducts->Count > 0) {
        int rank = 1;
        for each (ProductSalesSummary^ p in topProducts) {
            if (rank > 7) break; // Keep cleanly within single page
            g->DrawString(rank.ToString(), regularFont, textBrush, (float)(left + 8), (float)(y + 4));
            g->DrawString(p->ProductName, regularFont, textBrush, (float)(left + 40), (float)(y + 4));
            g->DrawString(p->TotalQuantitySold.ToString(), regularFont, textBrush, (float)(left + 380), (float)(y + 4));
            g->DrawString(String::Format("{0:N2}", p->TotalRevenue), regularFont, textBrush, (float)(left + 520), (float)(y + 4));
            y += 20;
            g->DrawLine(thinPen, left, y, left + width, y);
            rank++;
        }
    }
    else {
        g->DrawString("No product sales recorded in the selected period.", regularFont, Brushes::Gray, (float)(left + 40), (float)(y + 4));
        y += 22;
    }

    y += 18;

    // SECTION 2: Daily Sales History
    g->DrawString("Daily Revenue Breakdown", boldFont, brandBrush, (float)left, (float)y);
    y += 20;

    g->FillRectangle(gcnew SolidBrush(Color::FromArgb(245, 245, 245)), left, y, width, 24);
    g->DrawRectangle(thinPen, left, y, width, 24);
    g->DrawString("Date", boldFont, textBrush, (float)(left + 8), (float)(y + 4));
    g->DrawString("Orders Processed", boldFont, textBrush, (float)(left + 250), (float)(y + 4));
    g->DrawString("Daily Total Revenue (LKR)", boldFont, textBrush, (float)(left + 470), (float)(y + 4));
    y += 24;

    List<DailyRevenueSummary^>^ dailySummary = dynamic_cast<List<DailyRevenueSummary^>^>(dgvReportDaily->DataSource);
    if (dailySummary != nullptr && dailySummary->Count > 0) {
        int dCount = 0;
        for each (DailyRevenueSummary^ d in dailySummary) {
            if (dCount >= 7) break; // fit on single page
            g->DrawString(d->DateString, regularFont, textBrush, (float)(left + 8), (float)(y + 4));
            g->DrawString(d->OrderCount.ToString(), regularFont, textBrush, (float)(left + 250), (float)(y + 4));
            g->DrawString(String::Format("{0:N2}", d->TotalRevenue), regularFont, textBrush, (float)(left + 470), (float)(y + 4));
            y += 20;
            g->DrawLine(thinPen, left, y, left + width, y);
            dCount++;
        }
    }
    else {
        g->DrawString("No daily revenue transactions found.", regularFont, Brushes::Gray, (float)(left + 8), (float)(y + 4));
        y += 22;
    }

    // Page Footer
    int footerY = e->PageBounds.Height - 60;
    g->DrawLine(thinPen, left, footerY, left + width, footerY);
    g->DrawString("FreshSip Juice Bar Management System  |  ICT 1306 Object Oriented Programming  |  Rajarata University of Sri Lanka", smallFont, Brushes::Gray, (float)left, (float)(footerY + 8));
    g->DrawString("Confidential & Proprietary", smallFont, Brushes::Gray, (float)(left + width - 150), (float)(footerY + 8));

    e->HasMorePages = false;
}

[STAThreadAttribute]
int main(array<System::String ^> ^args) {
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);

    bool keepRunning = true;
    while (keepRunning) {
        FreshSipJuiceBar::Forms::LoginForm^ login = gcnew FreshSipJuiceBar::Forms::LoginForm();
        if (login->ShowDialog() != System::Windows::Forms::DialogResult::OK) {
            // User closed the login window or canceled
            break;
        }

        String^ authenticatedRole = login->AuthenticatedRole;
        String^ authenticatedUser = login->AuthenticatedUserName;
        delete login;

        try {
            MainForm^ mainForm = gcnew MainForm(authenticatedRole, authenticatedUser);
            Application::Run(mainForm);
            // If user explicitly clicked Logout, IsLoggingOut is true -> loop re-shows LoginForm!
            keepRunning = mainForm->IsLoggingOut;
        }
        catch (Exception^ ex) {
            MessageBox::Show("FreshSip encountered an unexpected error:\n\n" + ex->ToString(),
                             "FreshSip Notice", MessageBoxButtons::OK, MessageBoxIcon::Error);
            keepRunning = false;
        }
    }
    return 0;
}
