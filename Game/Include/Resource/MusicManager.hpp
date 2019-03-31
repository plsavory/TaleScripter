/**
 * [MusicManager This class manages audio streams.
 * These can be either added programatically, or all assigned at once through the resource Database
 * Only one stream can play at any given time.]
 */

#ifndef AUDIO_STREAM_INCLUDED
#define AUDIO_STREAM_INCLUDED

#include <SFML/Audio.hpp>
#include <queue>

#define MAX_AUDIO_STREAMS 50

enum AudioStreamState {Unloaded, Stopped, Playing, Paused, Error};

struct PlayRequest {
public:
  PlayRequest(int id) {
    streamId = id;
  };
  int getId() {
    return streamId;
  };
private:
  int streamId;
};

struct AudioStream {
public:
  AudioStream(std::string asName, std::string asFname) {
    music = nullptr;
    state = AudioStreamState::Unloaded;
    name = asName;
    fileName = asFname;

    #ifdef DEBUG_AUDIO_STREAM
      std::cout<<"Audio stream added, name: "<<name<<" filename: "<<fileName<<std::endl;
    #endif
  };

  ~AudioStream() {
    if (music) {
      delete music;
    }
  };

  std::string getName() {
    return name;
  };

  std::string getFileName() {
    return fileName;
  };

  sf::Music* getMusicObject() {
    return music;
  };

  void play() {

    // File hasn't been loaded
    if (!music) {
      music = new sf::Music();
      if (!music->openFromFile(fileName)) {
        state = AudioStreamState::Error;
        return;
      }
    }

    music->play();
    state = AudioStreamState::Playing;

  };

  // Stop music from playing and destroy the stream
  void stop(bool unload) {
    if (music) {
      music->stop();

      if (unload) {
        delete music;
        music = nullptr;
        state = AudioStreamState::Unloaded;
        return;
      }

      state = AudioStreamState::Stopped;
    }
  };

  void pause() {
    if (music) {
      music->pause();
      state = AudioStreamState::Paused;
    }
  };

  bool isPlaying() {
    return state == AudioStreamState::Playing;
  }
private:
  std::string name;
  std::string fileName;
  sf::Music *music;
  AudioStreamState state;
};

class MusicManager {
public:
  MusicManager();
  ~MusicManager();
  AudioStream* addStream(std::string name, std::string fname);
  void addAllStreamsFromDatabase();
  void processQueue();
  bool isQueueEmpty();
  AudioStream* getAudioStream(int id);
  AudioStream* getAudioStream(std::string name);
  void playAudioStream(int id);
  void playAudioStream(std::string name);
  int findAudioStream(std::string name);
  void loadAllFromDatabase(DatabaseConnection *database);
private:
  AudioStream *audioStream[MAX_AUDIO_STREAMS];
  std::queue<PlayRequest> playRequestQueue;
};

#endif
