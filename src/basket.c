#include "basket.h"

static void logic(Basket *this) {
  GPoint pos = pge_sprite_get_position(this->sprite);
  GRect rect = pge_sprite_get_bounds(this->sprite);

  // Move square and wrap to screen
  if(this->moving) {
    switch(this->direction) {
      case DIRECTION_RIGHT:
        if(pos.x <= 144 - rect.size.w) {
          pos.x += SPEED;
        }
        break;
      case DIRECTION_LEFT:
        if(pos.x >= 0) {
          pos.x -= SPEED;
        }
        break;
      case DIRECTION_STOP:
        break;
    }

    // Update sprite
    pge_sprite_set_position(this->sprite, pos);
  }
}

static void render(Basket *this, GContext *ctx) {
  pge_sprite_draw(this->sprite, ctx);
}

Basket* basket_create(GPoint start_position) {
  Basket *this = malloc(sizeof(Basket));
  this->direction = DIRECTION_UP;

  this->sprite = pge_sprite_create(start_position, RESOURCE_ID_PANDA);

  return this;
}

void basket_destroy(Basket *this) {
  pge_sprite_destroy(this->sprite);
  free(this);
}

void basket_set_direction(Basket *this, int new_direction) {
  this->direction = new_direction;
}

void basket_set_is_moving(Basket *this, bool new_state) {
  this->moving = new_state;
}

void basket_set_state(Basket *this, basket_state state) {
  // Set the sprite bitmap
  switch(state) {
    case panda:
      pge_sprite_set_anim_frame(this->sprite, RESOURCE_ID_PANDA);
      break;
    case explosion:
      pge_sprite_set_anim_frame(this->sprite, RESOURCE_ID_EXPLOSION);
      break;
    default:
      break;
  }
}


void basket_logic(Basket *this) {
  logic(this);
}

void basket_render(Basket *this, GContext *ctx) {
  render(this, ctx);
}
