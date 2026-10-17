# FreshSip Juice Bar Management System
### Modern Desktop POS & Inventory Management System in C++/CLI Windows Forms
**Course:** ICT 1306 — Object Oriented Programming  
**Institution:** Rajarata University of Sri Lanka  
**Target Environment:** Visual Studio 2022 (or VS 2026) / C++/CLI / .NET Framework 4.8 / MySQL Server 8.0 or MariaDB 10.4+  

---

## 1. Project Overview

**FreshSip Juice Bar Management System** is a complete, desktop application built for an artisanal juice bar and beverage establishment in Mihintale, Sri Lanka. Built strictly in **C++/CLI with Windows Forms**, the application provides small business operators with a full-featured Point-of-Sale (POS) cashier register, product catalog with high-resolution fruit imagery, customer directory, inventory monitoring with low-stock alerts, sales analytics with custom GDI+ charts, and print-ready transaction receipts.

### Key Capabilities
* **Screen A — Dashboard:** Real-time KPI summary (Today's Sales in LKR, Orders, Available Juices, Low Stock alerts), branded fruit banner, top-selling blends, and recent order transactions.
* **Screen B — Product Management (Full CRUD):** Complete database create, read, update, delete, search, and category filtering with local photograph preview and input validation.
* **Screen C — Orders & POS Billing:** Visual catalog selection, shopping cart with quantity adjustment, customer selector, subtotal and discount calculations, Cash and Card payment simulation, and print-friendly receipt modal dialog.
* **Screen D — Customer Management (Full CRUD):** Register, view, update, delete, and search customers with phone number uniqueness constraints.
* **Screen E — Ingredient Inventory:** Track raw supplies (fruits, milk, honey, ice) with unit measurements, supplier details, and soft-red highlighting for items below reorder thresholds.
* **Screen F — Sales Reports & Charts:** Date-range financial summaries, best-selling product rankings, and custom-drawn GDI+ bar charts of daily revenue.
* **Screen G — Database Settings:** Real-time connection string editor and connection testing with live status indicator.

---

## 2. Implementation of Mandatory OOP Concepts

The application implements all six mandatory Object-Oriented Programming concepts in functional business code:

1. **Class and Object (Concept A):** Core entities (`Product`, `Customer`, `Order`, `OrderItem`, `Ingredient`, `Staff`) instantiated dynamically on the CLR managed heap via `gcnew`.
2. **Encapsulation (Concept B):** In `Models/Product.h`, private attributes (`m_price`, `m_quantity`) are protected with public property accessors that validate data and throw `ArgumentException` on negative values.
3. **Abstraction (Concept C):** In `Models/PaymentMethod.h`, the abstract ref class `PaymentMethod` declares pure virtual methods (`ProcessPayment`, `GetMethodName`), establishing a universal billing interface without exposing concrete payment mechanics.
4. **Inheritance (Concept D):** The abstract base class `Person` (`Models/Person.h`) provides common identity attributes (`Name`, `Phone`, `Email`), which are inherited by `Customer` and `Staff`.
5. **Polymorphism (Concept E):** In `Services/OrderService.cpp`, the checkout engine interacts with a base `PaymentMethod^` handle that dynamically dispatches to either `CashPayment` (change calculation) or `CardPayment` (simulated authorization code generation) via runtime virtual method late binding.
6. **Method Overloading (Concept F):** In `Models/Product.h` and `Models/OrderItem.h`, multiple `CalculateSubtotal` and `CalculateTotal` methods share the same identifier with varying parameter signatures:
   - `CalculateTotal()` (single unit)
   - `CalculateTotal(int quantity)` (multiple units)
   - `CalculateTotal(int quantity, Decimal discount)` (discounted pricing)

---

## 3. Directory Layout

```
Juice_Bar/
└── FreshSipJuiceBar/
    ├── FreshSipJuiceBar.sln                   # Visual Studio Solution
    ├── README.md                              # Setup and execution guide
    ├── FreshSipJuiceBar/
    │   ├── FreshSipJuiceBar.vcxproj          # C++/CLI Project Configuration
    │   ├── FreshSipJuiceBar.vcxproj.filters  # VS Solution Explorer Filters
    │   ├── App.config                         # MySQL connection configuration
    │   ├── MainForm.h                         # Master GUI form declarations
    │   ├── MainForm.cpp                       # GUI event handlers & application entry point
    │   ├── Models/                            # Domain entities & OOP implementations
    │   │   ├── Person.h                       # Base class (Inheritance)
    │   │   ├── Customer.h                     # Derived class
    │   │   ├── Staff.h                        # Derived class
    │   │   ├── Product.h                      # Encapsulation & Overloading
    │   │   ├── Ingredient.h                   # Inventory entity
    │   │   ├── OrderItem.h                    # Line item model
    │   │   ├── Order.h                        # Order aggregate entity
    │   │   └── PaymentMethod.h                # Abstraction & Polymorphism
    │   ├── Data/
    │   │   ├── DatabaseManager.h              # Database coordination header
    │   │   └── DatabaseManager.cpp            # Parameterized queries & connection handling
    │   ├── Services/
    │   │   ├── ProductService.h / .cpp        # Product business logic & CRUD
    │   │   ├── CustomerService.h / .cpp       # Customer business logic & CRUD
    │   │   ├── InventoryService.h / .cpp      # Inventory & low stock logic
    │   │   ├── OrderService.h / .cpp          # Order processing & ACID transactions
    │   │   └── ReportService.h / .cpp         # Sales reporting & aggregations
    │   ├── Forms/
    │   │   └── ReceiptForm.h                  # Print-friendly receipt modal dialog
    │   ├── Assets/
    │   │   ├── Products/                      # Local juice photographs & graphic assets
    │   │   └── Banners/                       # Banner graphics
    │   └── lib/
    │       └── MySql.Data.dll                 # Bundled ADO.NET MySQL connector
    ├── Database/
    │   └── freshsip_db.sql                    # Database schema and seed data
    └── Documentation/
        ├── UML_Class_Diagram.md               # Mermaid UML and OOP mapping
        ├── Test_Cases.md                      # Test plan and execution results
        ├── Assignment_Report_Outline.md       # Report template fitting 8 pages
        └── Demo_Video_Script.md               # 5-minute video presentation script
```

---

## 4. Setup & Execution Instructions

### Prerequisites
1. **Visual Studio 2022 (or VS 2026):**
   - Ensure the **Desktop development with C++** workload is installed.
   - Verify that **C++/CLI support for v143 build tools (14.3x)** is checked in the Visual Studio Installer (Individual Components tab).
   - Ensure **.NET Framework 4.8 targeting pack** is installed.
2. **Database Engine:**
   - MySQL Server 8.0 or MariaDB 10.4+.
   - MySQL Workbench, phpMyAdmin, or MySQL CLI client.

---

### Step 1: Database Setup
1. Start your local MySQL / MariaDB service (default port: `3306` or `3307`).
2. Open MySQL Workbench or your command prompt:
   ```powershell
   mysql -u root -p < "FreshSipJuiceBar\Database\freshsip_db.sql"
   ```
3. Verify that the database `freshsip_db` and its tables (`products`, `customers`, `staff`, `ingredients`, `orders`, `order_items`) have been populated with sample seed data.

---

### Step 2: Configure Database Connection
Open `FreshSipJuiceBar\FreshSipJuiceBar\App.config` and check the connection string:
```xml
<connectionStrings>
  <add name="FreshSipConnection"
       connectionString="Server=localhost;Port=3306;Database=freshsip_db;Uid=root;Pwd=YOUR_PASSWORD;SslMode=none;AllowPublicKeyRetrieval=True;CharSet=utf8mb4;"
       providerName="MySql.Data.MySqlClient" />
</connectionStrings>
```
* If your local MySQL Server has a password, enter it in `Pwd=YOUR_PASSWORD;`.
* If your server runs on standard port `3306`, set `Port=3306;`.
*(Note: You can also update and test the connection string directly inside the application GUI via the **Database Setup** screen).*

---

### Step 3: Open and Build the Project
1. Launch **Visual Studio 2022**.
2. Open `FreshSipJuiceBar\FreshSipJuiceBar.sln`.
3. Set the Solution Configuration to **Debug** (or **Release**) and the Solution Platform to **x64** (or **Win32**).
4. Select **Build > Build Solution** (`Ctrl+Shift+B`).
5. The build automatically copies `MySql.Data.dll`, `App.config`, and all local images in `Assets\` into the output `bin\x64\Debug\` directory.

---

### Step 4: Run the Application
1. Press `F5` or click **Debug > Start Debugging** in Visual Studio.
2. The application opens to the **Dashboard Overview**.
3. Check the database indicator in the top right header:
   - **Green "DB Connected":** Successfully connected to MySQL.
   - **Red "DB Disconnected":** Click the indicator to open the Database Configuration screen, adjust your password or port, and click **Test Connection** followed by **Save & Reconnect**.

---

## 5. Local Image Asset Acquisition Guide

The project includes pre-rendered, high-resolution fruit illustrations in `Assets/Products/` and `Assets/Banners/`. If you wish to substitute real commercial photographs for your assignment demonstration:

1. **Recommended Free & Open License Sources:**
   - [Unsplash Fruit & Juice Collection](https://unsplash.com/s/photos/fruit-juice) (Unsplash License — free for commercial and academic use)
   - [Pexels Juices Collection](https://www.pexels.com/search/juice/) (Pexels License — free)
   - [Pixabay Drinks Collection](https://pixabay.com/images/search/fruit%20juice/) (Pixabay Content License)
2. **File Naming & Sizing Specifications:**
   - Recommended resolution: 400x300 pixels (PNG or JPG format).
   - Place image files inside `FreshSipJuiceBar\FreshSipJuiceBar\Assets\Products\`:
     * `orange_juice.png`
     * `mango_juice.png`
     * `watermelon_juice.png`
     * `pineapple_juice.png`
     * `strawberry_juice.png`
     * `mixed_fruit_juice.png`
     * `avocado_smoothie.png`
     * `green_apple_juice.png`
     * `fruit_assortment.png`
     * `banana_smoothie.png`
     * `chocolate_shake.png`
     * `vanilla_berry_shake.png`
   - Banner: 960x160 pixels in `Assets/Banners/juice_bar_banner.png`.
3. The application uses `Image::FromStream()` to avoid file locking, enabling dynamic image hot-swapping at runtime.

---

## 6. Troubleshooting Common Issues

| Issue / Error Message | Root Cause | Solution |
| :--- | :--- | :--- |
| `Cannot find MySql.Data.dll` | Reference assembly not located | The DLL is pre-bundled in `FreshSipJuiceBar\FreshSipJuiceBar\lib\`. The `.vcxproj` automatically resolves and copies it during build. |
| `Access denied for user 'root'@'localhost'` | MySQL password mismatch | Open the GUI **Database Setup** panel or `App.config`, enter the correct MySQL root password in `Pwd=...;`, and click Save. |
| `Unable to connect to any of the specified MySQL hosts` | MySQL service stopped or port mismatch | Start MySQL Server in Windows Services (`services.msc`) and verify whether your instance runs on port `3306` or `3307`. |
| `C3861: 'identifier not found'` | Out of order header includes | Headers are structured hierarchically: Models -> Data -> Services -> Forms. Compile the solution using the provided `.sln`. |

---

## 7. Academic Integrity & Course Declaration

This software artifact was developed for the **ICT 1306 — Object Oriented Programming** module at Rajarata University of Sri Lanka. All classes, design patterns, and database interactions adhere to academic programming standards.
