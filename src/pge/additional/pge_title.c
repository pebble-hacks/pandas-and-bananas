#include "pge_title.h"

// UI
static Window *s_window = NULL;
static TextLayer *s_title_layer, *s_up_layer, *s_select_layer, *s_down_layer;
static TextLayer *s_score_layer;
static BitmapLayer *s_bg_layer = NULL;
static GBitmap *s_bg_bitmap = NULL;

static PGEClickHandler *s_click_handler = NULL;

// State
static int s_background_res_id = -1;
static GColor s_title_color;
static char s_title_buffer[PGE_TITLE_LENGTH_MAX] = {0};
static char s_up_buffer[PGE_TITLE_ACTION_MAX] = {0};
static char s_select_buffer[PGE_TITLE_ACTION_MAX] = {0};
static char s_down_buffer[PGE_TITLE_ACTION_MAX] = {0};
static char s_score_buffer[PGE_TITLE_ACTION_MAX] = {0};
static bool s_light_on;

static bool s_show_highscore = false;

/*********************************** Clicks ***********************************/

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  // Toggle light
  s_light_on = !s_light_on;
  light_enable(s_light_on);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_click_handler(BUTTON_ID_SELECT);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_click_handler(BUTTON_ID_DOWN);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

/********************************* Window *************************************/
static void window_appear(Window *window) {
  // Update score
  snprintf(s_score_buffer, sizeof(s_score_buffer), "High Score %d", pge_title_get_highscore());

  // Allocate background
  s_bg_bitmap = gbitmap_create_with_resource(s_background_res_id);
  bitmap_layer_set_bitmap(s_bg_layer, s_bg_bitmap);
}

static void window_disappear(Window *window) {
  if (s_bg_bitmap) {
    gbitmap_destroy(s_bg_bitmap);
    s_bg_bitmap = NULL;
  }
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);

  // BG Layer
  s_bg_layer = bitmap_layer_create(GRect(0, 0, window_bounds.size.w, window_bounds.size.h));
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bg_layer));

  // Title
  s_title_layer = text_layer_create(GRect(10, 40, window_bounds.size.w - 20, 60));
  text_layer_set_text_color(s_title_layer, s_title_color);
  text_layer_set_background_color(s_title_layer, GColorClear);
  text_layer_set_text_alignment(s_title_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(s_title_layer, GTextOverflowModeWordWrap);
  text_layer_set_font(s_title_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text(s_title_layer, s_title_buffer);
  layer_add_child(window_layer, text_layer_get_layer(s_title_layer));

  // UP TextLayer
  s_up_layer = text_layer_create(GRect(0, 20, window_bounds.size.w, 30));
  text_layer_set_text_color(s_up_layer, s_title_color);
  text_layer_set_background_color(s_up_layer, GColorClear);
  text_layer_set_text_alignment(s_up_layer, GTextAlignmentRight);
  text_layer_set_font(s_up_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text(s_up_layer, s_up_buffer);
  layer_add_child(window_layer, text_layer_get_layer(s_up_layer));

  // SELECT TextLayer
  s_select_layer = text_layer_create(GRect(0, 90, window_bounds.size.w, 30));
  text_layer_set_text_color(s_select_layer, s_title_color);
  text_layer_set_background_color(s_select_layer, GColorClear);
  text_layer_set_text_alignment(s_select_layer, GTextAlignmentRight);
  text_layer_set_font(s_select_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text(s_select_layer, s_select_buffer);
  layer_add_child(window_layer, text_layer_get_layer(s_select_layer));

  // DOWN TextLayer
  s_down_layer = text_layer_create(GRect(0, 130, window_bounds.size.w, 30));
  text_layer_set_text_color(s_down_layer, s_title_color);
  text_layer_set_background_color(s_down_layer, GColorClear);
  text_layer_set_text_alignment(s_down_layer, GTextAlignmentRight);
  text_layer_set_font(s_down_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text(s_down_layer, s_down_buffer);
  layer_add_child(window_layer, text_layer_get_layer(s_down_layer));

  // Score TextLayer
  s_score_layer = text_layer_create(GRect(18, 148, window_bounds.size.w - 36, 20));
  text_layer_set_text_color(s_score_layer, s_title_color);
  text_layer_set_background_color(s_score_layer, GColorWhite);
  text_layer_set_text_alignment(s_score_layer, GTextAlignmentCenter);
  text_layer_set_font(s_score_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text(s_score_layer, s_score_buffer);
  layer_add_child(window_layer, text_layer_get_layer(s_score_layer));
}

static void window_unload(Window *window) {
  bitmap_layer_destroy(s_bg_layer);
  text_layer_destroy(s_title_layer);
  text_layer_destroy(s_up_layer);
  text_layer_destroy(s_select_layer);
  text_layer_destroy(s_down_layer);
  text_layer_destroy(s_score_layer);

  // Finally
  window_destroy(window);
  window = NULL;
}

/********************************* Public *************************************/

void pge_title_push(char *title, char *up_action, char *select_action, char *down_action, GColor title_color, int background_res_id, PGEClickHandler *click_handler) {
  // Store values
  s_background_res_id = background_res_id;
  s_title_color = title_color;
  snprintf(s_title_buffer, sizeof(s_title_buffer), "%s", title);
  snprintf(s_up_buffer, sizeof(s_up_buffer), "%s", up_action);
  snprintf(s_select_buffer, sizeof(s_select_buffer), "%s", select_action);
  snprintf(s_down_buffer, sizeof(s_down_buffer), "%s", down_action);
  snprintf(s_score_buffer, sizeof(s_score_buffer), "High Score %d", pge_title_get_highscore());

  s_click_handler = click_handler;

  // Create Window
  if(!s_window) {
    s_window = window_create();
    window_set_click_config_provider(s_window, click_config_provider);
    window_set_fullscreen(s_window, true);
    window_set_window_handlers(s_window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload,
      .appear = window_appear,
      .disappear = window_disappear
    });
  }
  window_stack_push(s_window, true);
}

void pge_title_pop() {
  window_stack_pop(true);
}

void pge_title_set_highscore(int new_highscore) {
  persist_write_int(PGE_TITLE_HIGHSCORE_PS_KEY, new_highscore);
}

int pge_title_get_highscore() {
  if(persist_exists(PGE_TITLE_HIGHSCORE_PS_KEY)) {
    return persist_read_int(PGE_TITLE_HIGHSCORE_PS_KEY);
  } else {
    return 0;
  }
}

void pge_title_show_highscore(bool show) {
  s_show_highscore = show;
}
