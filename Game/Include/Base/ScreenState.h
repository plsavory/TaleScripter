//
// Created by Pete on 2019-06-09.
// A header-only class for managing which screen the engine should currently be drawing
// The state defined in this class overrides the entire screen, it shouldn't be used for example to display a game save menu during the novel

#ifndef TALESCRIPTER_SCREENSTATE_H
#define TALESCRIPTER_SCREENSTATE_H

class ScreenState {
public:
    ScreenState() {
        currentState = STATE_INIT;
        previousState = currentState;
        upcomingState = STATE_NONE;
    }

    void changeState(int newState) {
        upcomingState = newState;
    };

    int getCurrentState() {
        return currentState;
    }

    int getPreviousState() {
        return previousState;
    }

    int getUpcomingState() {
        return upcomingState;
    }

    void update() {
        currentState = upcomingState;
    }

    /**
     * Returns true if the state of the game has changed. Sets previousState to currentState when called.
     * @return
     */
    bool hasStateChanged() {
        int previousStateBeforeReset = upcomingState;
        previousState = currentState;
        return previousStateBeforeReset != currentState;
    }

    // Constants being used to refer to each state
    static const int STATE_INIT = 0;
    static const int STATE_TITLE = 1;
    static const int STATE_OPTIONS = 2;
    static const int STATE_OPENING_VIDEO = 3;
    static const int STATE_NOVEL = 4;
    static const int STATE_CREDITS = 5;
    static const int STATE_SOUNDTRACK = 6;
    static const int STATE_ART_GALLERY = 7;
    static const int STATE_ERROR = 8;
    static const int STATE_NONE = 9;

private:
    int currentState;
    int previousState;
    int upcomingState;
};

#endif //TALESCRIPTER_SCREENSTATE_H
