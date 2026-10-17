#pragma once

#include "Person.h"

using namespace System;

namespace FreshSipJuiceBar {
    namespace Models {

        /// <summary>
        /// Customer entity inheriting from Person (OOP Concept D: Inheritance).
        /// Adds customer-specific attributes such as residential address and loyalty registration date.
        /// </summary>
        public ref class Customer : public Person {
        private:
            String^ m_address;
            DateTime m_registeredDate;

        public:
            Customer() : Person() {
                m_address = "";
                m_registeredDate = DateTime::Now;
            }

            Customer(int id, String^ name, String^ phone, String^ email, String^ address, DateTime registeredDate)
                : Person(id, name, phone, email) {
                m_address = (address == nullptr) ? "" : address->Trim();
                m_registeredDate = registeredDate;
            }

            property String^ Address {
                String^ get() { return m_address; }
                void set(String^ value) { m_address = (value == nullptr) ? "" : value->Trim(); }
            }

            property DateTime RegisteredDate {
                DateTime get() { return m_registeredDate; }
                void set(DateTime value) { m_registeredDate = value; }
            }

            virtual String^ GetRole() override {
                return "Customer";
            }

            virtual String^ GetDisplayText() override {
                return m_name + " | Phone: " + m_phone + (String::IsNullOrEmpty(m_address) ? "" : " (" + m_address + ")");
            }
        };

    }
}
