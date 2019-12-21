#ifndef BACKGROUND_IMAGE_RENDERER_INCLUDED
#define BACKGROUND_IMAGE_RENDERER_INCLUDED

#define MAX_BACKGROUNDS 50

#include <queue>

enum BackgroundStatus {
    bgLoaded, bgUnloaded, bgError
};

#include <cmath>
#include <ResourceException.hpp>

class BackgroundTexture {
public:
    explicit BackgroundTexture(DataSetRow *backgroundTextureRow) {
        id = backgroundTextureRow->getColumn("id")->getData()->asInteger();
        name = backgroundTextureRow->getColumn("name")->getData()->asString();
        fileName = backgroundTextureRow->getColumn("filename")->getData()->asString();
        texture = nullptr;
    }

    ~BackgroundTexture() {
        unload();
    }

    bool isLoaded() {
        return texture != nullptr;
    }

    void load() {

        if (texture) {
            unload();
        }

        texture = new sf::Texture();

        std::string fullFileName = "resource/backgrounds/";
        fullFileName.append(fileName);

        if (!texture->loadFromFile(fullFileName)) {
            throw ResourceException(
                    Utils::implodeString({"Unable to load background texture (", fileName, ")"}));
        }

    }

    void unload() {
        if (texture) {
            delete (texture);
        }
    }

    int getId() {
        return id;
    }

    sf::Texture* getTexture() {
        return texture;
    }

private:
    int id;

    std::string name;

    std::string fileName;

    sf::Texture *texture;
};

class BackgroundTextureManager {
public:
    BackgroundTextureManager() = default;

    ~BackgroundTextureManager() {

        for (auto &texture: textures) {
            delete (texture);
        }

    }

    /**
     * Load all of the background texture information from the database
     * This does not actually load the textures into memory - only the information about them.
     * @param db - Resource database connection
     */
    void loadAllFromDatabase(DatabaseConnection *db) {
        // TODO: Also ensure that we're not doing this twice, I think we have two BackgroundImageRenderers... (One for foregrounds)

        this->resourceDb = db;

        auto *dataSet = new DataSet();
        resourceDb->executeQuery("SELECT * FROM background_textures", dataSet);

        for (auto &row : dataSet->getRows()) {
            // Add a new background texture for every row.
            auto *newTexture = new BackgroundTexture(row);
            textures.push_back(newTexture);
        }

        delete (dataSet);
    }

    BackgroundTexture *getBackgroundTexture(int id) {
        for (auto &texture : textures) {
            if (texture->getId() == id) {
                return texture;
            }
        }

        throw ResourceException(Utils::implodeString({"No background texture with id ", std::to_string(id), " exists."}));
    }

private:

    std::vector<BackgroundTexture *> textures;

    DatabaseConnection *resourceDb;
};

class BackgroundLayer {
public:

    BackgroundLayer(sf::RenderWindow *window) {
        this->window = window;
        texture = nullptr;
        this->sprite = nullptr;
        tilingEnabled = false;

        // Set the default position to be in the middle of the screen.
        position.x = window->getSize().x / 2;
        position.y = window->getSize().y / 2;

        horizontalScrollSpeed = 0;
        verticalScrollSpeed = 0;
        xOffset = 0;
        yOffset = 0;
        tilingEnabled = false;

    }

    /**
     * Constructor
     * @param window - A pointer to a SFML window
     * @param row - A row from the background_layers table
     * @param bgTextureManager - A pointer to a BackgroundTextureManager object
     */
    BackgroundLayer(sf::RenderWindow *window, DataSetRow *row, BackgroundTextureManager *bgTextureManager) {
        backgroundTexture = nullptr;
        this->window = window;
        this->sprite = nullptr;

        id = row->getColumn("id")->getData()->asInteger();

        // Ensure that the image exists and set it...
        backgroundTexture = bgTextureManager->getBackgroundTexture(row->getColumn("background_texture_id")->getData()->asInteger());

        // Load the appropriate background attributes from the database and store them
        xOffset = (float)row->getColumn("offset_left")->getData()->asInteger();
        yOffset = (float)row->getColumn("offset_top")->getData()->asInteger();
        tilingEnabled = row->getColumn("is_tiled")->getData()->asBoolean();

        maxWidth = row->getColumn("max_width")->getData()->asInteger();
        maxHeight = row->getColumn("max_height")->getData()->asInteger();

        horizontalScrollSpeed = row->getColumn("horizontal_scroll_speed")->getData()->asFloat();
        verticalScrollSpeed = row->getColumn("vertical_scroll_speed")->getData()->asFloat();
    }

    ~BackgroundLayer() {

        if (sprite) {
            delete (sprite);
        }

    }

    void update(sf::Clock *gameTime) {
        // TODO: Handle background animation frame switching here
        if (!this->isLoaded()) {
            return;
        }

        // Handle background scrolling and wrapping
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

        if (!this->sprite) {
            return; // Can't draw if we've not loaded an image yet...
        }

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

    void setTexture(BackgroundTexture *texture) {
        this->backgroundTexture = texture;
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

        // Load the texture if we need to
        if (!backgroundTexture) {
            throw ResourceException("No background texture has been assigned to a background layer");
        }

        if (!backgroundTexture->isLoaded()) {
            backgroundTexture->load();
        }

        if (!this->sprite) {
            this->sprite = new sf::Sprite();
            this->sprite->setTexture(*backgroundTexture->getTexture());
            sprite->setPosition(position.x, position.y);
            fitToSize((float)maxWidth, (float)maxHeight);
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
        return backgroundTexture != nullptr && backgroundTexture->isLoaded() &&
        sprite != nullptr;
    }

    void fitToSize(float width, float height) {
        if (width == 0 || height == 0) {
            return;
        }

        this->sprite->setScale(width / backgroundTexture->getTexture()->getSize().x, height / backgroundTexture->getTexture()->getSize().y);
    }

private:

    sf::RenderWindow *window;

    sf::Texture *texture;

    sf::Sprite *sprite;

    int id;

    bool tilingEnabled;

    sf::Vector2u position;

    float verticalScrollSpeed;

    float horizontalScrollSpeed;

    float xOffset;

    float yOffset;

    BackgroundTexture *backgroundTexture;

    int maxWidth;

    int maxHeight;
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
    Background(DataSetRow *row, sf::RenderWindow *windowPointer, DatabaseConnection *resourceDb,
               BackgroundTextureManager *bgTextureManager) {
        id = row->getColumn("id")->getData()->asInteger();
        name = row->getColumn("name")->getData()->asString();
        myStatus = BackgroundStatus::bgUnloaded;
        window = windowPointer;

        // Fetch all of the background layer information and store it
        auto *dataSet = new DataSet();

        resourceDb->execute(Utils::implodeString({
                                                         "SELECT *",
                                                         "FROM background_layers",
                                                         "WHERE background_id = ?",
                                                         "ORDER BY depth DESC"
                                                 }, " "), dataSet, {std::to_string(id)}, {DatabaseConnection::TYPE_INT});

        for (auto &layerRow : dataSet->getRows()) {
            addLayer(layerRow, bgTextureManager);
        }

        delete (dataSet);
    }

    ~Background() {
        for (auto &layer : layers) {
            delete (layer);
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

    void update(sf::Clock *gameTime) {
        for (auto &layer : layers) {
            layer->update(gameTime);
        }
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

    /**
     * Adds a layer using a filename
     * @param fileName
     * @return
     */
    BackgroundLayer *addLayer(const std::string &fileName) {
        auto *newLayer = new BackgroundLayer(window);
        layers.push_back(newLayer);
        return newLayer;
    }

    /**
     * Adds a background layer
     * @param row - A row from the background_layers table
     * @param bgTextureManager - A pointer to a BackgroundTextureManager
     * @return - The newly-created layer
     */
    BackgroundLayer *addLayer(DataSetRow *row, BackgroundTextureManager *bgTextureManager) {
        auto *newLayer = new BackgroundLayer(window, row, bgTextureManager);
        layers.push_back(newLayer);
        return newLayer;
    };

    int getId() {
        return id;
    }

    /**
     * Will return false if any of the layers are not yet loaded
     * @return
     */
    bool isLoaded() {
        for (auto &layer : layers) {
            if (!layer->isLoaded()) {
                return false;
            }
        }

        return true;
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

    void update(sf::Clock *gameTime);

    void draw();

    void setBackground(const std::string &name);

    void setBackground(int id);

    void setUpcomingBackground(const std::string &name);

    Background* findBackground(const std::string &name);

    void setBackgroundColour(sf::Color *colour);

    void disableImageDrawing();

    void enableImageDrawing();

    bool isDrawingEnabled();

    sf::Color *getBackgroundColour();

    void setBackgroundAlpha(int alpha);

    Background* getBackground(int id);

private:
    sf::RenderWindow *window;

    std::vector<Background *> backgrounds;
    std::queue<BackgroundLoadRequest> backgroundLoadQueue;
    Background *currentBackground;
    Background *upcomingBackground; // Used for transitions
    void loadBackground(int id);

    sf::Color *backgroundColour;
    bool drawingEnabled;
    int backgroundAlpha;

    BackgroundTextureManager *backgroundTextureManager;

};

#endif
