#pragma once

using namespace System;
using namespace System::Runtime::InteropServices;

namespace FreshSipJuiceBar {
    namespace Models {

        /// <summary>
        /// Abstraction (OOP Concept C):
        /// Abstract base class establishing the contract for payment processing.
        /// Polymorphism (OOP Concept E):
        /// Runtime polymorphism enables dynamic dispatch to CashPayment or CardPayment.
        /// </summary>
        public ref class PaymentMethod abstract {
        public:
            PaymentMethod() {}

            /// <summary>
            /// Pure virtual method returning the readable payment mode name.
            /// </summary>
            virtual String^ GetMethodName() abstract;

            /// <summary>
            /// Pure virtual polymorphic method executing payment logic and generating confirmation text.
            /// </summary>
            virtual bool ProcessPayment(Decimal amount, [Out] String^% confirmationMessage) abstract;
        };

        /// <summary>
        /// Concrete Cash Payment Implementation.
        /// Evaluates physical currency tendered against net order total.
        /// </summary>
        public ref class CashPayment : public PaymentMethod {
        private:
            Decimal m_tenderedAmount;
            Decimal m_changeGiven;

        public:
            CashPayment(Decimal tenderedAmount) {
                if (tenderedAmount < Decimal(0))
                    throw gcnew ArgumentException("Tendered cash amount cannot be negative.");
                m_tenderedAmount = tenderedAmount;
                m_changeGiven = Decimal(0);
            }

            property Decimal TenderedAmount {
                Decimal get() { return m_tenderedAmount; }
            }

            property Decimal ChangeGiven {
                Decimal get() { return m_changeGiven; }
            }

            virtual String^ GetMethodName() override {
                return "Cash";
            }

            virtual bool ProcessPayment(Decimal amount, [Out] String^% confirmationMessage) override {
                if (m_tenderedAmount < amount) {
                    Decimal shortage = amount - m_tenderedAmount;
                    confirmationMessage = String::Format("Insufficient cash tendered. Shortage: LKR {0:N2}", shortage);
                    return false;
                }

                m_changeGiven = m_tenderedAmount - amount;
                confirmationMessage = String::Format(
                    "Cash Payment Verified.\n" +
                    "Order Total:    LKR {0:N2}\n" +
                    "Cash Tendered:  LKR {1:N2}\n" +
                    "Change Return:  LKR {2:N2}",
                    amount, m_tenderedAmount, m_changeGiven);
                return true;
            }
        };

        /// <summary>
        /// Concrete Card Payment Implementation.
        /// Simulates EMV chip/contactless terminal transaction authorization.
        /// </summary>
        public ref class CardPayment : public PaymentMethod {
        private:
            String^ m_cardType; // "Visa", "Mastercard", "Amex"
            String^ m_lastFourDigits;
            String^ m_authCode;

        public:
            CardPayment(String^ cardType, String^ lastFourDigits) {
                m_cardType = (String::IsNullOrWhiteSpace(cardType)) ? "Visa" : cardType->Trim();
                m_lastFourDigits = (String::IsNullOrWhiteSpace(lastFourDigits)) ? "0000" : lastFourDigits->Trim();
                // Generate a simulated 6-digit cryptographic approval authorization code
                Random^ rnd = gcnew Random();
                m_authCode = String::Format("AUTH-{0:D6}", rnd->Next(100000, 999999));
            }

            property String^ CardType {
                String^ get() { return m_cardType; }
            }

            property String^ LastFourDigits {
                String^ get() { return m_lastFourDigits; }
            }

            property String^ AuthCode {
                String^ get() { return m_authCode; }
            }

            virtual String^ GetMethodName() override {
                return "Card (" + m_cardType + ")";
            }

            virtual bool ProcessPayment(Decimal amount, [Out] String^% confirmationMessage) override {
                if (amount <= Decimal(0)) {
                    confirmationMessage = "Invalid transaction amount for card settlement.";
                    return false;
                }

                confirmationMessage = String::Format(
                    "Electronic Terminal Transaction Approved.\n" +
                    "Card Network:   {0}\n" +
                    "Card Account:   **** **** **** {1}\n" +
                    "Approval Code:  {2}\n" +
                    "Charged Amount: LKR {3:N2}",
                    m_cardType, m_lastFourDigits, m_authCode, amount);
                return true;
            }
        };

    }
}
