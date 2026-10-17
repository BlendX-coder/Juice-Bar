#pragma once

#using <System.dll>
#using <System.Data.dll>
#using <System.Configuration.dll>
#using "MySql.Data.dll"

using namespace System;
using namespace System::Data;
using namespace System::Configuration;
using namespace MySql::Data::MySqlClient;

namespace FreshSipJuiceBar {
    namespace Data {

        
        public ref class DatabaseManager {
        public:
            
            static String^ DB_SERVER   = "localhost";
            static String^ DB_PORT     = "3306";          
            static String^ DB_DATABASE = "freshsip_db";   
            static String^ DB_USERNAME = "root";           
            static String^ DB_PASSWORD = "Root123";   
            

        private:
            static String^ s_connectionString = nullptr;
            static bool s_isMockMode = false;

        public:
            static void InitializeConnectionString();
            static String^ BuildConnectionString(String^ server, String^ port, String^ database, String^ user, String^ password);
            static void SetCredentials(String^ server, String^ port, String^ database, String^ user, String^ password);
            static String^ GetConnectionString();
            static void SetConnectionString(String^ connStr);

            static bool IsMockMode() { return s_isMockMode; }
            static void SetMockMode(bool enabled) { s_isMockMode = enabled; }

            static MySqlConnection^ CreateConnection();
            static bool TestConnection([System::Runtime::InteropServices::Out] String^% errorMessage);

            static DataTable^ ExecuteQuery(String^ sql, array<MySqlParameter^>^ parameters);
            static int ExecuteNonQuery(String^ sql, array<MySqlParameter^>^ parameters);
            static Object^ ExecuteScalar(String^ sql, array<MySqlParameter^>^ parameters);
        };

    }
}
