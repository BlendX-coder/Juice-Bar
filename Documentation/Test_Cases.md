# FreshSip Juice Bar Management System — Test Plan & Execution Report

**Course:** ICT 1306 — Object Oriented Programming  
**Institution:** Rajarata University of Sri Lanka  
**Test Environment:** Windows 11 / Visual Studio 2022 (MSVC v180/v170 C++/CLI) / MariaDB 13.0 (MySQL 8.0 Protocol) / .NET Framework 4.8  

---

## 1. Test Matrix & Results

| Test ID | Test Category & Description | Test Input Data | Expected Result | Actual Result / Verification | Status |
| :--- | :--- | :--- | :--- | :--- | :---: |
| **TC-01** | Database Connection (Success) | Valid connection string: `Server=localhost;Port=3307;Database=freshsip_db;Uid=root;Pwd=;` | `DatabaseManager::TestConnection()` returns `true`, dashboard status pill turns Green ("DB Connected"). | Verified: Connected to MariaDB instance on port 3307; returned `true`. Indicator displayed green. | **PASS** |
| **TC-02** | Database Connection (Failure Handling) | Invalid connection string: `Server=localhost;Port=9999;Database=fake_db;` | `DatabaseManager::TestConnection()` returns `false`, returns descriptive error message, status pill turns Red. | Verified: Correctly caught socket connection exception; returned `false` without crashing application. | **PASS** |
| **TC-03** | Product Creation (Valid Input) | Name: "Kiwi Zing", Category: "Fresh Juices", Price: `480.00`, Qty: `25`, Image: `Assets/Products/orange_juice.png` | Product successfully inserted into `products` table, auto-assigned `product_id`, DataGridView refreshed. | Verified: Insert query executed; returned rows affected = 1; record viewable in database. | **PASS** |
| **TC-04** | Product Duplicate Name Constraint | Duplicate name: "Fresh Orange Juice", Price: `350.00`, Qty: `10` | Database rejects insert due to UNIQUE constraint (`1062`), application displays friendly error dialog. | Verified: MySqlException 1062 caught; displayed "A product with this name already exists in the system." | **PASS** |
| **TC-05** | Product Retrieval & Category Filter | Keyword: "Mango", Category: "Fresh Juices" | DataGridView filters to display only "Golden Mango Nectar" matching criteria. | Verified: Search query returned 1 record matching `%Mango%` and `category = 'Fresh Juices'`. | **PASS** |
| **TC-06** | Product Update (Valid Input) | Product ID: `1`, Name: "Fresh Orange Juice", Price: `375.00` (updated from `350.00`) | Record updated in `products` table; DataGridView reflects new price. | Verified: Update query succeeded; database price confirmed at `375.00`. | **PASS** |
| **TC-07** | Product Deletion & Referential Integrity | Attempt to delete Product ID: `1` ("Fresh Orange Juice", which has existing sales in `order_items`) | System detects existing sales history, prevents destructive cascading deletion, and marks `is_active = 0` (safe archival). | Verified: Referential check counted >0 order items; executed safe deactivation and informed user. | **PASS** |
| **TC-08** | Customer Full CRUD | Name: "Nimal Siripala", Phone: `0779988776`, Email: `nimal.s@gmail.com`, Address: `Mihintale` | Record added, searchable by phone, updated to address `Anuradhapura`, and deleted cleanly. | Verified: Insert, search by `%0779988776%`, update, and delete executed with 100% data consistency. | **PASS** |
| **TC-09** | Encapsulation Guard (Negative Price/Qty) | `Product->Price = -50.00` or `Product->Quantity = -10` | Guarded property setter rejects negative value and throws `ArgumentException`. UI warns user. | Verified: Threw `ArgumentException("Selling price cannot be negative.")`. DB unaffected. | **PASS** |
| **TC-10** | Cart Calculations (Method Overloading) | Base Price: `350.00`. Test overloads: `CalculateSubtotal()`, `CalculateSubtotal(3)`, `CalculateSubtotal(3, 10.00)` | Overload 1: `350.00`; Overload 2: `1050.00`; Overload 3 (10% off): `945.00`. | Verified: All 3 method signatures evaluated accurately to the exact cent. | **PASS** |
| **TC-11** | Cash Payment Simulation | Order Total: `700.00`, Cash Tendered: `1000.00` | `CashPayment->ProcessPayment(...)` returns `true`, computes Change = `300.00`, prints tender details on receipt. | Verified: Tendered >= Total satisfied; Change returned `300.00` in green text on UI and receipt. | **PASS** |
| **TC-12** | Cash Payment Insufficient Funds | Order Total: `700.00`, Cash Tendered: `500.00` | `CashPayment->ProcessPayment(...)` returns `false`, reports shortage of `200.00`, halts checkout. | Verified: Returned `false` with message "Insufficient cash tendered. Shortage: LKR 200.00". | **PASS** |
| **TC-13** | Card Payment Simulation | Order Total: `850.00`, Card Network: "Visa", Last 4 Digits: `4321` | `CardPayment->ProcessPayment(...)` returns `true`, generates random `AUTH-XXXXXX` approval token. | Verified: Simulated terminal handshake succeeded; receipt stamped with `AUTH-XXXXXX` authorization code. | **PASS** |
| **TC-14** | Runtime Polymorphism Dispatch | Checkout handler invokes `paymentMethod->ProcessPayment(...)` through base handle `PaymentMethod^` | Late-binding virtual dispatch executes `CashPayment` or `CardPayment` appropriately based on radio selection. | Verified: Base class pointer correctly routed calls to the active payment derived instance. | **PASS** |
| **TC-15** | Transactional Order Persistence | 2 items added to cart (Orange Juice x 2, Mango Nectar x 1). Final Total: `1100.00`. | Order saved in `orders`, items saved in `order_items`, product quantities in `products` decremented atomically. | Verified: `orders` and `order_items` persisted; Orange Juice stock decremented from 45 to 43. | **PASS** |
| **TC-16** | Transaction Rollback on Stock Depletion | Customer requests 100 units of "Strawberry Splash" when only 20 are in stock. | Pre-validation detects insufficient stock, aborts checkout prior to committing database transaction. | Verified: Error caught: "Insufficient stock for 'Wild Strawberry Splash'. Available: 20, Requested: 100." Database rolled back. | **PASS** |
| **TC-17** | Low-Stock Inventory Warning | Ingredient "Strawberries" quantity = `4.50 kg`, reorder level = `5.00 kg`. | Row in `dgvIngredients` highlighted in soft red (`RGB 255, 235, 235`), counted on Dashboard KPI badge. | Verified: `chkOnlyLowStock` filtered low stock items; cell formatting event painted alert background. | **PASS** |
| **TC-18** | Missing Asset Fallback | Product image path set to non-existent file: `Assets/Products/fake.png` | `SafeLoadImage()` catches file missing exception, returns `nullptr`, PictureBox displays blank without crash. | Verified: Application did not throw unhandled exception; smoothly showed clean empty picture box. | **PASS** |
| **TC-19** | Empty Database Behavior | Database contains 0 orders. | Dashboard renders `LKR 0.00`, 0 orders, DataGridViews show empty states without throwing null reference exceptions. | Verified: Queries with `COALESCE` and `COUNT` returned clean default zeroes. | **PASS** |
| **TC-20** | Sales Reports & Visual Chart | Selected date range: Today - 7 days to Today. | ReportService aggregates total revenue, order count, top selling items, and GDI+ renders bar chart. | Verified: Correctly summed revenue; custom GDI+ graphics painted bars with values and dates. | **PASS** |

---

## 2. Testing Summary

- **Total Test Cases Planned:** 20
- **Total Test Cases Executed:** 20
- **Tests Passed:** 20
- **Tests Failed:** 0
- **Pass Rate:** 100%
- **Critical Defects Observed:** None. All referential integrity constraints, input guards, and database rollbacks functioned as designed.
