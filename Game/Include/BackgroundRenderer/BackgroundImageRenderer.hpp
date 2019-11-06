#ifndef BACKGROUND_IMAGE_RENDERER_INCLUDED
#define BACKGROUND_IMAGE_RENDERER_INCLUDED

#define MAX_BACKGROUNDS 50

#include <queue>

enum BackgroundStatus {
    bgLoaded, bgUnloaded, bgError
};

#include <cmath>
#include <ResourceException.hpp>

class BackgroundLayer {
public:

    BackgroundLayer(sf::RenderWindow *window) {
        this->window = window;
        texture = nullptr;
        sprite = nullptr;
        tilingEnabled = false;

        // Set the default position to be in the middle of the screen.
        position.x = window->getSize().x / 2;
        position.y = window->getSize().y / 2;

        horizontalScrollSpeed = 0;
        verticalScrollSpeed = 0;
        xOffset = 0;
        yOffset = 0;

    }

    ~BackgroundLayer() {

        if (sprite) {
            delete (sprite);
        }

    }

    void update(sf::Clock *gameTime) {
        // TODO: Handle background animation frame switching here

        sf::FloatRect textureRect = this->sprite->getGlobalBounds();

        xOffset += horizontalScrollSpeed;
        yOffset += verticalScrollSpeed;

        if (xOffset > textureRect.width) {
            xOffset = 0;
        }

        if (xOffset < -textureRect.width) {
            xOffset = 0;
        }

        if (yOffset > textureRect.height) {
            yOffset = 0;
        }

        if (yOffset < -textureRect.height) {
            yOffset = 0;
        }
    }

    void draw() {

        // If tiling is not enabled, simply draw the sprite in its standard position - the quickest mode...
        if (!tilingEnabled) {
            this->sprite->setPosition(this->xOffset, this->yOffset);
            window->draw(*this->sprite);
            return;
        }

        sf::Vector2i textureSize = sf::Vector2i(this->sprite->getGlobalBounds().width,
                                                this->sprite->getGlobalBounds().height);

        // Calculate how many times we need to repeat the texture, it should overlap the edges of the screen for smooth scrolling.
        int requiredXRepeats = ceil((float) window->getSize().x / textureSize.x);
        requiredXRepeats += ((window->getSize().x % textureSize.x) || this->xOffset < 0 ? 1 : 0);

        int requiredYRepeats = ceil((float) window->getSize().y / textureSize.y);
        requiredYRepeats += ((window->getSize().y % textureSize.y) || this->yOffset < 0 ? 1 : 0);

        // Draw each repetition, starting at the left-side of the screen to deal with scroll overlapping.
        int startTileX = requiredXRepeats == 1 && this->xOffset <= 0 ? 0 : -1;
        int startTileY = requiredYRepeats == 1 && this->yOffset <= 0 ? 0 : -1;

        for (int iY = startTileY; iY < requiredYRepeats; iY++) {
            for (int iX = startTileX; iX < requiredXRepeats; iX++) {
                int xPosition = (int) (xOffset + ((float) textureSize.x * (float) iX));
                int yPosition = (int) (yOffset + ((float) textureSize.y * (float) iY));

                this->sprite->setPosition((float) xPosition, (float) yPosition);
                this->window->draw(*this->sprite);
            }
        }


    }

    void setTexture(std::string textureFileName) {
        this->textureFileName = textureFileName;
    }

    void enableTiling() {
        this->tilingEnabled = true;
    }

    void setHorizontalScrollSpeed(float speed) {
        horizontalScrollSpeed = speed;
    }

    void setVerticalScrollSpeed(float speed) {
        verticalScrollSpeed = speed;
    }

    void setScale(sf::Vector2f scale) {
        this->sprite->setScale(scale);
    }

    // Actually load the texture into memory
    void load() {

        if (texture) {
            unload();
        }

        texture = new sf::Texture();

        if (!texture->loadFromFile(textureFileName)) {
            throw ResourceException(
                    Utils::implodeString({"Unable to load background texture (", textureFileName, ")"}));
        }

        if (!this->sprite) {
            this->sprite = new sf::Sprite();
            this->sprite->setTexture(*texture);
            sprite->setPosition(position.x, position.y);
        }

    }

    void unload() {
        delete (texture);
        texture = nullptr;
    }

    void setAlpha(float alpha) {

        if (isLoaded()) {
            sf::Color color = sprite->getColor();
            sprite->setColor(sf::Color(color.r, color.g, color.b, alpha));
        }
    }

    bool isLoaded() {
        return texture != nullptr;
    }

    void fitToSize(float width, float height) {
        this->sprite->setScale(width/texture->getSize().x,height/texture->getSize().y);
    }

private:

    sf::RenderWindow *window;

    sf::Texture *texture;

    sf::Sprite *sprite;

    bool tilingEnabled;

    sf::Vector2u position;

    float verticalScrollSpeed;

    float horizontalScrollSpeed;

    float xOffset;

    float yOffset;

    std::string textureFileName;
};

class BackgroundImageAttributes {
public:
    BackgroundImageAttributes(float mWidth, float mHeight, float oLeft, float oTop) {
        maxHeight = mHeight;
        maxWidth = mWidth;
        offsetLeft = oLeft;
        offsetTop = oTop;
    }

    float getMaxHeight() {
        return maxHeight;
    }

    float getMaxWidth() {
        return maxWidth;
    }

    float getOffsetLeft() {
        return offsetLeft;
    }

    float getOffsetTop() {
        return offsetTop;
    }

private:
    float maxHeight;
    float maxWidth;
    float offsetLeft;
    float offsetTop;
};

class Background {
public:
    Background(std::string bName, std::string bFilename, sf::RenderWindow *windowPointer) {
        id = 0; // TODO: fix;
        name = bName;
        myStatus = BackgroundStatus::bgUnloaded;
        window = windowPointer;
        addLayer(bFilename);
    }

    ~Background() {
        for (auto &layer : layers) {
            delete(layer);
        }
    }

    void load() {

        for (auto &layer : layers) {
            layer->load();

            if (attributes) {
                // Apply the attributes onto the sprite object (Scale)
                layer->fitToSize(attributes->getMaxWidth(), attributes->getMaxHeight());
            }
        }
        myStatus = BackgroundStatus::bgLoaded;
    }

    BackgroundStatus getStatus() {
        return myStatus;
    }

    void draw() {
        for (auto &layer : layers) {
            layer->draw();
        }
    }

    std::string getName() {
        return name;
    }

    void setAlpha(float alpha) {
        for (auto &layer : layers) {
            layer->setAlpha(alpha);
        }
    }

    void setAttributes(BackgroundImageAttributes *a) {
        if (a) {
            attributes = a;
        }
    }

    BackgroundLayer* addLayer(const std::string& fileName) {
        auto *newLayer = new BackgroundLayer(window);
        newLayer->setTexture(fileName);
        layers.push_back(newLayer);
        return newLayer;
    }

private:
    std::string name;
    std::string fileName;
    int id;
    BackgroundStatus myStatus;
    sf::RenderWindow *window;
    BackgroundImageAttributes *attributes = nullptr;
    std::vector<BackgroundLayer *> layers;
};

class BackgroundLoadRequest {
public:
    BackgroundLoadRequest(int backgroundId) {
        id = backgroundId;
    }

    ~BackgroundLoadRequest() {

    }

    int getId() {
        return id;
    }

private:
    int id;
};

class BackgroundImageRenderer {
public:
    BackgroundImageRenderer(sf::RenderWindow *windowPointer);

    ~BackgroundImageRenderer();

    void addAllFromDatabase(DatabaseConnection *db);

    void processQueue();

    bool isQueueEmpty();

    void update();

    void draw();

    void setBackground(std::string name);

    void setBackground(int id);

    void setUpcomingBackground(std::string name);

    int findBackground(std::string name);

    void setBackgroundColour(sf::Color *colour);

    void disableImageDrawing();

    void enableImageDrawing();

    bool isDrawingEnabled();

    sf::Color *getBackgroundColour();

    void setBackgroundAlpha(int alpha);

private:
    sf::RenderWindow *window;

    Background *addBackground(std::string name, std::string filename);

    Background *background[MAX_BACKGROUNDS];
    std::queue<BackgroundLoadRequest> backgroundLoadQueue;
    Background *currentBackground;
    Background *upcomingBackground; // Used for transitions
    void loadBackground(int id);

    sf::Color *backgroundColour;
    bool drawingEnabled;
    int backgroundAlpha;
};

#endif
