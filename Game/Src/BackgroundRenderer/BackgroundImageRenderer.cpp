#include <iostream>
#include <queue>
#include "SFML/Graphics.hpp"
#include "Database/DatabaseConnection.hpp"
#include "BackgroundRenderer/BackgroundImageRenderer.hpp"

BackgroundImageRenderer::BackgroundImageRenderer(sf::RenderWindow *windowPointer) {
  window = windowPointer;

  for (int i = 0; i < MAX_BACKGROUNDS; i++) {
    background[i] = NULL;
  }

  currentBackground = NULL;
  upcomingBackground = NULL;

}

BackgroundImageRenderer::~BackgroundImageRenderer() {

  for (int i = 0; i < MAX_BACKGROUNDS; i++) {
    if (background[i]) {
      delete background[i];
    }
  }

}

Background* BackgroundImageRenderer::addBackground(std::string name, std::string filename) {

  for (int i = 0; i < MAX_BACKGROUNDS; i++) {
    if (!background[i]) {
      background[i] = new Background(name, filename, window);

      // TODO: Load and delete backgrounds depending on whether they are used in a scene segment or not
      // For now, just load immediately (This is a big waste of VRAM, but dev machine has 6GB of it so who cares right now)
      loadBackground(i);

      return background[i];
    }
  }

  return NULL;

}

/**
 * [processQueue Should be called from another thread - load a background]
 */
void BackgroundImageRenderer::processQueue() {
  // It might be a good idea to load every background used in one scene, and unload them when the scene changes
  if (isQueueEmpty()) {
    return;
  }

  background[backgroundLoadQueue.front().getId()]->load();

  backgroundLoadQueue.pop();
}

bool BackgroundImageRenderer::isQueueEmpty() {
  return (backgroundLoadQueue.size() == 0);
}

void BackgroundImageRenderer::draw() {
  // Possible error that may occur here: currentBackground or upcomingBackground may be null and cause a crash
  // TODO: Do something about this.

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

void BackgroundImageRenderer::update() {
  // TODO: Handle background transitions
}

void BackgroundImageRenderer::addAllFromDatabase(DatabaseConnection *db) {

  DataSet *result = new DataSet();

  db->executeQuery("SELECT * FROM background_images WHERE enabled IS TRUE;", result);

  for (int i = 0; i < result->getRowCount()-1; i++) {

    // Ignore this entry if either of the columns are missing data
    if (!(result->getRow(i)->doesColumnExist("name") && result->getRow(i)->doesColumnExist("filename"))) {
      continue;
    }

    std::string name = result->getRow(i)->getColumn("name")->getData();
    std::string fname = result->getRow(i)->getColumn("filename")->getData();

    std::string fullFileName = "resource\\backgrounds\\";
    fullFileName.append(fname);

    addBackground(name, fullFileName);

  }
}

void BackgroundImageRenderer::loadBackground(int id) {
  backgroundLoadQueue.push(BackgroundLoadRequest(id));
}

int BackgroundImageRenderer::findBackground(std::string name) {

  for (int i = 0; i < MAX_BACKGROUNDS; i++) {
    if (background[i]) {
      if (background[i]->getName() == name) {
        return i;
      }
    }
  }

  return -1;
}

void BackgroundImageRenderer::setBackground(std::string name) {
  int bgId = findBackground(name);

  if (bgId < 0) {
    return;
  }

  // TODO: Handle background transitions
  currentBackground = background[bgId];
}
