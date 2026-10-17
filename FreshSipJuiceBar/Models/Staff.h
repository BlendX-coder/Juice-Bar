#pragma once

#include "Person.h"

using namespace System;

namespace FreshSipJuiceBar {
    namespace Models {

        /// <summary>
        /// Staff entity inheriting from Person (OOP Concept D: Inheritance).
        /// Adds staff-specific attributes like role, shift, and compensation.
        /// </summary>
        public ref class Staff : public Person {
        private:
            String^ m_role;
            String^ m_shift;
            Decimal m_salary;

        public:
            Staff() : Person() {
                m_role = "Cashier";
                m_shift = "Morning";
                m_salary = Decimal(0);
            }

            Staff(int id, String^ name, String^ phone, String^ email, String^ role, String^ shift, Decimal salary)
                : Person(id, name, phone, email) {
                this->Role = role;
                this->Shift = shift;
                this->Salary = salary;
            }

            property String^ Role {
                String^ get() { return m_role; }
                void set(String^ value) { m_role = (value == nullptr) ? "Cashier" : value->Trim(); }
            }

            property String^ Shift {
                String^ get() { return m_shift; }
                void set(String^ value) { m_shift = (value == nullptr) ? "Morning" : value->Trim(); }
            }

            property Decimal Salary {
                Decimal get() { return m_salary; }
                void set(Decimal value) {
                    if (value < Decimal(0))
                        throw gcnew ArgumentException("Salary cannot be negative.");
                    m_salary = value;
                }
            }

            virtual String^ GetRole() override {
                return m_role;
            }

            virtual String^ GetDisplayText() override {
                return m_name + " [" + m_role + " - " + m_shift + "]";
            }
        };

    }
}
