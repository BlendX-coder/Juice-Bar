#include "OrderService.h"
#include "ProductService.h"

namespace FreshSipJuiceBar {
    namespace Services {

        Order^ OrderService::DataRowToOrder(DataRow^ row) {
            Order^ ord = gcnew Order();
            ord->OrderId = Convert::ToInt32(row["order_id"]);
            ord->OrderNumber = row["order_number"]->ToString();
            if (row["customer_id"] != DBNull::Value) {
                ord->CustomerId = Nullable<int>(Convert::ToInt32(row["customer_id"]));
            }
            ord->CustomerName = row["customer_name"]->ToString();
            ord->OrderDate = Convert::ToDateTime(row["order_date"]);
            ord->Subtotal = Convert::ToDecimal(row["subtotal"]);
            ord->Discount = Convert::ToDecimal(row["discount"]);
            ord->FinalTotal = Convert::ToDecimal(row["final_total"]);
            ord->PaymentMethodName = row["payment_method"]->ToString();
            ord->Status = row["status"]->ToString();
            ord->Notes = (row["notes"] == DBNull::Value) ? "" : row["notes"]->ToString();
            return ord;
        }

        bool OrderService::ProcessAndSaveOrder(Order^ order, PaymentMethod^ paymentMethod,
            [System::Runtime::InteropServices::Out] String^% transactionReceipt,
            [System::Runtime::InteropServices::Out] String^% errorMsg) {

            transactionReceipt = "";
            errorMsg = "";

            if (order == nullptr || order->Items == nullptr || order->Items->Count == 0) {
                errorMsg = "Order contains no items to purchase.";
                return false;
            }
            if (paymentMethod == nullptr) {
                errorMsg = "A valid payment method must be selected.";
                return false;
            }

            // 1. Pre-validation: Verify stock availability in database
            for each (OrderItem^ item in order->Items) {
                Product^ prod = ProductService::GetProductById(item->ProductId);
                if (prod == nullptr) {
                    errorMsg = String::Format("Product '{0}' no longer exists in system.", item->ProductName);
                    return false;
                }
                if (prod->Quantity < item->Quantity) {
                    errorMsg = String::Format("Insufficient stock for '{0}'. Available: {1}, Requested: {2}.",
                        prod->ProductName, prod->Quantity, item->Quantity);
                    return false;
                }
            }

            // 2. Polymorphic Payment Processing (OOP Concept E)
            String^ paymentDetails = "";
            bool paymentOk = paymentMethod->ProcessPayment(order->FinalTotal, paymentDetails);
            if (!paymentOk) {
                errorMsg = "Payment verification declined: " + paymentDetails;
                return false;
            }

            order->PaymentMethodName = paymentMethod->GetMethodName();

            // 3. Database Transactional Persistence
            MySqlConnection^ conn = DatabaseManager::CreateConnection();
            MySqlTransaction^ trans = nullptr;

            try {
                conn->Open();
                trans = conn->BeginTransaction();

                // Insert into orders table
                String^ orderSql = "INSERT INTO orders (order_number, customer_id, customer_name, order_date, subtotal, discount, final_total, payment_method, status, notes) " +
                                   "VALUES (@onum, @cid, @cname, @odate, @sub, @disc, @total, @pay, @status, @notes)";
                MySqlCommand^ orderCmd = gcnew MySqlCommand(orderSql, conn, trans);
                orderCmd->Parameters->AddWithValue("@onum", order->OrderNumber);
                if (order->CustomerId.HasValue) {
                    orderCmd->Parameters->AddWithValue("@cid", order->CustomerId.Value);
                } else {
                    orderCmd->Parameters->AddWithValue("@cid", DBNull::Value);
                }
                orderCmd->Parameters->AddWithValue("@cname", order->CustomerName);
                orderCmd->Parameters->AddWithValue("@odate", order->OrderDate);
                orderCmd->Parameters->AddWithValue("@sub", order->Subtotal);
                orderCmd->Parameters->AddWithValue("@disc", order->Discount);
                orderCmd->Parameters->AddWithValue("@total", order->FinalTotal);
                orderCmd->Parameters->AddWithValue("@pay", order->PaymentMethodName);
                orderCmd->Parameters->AddWithValue("@status", order->Status);
                orderCmd->Parameters->AddWithValue("@notes", order->Notes);

                orderCmd->ExecuteNonQuery();
                int generatedOrderId = Convert::ToInt32(orderCmd->LastInsertedId);
                order->OrderId = generatedOrderId;

                // Insert order items & reduce product stock
                for each (OrderItem^ item in order->Items) {
                    String^ itemSql = "INSERT INTO order_items (order_id, product_id, product_name, quantity, unit_price, subtotal) " +
                                      "VALUES (@oid, @pid, @pname, @qty, @price, @sub)";
                    MySqlCommand^ itemCmd = gcnew MySqlCommand(itemSql, conn, trans);
                    itemCmd->Parameters->AddWithValue("@oid", generatedOrderId);
                    itemCmd->Parameters->AddWithValue("@pid", item->ProductId);
                    itemCmd->Parameters->AddWithValue("@pname", item->ProductName);
                    itemCmd->Parameters->AddWithValue("@qty", item->Quantity);
                    itemCmd->Parameters->AddWithValue("@price", item->UnitPrice);
                    itemCmd->Parameters->AddWithValue("@sub", item->Subtotal);
                    itemCmd->ExecuteNonQuery();

                    // Stock deduction with race-condition guard
                    String^ stockSql = "UPDATE products SET quantity = quantity - @qty WHERE product_id = @pid AND quantity >= @qty";
                    MySqlCommand^ stockCmd = gcnew MySqlCommand(stockSql, conn, trans);
                    stockCmd->Parameters->AddWithValue("@qty", item->Quantity);
                    stockCmd->Parameters->AddWithValue("@pid", item->ProductId);
                    int updatedRows = stockCmd->ExecuteNonQuery();

                    if (updatedRows == 0) {
                        throw gcnew Exception(String::Format("Stock depletion conflict occurred for item '{0}'. Transaction aborted.", item->ProductName));
                    }
                }

                // If all succeed, commit transaction
                trans->Commit();

                // 4. Generate Print-Friendly Receipt string
                System::Text::StringBuilder^ sb = gcnew System::Text::StringBuilder();
                sb->AppendLine("=================================================");
                sb->AppendLine("             FRESHSIP JUICE BAR                  ");
                sb->AppendLine("    Natural Cold-Pressed & Artisanal Juices      ");
                sb->AppendLine("        Mihintale, Anuradhapura, Sri Lanka       ");
                sb->AppendLine("               Tel: +94 25 226 6789              ");
                sb->AppendLine("=================================================");
                sb->AppendLine(String::Format("Order Number: {0}", order->OrderNumber));
                sb->AppendLine(String::Format("Date & Time:  {0:yyyy-MM-dd HH:mm:ss}", order->OrderDate));
                sb->AppendLine(String::Format("Customer:     {0}", order->CustomerName));
                sb->AppendLine(String::Format("Payment:      {0}", order->PaymentMethodName));
                sb->AppendLine("-------------------------------------------------");
                sb->AppendLine(String::Format("{0,-24} {1,4} {2,10} {3,10}", "Item", "Qty", "Price", "Total"));
                sb->AppendLine("-------------------------------------------------");
                for each (OrderItem^ item in order->Items) {
                    String^ truncatedName = item->ProductName;
                    if (truncatedName->Length > 24) {
                        truncatedName = truncatedName->Substring(0, 21) + "...";
                    }
                    sb->AppendLine(String::Format("{0,-24} {1,4} {2,10:N2} {3,10:N2}",
                        truncatedName, item->Quantity, item->UnitPrice, item->Subtotal));
                }
                sb->AppendLine("-------------------------------------------------");
                sb->AppendLine(String::Format("{0,-38} {1,10:N2}", "Subtotal (LKR):", order->Subtotal));
                if (order->Discount > Decimal(0)) {
                    sb->AppendLine(String::Format("{0,-38} {1,10:N2}", "Discount (LKR):", -order->Discount));
                }
                sb->AppendLine(String::Format("{0,-38} {1,10:N2}", "FINAL TOTAL (LKR):", order->FinalTotal));
                sb->AppendLine("=================================================");
                sb->AppendLine("Payment Authorization Summary:");
                sb->AppendLine(paymentDetails);
                sb->AppendLine("=================================================");
                sb->AppendLine("        Thank you for choosing FreshSip!         ");
                sb->AppendLine("       Please visit us again for fresh health!   ");
                sb->AppendLine("=================================================");

                transactionReceipt = sb->ToString();
                return true;
            }
            catch (Exception^ ex) {
                if (trans != nullptr) {
                    try { trans->Rollback(); } catch (Exception^) {}
                }
                errorMsg = "Order transaction failed: " + ex->Message;
                return false;
            }
            finally {
                if (conn != nullptr) {
                    conn->Close();
                }
            }
        }

        List<Order^>^ OrderService::GetRecentOrders(int limit) {
            List<Order^>^ list = gcnew List<Order^>();
            try {
                String^ sql = "SELECT order_id, order_number, customer_id, customer_name, order_date, subtotal, discount, final_total, payment_method, status, notes " +
                              "FROM orders ORDER BY order_date DESC LIMIT @lim";
                array<MySqlParameter^>^ params = {
                    gcnew MySqlParameter("@lim", limit)
                };
                DataTable^ dt = DatabaseManager::ExecuteQuery(sql, params);
                for each (DataRow^ row in dt->Rows) {
                    list->Add(DataRowToOrder(row));
                }
            }
            catch (Exception^) {
            }
            return list;
        }

        List<Order^>^ OrderService::GetOrdersByDateRange(DateTime start, DateTime end) {
            List<Order^>^ list = gcnew List<Order^>();
            try {
                String^ sql = "SELECT order_id, order_number, customer_id, customer_name, order_date, subtotal, discount, final_total, payment_method, status, notes " +
                              "FROM orders WHERE order_date >= @sdate AND order_date <= @edate ORDER BY order_date DESC";
                array<MySqlParameter^>^ params = {
                    gcnew MySqlParameter("@sdate", start),
                    gcnew MySqlParameter("@edate", end)
                };
                DataTable^ dt = DatabaseManager::ExecuteQuery(sql, params);
                for each (DataRow^ row in dt->Rows) {
                    list->Add(DataRowToOrder(row));
                }
            }
            catch (Exception^) {
            }
            return list;
        }

        List<OrderItem^>^ OrderService::GetOrderItems(int orderId) {
            List<OrderItem^>^ list = gcnew List<OrderItem^>();
            try {
                String^ sql = "SELECT order_item_id, order_id, product_id, product_name, quantity, unit_price, subtotal " +
                              "FROM order_items WHERE order_id = @oid";
                array<MySqlParameter^>^ params = {
                    gcnew MySqlParameter("@oid", orderId)
                };
                DataTable^ dt = DatabaseManager::ExecuteQuery(sql, params);
                for each (DataRow^ row in dt->Rows) {
                    OrderItem^ it = gcnew OrderItem(
                        Convert::ToInt32(row["order_item_id"]),
                        Convert::ToInt32(row["order_id"]),
                        Convert::ToInt32(row["product_id"]),
                        row["product_name"]->ToString(),
                        Convert::ToInt32(row["quantity"]),
                        Convert::ToDecimal(row["unit_price"]),
                        Convert::ToDecimal(row["subtotal"])
                    );
                    list->Add(it);
                }
            }
            catch (Exception^) {
            }
            return list;
        }

        Order^ OrderService::GetOrderByNumber(String^ orderNumber) {
            try {
                String^ sql = "SELECT order_id, order_number, customer_id, customer_name, order_date, subtotal, discount, final_total, payment_method, status, notes " +
                              "FROM orders WHERE order_number = @onum";
                array<MySqlParameter^>^ params = {
                    gcnew MySqlParameter("@onum", orderNumber->Trim())
                };
                DataTable^ dt = DatabaseManager::ExecuteQuery(sql, params);
                if (dt->Rows->Count > 0) {
                    Order^ ord = DataRowToOrder(dt->Rows[0]);
                    ord->Items = GetOrderItems(ord->OrderId);
                    return ord;
                }
            }
            catch (Exception^) {
            }
            return nullptr;
        }

    }
}
