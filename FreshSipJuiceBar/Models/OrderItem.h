#pragma once

using namespace System;

namespace FreshSipJuiceBar {
    namespace Models {

        /// <summary>
        /// OrderItem represents an individual line item within a customer's order.
        /// Also demonstrates Method Overloading (OOP Concept F).
        /// </summary>
        public ref class OrderItem {
        private:
            int m_orderItemId;
            int m_orderId;
            int m_productId;
            String^ m_productName;
            int m_quantity;
            Decimal m_unitPrice;
            Decimal m_subtotal;

        public:
            OrderItem() {
                m_orderItemId = 0;
                m_orderId = 0;
                m_productId = 0;
                m_productName = "";
                m_quantity = 1;
                m_unitPrice = Decimal(0);
                m_subtotal = Decimal(0);
            }

            OrderItem(int productId, String^ productName, int quantity, Decimal unitPrice) {
                this->OrderItemId = 0;
                this->OrderId = 0;
                this->ProductId = productId;
                this->ProductName = productName;
                this->UnitPrice = unitPrice;
                this->Quantity = quantity;
                this->m_subtotal = this->CalculateTotal();
            }

            OrderItem(int orderItemId, int orderId, int productId, String^ productName, int quantity, Decimal unitPrice, Decimal subtotal) {
                this->OrderItemId = orderItemId;
                this->OrderId = orderId;
                this->ProductId = productId;
                this->ProductName = productName;
                this->UnitPrice = unitPrice;
                this->Quantity = quantity;
                this->m_subtotal = subtotal;
            }

            property int OrderItemId {
                int get() { return m_orderItemId; }
                void set(int value) { m_orderItemId = value; }
            }

            property int OrderId {
                int get() { return m_orderId; }
                void set(int value) { m_orderId = value; }
            }

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

            property int Quantity {
                int get() { return m_quantity; }
                void set(int value) {
                    if (value <= 0)
                        throw gcnew ArgumentException("Quantity must be greater than zero.");
                    m_quantity = value;
                    m_subtotal = this->CalculateTotal();
                }
            }

            property Decimal UnitPrice {
                Decimal get() { return m_unitPrice; }
                void set(Decimal value) {
                    if (value < Decimal(0))
                        throw gcnew ArgumentException("Unit price cannot be negative.");
                    m_unitPrice = value;
                    m_subtotal = this->CalculateTotal();
                }
            }

            property Decimal Subtotal {
                Decimal get() { return m_subtotal; }
                void set(Decimal value) { m_subtotal = value; }
            }

            // --- Method Overloading (OOP Concept F) ---

            /// <summary>
            /// Overload 1: Computes subtotal using internal quantity and unit price.
            /// </summary>
            Decimal CalculateTotal() {
                return this->m_unitPrice * Decimal(this->m_quantity);
            }

            /// <summary>
            /// Overload 2: Computes subtotal for an override quantity.
            /// </summary>
            Decimal CalculateTotal(int customQuantity) {
                if (customQuantity < 0)
                    throw gcnew ArgumentException("Custom quantity cannot be negative.");
                return this->m_unitPrice * Decimal(customQuantity);
            }

            /// <summary>
            /// Overload 3: Computes subtotal with an override quantity and an absolute line discount.
            /// </summary>
            Decimal CalculateTotal(int customQuantity, Decimal lineDiscount) {
                if (customQuantity < 0)
                    throw gcnew ArgumentException("Custom quantity cannot be negative.");
                if (lineDiscount < Decimal(0))
                    throw gcnew ArgumentException("Discount cannot be negative.");

                Decimal gross = this->m_unitPrice * Decimal(customQuantity);
                Decimal net = gross - lineDiscount;
                return (net < Decimal(0)) ? Decimal(0) : net;
            }
        };

    }
}
