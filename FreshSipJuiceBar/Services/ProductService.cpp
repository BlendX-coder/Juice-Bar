#include "ProductService.h"

namespace FreshSipJuiceBar {
    namespace Services {

        Product^ ProductService::DataRowToProduct(DataRow^ row) {
            Product^ p = gcnew Product();
            p->ProductId = Convert::ToInt32(row["product_id"]);
            p->ProductName = row["product_name"]->ToString();
            p->Category = row["category"]->ToString();
            p->Description = (row["description"] == DBNull::Value) ? "" : row["description"]->ToString();
            p->Price = Convert::ToDecimal(row["price"]);
            p->Quantity = Convert::ToInt32(row["quantity"]);
            p->ImagePath = (row["image_path"] == DBNull::Value) ? "" : row["image_path"]->ToString();
            p->IsActive = Convert::ToBoolean(row["is_active"]);
            return p;
        }

        List<Product^>^ ProductService::GetAllProducts() {
            List<Product^>^ list = gcnew List<Product^>();
            try {
                String^ sql = "SELECT product_id, product_name, category, description, price, quantity, image_path, is_active FROM products ORDER BY product_id DESC";
                DataTable^ dt = DatabaseManager::ExecuteQuery(sql, nullptr);
                for each (DataRow^ row in dt->Rows) {
                    list->Add(DataRowToProduct(row));
                }
            }
            catch (Exception^) {
                // Return whatever was loaded
            }
            return list;
        }

        List<Product^>^ ProductService::GetActiveProducts() {
            List<Product^>^ list = gcnew List<Product^>();
            try {
                String^ sql = "SELECT product_id, product_name, category, description, price, quantity, image_path, is_active FROM products WHERE is_active = 1 ORDER BY product_name ASC";
                DataTable^ dt = DatabaseManager::ExecuteQuery(sql, nullptr);
                for each (DataRow^ row in dt->Rows) {
                    list->Add(DataRowToProduct(row));
                }
            }
            catch (Exception^) {
            }
            return list;
        }

        Product^ ProductService::GetProductById(int productId) {
            try {
                String^ sql = "SELECT product_id, product_name, category, description, price, quantity, image_path, is_active FROM products WHERE product_id = @id";
                array<MySqlParameter^>^ params = {
                    gcnew MySqlParameter("@id", productId)
                };
                DataTable^ dt = DatabaseManager::ExecuteQuery(sql, params);
                if (dt->Rows->Count > 0) {
                    return DataRowToProduct(dt->Rows[0]);
                }
            }
            catch (Exception^) {
            }
            return nullptr;
        }

        List<Product^>^ ProductService::SearchProducts(String^ keyword, String^ category) {
            List<Product^>^ list = gcnew List<Product^>();
            try {
                String^ sql = "SELECT product_id, product_name, category, description, price, quantity, image_path, is_active FROM products WHERE 1=1";
                List<MySqlParameter^>^ paramList = gcnew List<MySqlParameter^>();

                if (!String::IsNullOrWhiteSpace(keyword)) {
                    sql += " AND (product_name LIKE @kw OR description LIKE @kw)";
                    paramList->Add(gcnew MySqlParameter("@kw", "%" + keyword->Trim() + "%"));
                }
                if (!String::IsNullOrWhiteSpace(category) && category != "All Categories") {
                    sql += " AND category = @cat";
                    paramList->Add(gcnew MySqlParameter("@cat", category->Trim()));
                }

                sql += " ORDER BY product_name ASC";
                DataTable^ dt = DatabaseManager::ExecuteQuery(sql, paramList->ToArray());
                for each (DataRow^ row in dt->Rows) {
                    list->Add(DataRowToProduct(row));
                }
            }
            catch (Exception^) {
            }
            return list;
        }

        bool ProductService::AddProduct(Product^ product, [System::Runtime::InteropServices::Out] String^% errorMsg) {
            errorMsg = "";
            if (product == nullptr) {
                errorMsg = "Product object cannot be null.";
                return false;
            }

            try {
                // Validation checks
                if (String::IsNullOrWhiteSpace(product->ProductName)) {
                    errorMsg = "Product name cannot be empty.";
                    return false;
                }
                if (product->Price < Decimal(0)) {
                    errorMsg = "Price cannot be negative.";
                    return false;
                }
                if (product->Quantity < 0) {
                    errorMsg = "Quantity cannot be negative.";
                    return false;
                }

                String^ sql = "INSERT INTO products (product_name, category, description, price, quantity, image_path, is_active) " +
                              "VALUES (@name, @cat, @desc, @price, @qty, @img, @active)";
                array<MySqlParameter^>^ params = {
                    gcnew MySqlParameter("@name", product->ProductName),
                    gcnew MySqlParameter("@cat", product->Category),
                    gcnew MySqlParameter("@desc", product->Description),
                    gcnew MySqlParameter("@price", product->Price),
                    gcnew MySqlParameter("@qty", product->Quantity),
                    gcnew MySqlParameter("@img", product->ImagePath),
                    gcnew MySqlParameter("@active", product->IsActive ? 1 : 0)
                };

                int rows = DatabaseManager::ExecuteNonQuery(sql, params);
                return rows > 0;
            }
            catch (MySqlException^ sqlEx) {
                if (sqlEx->Number == 1062) {
                    errorMsg = "A product with this name already exists in the system.";
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

        bool ProductService::UpdateProduct(Product^ product, [System::Runtime::InteropServices::Out] String^% errorMsg) {
            errorMsg = "";
            if (product == nullptr || product->ProductId <= 0) {
                errorMsg = "Valid Product ID is required for update.";
                return false;
            }

            try {
                if (String::IsNullOrWhiteSpace(product->ProductName)) {
                    errorMsg = "Product name cannot be empty.";
                    return false;
                }
                if (product->Price < Decimal(0)) {
                    errorMsg = "Price cannot be negative.";
                    return false;
                }
                if (product->Quantity < 0) {
                    errorMsg = "Quantity cannot be negative.";
                    return false;
                }

                String^ sql = "UPDATE products SET product_name = @name, category = @cat, description = @desc, " +
                              "price = @price, quantity = @qty, image_path = @img, is_active = @active " +
                              "WHERE product_id = @id";
                array<MySqlParameter^>^ params = {
                    gcnew MySqlParameter("@name", product->ProductName),
                    gcnew MySqlParameter("@cat", product->Category),
                    gcnew MySqlParameter("@desc", product->Description),
                    gcnew MySqlParameter("@price", product->Price),
                    gcnew MySqlParameter("@qty", product->Quantity),
                    gcnew MySqlParameter("@img", product->ImagePath),
                    gcnew MySqlParameter("@active", product->IsActive ? 1 : 0),
                    gcnew MySqlParameter("@id", product->ProductId)
                };

                int rows = DatabaseManager::ExecuteNonQuery(sql, params);
                return rows > 0;
            }
            catch (MySqlException^ sqlEx) {
                if (sqlEx->Number == 1062) {
                    errorMsg = "A product with this name already exists.";
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

        bool ProductService::DeleteProduct(int productId, [System::Runtime::InteropServices::Out] String^% errorMsg) {
            errorMsg = "";
            try {
                // Check referential integrity: has this product been ordered in existing transactions?
                String^ checkSql = "SELECT COUNT(*) FROM order_items WHERE product_id = @id";
                array<MySqlParameter^>^ checkParams = {
                    gcnew MySqlParameter("@id", productId)
                };
                Object^ countObj = DatabaseManager::ExecuteScalar(checkSql, checkParams);
                int orderCount = (countObj != nullptr) ? Convert::ToInt32(countObj) : 0;

                if (orderCount > 0) {
                    // Safe archival approach required by Section 5: Deactivate rather than break sales records
                    String^ archiveSql = "UPDATE products SET is_active = 0 WHERE product_id = @id";
                    array<MySqlParameter^>^ archiveParams = {
                        gcnew MySqlParameter("@id", productId)
                    };
                    DatabaseManager::ExecuteNonQuery(archiveSql, archiveParams);
                    errorMsg = "Product is referenced in existing sales orders. It has been safely archived (deactivated) to preserve historical accounting integrity.";
                    return true;
                }

                // If not referenced, safe to permanently delete
                String^ delSql = "DELETE FROM products WHERE product_id = @id";
                array<MySqlParameter^>^ delParams = {
                    gcnew MySqlParameter("@id", productId)
                };
                int rows = DatabaseManager::ExecuteNonQuery(delSql, delParams);
                return rows > 0;
            }
            catch (Exception^ ex) {
                errorMsg = ex->Message;
                return false;
            }
        }

        bool ProductService::UpdateStock(int productId, int deltaQuantity, [System::Runtime::InteropServices::Out] String^% errorMsg) {
            errorMsg = "";
            try {
                String^ sql = "UPDATE products SET quantity = quantity + @delta WHERE product_id = @id AND (quantity + @delta) >= 0";
                array<MySqlParameter^>^ params = {
                    gcnew MySqlParameter("@delta", deltaQuantity),
                    gcnew MySqlParameter("@id", productId)
                };
                int rows = DatabaseManager::ExecuteNonQuery(sql, params);
                if (rows == 0) {
                    errorMsg = "Insufficient stock available to satisfy the request.";
                    return false;
                }
                return true;
            }
            catch (Exception^ ex) {
                errorMsg = ex->Message;
                return false;
            }
        }

    }
}
