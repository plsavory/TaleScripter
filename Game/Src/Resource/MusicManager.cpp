#include <iostream>

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

  // TODO: Stop all other audio streams

  // Play the stream

  if (!audioStream[playRequestQueue.front().getId()]) {
    std::cout<<"Audio stream error: Stream with ID ("<<playRequestQueue.front().getId()<<") does not exist."<<std::endl;
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
