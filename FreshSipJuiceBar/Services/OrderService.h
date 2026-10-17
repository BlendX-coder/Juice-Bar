#pragma once

#include "../Models/Order.h"
#include "../Models/OrderItem.h"
#include "../Models/PaymentMethod.h"
#include "../Data/DatabaseManager.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Data;
using namespace MySql::Data::MySqlClient;
using namespace FreshSipJuiceBar::Models;
using namespace FreshSipJuiceBar::Data;

namespace FreshSipJuiceBar {
    namespace Services {

        /// <summary>
        /// Service coordinating atomic sales transactions, stock deduction, and polymorphic payment execution.
        /// </summary>
        public ref class OrderService {
        public:
            /// <summary>
            /// Commits an order with polymorphic payment execution and transactional stock management.
            /// </summary>
            static bool ProcessAndSaveOrder(Order^ order, PaymentMethod^ paymentMethod,
                [System::Runtime::InteropServices::Out] String^% transactionReceipt,
                [System::Runtime::InteropServices::Out] String^% errorMsg);

            static List<Order^>^ GetRecentOrders(int limit);
            static List<Order^>^ GetOrdersByDateRange(DateTime start, DateTime end);
            static List<OrderItem^>^ GetOrderItems(int orderId);
            static Order^ GetOrderByNumber(String^ orderNumber);

        private:
            static Order^ DataRowToOrder(DataRow^ row);
        };

    }
}
