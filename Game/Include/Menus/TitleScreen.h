//
// Created by Pete on 2019-06-09.
//

#ifndef TALESCRIPTER_TITLESCREEN_H
#define TALESCRIPTER_TITLESCREEN_H

class TitleScreen {
public:
    TitleScreen(sf::RenderWindow *renderWindow, DatabaseConnection *novelDatabase, ResourceManager *rManager);
    ~TitleScreen();
    void start();
    void update(sf::Clock *gameTime);
    void draw();
private:
    sf::RenderWindow *window;
    DatabaseConnection *novelDb;
    ResourceManager *resourceManager;
};

#endif //TALESCRIPTER_TITLESCREEN_H
