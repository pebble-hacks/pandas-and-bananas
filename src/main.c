/**
 * Pandas and Bananas game
 * Uses PGE for collisions and sprite loading
 * All artwork is from OpenGameArt.org
 *
 * Author:
 * Matthew Hungerford
 *
 * Accelerometer moves panda left and right,
 * catching fruit increases score
 * catching bombs decreases lives
 */

#include <pebble.h>

#include "pge/pge.h"
#include "pge/additional/pge_sprite.h"
#include "pge/additional/pge_collision.h"
#include "pge/additional/pge_title.h"

#include "basket.h"
#include "fruit.h"

#define FRUITS 8
#define FPS 20
#define GOAL 20

#define ACCEL_DEADZONE 50

static Window *game_window;
static Basket *basket;
static Fruit *fruit[FRUITS] = {NULL};
static PGESprite *pipes;
static PGESprite *hearts;

static int score = 0;
static char score_str[8] = "";
static GFont *score_font = NULL;

static int fruit_count = 0;
static int fruit_level = 3;
static int lives = 3;
static int level = 1;
static bool enable_accel = true;

static int32_t explosion_countdown = 0;

static void game_start();
static void title_click_handler(int button_id);

/******************************** Game ****************************************/

static void logic() {
  if (lives <= 0) {
    // Exit to title on button press
    if (pge_get_button_state(BUTTON_ID_DOWN) || pge_get_button_state(BUTTON_ID_SELECT)) {
      window_stack_pop(false);
    }
    return;
  }

  if (explosion_countdown == 0) {
    // Special logic for button state & accelerometer
    if (enable_accel) {
      AccelData accel = {0};
      accel_service_peek(&accel);
      if (accel.x > ACCEL_DEADZONE) {
        basket_set_direction(basket, DIRECTION_RIGHT);
      } else if (accel.x < -ACCEL_DEADZONE) {
        basket_set_direction(basket, DIRECTION_LEFT);
      } else {
        basket_set_direction(basket, DIRECTION_STOP);
      }
    } else { 
      if (pge_get_button_state(BUTTON_ID_UP)) {
        basket_set_direction(basket, DIRECTION_LEFT);
      } else if (pge_get_button_state(BUTTON_ID_DOWN)) {
        basket_set_direction(basket, DIRECTION_RIGHT);
      } else {
        basket_set_direction(basket, DIRECTION_STOP);
      }
    }
  } else {
    basket_set_direction(basket, DIRECTION_STOP);
    explosion_countdown--;
    if (explosion_countdown == 0) {
      basket_set_state(basket, panda);
    }
  }

  basket_logic(basket);

  // Level is based of score, every GOAL fruits go up a level
  level = (score / GOAL) + 1;

  // Fruit generation
  for (int i = 0; i < FRUITS; i++) {
    if (!fruit[i] && (fruit_count < fruit_level)) {
      // Randomly create the fruit at one of the 4 pipe locations
      fruit[i] = fruit_create(GPoint(rand() % 4 * 34 + 12 , 22));
      // Randomly set the fruit type
      fruit_set_type(fruit[i], rand() % 5);
      fruit_count++;
    }
  }

  for (int i = 0; i < FRUITS; i++) {
    if (fruit[i]) {
      // as the level gets higher, fruits move faster
      pge_sprite_move(fruit[i]->sprite, 0, 3 + level);
    }
  }

  // Only GRect of basket for collision test
  GRect basket_rect = pge_sprite_get_bounds(basket->sprite);
  basket_rect.origin.x += 16;
  basket_rect.origin.y += 32;
  basket_rect.size.w -= 32;
  basket_rect.size.h -= 32;

  for (int i = 0; i < FRUITS; i++) {
    if (!fruit[i]) {
      continue;
    }
    // Destroy baskets collided with
    GRect fruit_grect = pge_sprite_get_bounds(fruit[i]->sprite);
    if (pge_collision_rectangle_rectangle(&basket_rect, &fruit_grect)) {
      if (fruit_get_type(fruit[i]) == bomb) {
        basket_set_state(basket, explosion);
        explosion_countdown = 30;
        lives--;
        vibes_short_pulse();
      } else {
        score++;
      }
      fruit_destroy(fruit[i]);
      fruit[i] = NULL;
      fruit_count--;
    } else if (pge_sprite_get_position(fruit[i]->sprite).y >= 140) {
      fruit_destroy(fruit[i]);
      fruit[i] = NULL;
      fruit_count--;
    }
  }

  // If game over, check and set high score
  if (lives == 0) {
    int highscore = pge_title_get_highscore();
    if (score > highscore) {
      pge_title_set_highscore(score);
    }
  }
}

static void draw(GContext *ctx) {
  graphics_context_set_compositing_mode(ctx, GCompOpSet);
  basket_render(basket, ctx);

  // Draw the fruits behind the bombs
  for (int i = 0; i < FRUITS; i++) {
    if (fruit[i] && fruit_get_type(fruit[i]) != bomb) {
      fruit_render(fruit[i], ctx);
    }
  }

  // Draw the bombs ontop of fruits
  for (int i = 0; i < FRUITS; i++) {
    if (fruit[i] && fruit_get_type(fruit[i]) == bomb) {
      fruit_render(fruit[i], ctx);
    }
  }

  pge_sprite_draw(pipes, ctx);


  for (int i = 0; i < lives; i++) {
    hearts->position.x = i * 24 + 2; 
    pge_sprite_draw(hearts, ctx);
  }

  if (lives <= 0) {
    graphics_context_set_text_color(ctx, GColorRed);
    graphics_draw_text(ctx, "GAME OVER", fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD), 
        (GRect) {.origin = {0,44}, .size = {144,44}}, 
        GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  }

  graphics_context_set_fill_color(ctx, GColorLightGray);
  graphics_fill_rect(ctx, (GRect) {.origin = {110, 2 }, .size = {34, 20}}, 2, GCornersAll);
  graphics_context_set_text_color(ctx, GColorBlack);
  snprintf(score_str, sizeof(score_str), "%d", score);
  graphics_draw_text(ctx, score_str, score_font, 
      (GRect) {.origin = {110,2 - 4}, .size = {34,20}}, 
      GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
}

static void click(int button_id) {
  switch (button_id) {
    default:
      break;
  }
}

// Game start is called each time we start the game code
// so everything allocated here has to be deallocated before allocating
// to allow for restarting the game from the title window
static void game_start() {
  // Create a basket
  if (basket) {
    basket_destroy(basket);
  }
  basket = basket_create(GPoint(72 - 24, 168 - 52));
  
  if (pipes) {
    pge_sprite_destroy(pipes);
  }
  pipes = pge_sprite_create((GPoint){0,0}, RESOURCE_ID_PIPES);

  if (hearts) {
    pge_sprite_destroy(hearts);
  }
  hearts = pge_sprite_create((GPoint){0,2}, RESOURCE_ID_HEART);

  for (int i = 0; i < FRUITS; i++) {
    if (fruit[i]) {
      fruit_destroy(fruit[i]);
      fruit[i] = NULL;
    }
  }

  fruit_count = 0;
  score = 0;
  lives = 3;
  level = 1;
  explosion_countdown = 0;
  pge_sprite_set_position(basket->sprite, GPoint(72 - 24, 168 - 52));
  basket_set_state(basket, panda);

  score_font = fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21);

  // Start game logic and push game window and setup logic/draw/click handlers
  game_window = pge_begin(logic, draw, click);
  window_set_background_color(game_window, GColorBlue);
  pge_set_framerate(FPS);
  accel_data_service_subscribe(0, NULL);
}

static void title_click_handler(int button_id) {
  switch (button_id) {
    case BUTTON_ID_UP:
      break;
    case BUTTON_ID_SELECT:
      enable_accel = !enable_accel;
      break;
    case BUTTON_ID_DOWN:
      game_start();
      break;
    default:
      break;
  }
}

void pge_init(void) {
  pge_title_show_highscore(true);
  pge_title_push("", "", "", "", GColorBlack, RESOURCE_ID_TITLE, title_click_handler);
}

void pge_deinit(void) {
  accel_data_service_unsubscribe();
}

