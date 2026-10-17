#pragma once

using namespace System;

namespace FreshSipJuiceBar {
    namespace Models {

        /// <summary>
        /// Product Model demonstrating:
        /// 1. Class & Object (OOP Concept A)
        /// 2. Encapsulation (OOP Concept B) - Private data members with guarded getters and setters.
        /// 3. Method Overloading (OOP Concept F) - Multiple CalculateSubtotal signatures for versatile pricing.
        /// </summary>
        public ref class Product {
        private:
            int m_productId;
            String^ m_productName;
            String^ m_category;
            String^ m_description;
            Decimal m_price;
            int m_quantity;
            String^ m_imagePath;
            bool m_isActive;

        public:
            Product() {
                m_productId = 0;
                m_productName = "";
                m_category = "Fresh Juices";
                m_description = "";
                m_price = Decimal(0);
                m_quantity = 0;
                m_imagePath = "";
                m_isActive = true;
            }

            Product(int productId, String^ productName, String^ category, String^ description,
                Decimal price, int quantity, String^ imagePath, bool isActive) {
                this->ProductId = productId;
                this->ProductName = productName;
                this->Category = category;
                this->Description = description;
                this->Price = price;
                this->Quantity = quantity;
                this->ImagePath = imagePath;
                this->IsActive = isActive;
            }

            // --- Encapsulation: Guarded Properties ---

            property int ProductId {
                int get() { return m_productId; }
                void set(int value) { m_productId = value; }
            }

            property String^ ProductName {
                String^ get() { return m_productName; }
                void set(String^ value) {
                    if (String::IsNullOrWhiteSpace(value))
                        throw gcnew ArgumentException("Product name cannot be empty.");
                    m_productName = value->Trim();
                }
            }

            property String^ Category {
                String^ get() { return m_category; }
                void set(String^ value) {
                    m_category = (String::IsNullOrWhiteSpace(value)) ? "Fresh Juices" : value->Trim();
                }
            }

            property String^ Description {
                String^ get() { return m_description; }
                void set(String^ value) {
                    m_description = (value == nullptr) ? "" : value->Trim();
                }
            }

            property Decimal Price {
                Decimal get() { return m_price; }
                void set(Decimal value) {
                    if (value < Decimal(0))
                        throw gcnew ArgumentException("Selling price cannot be negative.");
                    m_price = value;
                }
            }

            property int Quantity {
                int get() { return m_quantity; }
                void set(int value) {
                    if (value < 0)
                        throw gcnew ArgumentException("Stock quantity cannot be negative.");
                    m_quantity = value;
                }
            }

            property String^ ImagePath {
                String^ get() { return m_imagePath; }
                void set(String^ value) {
                    m_imagePath = (value == nullptr) ? "" : value->Trim();
                }
            }

            property bool IsActive {
                bool get() { return m_isActive; }
                void set(bool value) { m_isActive = value; }
            }

            // --- Method Overloading (OOP Concept F) ---

            /// <summary>
            /// Overload 1: Calculates subtotal for a default quantity of 1 unit.
            /// </summary>
            Decimal CalculateSubtotal() {
                return this->m_price * Decimal(1);
            }

            /// <summary>
            /// Overload 2: Calculates subtotal for an arbitrary integer quantity.
            /// </summary>
            Decimal CalculateSubtotal(int quantity) {
                if (quantity < 0)
                    throw gcnew ArgumentException("Quantity cannot be negative.");
                return this->m_price * Decimal(quantity);
            }

            /// <summary>
            /// Overload 3: Calculates subtotal with quantity and a percentage discount (0 - 100).
            /// </summary>
            Decimal CalculateSubtotal(int quantity, Decimal discountPercentage) {
                if (quantity < 0)
                    throw gcnew ArgumentException("Quantity cannot be negative.");
                if (discountPercentage < Decimal(0) || discountPercentage > Decimal(100))
                    throw gcnew ArgumentException("Discount percentage must be between 0 and 100.");

                Decimal gross = this->m_price * Decimal(quantity);
                Decimal discountMultiplier = (Decimal(100) - discountPercentage) / Decimal(100);
                return Decimal::Round(gross * discountMultiplier, 2);
            }

            virtual String^ ToString() override {
                return String::Format("{0} - LKR {1:N2} (Stock: {2})", m_productName, m_price, m_quantity);
            }
        };

    }
}
