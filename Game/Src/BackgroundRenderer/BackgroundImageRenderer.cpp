#include <iostream>
#include <queue>
#include <sstream>
#include <SFML/Graphics.hpp>
#include "Database/DatabaseConnection.hpp"
#include "BackgroundRenderer/BackgroundImageRenderer.hpp"

BackgroundImageRenderer::BackgroundImageRenderer(sf::RenderWindow *windowPointer) {
  window = windowPointer;

  for (int i = 0; i < MAX_BACKGROUNDS; i++) {
    background[i] = nullptr;
  }

  currentBackground = nullptr;
  upcomingBackground = nullptr;
  backgroundColour = nullptr;
  drawingEnabled = true;

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

  return nullptr;

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
  if (!background[backgroundLoadQueue.front().getId()]) {
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

void BackgroundImageRenderer::update() {
  // TODO: Handle background transitions
}

void BackgroundImageRenderer::addAllFromDatabase(DatabaseConnection *db) {

  DataSet *result = new DataSet();

  db->executeQuery("SELECT * FROM background_images WHERE enabled IS TRUE;", result);

  for (int i = 0; i < result->getRowCount(); i++) {

    // Ignore this entry if either of the columns are missing data
    if (!(result->getRow(i)->doesColumnExist("name") && result->getRow(i)->doesColumnExist("filename"))) {
      continue;
    }

    std::string name = result->getRow(i)->getColumn("name")->getData();
    std::string fname = result->getRow(i)->getColumn("filename")->getData();
    std::string id = result->getRow(i)->getColumn("id")->getData();

    std::string fullFileName = "resource/backgrounds/";
    fullFileName.append(fname);

    Background *addedBackground = addBackground(name, fullFileName);

    if (!addedBackground) {
      return;
    }

    // If we have loaded a background, see if the background has any attributes in the database and apply them
    DataSet *attributeResult = new DataSet();
    std::ostringstream ss;
    ss << "SELECT * FROM background_image_attributes WHERE background_image_id = " << id << " ORDER BY id DESC LIMIT 1;";
    db->executeQuery(ss.str(), attributeResult);

    if (attributeResult->getRowCount() == 1) {

      float maxWidth = 0;
      float maxHeight = 0;
      float offsetLeft = 0;
      float offsetTop = 0;

      if (attributeResult->getRow(0)->doesColumnExist("max_width")) {
        maxWidth = std::atof(attributeResult->getRow(0)->getColumn("max_width")->getData().c_str());
      }

      if (attributeResult->getRow(0)->doesColumnExist("max_height")) {
        maxHeight = std::atof(attributeResult->getRow(0)->getColumn("max_height")->getData().c_str());
      }

      if (attributeResult->getRow(0)->doesColumnExist("offset_left")) {
        offsetLeft = std::atof(attributeResult->getRow(0)->getColumn("offset_left")->getData().c_str());
      }

      if (attributeResult->getRow(0)->doesColumnExist("offset_top")) {
        offsetTop = std::atof(attributeResult->getRow(0)->getColumn("offset_top")->getData().c_str());
      }

      addedBackground->setAttributes(new BackgroundImageAttributes(maxWidth, maxHeight, offsetLeft, offsetTop));
    }

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

  // TODO: Handle background fade transitions
  currentBackground = background[bgId];
  enableImageDrawing();
}

void BackgroundImageRenderer::setBackgroundColour(sf::Color *colour) {

  if (backgroundColour) {
    delete(backgroundColour);
    backgroundColour = nullptr;
  }

  backgroundColour = colour;
}

sf::Color* BackgroundImageRenderer::getBackgroundColour() {

  if (!backgroundColour) {
    return new sf::Color(0,0,0,255);
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
