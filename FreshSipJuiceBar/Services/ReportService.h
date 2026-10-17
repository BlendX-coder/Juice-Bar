#pragma once

#include "../Data/DatabaseManager.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Data;
using namespace MySql::Data::MySqlClient;
using namespace FreshSipJuiceBar::Data;

namespace FreshSipJuiceBar {
    namespace Services {

        /// <summary>
        /// Struct or Value Holder for Best Selling Product reporting.
        /// </summary>
        public ref class ProductSalesSummary {
        public:
            property String^ ProductName;
            property int TotalQuantitySold;
            property Decimal TotalRevenue;

            ProductSalesSummary(String^ name, int qty, Decimal rev) {
                ProductName = name;
                TotalQuantitySold = qty;
                TotalRevenue = rev;
            }
        };

        /// <summary>
        /// Struct or Value Holder for Daily Revenue reporting.
        /// </summary>
        public ref class DailyRevenueSummary {
        public:
            property String^ DateString;
            property int OrderCount;
            property Decimal TotalRevenue;

            DailyRevenueSummary(String^ d, int count, Decimal rev) {
                DateString = d;
                OrderCount = count;
                TotalRevenue = rev;
            }
        };

        /// <summary>
        /// Service calculating dashboard KPIs, sales aggregation, and historical reporting metrics.
        /// </summary>
        public ref class ReportService {
        public:
            static Decimal GetTodaySalesTotal();
            static int GetTodayOrderCount();
            static int GetTotalOrdersCount();
            static int GetAvailableProductsCount();

            static List<ProductSalesSummary^>^ GetTopSellingProducts(int limit);
            static List<DailyRevenueSummary^>^ GetRevenueSummary(DateTime start, DateTime end);
        };

    }
}
