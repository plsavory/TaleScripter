#ifndef MUSIC_PLAY_REQUEST_METADATA_MODEL_INCLUDED
#define MUSIC_PLAY_REQUEST_METADATA_MODEL_INCLUDED

class MusicPlaybackRequestMetadata {
public:
    explicit MusicPlaybackRequestMetadata(DataSet *data);
    int getId() {
        return id;
    }
    bool shouldLoop() {
        return loop;
    }

    float getPitch() {
        return pitch;
    }

    float getVolume() {
        return float(volume);
    }
private:
    int id;
    float pitch;
    int volume;
    int startInMilliseconds;
    int endInMilliseconds;
    bool loop;
};

#endif