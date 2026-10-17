#pragma once

using namespace System;

namespace FreshSipJuiceBar {
    namespace Models {

        /// <summary>
        /// Raw Ingredient entity for juice bar inventory and reorder monitoring.
        /// </summary>
        public ref class Ingredient {
        private:
            int m_ingredientId;
            String^ m_ingredientName;
            Decimal m_quantityInStock;
            String^ m_unitOfMeasure;
            Decimal m_reorderLevel;
            String^ m_supplierName;
            String^ m_supplierPhone;
            DateTime m_lastRestocked;

        public:
            Ingredient() {
                m_ingredientId = 0;
                m_ingredientName = "";
                m_quantityInStock = Decimal(0);
                m_unitOfMeasure = "kg";
                m_reorderLevel = Decimal(5);
                m_supplierName = "";
                m_supplierPhone = "";
                m_lastRestocked = DateTime::Now;
            }

            Ingredient(int id, String^ name, Decimal stock, String^ unit, Decimal reorderLevel,
                String^ supplier, String^ supplierPhone, DateTime lastRestocked) {
                this->IngredientId = id;
                this->IngredientName = name;
                this->QuantityInStock = stock;
                this->UnitOfMeasure = unit;
                this->ReorderLevel = reorderLevel;
                this->SupplierName = supplier;
                this->SupplierPhone = supplierPhone;
                this->LastRestocked = lastRestocked;
            }

            property int IngredientId {
                int get() { return m_ingredientId; }
                void set(int value) { m_ingredientId = value; }
            }

            property String^ IngredientName {
                String^ get() { return m_ingredientName; }
                void set(String^ value) {
                    if (String::IsNullOrWhiteSpace(value))
                        throw gcnew ArgumentException("Ingredient name cannot be empty.");
                    m_ingredientName = value->Trim();
                }
            }

            property Decimal QuantityInStock {
                Decimal get() { return m_quantityInStock; }
                void set(Decimal value) {
                    if (value < Decimal(0))
                        throw gcnew ArgumentException("Quantity in stock cannot be negative.");
                    m_quantityInStock = value;
                }
            }

            property String^ UnitOfMeasure {
                String^ get() { return m_unitOfMeasure; }
                void set(String^ value) {
                    m_unitOfMeasure = (String::IsNullOrWhiteSpace(value)) ? "kg" : value->Trim();
                }
            }

            property Decimal ReorderLevel {
                Decimal get() { return m_reorderLevel; }
                void set(Decimal value) {
                    if (value < Decimal(0))
                        throw gcnew ArgumentException("Reorder level cannot be negative.");
                    m_reorderLevel = value;
                }
            }

            property String^ SupplierName {
                String^ get() { return m_supplierName; }
                void set(String^ value) { m_supplierName = (value == nullptr) ? "" : value->Trim(); }
            }

            property String^ SupplierPhone {
                String^ get() { return m_supplierPhone; }
                void set(String^ value) { m_supplierPhone = (value == nullptr) ? "" : value->Trim(); }
            }

            property DateTime LastRestocked {
                DateTime get() { return m_lastRestocked; }
                void set(DateTime value) { m_lastRestocked = value; }
            }

            /// <summary>
            /// Indicates if current stock has dropped below or equal to the designated reorder threshold.
            /// </summary>
            property bool IsLowStock {
                bool get() { return m_quantityInStock <= m_reorderLevel; }
            }

            virtual String^ ToString() override {
                return String::Format("{0}: {1:N2} {2} (Reorder at: {3:N2})",
                    m_ingredientName, m_quantityInStock, m_unitOfMeasure, m_reorderLevel);
            }
        };

    }
}
