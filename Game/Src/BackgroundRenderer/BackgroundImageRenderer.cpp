#include <iostream>
#include <queue>
#include <sstream>
#include <SFML/Graphics.hpp>
#include "Database/DatabaseConnection.hpp"
#include "BackgroundRenderer/BackgroundImageRenderer.hpp"

BackgroundImageRenderer::BackgroundImageRenderer(sf::RenderWindow *windowPointer) {
    window = windowPointer;

    backgroundTextureManager = new BackgroundTextureManager();

    currentBackground = nullptr;
    upcomingBackground = nullptr;
    backgroundColour = nullptr;
    drawingEnabled = true;
    backgroundAlpha = 255;

}

BackgroundImageRenderer::~BackgroundImageRenderer() {

    for (auto &background : backgrounds) {
        delete (background);
    }

}

/**
 * [processQueue Should be called from another thread - load a background]
 */
void BackgroundImageRenderer::processQueue() {
    // It might be a good idea to load every background used in one scene, and unload them when the scene changes
    if (isQueueEmpty()) {
        return;
    }

    // Do nothing if the background doesn't exist yet (Race conditions with threads)
    if (!getBackground(backgroundLoadQueue.front().getId())) {
        return;
    }

    getBackground(backgroundLoadQueue.front().getId())->load();
    backgroundLoadQueue.pop();
}

bool BackgroundImageRenderer::isQueueEmpty() {
    return (backgroundLoadQueue.empty());
}

void BackgroundImageRenderer::draw() {

    if (!drawingEnabled) {
        return;
    }

    if (upcomingBackground) {
        if (upcomingBackground->getStatus() == bgLoaded) {
            upcomingBackground->draw();
        }
    }

    if (currentBackground) {
        if (currentBackground->getStatus() == bgLoaded) {
            currentBackground->draw();
        }
    }
}

void BackgroundImageRenderer::update(sf::Clock *gameTime) {

    for (auto &background : backgrounds) {
        background->update(gameTime);
    }
}

void BackgroundImageRenderer::addAllFromDatabase(DatabaseConnection *db) {

    // Load information about all of the background textures first
    backgroundTextureManager->loadAllFromDatabase(db);

    // Load information about each background
    auto *rows = new DataSet();

    db->executeQuery("SELECT * FROM backgrounds", rows);

    for (auto &row : rows->getRows()) {
        auto *newBackground = new Background(row, window, db, backgroundTextureManager);
        backgrounds.push_back(newBackground);
    }

    delete (rows);

}

void BackgroundImageRenderer::loadBackground(int id) {
    backgroundLoadQueue.push(BackgroundLoadRequest(id));
}

Background* BackgroundImageRenderer::findBackground(const std::string &name) {

    for (int i = 0; i < MAX_BACKGROUNDS; i++) {
        if (backgrounds[i]) {
            if (backgrounds[i]->getName() == name) {
                return backgrounds[i];
            }
        }
    }

    throw ResourceException(Utils::implodeString({"Could not find background with name ", name}));
}

void BackgroundImageRenderer::setBackground(const std::string &name) {
    setBackground(findBackground(name)->getId());
}

void BackgroundImageRenderer::setBackground(int id) {

    if (id == 0) {
        return;
    }

    auto *background = getBackground(id);

    if (!background->isLoaded()) {
        loadBackground(id);
    }

    currentBackground = background;
    enableImageDrawing();
}

Background* BackgroundImageRenderer::getBackground(int id) {

    for (auto &background : backgrounds) {
        if (background->getId() == id) {
            return background;
        }
    }

    throw ResourceException(Utils::implodeString({"No background with id ", std::to_string(id), "was found."}));
};

void BackgroundImageRenderer::setUpcomingBackground(const std::string &name) {
    // TODO: Don't use the name as a function parameter - this is poppycock.
    upcomingBackground = findBackground(name);

    if (!upcomingBackground->isLoaded()) {
        loadBackground(upcomingBackground->getId());
    }

    enableImageDrawing();
}

void BackgroundImageRenderer::setBackgroundColour(sf::Color *colour) {

    if (backgroundColour) {
        delete (backgroundColour);
        backgroundColour = nullptr;
    }

    backgroundColour = colour;
}

sf::Color *BackgroundImageRenderer::getBackgroundColour() {

    if (!backgroundColour) {
        return new sf::Color(0, 0, 0, 255);
    }

    return backgroundColour;

}

void BackgroundImageRenderer::disableImageDrawing() {
    drawingEnabled = false;
}

void BackgroundImageRenderer::enableImageDrawing() {
    drawingEnabled = true;
}

bool BackgroundImageRenderer::isDrawingEnabled() {
    return drawingEnabled;
}

/**
 * Fades-out the current background by the given amount on every call
 * Removes the upcoming background and makes that one the current background when alpha of current background becomes 0
 */
void BackgroundImageRenderer::setBackgroundAlpha(int alpha) {

    if (alpha <= 0) {
        drawingEnabled = false;
        currentBackground = upcomingBackground;
        upcomingBackground = nullptr; // Don't delete it as the resource manager can do this
        backgroundAlpha = 255;
        drawingEnabled = true;
    } else {
        backgroundAlpha = alpha;
    }

    if (currentBackground) {
        currentBackground->setAlpha(backgroundAlpha);
    }

};
