#pragma once

#include "OrderItem.h"

using namespace System;
using namespace System::Collections::Generic;

namespace FreshSipJuiceBar {
    namespace Models {

        /// <summary>
        /// Order Aggregate entity containing high-level sales data and line items.
        /// Demonstrates Object Composition and business total calculations.
        /// </summary>
        public ref class Order {
        private:
            int m_orderId;
            String^ m_orderNumber;
            Nullable<int> m_customerId;
            String^ m_customerName;
            DateTime m_orderDate;
            Decimal m_subtotal;
            Decimal m_discount;
            Decimal m_finalTotal;
            String^ m_paymentMethod;
            String^ m_status;
            String^ m_notes;
            List<OrderItem^>^ m_items;

        public:
            Order() {
                m_orderId = 0;
                m_orderNumber = GenerateOrderNumber();
                m_customerId = Nullable<int>();
                m_customerName = "Walk-in Customer";
                m_orderDate = DateTime::Now;
                m_subtotal = Decimal(0);
                m_discount = Decimal(0);
                m_finalTotal = Decimal(0);
                m_paymentMethod = "Cash";
                m_status = "Completed";
                m_notes = "";
                m_items = gcnew List<OrderItem^>();
            }

            Order(int orderId, String^ orderNumber, Nullable<int> customerId, String^ customerName,
                DateTime orderDate, Decimal subtotal, Decimal discount, Decimal finalTotal,
                String^ paymentMethod, String^ status, String^ notes) {
                m_orderId = orderId;
                m_orderNumber = orderNumber;
                m_customerId = customerId;
                m_customerName = customerName;
                m_orderDate = orderDate;
                m_subtotal = subtotal;
                m_discount = discount;
                m_finalTotal = finalTotal;
                m_paymentMethod = paymentMethod;
                m_status = status;
                m_notes = notes;
                m_items = gcnew List<OrderItem^>();
            }

            property int OrderId {
                int get() { return m_orderId; }
                void set(int value) { m_orderId = value; }
            }

            property String^ OrderNumber {
                String^ get() { return m_orderNumber; }
                void set(String^ value) { m_orderNumber = value; }
            }

            property Nullable<int> CustomerId {
                Nullable<int> get() { return m_customerId; }
                void set(Nullable<int> value) { m_customerId = value; }
            }

            property String^ CustomerName {
                String^ get() { return m_customerName; }
                void set(String^ value) {
                    m_customerName = (String::IsNullOrWhiteSpace(value)) ? "Walk-in Customer" : value->Trim();
                }
            }

            property DateTime OrderDate {
                DateTime get() { return m_orderDate; }
                void set(DateTime value) { m_orderDate = value; }
            }

            property Decimal Subtotal {
                Decimal get() { return m_subtotal; }
                void set(Decimal value) { m_subtotal = value; }
            }

            property Decimal Discount {
                Decimal get() { return m_discount; }
                void set(Decimal value) {
                    if (value < Decimal(0))
                        throw gcnew ArgumentException("Discount cannot be negative.");
                    m_discount = value;
                }
            }

            property Decimal FinalTotal {
                Decimal get() { return m_finalTotal; }
                void set(Decimal value) { m_finalTotal = value; }
            }

            property String^ PaymentMethodName {
                String^ get() { return m_paymentMethod; }
                void set(String^ value) { m_paymentMethod = value; }
            }

            property String^ Status {
                String^ get() { return m_status; }
                void set(String^ value) { m_status = value; }
            }

            property String^ Notes {
                String^ get() { return m_notes; }
                void set(String^ value) { m_notes = (value == nullptr) ? "" : value->Trim(); }
            }

            property List<OrderItem^>^ Items {
                List<OrderItem^>^ get() { return m_items; }
                void set(List<OrderItem^>^ value) { m_items = value; }
            }

            /// <summary>
            /// Re-evaluates subtotal and final total from items list.
            /// </summary>
            void RecalculateTotals() {
                Decimal sum = Decimal(0);
                if (m_items != nullptr) {
                    for each (OrderItem^ item in m_items) {
                        sum += item->Subtotal;
                    }
                }
                m_subtotal = sum;
                Decimal net = m_subtotal - m_discount;
                m_finalTotal = (net < Decimal(0)) ? Decimal(0) : net;
            }

            static String^ GenerateOrderNumber() {
                return String::Format("ORD-{0:yyyyMMdd}-{1:D4}", DateTime::Now, (gcnew Random())->Next(1, 9999));
            }
        };

    }
}
