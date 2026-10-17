#include "ReportService.h"

namespace FreshSipJuiceBar {
    namespace Services {

        Decimal ReportService::GetTodaySalesTotal() {
            try {
                String^ sql = "SELECT COALESCE(SUM(final_total), 0.00) FROM orders WHERE DATE(order_date) = CURDATE()";
                Object^ result = DatabaseManager::ExecuteScalar(sql, nullptr);
                return (result != nullptr && result != DBNull::Value) ? Convert::ToDecimal(result) : Decimal(0);
            }
            catch (Exception^) {
                return Decimal(0);
            }
        }

        int ReportService::GetTodayOrderCount() {
            try {
                String^ sql = "SELECT COUNT(*) FROM orders WHERE DATE(order_date) = CURDATE()";
                Object^ result = DatabaseManager::ExecuteScalar(sql, nullptr);
                return (result != nullptr && result != DBNull::Value) ? Convert::ToInt32(result) : 0;
            }
            catch (Exception^) {
                return 0;
            }
        }

        int ReportService::GetTotalOrdersCount() {
            try {
                String^ sql = "SELECT COUNT(*) FROM orders";
                Object^ result = DatabaseManager::ExecuteScalar(sql, nullptr);
                return (result != nullptr && result != DBNull::Value) ? Convert::ToInt32(result) : 0;
            }
            catch (Exception^) {
                return 0;
            }
        }

        int ReportService::GetAvailableProductsCount() {
            try {
                String^ sql = "SELECT COUNT(*) FROM products WHERE is_active = 1 AND quantity > 0";
                Object^ result = DatabaseManager::ExecuteScalar(sql, nullptr);
                return (result != nullptr && result != DBNull::Value) ? Convert::ToInt32(result) : 0;
            }
            catch (Exception^) {
                return 0;
            }
        }

        List<ProductSalesSummary^>^ ReportService::GetTopSellingProducts(int limit) {
            List<ProductSalesSummary^>^ list = gcnew List<ProductSalesSummary^>();
            try {
                String^ sql = "SELECT product_name, SUM(quantity) as total_qty, SUM(subtotal) as total_rev " +
                              "FROM order_items GROUP BY product_id, product_name ORDER BY total_qty DESC LIMIT @lim";
                array<MySqlParameter^>^ params = {
                    gcnew MySqlParameter("@lim", limit)
                };
                DataTable^ dt = DatabaseManager::ExecuteQuery(sql, params);
                for each (DataRow^ row in dt->Rows) {
                    list->Add(gcnew ProductSalesSummary(
                        row["product_name"]->ToString(),
                        Convert::ToInt32(row["total_qty"]),
                        Convert::ToDecimal(row["total_rev"])
                    ));
                }
            }
            catch (Exception^) {
            }
            return list;
        }

        List<DailyRevenueSummary^>^ ReportService::GetRevenueSummary(DateTime start, DateTime end) {
            List<DailyRevenueSummary^>^ list = gcnew List<DailyRevenueSummary^>();
            try {
                String^ sql = "SELECT DATE_FORMAT(order_date, '%Y-%m-%d') as sale_date, COUNT(*) as ord_count, SUM(final_total) as day_revenue " +
                              "FROM orders WHERE order_date >= @sdate AND order_date <= @edate " +
                              "GROUP BY DATE_FORMAT(order_date, '%Y-%m-%d') ORDER BY sale_date ASC";
                array<MySqlParameter^>^ params = {
                    gcnew MySqlParameter("@sdate", start.Date),
                    gcnew MySqlParameter("@edate", end.Date.AddDays(1).AddTicks(-1))
                };
                DataTable^ dt = DatabaseManager::ExecuteQuery(sql, params);
                for each (DataRow^ row in dt->Rows) {
                    list->Add(gcnew DailyRevenueSummary(
                        row["sale_date"]->ToString(),
                        Convert::ToInt32(row["ord_count"]),
                        Convert::ToDecimal(row["day_revenue"])
                    ));
                }
            }
            catch (Exception^) {
            }
            return list;
        }

    }
}
