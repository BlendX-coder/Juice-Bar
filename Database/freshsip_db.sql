DROP DATABASE IF EXISTS freshsip_db;
CREATE DATABASE freshsip_db CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
USE freshsip_db;

-- ----------------------------------------------------------------------------
-- 1. Table: products
-- Stores finished juice beverages, smoothies, and fruit bowls.
-- ----------------------------------------------------------------------------
CREATE TABLE products (
    product_id INT AUTO_INCREMENT PRIMARY KEY,
    product_name VARCHAR(100) NOT NULL UNIQUE,
    category VARCHAR(50) NOT NULL,
    description VARCHAR(255) NULL,
    price DECIMAL(10, 2) NOT NULL,
    quantity INT NOT NULL DEFAULT 0,
    image_path VARCHAR(255) NULL,
    is_active TINYINT(1) NOT NULL DEFAULT 1,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    INDEX idx_product_category (category),
    INDEX idx_product_active (is_active)
) ENGINE=InnoDB;

-- ----------------------------------------------------------------------------
-- 2. Table: customers
-- Inherits conceptual person attributes and stores customer records.
-- ----------------------------------------------------------------------------
CREATE TABLE customers (
    customer_id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    phone VARCHAR(20) NOT NULL UNIQUE,
    email VARCHAR(100) NULL,
    address VARCHAR(255) NULL,
    registered_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_customer_phone (phone),
    INDEX idx_customer_name (name)
) ENGINE=InnoDB;

-- ----------------------------------------------------------------------------
-- 3. Table: staff
-- Stores staff members (Baristas, Cashiers, Managers) inheriting from Person.
-- ----------------------------------------------------------------------------
CREATE TABLE staff (
    staff_id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    phone VARCHAR(20) NOT NULL,
    email VARCHAR(100) NULL,
    role VARCHAR(50) NOT NULL DEFAULT 'Cashier',
    shift VARCHAR(30) NOT NULL DEFAULT 'Morning',
    salary DECIMAL(10, 2) NOT NULL DEFAULT 0.00,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB;

-- ----------------------------------------------------------------------------
-- 4. Table: ingredients
-- Tracks raw produce, milk, sweeteners, and consumables in inventory.
-- ----------------------------------------------------------------------------
CREATE TABLE ingredients (
    ingredient_id INT AUTO_INCREMENT PRIMARY KEY,
    ingredient_name VARCHAR(100) NOT NULL UNIQUE,
    quantity_in_stock DECIMAL(10, 2) NOT NULL DEFAULT 0.00,
    unit_of_measure VARCHAR(20) NOT NULL,
    reorder_level DECIMAL(10, 2) NOT NULL DEFAULT 5.00,
    supplier_name VARCHAR(100) NULL,
    supplier_phone VARCHAR(20) NULL,
    last_restocked TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    INDEX idx_ingredient_stock (quantity_in_stock, reorder_level)
) ENGINE=InnoDB;

-- ----------------------------------------------------------------------------
-- 5. Table: orders
-- Stores transaction headers, customer links, totals, and payment methods.
-- ----------------------------------------------------------------------------
CREATE TABLE orders (
    order_id INT AUTO_INCREMENT PRIMARY KEY,
    order_number VARCHAR(30) NOT NULL UNIQUE,
    customer_id INT NULL,
    customer_name VARCHAR(100) NOT NULL,
    order_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    subtotal DECIMAL(10, 2) NOT NULL,
    discount DECIMAL(10, 2) NOT NULL DEFAULT 0.00,
    final_total DECIMAL(10, 2) NOT NULL,
    payment_method VARCHAR(30) NOT NULL,
    status VARCHAR(20) NOT NULL DEFAULT 'Completed',
    notes VARCHAR(255) NULL,
    CONSTRAINT fk_orders_customer FOREIGN KEY (customer_id)
        REFERENCES customers(customer_id)
        ON DELETE SET NULL
        ON UPDATE CASCADE,
    INDEX idx_orders_date (order_date),
    INDEX idx_orders_number (order_number)
) ENGINE=InnoDB;

-- ----------------------------------------------------------------------------
-- 6. Table: order_items
-- Stores line-item details for each order referencing products.
-- ----------------------------------------------------------------------------
CREATE TABLE order_items (
    order_item_id INT AUTO_INCREMENT PRIMARY KEY,
    order_id INT NOT NULL,
    product_id INT NOT NULL,
    product_name VARCHAR(100) NOT NULL,
    quantity INT NOT NULL,
    unit_price DECIMAL(10, 2) NOT NULL,
    subtotal DECIMAL(10, 2) NOT NULL,
    CONSTRAINT fk_order_items_order FOREIGN KEY (order_id)
        REFERENCES orders(order_id)
        ON DELETE CASCADE
        ON UPDATE CASCADE,
    CONSTRAINT fk_order_items_product FOREIGN KEY (product_id)
        REFERENCES products(product_id)
        ON DELETE RESTRICT
        ON UPDATE CASCADE,
    INDEX idx_order_items_order (order_id),
    INDEX idx_order_items_product (product_id)
) ENGINE=InnoDB;

-- ============================================================================
-- SAMPLE SEED DATA
-- ============================================================================

-- Products
INSERT INTO products (product_name, category, description, price, quantity, image_path, is_active) VALUES
('Fresh Orange Juice', 'Fresh Juices', '100% pure cold-pressed citrus juice packed with Vitamin C', 350.00, 45, 'Assets/Products/orange_juice.png', 1),
('Golden Mango Nectar', 'Fresh Juices', 'Rich and velvety Alphonso mango nectar blended with ice', 400.00, 30, 'Assets/Products/mango_juice.png', 1),
('Chilled Watermelon', 'Fresh Juices', 'Hydrating, crisp watermelon juice with a hint of mint', 300.00, 50, 'Assets/Products/watermelon_juice.png', 1),
('Tropical Pineapple', 'Fresh Juices', 'Sweet and tangy fresh pineapple juice pressed daily', 350.00, 25, 'Assets/Products/pineapple_juice.png', 1),
('Wild Strawberry Splash', 'Fresh Juices', 'Fresh highland strawberries blended with natural honey', 450.00, 20, 'Assets/Products/strawberry_juice.png', 1),
('FreshSip Signature Blend', 'Specialty', 'Multi-fruit fusion of orange, pineapple, passion fruit, and apple', 420.00, 35, 'Assets/Products/mixed_fruit_juice.png', 1),
('Creamy Avocado Shake', 'Smoothies', 'Rich Haas avocado shake blended with fresh dairy milk and honey', 500.00, 18, 'Assets/Products/avocado_smoothie.png', 1),
('Crisp Green Apple', 'Detox Blends', 'Tart granny smith apple juice with ginger and lime', 380.00, 22, 'Assets/Products/green_apple_juice.png', 1),
('Fresh Fruit Assortment', 'Fruit Bowls', 'Generous bowl of seasonal sliced fresh tropical fruits', 600.00, 15, 'Assets/Products/fruit_assortment.png', 1),
('Classic Banana Smoothie', 'Smoothies', 'Creamy rich banana smoothie blended with fresh highland milk', 380.00, 25, 'Assets/Products/banana_smoothie.png', 1),
('Artisanal Chocolate Shake', 'Specialty', 'Decadent chocolate shake topped with whipped cream and cocoa drizzle', 480.00, 20, 'Assets/Products/chocolate_shake.png', 1),
('Vanilla Berry Dream', 'Smoothies', 'Velvety vanilla cream shake infused with mountain berries', 460.00, 18, 'Assets/Products/vanilla_berry_shake.png', 1);

-- Customers
INSERT INTO customers (name, phone, email, address) VALUES
('Kasun Perera', '0771234567', 'kasun.perera@gmail.com', 'No. 42, Main Street, Mihintale'),
('Nadeesha Fernando', '0719876543', 'nadeesha.f@yahoo.com', 'Kandy Road, Anuradhapura'),
('Ruwan Jayasinghe', '0765554321', 'ruwan.j@outlook.com', 'Stage 2, Anuradhapura'),
('Sanduni Wickramasinghe', '0783332211', 'sanduni.w@gmail.com', 'University Hostel, Mihintale'),
('Malik Al-Hassan', '0754443322', 'malik.ah@gmail.com', 'New Town, Anuradhapura');

-- Staff
INSERT INTO staff (name, phone, email, role, shift, salary) VALUES
('Chamara Bandara', '0711122334', 'chamara.b@freshsip.lk', 'Store Manager', 'Morning', 75000.00),
('Dinu Dilshan', '0723344556', 'dinu.d@freshsip.lk', 'Head Barista', 'Morning', 50000.00),
('Anuki Samarasinghe', '0778899001', 'anuki.s@freshsip.lk', 'Cashier', 'Evening', 45000.00);

-- Ingredients
INSERT INTO ingredients (ingredient_name, quantity_in_stock, unit_of_measure, reorder_level, supplier_name, supplier_phone) VALUES
('Fresh Oranges', 35.50, 'kg', 10.00, 'Lanka Agro Farms', '0774433221'),
('Alphonso Mangoes', 8.00, 'kg', 10.00, 'Dambulla Wholesale', '0715566778'),
('Watermelons', 40.00, 'kg', 15.00, 'Polonnaruwa Fruit Dist.', '0789900112'),
('Pineapples', 18.00, 'kg', 8.00, 'Giriulla Estates', '0761122334'),
('Strawberries', 4.50, 'kg', 5.00, 'Nuwara Eliya Berries', '0522233445'),
('Avocados', 6.00, 'kg', 8.00, 'Kandy Hill Organics', '0812233445'),
('Green Apples', 12.00, 'kg', 6.00, 'Colombo Cold Storage', '0112345678'),
('Pure Bee Honey', 15.00, 'L', 3.00, 'Indigenous Honey Co.', '0703344556'),
('Fresh Dairy Milk', 25.00, 'L', 10.00, 'Highland Dairies', '0252233445'),
('Crushed Ice Blocks', 50.00, 'kg', 20.00, 'Polar Ice Suppliers', '0253344556');

-- Orders & Order Items (Initial transactions for Dashboard & Reports)
INSERT INTO orders (order_number, customer_id, customer_name, order_date, subtotal, discount, final_total, payment_method, status, notes) VALUES
('ORD-20261015-001', 1, 'Kasun Perera', NOW() - INTERVAL 4 HOUR, 700.00, 0.00, 700.00, 'Cash', 'Completed', 'Takeaway'),
('ORD-20261015-002', 2, 'Nadeesha Fernando', NOW() - INTERVAL 3 HOUR, 850.00, 50.00, 800.00, 'Card', 'Completed', 'Dine-in'),
('ORD-20261015-003', NULL, 'Walk-in Customer', NOW() - INTERVAL 1 HOUR, 500.00, 0.00, 500.00, 'Cash', 'Completed', 'Counter Sale');

INSERT INTO order_items (order_id, product_id, product_name, quantity, unit_price, subtotal) VALUES
(1, 1, 'Fresh Orange Juice', 2, 350.00, 700.00),
(2, 2, 'Golden Mango Nectar', 1, 400.00, 400.00),
(2, 5, 'Wild Strawberry Splash', 1, 450.00, 450.00),
(3, 7, 'Creamy Avocado Shake', 1, 500.00, 500.00);

-- End of script
