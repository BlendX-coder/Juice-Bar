#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Drawing;
using namespace System::Drawing::Printing;

namespace FreshSipJuiceBar {
    namespace Forms {

        /// <summary>
        /// Modal dialog displaying formatted, print-ready sales transaction receipts.
        /// </summary>
        public ref class ReceiptForm : public System::Windows::Forms::Form {
        private:
            String^ m_receiptText;
            RichTextBox^ txtReceipt;
            Button^ btnPrintPreview;
            Button^ btnPrint;
            Button^ btnClose;
            PrintDocument^ printDoc;
            Panel^ topPanel;
            Label^ lblTitle;

        public:
            ReceiptForm(String^ receiptContent) {
                m_receiptText = receiptContent;
                InitializeComponent();
                txtReceipt->Text = receiptContent;
            }

        protected:
            ~ReceiptForm() {
                if (components) {
                    delete components;
                }
            }

        private:
            System::ComponentModel::Container^ components;

            void InitializeComponent(void) {
                this->components = gcnew System::ComponentModel::Container();
                this->txtReceipt = gcnew RichTextBox();
                this->btnPrintPreview = gcnew Button();
                this->btnPrint = gcnew Button();
                this->btnClose = gcnew Button();
                this->printDoc = gcnew PrintDocument();
                this->topPanel = gcnew Panel();
                this->lblTitle = gcnew Label();

                // topPanel
                this->topPanel->BackColor = Color::FromArgb(46, 139, 87);
                this->topPanel->Dock = DockStyle::Top;
                this->topPanel->Height = 55;

                // lblTitle
                this->lblTitle->Text = "Customer Sales Receipt";
                this->lblTitle->Font = gcnew System::Drawing::Font("Segoe UI", 13, FontStyle::Bold);
                this->lblTitle->ForeColor = Color::White;
                this->lblTitle->Location = Point(20, 15);
                this->lblTitle->AutoSize = true;
                this->topPanel->Controls->Add(this->lblTitle);

                // txtReceipt
                this->txtReceipt->Location = Point(20, 70);
                this->txtReceipt->Size = System::Drawing::Size(460, 480);
                this->txtReceipt->Font = gcnew System::Drawing::Font("Consolas", 10);
                this->txtReceipt->BackColor = Color::FromArgb(255, 253, 248);
                this->txtReceipt->ForeColor = Color::FromArgb(38, 50, 56);
                this->txtReceipt->ReadOnly = true;
                this->txtReceipt->BorderStyle = BorderStyle::FixedSingle;

                // btnPrintPreview
                this->btnPrintPreview->Text = "Print Preview";
                this->btnPrintPreview->Font = gcnew System::Drawing::Font("Segoe UI", 9.5f, FontStyle::Bold);
                this->btnPrintPreview->BackColor = Color::FromArgb(255, 159, 67);
                this->btnPrintPreview->ForeColor = Color::White;
                this->btnPrintPreview->FlatStyle = FlatStyle::Flat;
                this->btnPrintPreview->FlatAppearance->BorderSize = 0;
                this->btnPrintPreview->Location = Point(45, 565);
                this->btnPrintPreview->Size = System::Drawing::Size(125, 38);
                this->btnPrintPreview->Cursor = Cursors::Hand;
                this->btnPrintPreview->Click += gcnew EventHandler(this, &ReceiptForm::OnPrintPreviewClick);

                // btnPrint
                this->btnPrint->Text = "Print Receipt";
                this->btnPrint->Font = gcnew System::Drawing::Font("Segoe UI", 9.5f, FontStyle::Bold);
                this->btnPrint->BackColor = Color::FromArgb(46, 139, 87);
                this->btnPrint->ForeColor = Color::White;
                this->btnPrint->FlatStyle = FlatStyle::Flat;
                this->btnPrint->FlatAppearance->BorderSize = 0;
                this->btnPrint->Location = Point(185, 565);
                this->btnPrint->Size = System::Drawing::Size(130, 38);
                this->btnPrint->Cursor = Cursors::Hand;
                this->btnPrint->Click += gcnew EventHandler(this, &ReceiptForm::OnPrintClick);

                // btnClose
                this->btnClose->Text = "Close";
                this->btnClose->Font = gcnew System::Drawing::Font("Segoe UI", 9.5f);
                this->btnClose->BackColor = Color::FromArgb(220, 224, 230);
                this->btnClose->ForeColor = Color::FromArgb(38, 50, 56);
                this->btnClose->FlatStyle = FlatStyle::Flat;
                this->btnClose->FlatAppearance->BorderSize = 0;
                this->btnClose->Location = Point(330, 565);
                this->btnClose->Size = System::Drawing::Size(120, 38);
                this->btnClose->Cursor = Cursors::Hand;
                this->btnClose->Click += gcnew EventHandler(this, &ReceiptForm::OnCloseClick);

                // printDoc
                this->printDoc->PrintPage += gcnew PrintPageEventHandler(this, &ReceiptForm::OnPrintPage);

                // Form settings
                this->ClientSize = System::Drawing::Size(500, 620);
                this->Controls->Add(this->topPanel);
                this->Controls->Add(this->txtReceipt);
                this->Controls->Add(this->btnPrintPreview);
                this->Controls->Add(this->btnPrint);
                this->Controls->Add(this->btnClose);
                this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
                this->MaximizeBox = false;
                this->MinimizeBox = false;
                this->StartPosition = FormStartPosition::CenterParent;
                this->Text = "FreshSip - Transaction Receipt";
                this->BackColor = Color::FromArgb(255, 249, 240);
            }

            void OnCloseClick(Object^ sender, EventArgs^ e) {
                this->Close();
            }

            void OnPrintPreviewClick(Object^ sender, EventArgs^ e) {
                try {
                    PrintPreviewDialog^ prevDlg = gcnew PrintPreviewDialog();
                    prevDlg->Document = this->printDoc;
                    prevDlg->Width = 600;
                    prevDlg->Height = 700;
                    prevDlg->StartPosition = FormStartPosition::CenterParent;
                    prevDlg->ShowDialog(this);
                }
                catch (Exception^ ex) {
                    MessageBox::Show("Print Preview error: " + ex->Message, "Preview Failed", MessageBoxButtons::OK, MessageBoxIcon::Error);
                }
            }

            void OnPrintClick(Object^ sender, EventArgs^ e) {
                PrintDialog^ dlg = gcnew PrintDialog();
                dlg->Document = this->printDoc;
                if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
                    try {
                        this->printDoc->Print();
                    }
                    catch (Exception^ ex) {
                        MessageBox::Show("Printing error: " + ex->Message, "Print Failed", MessageBoxButtons::OK, MessageBoxIcon::Error);
                    }
                }
            }

            void OnPrintPage(Object^ sender, PrintPageEventArgs^ e) {
                System::Drawing::Font^ printFont = gcnew System::Drawing::Font("Consolas", 9);
                SolidBrush^ printBrush = gcnew SolidBrush(Color::Black);
                e->Graphics->DrawString(m_receiptText, printFont, printBrush, 40, 40);
            }
        };

    }
}
