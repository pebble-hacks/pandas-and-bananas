#pragma once

#include "pge/pge.h"
#include "pge/additional/pge_sprite.h"

typedef enum fruit_type {
  banana,
  pineapple,
  strawberry,
  watermellon,
  bomb
} fruit_type;

// An extension of PGameEntity with extra logic attributes
typedef struct {
  fruit_type fruit;
  PGESprite *sprite;
} Fruit;

Fruit* fruit_create(GPoint position);

void fruit_destroy(Fruit *this);

void fruit_set_type(Fruit *this, fruit_type type);

fruit_type fruit_get_type(Fruit *this);

void fruit_logic(Fruit *this);

void fruit_render(Fruit *this, GContext *ctx);

