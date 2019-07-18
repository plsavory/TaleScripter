//
// Created by Pete on 2019-06-15.
//

#ifndef TALESCRIPTER_UI_CONSTANTS_H
#define TALESCRIPTER_UI_CONSTANTS_H

#include "MisuseException.hpp"

namespace UIConstants {

    struct ElementType {
    public:
        ElementType(const std::string &aName, bool aRequired, bool aRequiresPosition) {
            id = 0;
            name = aName;
            required = aRequired;
        };

        void addId(int newId) {
            id = newId;
        }

        std::string getName() {
            return name;
        };

        bool isRequired() {
            return required;
        };

        int getId() {

            if (id == 0) {
                throw MisuseException("getId called on a ElementType without id being set from the database");
            }

            return id;
        };
    private:
        std::string name;
        bool required;
        int id;
    };

    struct Element {

    };

    struct AttributeType {
    public:
        static const int UIContainer = 0; // A general container to contain all ui elements

        static std::vector<std::string> getTypeNames() {
            const std::vector<std::string> types = {"UIContainer"};
            return types;
        }

        static std::string getTypeName(int id) {
            return getTypeNames()[id];
        };
    };

};

#endif //TALESCRIPTER_UICONSTANTS_H
