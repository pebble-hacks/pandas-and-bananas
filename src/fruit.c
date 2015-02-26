#include "fruit.h"

static void logic(Fruit *this) {
  GPoint pos = pge_sprite_get_position(this->sprite);

  // Update sprite
  pge_sprite_set_position(this->sprite, pos);
}

static void render(Fruit *this, GContext *ctx) {
  pge_sprite_draw(this->sprite, ctx);
}

Fruit* fruit_create(GPoint position) {
  Fruit *this = malloc(sizeof(Fruit));
  this->sprite = pge_sprite_create(position, RESOURCE_ID_BANANA);
  this->fruit = banana;
  return this;
}

void fruit_destroy(Fruit *this) {
  pge_sprite_destroy(this->sprite);
  free(this);
}

void fruit_set_type(Fruit *this, fruit_type type) {
  this->fruit = type;

  // Set the sprite bitmap
  switch(this->fruit) {
    case banana:
      pge_sprite_set_anim_frame(this->sprite, RESOURCE_ID_BANANA);
      break;
    case pineapple:
      pge_sprite_set_anim_frame(this->sprite, RESOURCE_ID_PINEAPPLE);
      break;
    case strawberry:
      pge_sprite_set_anim_frame(this->sprite, RESOURCE_ID_STRAWBERRY);
      break;
    case watermellon:
      pge_sprite_set_anim_frame(this->sprite, RESOURCE_ID_WATERMELLON);
      break;
    case bomb:
      pge_sprite_set_anim_frame(this->sprite, RESOURCE_ID_BOMB);
      break;
    default:
      break;
  }
}

fruit_type fruit_get_type(Fruit *this) {
  return this->fruit;
}

void fruit_logic(Fruit *this) {
  logic(this);
}

void fruit_render(Fruit *this, GContext *ctx) {
  render(this, ctx);
}
