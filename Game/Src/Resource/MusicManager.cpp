#include <iostream>
#include "Database/DatabaseConnection.hpp"
#include "Exceptions/ResourceException.hpp"
#include "Resource/MusicManager.hpp"

MusicManager::MusicManager() {

}

MusicManager::~MusicManager() {

  for (auto & currentAudioStream : audioStream) {
    currentAudioStream->stop(true);
    delete currentAudioStream;
  }

}

AudioStream* MusicManager::addStream(std::string name, std::string fname) {

  // Add a stream
  auto *stream = new AudioStream(name, fname);
  audioStream.push_back(stream);
  return stream;
}

/**
 * [playAudioStream creates a play request for an audio stream]
 * @param name [description]
 */
void MusicManager::playAudioStream(std::string name) {
    playAudioStream(name, nullptr);
}

void MusicManager::playAudioStream(std::string name, MusicPlaybackRequestMetadata* metadata) {

    int id = findAudioStream(name);

    if (id < 0) {
        return;
    }

    MusicPlayRequest playRequest(id);
    playRequest.setMetadata(metadata);
    playRequestQueue.push(playRequest);
}

void MusicManager::playAudioStream(int id) {
  // TODO (I don't think this is needed, decide later when it isn't 2am.)
}

/**
 * [update Process any load requests]
 */
void MusicManager::processQueue() {
  if (playRequestQueue.empty()) {
    return;
  }

  // Stop all other audio streams, do nothing if we're trying to re-play the already-playing stream
  for (int i = 0; i < audioStream.size(); i++) {
      if (audioStream[i]->isPlaying()) {

        // Do nothing if we're trying to re-play the stream which is already playing
        if (playRequestQueue.front().getId() == i) {
          playRequestQueue.pop();
          return;
        }

        audioStream[i]->stop(true);
      }
  }

  // Play the stream

  if (!audioStream[playRequestQueue.front().getId()]) {
    std::cout<<"Audio stream error: Stream with ID ("<<playRequestQueue.front().getId()<<") does not exist."<<std::endl;
    playRequestQueue.pop();
    return;
  }

  MusicPlayRequest playRequest = playRequestQueue.front();
  audioStream[playRequest.getId()]->play(&playRequest);

  // Remove the audio load request from memory
  playRequestQueue.pop();
}

bool MusicManager::isQueueEmpty() {
  return (playRequestQueue.empty());
}


AudioStream* MusicManager::getAudioStream(int id) {

  if (id < audioStream.size()) {
    return audioStream[id];
  }

  return nullptr;

}

AudioStream* MusicManager::getAudioStream(std::string name) {

  int id = findAudioStream(name);

  if (id > -1) {
    return audioStream[id];
  }

  return nullptr;

}

int MusicManager::findAudioStream(std::string name) {

  for (int i = 0; i < audioStream.size(); i++) {
      if (audioStream[i]->getName() == name) {
        return i;
      }
  }

  return -1;

}

/**
 * [MusicManager::loadAllFromDatabase Fetch the audio streams and their names from the database]
 * @param database [Database connection]
 */
void MusicManager::loadAllFromDatabase(DatabaseConnection *database) {

  DataSet *result = new DataSet();

  database->executeQuery("SELECT * FROM music", result);

  for (int i = 0; i < result->getRowCount(); i++) {

    // Ignore this entry if either of the columns are missing data
    if (!(result->getRow(i)->doesColumnExist("name") && result->getRow(i)->doesColumnExist("filename"))) {
      continue;
    }

    std::string name = result->getRow(i)->getColumn("name")->getRawData();
    std::string fname = result->getRow(i)->getColumn("filename")->getRawData();

    std::string fullFileName = "resource/music/";
    fullFileName.append(fname);

    addStream(name, fullFileName);
  }

  delete result;

}
