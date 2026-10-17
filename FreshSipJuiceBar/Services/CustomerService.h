#pragma once

#include "../Models/Customer.h"
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
        /// Service handling customer registrations, updates, and loyalty searches.
        /// </summary>
        public ref class CustomerService {
        public:
            static List<Customer^>^ GetAllCustomers();
            static Customer^ GetCustomerById(int customerId);
            static List<Customer^>^ SearchCustomers(String^ keyword);

            static bool AddCustomer(Customer^ customer, [System::Runtime::InteropServices::Out] String^% errorMsg);
            static bool UpdateCustomer(Customer^ customer, [System::Runtime::InteropServices::Out] String^% errorMsg);
            static bool DeleteCustomer(int customerId, [System::Runtime::InteropServices::Out] String^% errorMsg);

        private:
            static Customer^ DataRowToCustomer(DataRow^ row);
        };

    }
}
