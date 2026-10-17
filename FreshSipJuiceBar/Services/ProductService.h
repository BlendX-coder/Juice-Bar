#pragma once

#include "../Models/Product.h"
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
        /// Service handling business logic, validation, and database operations for Products.
        /// </summary>
        public ref class ProductService {
        public:
            static List<Product^>^ GetAllProducts();
            static List<Product^>^ GetActiveProducts();
            static Product^ GetProductById(int productId);
            static List<Product^>^ SearchProducts(String^ keyword, String^ category);

            static bool AddProduct(Product^ product, [System::Runtime::InteropServices::Out] String^% errorMsg);
            static bool UpdateProduct(Product^ product, [System::Runtime::InteropServices::Out] String^% errorMsg);
            static bool DeleteProduct(int productId, [System::Runtime::InteropServices::Out] String^% errorMsg);
            static bool UpdateStock(int productId, int deltaQuantity, [System::Runtime::InteropServices::Out] String^% errorMsg);

        private:
            static Product^ DataRowToProduct(DataRow^ row);
        };

    }
}
