#include "CustomerService.h"

namespace FreshSipJuiceBar {
    namespace Services {

        Customer^ CustomerService::DataRowToCustomer(DataRow^ row) {
            Customer^ c = gcnew Customer();
            c->Id = Convert::ToInt32(row["customer_id"]);
            c->Name = row["name"]->ToString();
            c->Phone = row["phone"]->ToString();
            c->Email = (row["email"] == DBNull::Value) ? "" : row["email"]->ToString();
            c->Address = (row["address"] == DBNull::Value) ? "" : row["address"]->ToString();
            c->RegisteredDate = Convert::ToDateTime(row["registered_date"]);
            return c;
        }

        List<Customer^>^ CustomerService::GetAllCustomers() {
            List<Customer^>^ list = gcnew List<Customer^>();
            try {
                String^ sql = "SELECT customer_id, name, phone, email, address, registered_date FROM customers ORDER BY name ASC";
                DataTable^ dt = DatabaseManager::ExecuteQuery(sql, nullptr);
                for each (DataRow^ row in dt->Rows) {
                    list->Add(DataRowToCustomer(row));
                }
            }
            catch (Exception^) {
            }
            return list;
        }

        Customer^ CustomerService::GetCustomerById(int customerId) {
            try {
                String^ sql = "SELECT customer_id, name, phone, email, address, registered_date FROM customers WHERE customer_id = @id";
                array<MySqlParameter^>^ params = {
                    gcnew MySqlParameter("@id", customerId)
                };
                DataTable^ dt = DatabaseManager::ExecuteQuery(sql, params);
                if (dt->Rows->Count > 0) {
                    return DataRowToCustomer(dt->Rows[0]);
                }
            }
            catch (Exception^) {
            }
            return nullptr;
        }

        List<Customer^>^ CustomerService::SearchCustomers(String^ keyword) {
            List<Customer^>^ list = gcnew List<Customer^>();
            try {
                String^ sql = "SELECT customer_id, name, phone, email, address, registered_date FROM customers " +
                              "WHERE name LIKE @kw OR phone LIKE @kw OR email LIKE @kw ORDER BY name ASC";
                array<MySqlParameter^>^ params = {
                    gcnew MySqlParameter("@kw", "%" + keyword->Trim() + "%")
                };
                DataTable^ dt = DatabaseManager::ExecuteQuery(sql, params);
                for each (DataRow^ row in dt->Rows) {
                    list->Add(DataRowToCustomer(row));
                }
            }
            catch (Exception^) {
            }
            return list;
        }

        bool CustomerService::AddCustomer(Customer^ customer, [System::Runtime::InteropServices::Out] String^% errorMsg) {
            errorMsg = "";
            if (customer == nullptr) {
                errorMsg = "Customer details cannot be empty.";
                return false;
            }

            try {
                if (String::IsNullOrWhiteSpace(customer->Name)) {
                    errorMsg = "Customer name is required.";
                    return false;
                }
                if (String::IsNullOrWhiteSpace(customer->Phone)) {
                    errorMsg = "Customer phone number is required.";
                    return false;
                }

                String^ sql = "INSERT INTO customers (name, phone, email, address) VALUES (@name, @phone, @email, @addr)";
                array<MySqlParameter^>^ params = {
                    gcnew MySqlParameter("@name", customer->Name),
                    gcnew MySqlParameter("@phone", customer->Phone),
                    gcnew MySqlParameter("@email", customer->Email),
                    gcnew MySqlParameter("@addr", customer->Address)
                };

                int rows = DatabaseManager::ExecuteNonQuery(sql, params);
                return rows > 0;
            }
            catch (MySqlException^ sqlEx) {
                if (sqlEx->Number == 1062) {
                    errorMsg = "A customer with this phone number is already registered.";
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

        bool CustomerService::UpdateCustomer(Customer^ customer, [System::Runtime::InteropServices::Out] String^% errorMsg) {
            errorMsg = "";
            if (customer == nullptr || customer->Id <= 0) {
                errorMsg = "Valid Customer ID is required for update.";
                return false;
            }

            try {
                if (String::IsNullOrWhiteSpace(customer->Name)) {
                    errorMsg = "Customer name is required.";
                    return false;
                }
                if (String::IsNullOrWhiteSpace(customer->Phone)) {
                    errorMsg = "Customer phone number is required.";
                    return false;
                }

                String^ sql = "UPDATE customers SET name = @name, phone = @phone, email = @email, address = @addr WHERE customer_id = @id";
                array<MySqlParameter^>^ params = {
                    gcnew MySqlParameter("@name", customer->Name),
                    gcnew MySqlParameter("@phone", customer->Phone),
                    gcnew MySqlParameter("@email", customer->Email),
                    gcnew MySqlParameter("@addr", customer->Address),
                    gcnew MySqlParameter("@id", customer->Id)
                };

                int rows = DatabaseManager::ExecuteNonQuery(sql, params);
                return rows > 0;
            }
            catch (MySqlException^ sqlEx) {
                if (sqlEx->Number == 1062) {
                    errorMsg = "Another customer already uses this phone number.";
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

        bool CustomerService::DeleteCustomer(int customerId, [System::Runtime::InteropServices::Out] String^% errorMsg) {
            errorMsg = "";
            try {
                String^ sql = "DELETE FROM customers WHERE customer_id = @id";
                array<MySqlParameter^>^ params = {
                    gcnew MySqlParameter("@id", customerId)
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
