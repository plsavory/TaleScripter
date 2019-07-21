//
// Created by Pete on 2019-06-18.
//

#ifndef TALESCRIPTER_UI_ASSET_BUILDER_H
#define TALESCRIPTER_UI_ASSET_BUILDER_H

#include "UI/Themes/UITheme.h"

// This methods in this class will build gradients and other assets to be used for drawing UI elements
class UIAssetBuilder {
public:
    static sf::Texture* getFillGradient(UIThemeGradient *gradient, sf::Vector2f size) {

        // Create the bitmap data
        sf::Uint8 *pixels;
        pixels = new sf::Uint8[size.x*size.y*4];

        unsigned int width = size.x;
        unsigned int height = size.y;

        // Looks like this is going to get ugly...
        int pixelMemoryPosition = 0;

        for (int y = 0; y < height; y++) {
            bool overHalfVertical = y > (height/2);
            for (int x = 0; x < width; x++) {
                bool overHalfHorizontal = x > (width/2);
                unsigned int colourToUse = overHalfHorizontal + (overHalfVertical << (unsigned int)1);
                sf::Color *colour = gradient->getColour(colourToUse);
            }
        }

        for (unsigned int i = 0; i < (size.x * size.y); i++) {

            // Calculate which instance of sf::Color that we want to use
            bool overHalfHorizontal = i > (i & width) / 2;
            bool overHalfVertical = (i - (i%width)) > (size.x * size.y)/2;



            // Calculate the colour that we will use for this pixel
            unsigned char r = 0;
            unsigned char g = 0;
            unsigned char b = 0;
            unsigned char a = 0;


            // Place the pixel in to the array
        }

        sf::Texture *texture = new sf::Texture();
        texture->create(size.x, size.y);

        delete(pixels);

    };
};

#endif //TALESCRIPTER_UIASSET_BUILDER_H
