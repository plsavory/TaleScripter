#ifndef CHARACTER_SPRITE_SLOT_INCLUDED
#define CHARACTER_SPRITE_SLOT_INCLUDED

class CharacterSpriteSlot {
public:
    CharacterSpriteSlot(SpriteRenderer *sRenderer, ResourceManager *rManager, int myId);

    ~CharacterSpriteSlot();

    void update();

    void push(CharacterSpriteDrawRequest *drawRequest);

    Sprite *getSprite(int id);

    bool isDoingNothing() {
        return updateState == UPDATE_STATE_NOTHING;
    }

private:
    Sprite *sprite[2]{}; // We need 2 of them for fade effects.
    SpriteRenderer *spriteRenderer;
    int id;
    int previousCharacterId;
    int currentCharacterId;
    int alpha[2];
    static const int UPDATE_STATE_MORPHING = 2;
    static const int UPDATE_STATE_FADING = 1;
    static const int UPDATE_STATE_NOTHING = 0;
    int updateState;
    bool spritesHaveFlipped; // Set to true when the sprites have been priority-flipped during a morph transition

    void fadeUpdate();

    void morphUpdate();

    /**
     *
     * @param transitionType
     * @param drawRequest
     * @return - The sprite object belonging to the new character which has been pushed
     */
    Sprite* startTransition(int transitionType, CharacterSpriteDrawRequest *drawRequest);

    sf::Clock *timer;
    int fadingOut;

    void setAlpha(int id, float alphaValue) {

        // Ensure that the values are in range
        if (alphaValue < 0) {
            alphaValue = 0;
        }

        if (alphaValue > 255) {
            alphaValue = 255;
        }

        alpha[id] = alphaValue;

        if (!sprite[id]) {
            return;
        }

        sf::Color colour = sprite[id]->getSfmlSprite()->getColor();
        colour.a = alpha[id];
        sprite[id]->setColour(colour);

    }
};

#endif
