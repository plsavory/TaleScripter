#ifndef MUSIC_PLAY_REQUEST_INCLUDED
#define MUSIC_PLAY_REQUEST_INCLUDED

struct MusicPlayRequest {
public:
    MusicPlayRequest(int id) {
        streamId = id;
        loop = false;
        metadata = nullptr;
    };

    int getId() {
        return streamId;
    };
    void setLooping(bool shouldLoop) {
        loop = shouldLoop;
    }
    bool shouldLoop() {
        return loop;
    }

    void setMetadata(MusicPlaybackRequestMetadata *pMetadata) {
        metadata = pMetadata;
    }

    MusicPlaybackRequestMetadata *getMetadata() {
        return metadata;
    }
private:
    int streamId;
    bool loop;
    MusicPlaybackRequestMetadata *metadata;
};

#endif