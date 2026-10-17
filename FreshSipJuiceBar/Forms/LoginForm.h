#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Windows::Forms;
using namespace System::Drawing;

namespace FreshSipJuiceBar {
    namespace Forms {

        /// <summary>
        /// Dedicated Secure Staff Login Form with Role Selection and Password Verification.
        /// Replaces basic switch role popup with an authentic system login workflow.
        /// </summary>
        public ref class LoginForm : public System::Windows::Forms::Form {
        private:
            Panel^ pnlHeader;
            Label^ lblBrandTitle;
            Label^ lblBrandSubtitle;
            Label^ lblAccessTag;

            Label^ lblRoleSelect;
            RadioButton^ rdoAdmin;
            RadioButton^ rdoCashier;

            Label^ lblUserTitle;
            TextBox^ txtUsername;

            Label^ lblPassTitle;
            TextBox^ txtPassword;
            CheckBox^ chkShowPass;

            Label^ lblStatusMessage;
            Button^ btnLogin;
            Button^ btnQuickAdmin;
            Button^ btnQuickCashier;
            Button^ btnExit;

            String^ m_authenticatedRole;
            String^ m_authenticatedUserName;

        public:
            LoginForm(void) {
                m_authenticatedRole = "";
                m_authenticatedUserName = "";
                InitializeComponent();
                ApplyRoleDefaults();
            }

            property String^ AuthenticatedRole {
                String^ get() { return m_authenticatedRole; }
            }

            property String^ AuthenticatedUserName {
                String^ get() { return m_authenticatedUserName; }
            }

        protected:
            ~LoginForm() {
                if (components) {
                    delete components;
                }
            }

        private:
            System::ComponentModel::Container^ components;

            void InitializeComponent(void) {
                this->components = gcnew System::ComponentModel::Container();

                // Form settings
                this->ClientSize = System::Drawing::Size(440, 520);
                this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
                this->StartPosition = FormStartPosition::CenterScreen;
                this->MaximizeBox = false;
                this->MinimizeBox = false;
                this->Text = "FreshSip Juice Bar - Staff Authentication";
                this->BackColor = Color::FromArgb(255, 249, 240); // Warm Cream

                // 1. Header Banner
                this->pnlHeader = gcnew Panel();
                this->pnlHeader->Dock = DockStyle::Top;
                this->pnlHeader->Height = 90;
                this->pnlHeader->BackColor = Color::FromArgb(46, 139, 87); // Fresh Green
                this->Controls->Add(this->pnlHeader);

                this->lblBrandTitle = gcnew Label();
                this->lblBrandTitle->Text = "FreshSip Juice Bar";
                this->lblBrandTitle->Font = gcnew System::Drawing::Font("Segoe UI", 16.0f, FontStyle::Bold);
                this->lblBrandTitle->ForeColor = Color::FromArgb(255, 159, 67); // Fresh Orange
                this->lblBrandTitle->Location = Point(24, 16);
                this->lblBrandTitle->AutoSize = true;
                this->pnlHeader->Controls->Add(this->lblBrandTitle);

                this->lblBrandSubtitle = gcnew Label();
                this->lblBrandSubtitle->Text = "Point of Sale & Management System Login";
                this->lblBrandSubtitle->Font = gcnew System::Drawing::Font("Segoe UI", 9.0f, FontStyle::Regular);
                this->lblBrandSubtitle->ForeColor = Color::FromArgb(235, 245, 240);
                this->lblBrandSubtitle->Location = Point(26, 50);
                this->lblBrandSubtitle->AutoSize = true;
                this->pnlHeader->Controls->Add(this->lblBrandSubtitle);

                this->lblAccessTag = gcnew Label();
                this->lblAccessTag->Text = "STAFF PORTAL";
                this->lblAccessTag->Font = gcnew System::Drawing::Font("Segoe UI", 8.0f, FontStyle::Bold);
                this->lblAccessTag->ForeColor = Color::White;
                this->lblAccessTag->BackColor = Color::FromArgb(38, 50, 56);
                this->lblAccessTag->Padding = System::Windows::Forms::Padding(6, 3, 6, 3);
                this->lblAccessTag->Location = Point(320, 20);
                this->lblAccessTag->AutoSize = true;
                this->pnlHeader->Controls->Add(this->lblAccessTag);

                int curY = 110;

                // 2. Role Selector
                this->lblRoleSelect = gcnew Label();
                this->lblRoleSelect->Text = "Select Account Role:";
                this->lblRoleSelect->Font = gcnew System::Drawing::Font("Segoe UI", 9.5f, FontStyle::Bold);
                this->lblRoleSelect->ForeColor = Color::FromArgb(38, 50, 56);
                this->lblRoleSelect->Location = Point(30, curY);
                this->lblRoleSelect->AutoSize = true;
                this->Controls->Add(this->lblRoleSelect);
                curY += 26;

                this->rdoAdmin = gcnew RadioButton();
                this->rdoAdmin->Text = "Store Administrator (Chamara Bandara)";
                this->rdoAdmin->Font = gcnew System::Drawing::Font("Segoe UI", 9.0f, FontStyle::Bold);
                this->rdoAdmin->ForeColor = Color::FromArgb(210, 105, 30);
                this->rdoAdmin->Location = Point(34, curY);
                this->rdoAdmin->AutoSize = true;
                this->rdoAdmin->Checked = true;
                this->rdoAdmin->CheckedChanged += gcnew EventHandler(this, &LoginForm::OnRoleChanged);
                this->Controls->Add(this->rdoAdmin);
                curY += 25;

                this->rdoCashier = gcnew RadioButton();
                this->rdoCashier->Text = "Cashier Terminal (Anuki Samarasinghe)";
                this->rdoCashier->Font = gcnew System::Drawing::Font("Segoe UI", 9.0f, FontStyle::Bold);
                this->rdoCashier->ForeColor = Color::FromArgb(46, 139, 87);
                this->rdoCashier->Location = Point(34, curY);
                this->rdoCashier->AutoSize = true;
                this->rdoCashier->CheckedChanged += gcnew EventHandler(this, &LoginForm::OnRoleChanged);
                this->Controls->Add(this->rdoCashier);
                curY += 32;

                // 3. Username
                this->lblUserTitle = gcnew Label();
                this->lblUserTitle->Text = "Username:";
                this->lblUserTitle->Font = gcnew System::Drawing::Font("Segoe UI", 9.0f, FontStyle::Regular);
                this->lblUserTitle->ForeColor = Color::FromArgb(60, 70, 80);
                this->lblUserTitle->Location = Point(30, curY);
                this->lblUserTitle->AutoSize = true;
                this->Controls->Add(this->lblUserTitle);
                curY += 20;

                this->txtUsername = gcnew TextBox();
                this->txtUsername->Location = Point(30, curY);
                this->txtUsername->Size = System::Drawing::Size(375, 25);
                this->txtUsername->Font = gcnew System::Drawing::Font("Segoe UI", 9.5f);
                this->Controls->Add(this->txtUsername);
                curY += 35;

                // 4. Password
                this->lblPassTitle = gcnew Label();
                this->lblPassTitle->Text = "Password:";
                this->lblPassTitle->Font = gcnew System::Drawing::Font("Segoe UI", 9.0f, FontStyle::Regular);
                this->lblPassTitle->ForeColor = Color::FromArgb(60, 70, 80);
                this->lblPassTitle->Location = Point(30, curY);
                this->lblPassTitle->AutoSize = true;
                this->Controls->Add(this->lblPassTitle);
                curY += 20;

                this->txtPassword = gcnew TextBox();
                this->txtPassword->Location = Point(30, curY);
                this->txtPassword->Size = System::Drawing::Size(375, 25);
                this->txtPassword->Font = gcnew System::Drawing::Font("Segoe UI", 9.5f);
                this->txtPassword->PasswordChar = '*';
                this->Controls->Add(this->txtPassword);
                curY += 28;

                this->chkShowPass = gcnew CheckBox();
                this->chkShowPass->Text = "Show Password";
                this->chkShowPass->Font = gcnew System::Drawing::Font("Segoe UI", 8.5f);
                this->chkShowPass->ForeColor = Color::FromArgb(80, 90, 100);
                this->chkShowPass->Location = Point(32, curY);
                this->chkShowPass->AutoSize = true;
                this->chkShowPass->CheckedChanged += gcnew EventHandler(this, &LoginForm::OnShowPassChanged);
                this->Controls->Add(this->chkShowPass);
                curY += 28;

                // 5. Error status message
                this->lblStatusMessage = gcnew Label();
                this->lblStatusMessage->Text = "";
                this->lblStatusMessage->Font = gcnew System::Drawing::Font("Segoe UI", 8.5f, FontStyle::Bold);
                this->lblStatusMessage->ForeColor = Color::FromArgb(220, 53, 69);
                this->lblStatusMessage->Location = Point(30, curY);
                this->lblStatusMessage->Size = System::Drawing::Size(375, 20);
                this->lblStatusMessage->TextAlign = ContentAlignment::MiddleCenter;
                this->Controls->Add(this->lblStatusMessage);
                curY += 22;

                // 6. Sign In Button
                this->btnLogin = gcnew Button();
                this->btnLogin->Text = "Sign In to Terminal";
                this->btnLogin->Location = Point(30, curY);
                this->btnLogin->Size = System::Drawing::Size(375, 40);
                this->btnLogin->BackColor = Color::FromArgb(46, 139, 87);
                this->btnLogin->ForeColor = Color::White;
                this->btnLogin->Font = gcnew System::Drawing::Font("Segoe UI", 11.0f, FontStyle::Bold);
                this->btnLogin->FlatStyle = FlatStyle::Flat;
                this->btnLogin->FlatAppearance->BorderSize = 0;
                this->btnLogin->Cursor = Cursors::Hand;
                this->btnLogin->Click += gcnew EventHandler(this, &LoginForm::OnLoginClick);
                this->Controls->Add(this->btnLogin);
                this->AcceptButton = this->btnLogin;
                curY += 48;

                // 7. Quick Demo Login Shortcuts
                this->btnQuickAdmin = gcnew Button();
                this->btnQuickAdmin->Text = "Demo: Admin";
                this->btnQuickAdmin->Location = Point(30, curY);
                this->btnQuickAdmin->Size = System::Drawing::Size(120, 28);
                this->btnQuickAdmin->BackColor = Color::FromArgb(255, 159, 67);
                this->btnQuickAdmin->ForeColor = Color::White;
                this->btnQuickAdmin->Font = gcnew System::Drawing::Font("Segoe UI", 8.5f, FontStyle::Bold);
                this->btnQuickAdmin->FlatStyle = FlatStyle::Flat;
                this->btnQuickAdmin->FlatAppearance->BorderSize = 0;
                this->btnQuickAdmin->Cursor = Cursors::Hand;
                this->btnQuickAdmin->Click += gcnew EventHandler(this, &LoginForm::OnQuickAdminClick);
                this->Controls->Add(this->btnQuickAdmin);

                this->btnQuickCashier = gcnew Button();
                this->btnQuickCashier->Text = "Demo: Cashier";
                this->btnQuickCashier->Location = Point(158, curY);
                this->btnQuickCashier->Size = System::Drawing::Size(120, 28);
                this->btnQuickCashier->BackColor = Color::FromArgb(46, 139, 87);
                this->btnQuickCashier->ForeColor = Color::White;
                this->btnQuickCashier->Font = gcnew System::Drawing::Font("Segoe UI", 8.5f, FontStyle::Bold);
                this->btnQuickCashier->FlatStyle = FlatStyle::Flat;
                this->btnQuickCashier->FlatAppearance->BorderSize = 0;
                this->btnQuickCashier->Cursor = Cursors::Hand;
                this->btnQuickCashier->Click += gcnew EventHandler(this, &LoginForm::OnQuickCashierClick);
                this->Controls->Add(this->btnQuickCashier);

                this->btnExit = gcnew Button();
                this->btnExit->Text = "Exit";
                this->btnExit->Location = Point(286, curY);
                this->btnExit->Size = System::Drawing::Size(119, 28);
                this->btnExit->BackColor = Color::FromArgb(108, 117, 125);
                this->btnExit->ForeColor = Color::White;
                this->btnExit->Font = gcnew System::Drawing::Font("Segoe UI", 8.5f);
                this->btnExit->FlatStyle = FlatStyle::Flat;
                this->btnExit->FlatAppearance->BorderSize = 0;
                this->btnExit->Cursor = Cursors::Hand;
                this->btnExit->Click += gcnew EventHandler(this, &LoginForm::OnExitClick);
                this->Controls->Add(this->btnExit);
                this->CancelButton = this->btnExit;
            }

            void ApplyRoleDefaults() {
                if (rdoAdmin->Checked) {
                    txtUsername->Text = "admin";
                    txtPassword->Text = "admin123";
                }
                else {
                    txtUsername->Text = "cashier";
                    txtPassword->Text = "cashier123";
                }
                lblStatusMessage->Text = "";
            }

            void OnRoleChanged(Object^ sender, EventArgs^ e) {
                ApplyRoleDefaults();
            }

            void OnShowPassChanged(Object^ sender, EventArgs^ e) {
                txtPassword->PasswordChar = chkShowPass->Checked ? '\0' : '*';
            }

            void OnLoginClick(Object^ sender, EventArgs^ e) {
                String^ user = txtUsername->Text->Trim();
                String^ pass = txtPassword->Text->Trim();

                if (String::IsNullOrEmpty(user) || String::IsNullOrEmpty(pass)) {
                    lblStatusMessage->Text = "Please enter both username and password.";
                    return;
                }

                // Verify Administrator
                if ((user->ToLower() == "admin" || user->ToLower() == "chamara") && pass == "admin123") {
                    m_authenticatedRole = "Admin";
                    m_authenticatedUserName = "Chamara Bandara";
                    this->DialogResult = System::Windows::Forms::DialogResult::OK;
                    this->Close();
                    return;
                }

                // Verify Cashier
                if ((user->ToLower() == "cashier" || user->ToLower() == "anuki") && pass == "cashier123") {
                    m_authenticatedRole = "Cashier";
                    m_authenticatedUserName = "Anuki Samarasinghe";
                    this->DialogResult = System::Windows::Forms::DialogResult::OK;
                    this->Close();
                    return;
                }

                lblStatusMessage->Text = "Authentication failed! Check credentials.";
            }

            void OnQuickAdminClick(Object^ sender, EventArgs^ e) {
                rdoAdmin->Checked = true;
                txtUsername->Text = "admin";
                txtPassword->Text = "admin123";
                OnLoginClick(sender, e);
            }

            void OnQuickCashierClick(Object^ sender, EventArgs^ e) {
                rdoCashier->Checked = true;
                txtUsername->Text = "cashier";
                txtPassword->Text = "cashier123";
                OnLoginClick(sender, e);
            }

            void OnExitClick(Object^ sender, EventArgs^ e) {
                this->DialogResult = System::Windows::Forms::DialogResult::Cancel;
                this->Close();
            }
        };

    }
}
