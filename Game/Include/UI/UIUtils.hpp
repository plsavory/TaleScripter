//
// Created by Pete on 2019-07-17.
//

#ifndef TALESCRIPTER_UI_UTILS_HPP
#define TALESCRIPTER_UI_UTILS_HPP

#include <iostream>
#include <vector>
#include "UIConstants.h"

using namespace UIConstants;

class UIUtils {
public:
    /**
     * Returns all element types that could ever exist in the database
     * Should be called once with the vector stored somewhere, as it creates objects without deleting them.
     */
    static std::vector<ElementType*> getAllPossibleElementTypes() {
        std::vector<ElementType*> types;

        // Add each element type
        types.push_back(new ElementType("novelTextBackground", false, false));
        types.push_back(new ElementType("menuBackground", false, false));
        types.push_back(new ElementType("menuButton", false, false));
        types.push_back(new ElementType("menuButtonHighlighted", false, false));

        return types;
    };

    static std::vector<DataAttributeGroup*> getAllPossibleNovelScreenAttributeGroups() {
        std::vector<DataAttributeGroup*> groups;

        groups.push_back(new DataAttributeGroup("textDisplay"));
        groups.push_back(new DataAttributeGroup("characterNameDisplay"));
        return groups;
    }
private:
};

#endif //TALESCRIPTER_UI_UTILS_HPP
