#include "Base/Engine.hpp"
#include "VisualNovelEngine/Classes/Data/Novel.hpp"
#include "VisualNovelEngine/Classes/UI/CharacterSpriteRenderer.hpp"

CharacterSpriteRenderer::CharacterSpriteRenderer(ResourceManager *rManager, SpriteRenderer *sRenderer) {
  resourceManager = rManager;
  spriteRenderer = sRenderer;
}

CharacterSpriteRenderer::~CharacterSpriteRenderer() {

}

void CharacterSpriteRenderer::update() {

}

void CharacterSpriteRenderer::draw() {

}

void CharacterSpriteRenderer::initData(NovelData *novelData) {
  novel = novelData;
}
