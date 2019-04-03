#ifndef CHAPTER_BUILDER_INCLUDED
#define CHAPTER_BUILDER_INCLUDED

class ChapterBuilder {
public:
    ChapterBuilder(const std::string &fileName, DatabaseConnection *novelDb, FileHandler *fileHandler);

    ~ChapterBuilder();

    void process();

private:
    DatabaseConnection *novel;
    std::string chapterFileName;

    void processScene(json sceneJson, int chapterId);

    void processSceneSegment(json sceneSegmentJson, int sceneId);

    void processLine(json lineJson, int sceneSegmentId);

    FileHandler *fHandler;
};

#endif