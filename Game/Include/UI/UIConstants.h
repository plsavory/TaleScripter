//
// Created by Pete on 2019-06-15.
//

#ifndef TALESCRIPTER_UI_CONSTANTS_H
#define TALESCRIPTER_UI_CONSTANTS_H

namespace UIConstants {

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
