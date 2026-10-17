#pragma once

using namespace System;

namespace FreshSipJuiceBar {
    namespace Models {

        /// <summary>
        /// Base class demonstrating Inheritance (OOP Concept D).
        /// Serves as the generalized entity for all individuals within the system.
        /// </summary>
        public ref class Person abstract {
        protected:
            int m_id;
            String^ m_name;
            String^ m_phone;
            String^ m_email;

        public:
            Person() {
                m_id = 0;
                m_name = "";
                m_phone = "";
                m_email = "";
            }

            Person(int id, String^ name, String^ phone, String^ email) {
                this->Id = id;
                this->Name = name;
                this->Phone = phone;
                this->Email = email;
            }

            property int Id {
                int get() { return m_id; }
                void set(int value) { m_id = value; }
            }

            property String^ Name {
                String^ get() { return m_name; }
                void set(String^ value) {
                    if (String::IsNullOrWhiteSpace(value))
                        throw gcnew ArgumentException("Person name cannot be null or empty.");
                    m_name = value->Trim();
                }
            }

            property String^ Phone {
                String^ get() { return m_phone; }
                void set(String^ value) {
                    if (String::IsNullOrWhiteSpace(value))
                        throw gcnew ArgumentException("Phone number is required.");
                    m_phone = value->Trim();
                }
            }

            property String^ Email {
                String^ get() { return m_email; }
                void set(String^ value) {
                    m_email = (value == nullptr) ? "" : value->Trim();
                }
            }

            /// <summary>
            /// Polymorphic virtual method to obtain formatted personal identification.
            /// </summary>
            virtual String^ GetDisplayText() {
                return m_name + " (" + m_phone + ")";
            }

            virtual String^ GetRole() abstract;
        };

    }
}
