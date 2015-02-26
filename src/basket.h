#pragma once

#include "pge/pge.h"
#include "pge/additional/pge_sprite.h"


#define DIRECTION_UP    0
#define DIRECTION_RIGHT 1
#define DIRECTION_DOWN  2
#define DIRECTION_LEFT  3
#define DIRECTION_STOP  4

#define SPEED 3

typedef enum basket_state {
  panda,
  explosion
} basket_state;

// An extension of PGameEntity with extra logic attributes
typedef struct {
  PGESprite *sprite;
  int direction;
  bool moving;
} Basket;

Basket* basket_create(GPoint start_position);

void basket_destroy(Basket *this);

void basket_set_direction(Basket *this, int new_direction);

void basket_set_is_moving(Basket *this, bool new_state);

void basket_set_state(Basket *this, basket_state state);

void basket_logic(Basket *this);

void basket_render(Basket *this, GContext *ctx);

