#include "InventoryService.h"

namespace FreshSipJuiceBar {
    namespace Services {

        Ingredient^ InventoryService::DataRowToIngredient(DataRow^ row) {
            Ingredient^ ing = gcnew Ingredient();
            ing->IngredientId = Convert::ToInt32(row["ingredient_id"]);
            ing->IngredientName = row["ingredient_name"]->ToString();
            ing->QuantityInStock = Convert::ToDecimal(row["quantity_in_stock"]);
            ing->UnitOfMeasure = row["unit_of_measure"]->ToString();
            ing->ReorderLevel = Convert::ToDecimal(row["reorder_level"]);
            ing->SupplierName = (row["supplier_name"] == DBNull::Value) ? "" : row["supplier_name"]->ToString();
            ing->SupplierPhone = (row["supplier_phone"] == DBNull::Value) ? "" : row["supplier_phone"]->ToString();
            ing->LastRestocked = Convert::ToDateTime(row["last_restocked"]);
            return ing;
        }

        List<Ingredient^>^ InventoryService::GetAllIngredients() {
            List<Ingredient^>^ list = gcnew List<Ingredient^>();
            try {
                String^ sql = "SELECT ingredient_id, ingredient_name, quantity_in_stock, unit_of_measure, reorder_level, supplier_name, supplier_phone, last_restocked FROM ingredients ORDER BY ingredient_name ASC";
                DataTable^ dt = DatabaseManager::ExecuteQuery(sql, nullptr);
                for each (DataRow^ row in dt->Rows) {
                    list->Add(DataRowToIngredient(row));
                }
            }
            catch (Exception^) {
            }
            return list;
        }

        List<Ingredient^>^ InventoryService::GetLowStockIngredients() {
            List<Ingredient^>^ list = gcnew List<Ingredient^>();
            try {
                String^ sql = "SELECT ingredient_id, ingredient_name, quantity_in_stock, unit_of_measure, reorder_level, supplier_name, supplier_phone, last_restocked FROM ingredients WHERE quantity_in_stock <= reorder_level ORDER BY quantity_in_stock ASC";
                DataTable^ dt = DatabaseManager::ExecuteQuery(sql, nullptr);
                for each (DataRow^ row in dt->Rows) {
                    list->Add(DataRowToIngredient(row));
                }
            }
            catch (Exception^) {
            }
            return list;
        }

        int InventoryService::GetLowStockCount() {
            try {
                String^ sql = "SELECT COUNT(*) FROM ingredients WHERE quantity_in_stock <= reorder_level";
                Object^ result = DatabaseManager::ExecuteScalar(sql, nullptr);
                return (result != nullptr) ? Convert::ToInt32(result) : 0;
            }
            catch (Exception^) {
                return 0;
            }
        }

        List<Ingredient^>^ InventoryService::SearchIngredients(String^ keyword) {
            List<Ingredient^>^ list = gcnew List<Ingredient^>();
            try {
                String^ sql = "SELECT ingredient_id, ingredient_name, quantity_in_stock, unit_of_measure, reorder_level, supplier_name, supplier_phone, last_restocked FROM ingredients " +
                              "WHERE ingredient_name LIKE @kw OR supplier_name LIKE @kw ORDER BY ingredient_name ASC";
                array<MySqlParameter^>^ params = {
                    gcnew MySqlParameter("@kw", "%" + keyword->Trim() + "%")
                };
                DataTable^ dt = DatabaseManager::ExecuteQuery(sql, params);
                for each (DataRow^ row in dt->Rows) {
                    list->Add(DataRowToIngredient(row));
                }
            }
            catch (Exception^) {
            }
            return list;
        }

        bool InventoryService::AddIngredient(Ingredient^ ing, [System::Runtime::InteropServices::Out] String^% errorMsg) {
            errorMsg = "";
            if (ing == nullptr) {
                errorMsg = "Ingredient details cannot be empty.";
                return false;
            }

            try {
                if (String::IsNullOrWhiteSpace(ing->IngredientName)) {
                    errorMsg = "Ingredient name is required.";
                    return false;
                }
                if (ing->QuantityInStock < Decimal(0)) {
                    errorMsg = "Quantity in stock cannot be negative.";
                    return false;
                }
                if (ing->ReorderLevel < Decimal(0)) {
                    errorMsg = "Reorder level cannot be negative.";
                    return false;
                }

                String^ sql = "INSERT INTO ingredients (ingredient_name, quantity_in_stock, unit_of_measure, reorder_level, supplier_name, supplier_phone) " +
                              "VALUES (@name, @qty, @unit, @reorder, @supp, @phone)";
                array<MySqlParameter^>^ params = {
                    gcnew MySqlParameter("@name", ing->IngredientName),
                    gcnew MySqlParameter("@qty", ing->QuantityInStock),
                    gcnew MySqlParameter("@unit", ing->UnitOfMeasure),
                    gcnew MySqlParameter("@reorder", ing->ReorderLevel),
                    gcnew MySqlParameter("@supp", ing->SupplierName),
                    gcnew MySqlParameter("@phone", ing->SupplierPhone)
                };

                int rows = DatabaseManager::ExecuteNonQuery(sql, params);
                return rows > 0;
            }
            catch (MySqlException^ sqlEx) {
                if (sqlEx->Number == 1062) {
                    errorMsg = "An ingredient with this name already exists in inventory.";
                } else {
                    errorMsg = "Database error: " + sqlEx->Message;
                }
                return false;
            }
            catch (Exception^ ex) {
                errorMsg = ex->Message;
                return false;
            }
        }

        bool InventoryService::UpdateIngredient(Ingredient^ ing, [System::Runtime::InteropServices::Out] String^% errorMsg) {
            errorMsg = "";
            if (ing == nullptr || ing->IngredientId <= 0) {
                errorMsg = "Valid Ingredient ID is required for update.";
                return false;
            }

            try {
                if (String::IsNullOrWhiteSpace(ing->IngredientName)) {
                    errorMsg = "Ingredient name is required.";
                    return false;
                }
                if (ing->QuantityInStock < Decimal(0)) {
                    errorMsg = "Quantity cannot be negative.";
                    return false;
                }
                if (ing->ReorderLevel < Decimal(0)) {
                    errorMsg = "Reorder level cannot be negative.";
                    return false;
                }

                String^ sql = "UPDATE ingredients SET ingredient_name = @name, quantity_in_stock = @qty, " +
                              "unit_of_measure = @unit, reorder_level = @reorder, supplier_name = @supp, " +
                              "supplier_phone = @phone WHERE ingredient_id = @id";
                array<MySqlParameter^>^ params = {
                    gcnew MySqlParameter("@name", ing->IngredientName),
                    gcnew MySqlParameter("@qty", ing->QuantityInStock),
                    gcnew MySqlParameter("@unit", ing->UnitOfMeasure),
                    gcnew MySqlParameter("@reorder", ing->ReorderLevel),
                    gcnew MySqlParameter("@supp", ing->SupplierName),
                    gcnew MySqlParameter("@phone", ing->SupplierPhone),
                    gcnew MySqlParameter("@id", ing->IngredientId)
                };

                int rows = DatabaseManager::ExecuteNonQuery(sql, params);
                return rows > 0;
            }
            catch (MySqlException^ sqlEx) {
                if (sqlEx->Number == 1062) {
                    errorMsg = "An ingredient with this name already exists.";
                } else {
                    errorMsg = "Database error: " + sqlEx->Message;
                }
                return false;
            }
            catch (Exception^ ex) {
                errorMsg = ex->Message;
                return false;
            }
        }

        bool InventoryService::UpdateStockQuantity(int ingredientId, Decimal newQuantity, [System::Runtime::InteropServices::Out] String^% errorMsg) {
            errorMsg = "";
            try {
                if (newQuantity < Decimal(0)) {
                    errorMsg = "Stock quantity cannot be negative.";
                    return false;
                }
                String^ sql = "UPDATE ingredients SET quantity_in_stock = @qty WHERE ingredient_id = @id";
                array<MySqlParameter^>^ params = {
                    gcnew MySqlParameter("@qty", newQuantity),
                    gcnew MySqlParameter("@id", ingredientId)
                };
                int rows = DatabaseManager::ExecuteNonQuery(sql, params);
                return rows > 0;
            }
            catch (Exception^ ex) {
                errorMsg = ex->Message;
                return false;
            }
        }

        bool InventoryService::DeleteIngredient(int ingredientId, [System::Runtime::InteropServices::Out] String^% errorMsg) {
            errorMsg = "";
            try {
                String^ sql = "DELETE FROM ingredients WHERE ingredient_id = @id";
                array<MySqlParameter^>^ params = {
                    gcnew MySqlParameter("@id", ingredientId)
                };
                int rows = DatabaseManager::ExecuteNonQuery(sql, params);
                return rows > 0;
            }
            catch (Exception^ ex) {
                errorMsg = ex->Message;
                return false;
            }
        }

    }
}
