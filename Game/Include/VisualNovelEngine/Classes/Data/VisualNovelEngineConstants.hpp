//
// Created by Pete on 2019-04-14.
//
// This class contains a bunch of constants which are unique to the visual novel engine
// More general engine constants should instead reside in EngineConstants.hpp
//

#ifndef TALESCRIPTER_VISUAL_NOVEL_ENGINE_CONSTANTS
#define TALESCRIPTER_VISUAL_NOVEL_ENGINE_CONSTANTS

class BackgroundTransitionType {
public:
    static const int BACKGROUND_TRANSITION_TYPE_INSTANT = 0;
    static const int BACKGROUND_TRANSITION_TYPE_CUSTOM = 1;
    static const int BACKGROUND_TRANSITION_TYPE_FADE = 2;
    static const int BACKGROUND_TRANSITION_TYPE_MORPH = 3;
};

#endif
