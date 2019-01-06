#include <iostream>
#include "Database/DatabaseConnection.hpp"
#include "Resource/MusicManager.hpp"

MusicManager::MusicManager() {
  // Null-out the audio audio streams
  for (int i = 0; i < MAX_AUDIO_STREAMS; i++) {
    audioStream[i] = NULL;
  }
}

MusicManager::~MusicManager() {

  for (int i = 0; i < MAX_AUDIO_STREAMS; i++) {
    if (audioStream[i]) {
      delete audioStream[i];
    }
  }

}

AudioStream* MusicManager::addStream(std::string name, std::string fname) {

  for (int i = 0; i < MAX_AUDIO_STREAMS; i++) {
    if (!audioStream[i]) {
      // Empty slot found, add a stream
      audioStream[i] = new AudioStream(name, fname);
      return audioStream[i];
    }
  }

  return NULL;
}

/**
 * [playAudioStream creates a play request for an audio stream]
 * @param name [description]
 */
void MusicManager::playAudioStream(std::string name) {

  int id = findAudioStream(name);

  if (id < 0) {
    return;
  }

  playRequestQueue.push(PlayRequest(id));
}

void MusicManager::playAudioStream(int id) {
  // TODO (I don't think this is needed, decide later when it isn't 2am.)
}

void MusicManager::addAllStreamsFromDatabase() {
  // TODO
}

/**
 * [update Process any load requests]
 */
void MusicManager::processQueue() {
  if (playRequestQueue.size() == 0) {
    return;
  }

  // Stop all other audio streams, do nothing if we're trying to re-play the already-playing stream
  for (int i = 0; i < MAX_AUDIO_STREAMS; i++) {
    if (audioStream[i]) {
      if (audioStream[i]->isPlaying()) {

        // Do nothing if we're trying to re-play the stream which is already playing
        if (playRequestQueue.front().getId() == i) {
          playRequestQueue.pop();
          return;
        }

        audioStream[i]->stop(true);
      }
    }
  }

  // Play the stream

  if (!audioStream[playRequestQueue.front().getId()]) {
    std::cout<<"Audio stream error: Stream with ID ("<<playRequestQueue.front().getId()<<") does not exist."<<std::endl;
    playRequestQueue.pop();
    return;
  }

  audioStream[playRequestQueue.front().getId()]->play();

  // Remove the audio load request from memory
  playRequestQueue.pop();
}

bool MusicManager::isQueueEmpty() {
  return (playRequestQueue.size() == 0);
}


AudioStream* MusicManager::getAudioStream(int id) {

  if (audioStream[id]) {
    return audioStream[id];
  }

  return NULL;

}

AudioStream* MusicManager::getAudioStream(std::string name) {

  int id = findAudioStream(name);

  if (id > -1) {
    return audioStream[id];
  }

  return NULL;

}

int MusicManager::findAudioStream(std::string name) {

  for (int i = 0; i < MAX_AUDIO_STREAMS; i++) {
    if (audioStream[i]) {
      if (audioStream[i]->getName() == name) {
        return i;
      }
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

  for (int i = 0; i < result->getRowCount()-1; i++) {

    // Ignore this entry if either of the columns are missing data
    if (!(result->getRow(i)->doesColumnExist("name") && result->getRow(i)->doesColumnExist("filename"))) {
      continue;
    }

    std::string name = result->getRow(i)->getColumn("name")->getData();
    std::string fname = result->getRow(i)->getColumn("filename")->getData();

    std::string fullFileName = "resource\\music\\";
    fullFileName.append(fname);

    addStream(name, fullFileName);
  }

  delete result;

}
