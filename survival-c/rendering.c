#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "rendering.h"
#include "error.h"
#include "game.h"

void displayGameOver(SDL_Renderer* renderer) {
    TTF_Font* font = TTF_OpenFont("AntonSC-Regular.ttf", 64);
    if (!font) {
        error("Failed to load font");
    }

    SDL_Color textColor = {255, 0, 0, 255};  // Red color
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, "Game Over", textColor);
    if (!surfaceMessage) {
        error("Failed to render text");
    }

    SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    if (!message) {
        error("Failed to create texture from rendered text");
    }

    int textWidth = surfaceMessage->w;
    int textHeight = surfaceMessage->h;
    SDL_Rect messageRect = {
        (WINDOW_WIDTH - textWidth) / 2,
        (WINDOW_HEIGHT - textHeight) / 2,
        textWidth,
        textHeight
    };

    SDL_RenderCopy(renderer, message, NULL, &messageRect);
    SDL_RenderPresent(renderer);

    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(message);
    TTF_CloseFont(font);

    SDL_Delay(3000);
}

void renderObject(SDL_Renderer* renderer, Object* obj) {
    SDL_Rect rect = {obj->position.x, obj->position.y, obj->width, obj->height};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Red color
    SDL_RenderFillRect(renderer, &rect);
}

SDL_Rect spriteClips[ANIMATION_FRAMES];
SDL_Rect enemyClips[ENEMY_ANIMATION_FRAMES];

SDL_Texture *loadEnemySpritesheet(SDL_Renderer *renderer, const char *path) {
  SDL_Surface* loadedSurface = IMG_Load(path);
  if (loadedSurface == NULL) {
    error(IMG_GetError());
  }

  SDL_Texture* spritesheet = SDL_CreateTextureFromSurface(renderer, loadedSurface);
  if (spritesheet == NULL) {
    error(SDL_GetError());
  }

  SDL_FreeSurface(loadedSurface);

  for (int i = 0; i < ENEMY_ANIMATION_FRAMES; i++) {
      int row = i / ENEMY_ROWS;
      int col = i % ENEMY_COLS;
      
      // -5 is a pixel error on the sheet
      enemyClips[i].x = (col * ENEMY_WIDTH) - 5;
      enemyClips[i].y = row * ENEMY_HEIGHT;
      enemyClips[i].w = ENEMY_WIDTH;
      enemyClips[i].h = ENEMY_HEIGHT;
    }
    return spritesheet;
}

SDL_Texture* loadSpritesheet(SDL_Renderer* renderer, const char* path) {
    SDL_Surface* loadedSurface = IMG_Load(path);
    if (loadedSurface == NULL) {
      error(IMG_GetError());
    }

    SDL_Texture* spritesheet = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    if (spritesheet == NULL) {
      error(SDL_GetError());
    }

    SDL_FreeSurface(loadedSurface);

    for (int i = 0; i < ANIMATION_FRAMES; i++) {
        int row = i / FRAME_COLS;
        int col = i % FRAME_COLS;
        
        spriteClips[i].x = col * SPRITE_WIDTH;
        spriteClips[i].y = row * SPRITE_HEIGHT;
        spriteClips[i].w = SPRITE_WIDTH;
        spriteClips[i].h = SPRITE_HEIGHT;
    }

    return spritesheet;
}

void renderCharacter(SDL_Renderer* renderer, SDL_Texture* spritesheet, int x, int y) {
    SDL_Rect renderQuad = { x, y, SPRITE_WIDTH, SPRITE_HEIGHT };
    frames_t currentFrame = getCurrentFrame();
    SDL_RenderCopy(renderer, spritesheet, &spriteClips[currentFrame], &renderQuad);
}

void renderEnemy(SDL_Renderer *renderer, SDL_Texture *spritesheet, Enemy *enemy) {
  SDL_Rect renderQuad = { enemy->position.x, enemy->position.y, ENEMY_WIDTH, ENEMY_HEIGHT };
  // frames_t currentFrame = getEnemyFrame(enemy);
  frames_t currentFrame = enemy->currentFrame;
  SDL_RenderCopy(renderer, spritesheet, &enemyClips[currentFrame], &renderQuad);
}

void renderEnemies(SDL_Renderer *renderer, SDL_Texture *spritesheet, Enemy **enemies) {
  for (int i = 0; i < NUM_ENEMIES; i++) {
    if (enemies[i] != NULL) {
      renderEnemy(renderer, spritesheet, enemies[i]);
    }
  }
}

void renderFloor(SDL_Renderer* renderer, const char* path) {
    int tileWidth = 45;
    int tileHeight = 20;

    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
      error(IMG_GetError());
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
      error(SDL_GetError());
    }

    SDL_Rect srcRect = {
        .x = 16,
        .y = 200,
        .w = 45,
        .h = 20,
    };

    int tilesX = (WINDOW_WIDTH + tileWidth - 1) / tileWidth;
    int tilesY = (WINDOW_HEIGHT + tileHeight - 1) / tileHeight;

    for (int y = 0; y < tilesY; y++) {
        for (int x = 0; x < tilesX; x++) {
            SDL_Rect dstRect = {
                .x = x * tileWidth,
                .y = y * tileHeight,
                .w = tileWidth,
                .h = tileHeight
            };

            SDL_RenderCopy(renderer, texture, &srcRect, &dstRect);
        }
    }

    SDL_DestroyTexture(texture);
}


