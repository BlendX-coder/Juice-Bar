#pragma once

#include "../Models/Ingredient.h"
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
        /// Service managing raw ingredient inventory and restocking alerts.
        /// </summary>
        public ref class InventoryService {
        public:
            static List<Ingredient^>^ GetAllIngredients();
            static List<Ingredient^>^ GetLowStockIngredients();
            static List<Ingredient^>^ SearchIngredients(String^ keyword);
            static int GetLowStockCount();

            static bool AddIngredient(Ingredient^ ing, [System::Runtime::InteropServices::Out] String^% errorMsg);
            static bool UpdateIngredient(Ingredient^ ing, [System::Runtime::InteropServices::Out] String^% errorMsg);
            static bool UpdateStockQuantity(int ingredientId, Decimal newQuantity, [System::Runtime::InteropServices::Out] String^% errorMsg);
            static bool DeleteIngredient(int ingredientId, [System::Runtime::InteropServices::Out] String^% errorMsg);

        private:
            static Ingredient^ DataRowToIngredient(DataRow^ row);
        };

    }
}
