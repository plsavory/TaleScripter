#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "Exceptions/GeneralException.hpp"
#include "Base/ErrorScreen.hpp"

/**
 * @param window the window which we will be drawing to
 */
ErrorScreen::ErrorScreen(sf::RenderWindow *window) {
    mainWindow = window;
}

/**
 * Start the error screen - Creates object and retrieves information that it needs to display.
 * @param exception the exception which to grab the error message from
 */
void ErrorScreen::start(GeneralException &exception) {
    // Create text objects, load fonts and set text to display the error message
    errorFont = new sf::Font();

    if (!errorFont->loadFromFile("Resource/fonts/system.ttf")) {
        // Can't do anything if we can't load a font to display the error in - Throw it again so it picks up by the console exception handler
        throw;
    }

    init(exception.what());

}

void ErrorScreen::start(const std::string& message) {

    // Create text objects, load fonts and set text to display the error message
    errorFont = new sf::Font();

    if (!errorFont->loadFromFile("Resource/fonts/system.ttf")) {
        // Can't do anything if we can't load a font to display the error in - Throw it again so it picks up by the console exception handler
        throw GeneralException(message);
    }

    init(message);
}

void ErrorScreen::update() {

}

/**
 * Draw our text to the screen
 */
void ErrorScreen::draw() {

    if (headingText) {
        mainWindow->draw(*headingText);
    }

    if (errorText) {
        mainWindow->draw(*errorText);
    }

}

/**
 * Initialise the screen with a string as a message
 * Used when an exception occurred before we enter the main game loop, we store the message and then pass it into here
 * as soon as the engine has initialised.
 * @param errorMessage
 */
void ErrorScreen::init(const std::string& errorMessage) {
    float windowWidth = mainWindow->getSize().x;
    float windowHeight = mainWindow->getSize().y;

    float yPosition = (windowHeight / 2) - 30;

    // Header text
    headingText = new sf::Text();
    headingText->setFont(*errorFont);
    headingText->setCharacterSize(24);
    headingText->setString("An error has occurred");
    headingText->setPosition((float) ((windowWidth/2) - (headingText->getGlobalBounds().width / 2)), yPosition);

    // The error message itself
    errorText = new sf::Text();
    errorText->setFont(*errorFont);
    errorText->setFillColor(sf::Color::White);
    errorText->setCharacterSize(16);
    errorText->setString(errorMessage);
    errorText->setPosition((float) ((windowWidth/2) - (errorText->getGlobalBounds().width / 2)), yPosition + 100);
}