/**
 * [MusicManager This class manages audio streams.
 * These can be either added programatically, or all assigned at once through the resource Database
 * Only one stream can play at any given time.]
 */

#ifndef AUDIO_STREAM_INCLUDED
#define AUDIO_STREAM_INCLUDED

#include <SFML/Audio.hpp>
#include <queue>
#include "VisualNovelEngine/Classes/Data/DataModels/MusicPlaybackRequestMetadata.hpp"
#include "Resource/MusicPlayRequest.hpp"

enum AudioStreamState {
    Unloaded, Stopped, Playing, Paused, Error
};

struct AudioStream {
public:
    AudioStream(const std::string& asName, const std::string& asFname) {
        music = nullptr;
        state = AudioStreamState::Unloaded;
        name = asName;
        fileName = asFname;

#ifdef DEBUG_AUDIO_STREAM
        std::cout<<"Audio stream added, name: "<<name<<" filename: "<<fileName<<std::endl;
#endif

        if (!Utils::fileExists(asFname)) {
            std::vector<std::string> errorMessage = {
                    "Unable to add audio stream with name '", name, "' (File '", fileName, "' does not exist)"
            };

            throw ResourceException(Utils::implodeString(errorMessage));
        }
    };

    ~AudioStream() {
        delete music;
    };

    std::string getName() {
        return name;
    };

    std::string getFileName() {
        return fileName;
    };

    sf::Music *getMusicObject() {
        return music;
    };

    void play() {
        play(nullptr);
    }

    void play(MusicPlayRequest *musicPlayRequest) {

        // File hasn't been loaded
        if (!music) {
            music = new sf::Music();
            if (!music->openFromFile(fileName)) {
                std::vector<std::string> errorMessage = {
                        "Unable to play audio stream '", name, "'. The file may be corrupted or in the wrong format."
                };

                throw ResourceException(Utils::implodeString(errorMessage));
            }
        }

        // Set values from the music play request if we need to
        if (musicPlayRequest) {
            MusicPlaybackRequestMetadata *metadata = musicPlayRequest->getMetadata();

            if (metadata) {
                // We have values to set on to the music
                music->setLoop(metadata->shouldLoop());
                music->setPitch(metadata->getPitch());
                music->setVolume(metadata->getVolume());
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

    AudioStream *addStream(std::string name, std::string fname);

    void addAllStreamsFromDatabase();

    void processQueue();

    bool isQueueEmpty();

    AudioStream *getAudioStream(int id);

    AudioStream *getAudioStream(std::string name);

    void playAudioStream(int id);

    void playAudioStream(std::string name);

    void playAudioStream(std::string name, MusicPlaybackRequestMetadata* metadata);

    int findAudioStream(std::string name);

    void loadAllFromDatabase(DatabaseConnection *database);

private:
    std::vector<AudioStream*> audioStream;
    std::queue<MusicPlayRequest> playRequestQueue;
};

#endif
