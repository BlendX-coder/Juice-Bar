#include "DatabaseManager.h"

namespace FreshSipJuiceBar {
    namespace Data {

        String^ DatabaseManager::BuildConnectionString(String^ server, String^ port, String^ database, String^ user, String^ password) {
            return String::Format(
                "Server={0};Port={1};Database={2};Uid={3};Pwd={4};SslMode=none;AllowPublicKeyRetrieval=True;CharSet=utf8mb4;Connection Timeout=3;",
                server, port, database, user, password
            );
        }

        void DatabaseManager::SetCredentials(String^ server, String^ port, String^ database, String^ user, String^ password) {
            DB_SERVER = server;
            DB_PORT = port;
            DB_DATABASE = database;
            DB_USERNAME = user;
            DB_PASSWORD = password;
            s_connectionString = BuildConnectionString(DB_SERVER, DB_PORT, DB_DATABASE, DB_USERNAME, DB_PASSWORD);
        }

        void DatabaseManager::InitializeConnectionString() {
            // Priority 1: Direct credentials in DatabaseManager.h
            String^ defaultConn = BuildConnectionString(DB_SERVER, DB_PORT, DB_DATABASE, DB_USERNAME, DB_PASSWORD);

            // Priority 2: App.config if configured
            try {
                ConnectionStringSettings^ setting = ConfigurationManager::ConnectionStrings["FreshSipConnection"];
                if (setting != nullptr && !String::IsNullOrWhiteSpace(setting->ConnectionString)) {
                    String^ appConn = setting->ConnectionString->Trim();
                    if (!appConn->Contains("Connection Timeout")) {
                        appConn += "Connection Timeout=3;";
                    }
                    s_connectionString = appConn;
                    return;
                }
            }
            catch (Exception^) {}

            s_connectionString = defaultConn;
        }

        String^ DatabaseManager::GetConnectionString() {
            if (String::IsNullOrEmpty(s_connectionString)) {
                InitializeConnectionString();
            }
            return s_connectionString;
        }

        void DatabaseManager::SetConnectionString(String^ connStr) {
            if (!String::IsNullOrWhiteSpace(connStr)) {
                s_connectionString = connStr->Trim();
            }
        }

        MySqlConnection^ DatabaseManager::CreateConnection() {
            return gcnew MySqlConnection(GetConnectionString());
        }

        bool DatabaseManager::TestConnection([System::Runtime::InteropServices::Out] String^% errorMessage) {
            errorMessage = "";
            MySqlConnection^ conn = nullptr;
            try {
                conn = CreateConnection();
                conn->Open();
                return true;
            }
            catch (Exception^ ex) {
                errorMessage = ex->Message;

                // Smart multi-environment auto fallback:
                // Tries common ports (3306 for XAMPP/MySQL, 3307 for custom MariaDB)
                // and passwords (custom configured password, blank password, and root)
                array<String^>^ testPorts = gcnew array<String^> { DB_PORT, (DB_PORT == "3306" ? "3307" : "3306") };
                array<String^>^ testPwds = gcnew array<String^> { DB_PASSWORD, "09060916@Ruz", "", "root" };

                for each (String^ port in testPorts) {
                    for each (String^ pwd in testPwds) {
                        try {
                            String^ candidateConn = BuildConnectionString(DB_SERVER, port, DB_DATABASE, DB_USERNAME, pwd);
                            MySqlConnection^ testConn = gcnew MySqlConnection(candidateConn);
                            testConn->Open();
                            testConn->Close();

                            // Successfully connected with this combination! Save and apply it
                            s_connectionString = candidateConn;
                            DB_PORT = port;
                            DB_PASSWORD = pwd;
                            errorMessage = "";
                            return true;
                        }
                        catch (Exception^) {
                            // Continue checking next candidate
                        }
                    }
                }

                return false;
            }
            finally {
                if (conn != nullptr && conn->State == ConnectionState::Open) {
                    conn->Close();
                }
            }
        }

        DataTable^ DatabaseManager::ExecuteQuery(String^ sql, array<MySqlParameter^>^ parameters) {
            DataTable^ table = gcnew DataTable();
            MySqlConnection^ conn = CreateConnection();
            try {
                conn->Open();
                MySqlCommand^ cmd = gcnew MySqlCommand(sql, conn);
                if (parameters != nullptr && parameters->Length > 0) {
                    cmd->Parameters->AddRange(parameters);
                }
                MySqlDataAdapter^ adapter = gcnew MySqlDataAdapter(cmd);
                adapter->Fill(table);
            }
            finally {
                if (conn != nullptr) {
                    conn->Close();
                }
            }
            return table;
        }

        int DatabaseManager::ExecuteNonQuery(String^ sql, array<MySqlParameter^>^ parameters) {
            int rowsAffected = 0;
            MySqlConnection^ conn = CreateConnection();
            try {
                conn->Open();
                MySqlCommand^ cmd = gcnew MySqlCommand(sql, conn);
                if (parameters != nullptr && parameters->Length > 0) {
                    cmd->Parameters->AddRange(parameters);
                }
                rowsAffected = cmd->ExecuteNonQuery();
            }
            finally {
                if (conn != nullptr) {
                    conn->Close();
                }
            }
            return rowsAffected;
        }

        Object^ DatabaseManager::ExecuteScalar(String^ sql, array<MySqlParameter^>^ parameters) {
            Object^ result = nullptr;
            MySqlConnection^ conn = CreateConnection();
            try {
                conn->Open();
                MySqlCommand^ cmd = gcnew MySqlCommand(sql, conn);
                if (parameters != nullptr && parameters->Length > 0) {
                    cmd->Parameters->AddRange(parameters);
                }
                result = cmd->ExecuteScalar();
            }
            finally {
                if (conn != nullptr) {
                    conn->Close();
                }
            }
            return result;
        }

    }
}
