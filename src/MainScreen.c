//IMPORTS
#include "Clock_Analog.h"
#include <Music_Window.c>
 #include <Nick_Cage.c>
#include <Power_Window.c>
#include "pebble.h"
#include "string.h"
#include "stdlib.h"

//WINDOWS
Window *window;
Window *cageWindow;
Window *powerWindow;
Window *musicWindow;

typedef enum {CLOCK, CAGE, POWER, MUSIC} WATCHFACE;
WATCHFACE currentWindow;
int windowValue;
//LAYERS 
Layer *simple_bg_layer;
//Layer *date_layer;
Layer *hands_layer;
BitmapLayer *background;

//TEXT LAYERS
//TextLayer *num_label;
//TextLayer *taps;

//HANDS
RotBitmapLayer *minutebutton;
RotBitmapLayer *hourbutton;
RotBitmapLayer *secondbutton;
static GPath *tick_paths[NUM_CLOCK_TICKS];

// buffers
char num_buffer[4];
//MISC
GBitmap *bacgroundimage;
GBitmap *hourbuttonimage;
GBitmap *minutebuttonimage;
GBitmap *secondbuttonimage;
DictionaryIterator *iter;
//Music_Window *music;

/***************************************************************
*                       Time
***************************************************************/
static void bg_update_proc(Layer *layer, GContext *ctx) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
 
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
  graphics_fill_rect(ctx, bounds, 11, GCornerNone);
  graphics_context_set_fill_color(ctx, GColorWhite);

}

static void hands_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  const GPoint center = grect_center_point(&bounds);
  /**
  const int16_t secondHandLength = bounds.size.w / 2;

  GPoint secondHand;

  

  int32_t second_angle = TRIG_MAX_ANGLE * t->tm_sec / 60;
  secondHand.y = (int16_t)(-cos_lookup(second_angle) * (int32_t)secondHandLength / TRIG_MAX_RATIO) + center.y;
  secondHand.x = (int16_t)(sin_lookup(second_angle) * (int32_t)secondHandLength / TRIG_MAX_RATIO) + center.x;

  // second hand
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_draw_line(ctx, secondHand, center);
  **/
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
/***************************************************************
*                       rot bitmap layer for the button hands
****************************************************************/
  rot_bitmap_set_compositing_mode(hourbutton, GCompOpOr);
  rot_bitmap_set_compositing_mode(minutebutton, GCompOpOr);
  rot_bitmap_set_compositing_mode(secondbutton, GCompOpOr);
  
  rot_bitmap_set_src_ic(hourbutton, GPoint(17, 14));
  rot_bitmap_set_src_ic(minutebutton, GPoint(17, 14));
  rot_bitmap_set_src_ic(secondbutton, GPoint(17, 14));
  
  layer_add_child(layer, (Layer*) hourbutton);
  layer_add_child(layer, (Layer*) minutebutton);
  layer_add_child(layer, (Layer*) secondbutton);
  
  rot_bitmap_layer_set_angle(minutebutton, (TRIG_MAX_ANGLE * t->tm_min / 60+TRIG_MAX_ANGLE/2) % TRIG_MAX_ANGLE);
  rot_bitmap_layer_set_angle(hourbutton, ((TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6)+TRIG_MAX_ANGLE/2)%TRIG_MAX_ANGLE);
  rot_bitmap_layer_set_angle(secondbutton, (TRIG_MAX_ANGLE * t->tm_sec / 60) % TRIG_MAX_ANGLE);
  
   // dot in the middle
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(bounds.size.w / 2 - 1, bounds.size.h / 2 - 1, 3, 3), 0, GCornerNone);
}
/***************************************************************
*                       .js
***************************************************************/
static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(window_get_root_layer(window));
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}


/***************************************************************
*                      Outward Communtion
***************************************************************/

static void out_handler(DictionaryIterator *iter, void *context) 
{
   //do nothing special
}
//Register AppMessage events
void sendButtonPress(int intValue) 
{

    Tuplet value = TupletInteger(1, intValue); //writing placeholder value
    dict_write_tuplet(iter, &value); 
    app_message_outbox_send();
}

/***************************************************************
*                       Button Listing
***************************************************************/
void down_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(currentWindow == CLOCK)
  {
    currentWindow = CAGE;
    window_stack_push(cageWindow,true);
    windowValue =6;
  }
  else
     sendButtonPress(windowValue+2 );
}
void up_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(currentWindow == CLOCK)
  {
    currentWindow = MUSIC;
    window_stack_push(musicWindow,true);
    windowValue = 0;
  } 
  else
    sendButtonPress(windowValue+0);
}
void select_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(currentWindow == CLOCK)
  {
    currentWindow = POWER;
    window_stack_push(powerWindow,true);  
    windowValue =3;
  }
  else
     sendButtonPress(windowValue+1);
}

void back_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(currentWindow != CLOCK)
  {
    currentWindow = CLOCK;
    window_stack_pop(true);
  } 
  else
  {
    window_stack_pop_all(true);
  }
}

void config_provider(Window *window) {
 // single click / repeat-on-hold config:
  window_single_click_subscribe(BUTTON_ID_DOWN, down_single_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_single_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_single_click_handler);
  window_single_click_subscribe(BUTTON_ID_BACK, back_single_click_handler);
  //window_single_repeating_click_subscribe(BUTTON_ID_SELECT, 1000, select_single_click_handler);
  // multi click config:
 // window_multi_click_subscribe(BUTTON_ID_SELECT, 2, 10, 0, true, select_multi_click_handler);
  // long click config:
 // window_long_click_subscribe(BUTTON_ID_SELECT, 700, select_long_click_handler, select_long_click_release_handler);
}


/***************************************************************
*                       LOAD and UNLOAD
***************************************************************/
static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  //init layers
  simple_bg_layer = layer_create(bounds);
  layer_set_update_proc(simple_bg_layer, bg_update_proc);
  layer_add_child(window_layer, simple_bg_layer);
  
  //background clock
  background = bitmap_layer_create(bounds);
  bacgroundimage = gbitmap_create_with_resource(RESOURCE_ID_BACK_GROUND);
  bitmap_layer_set_bitmap(background, bacgroundimage);
  bitmap_layer_set_alignment(background, GAlignBottom);
  bitmap_layer_set_compositing_mode(background, GCompOpAssign);
  layer_add_child(window_layer, bitmap_layer_get_layer(background));
  
  // init hands
  hands_layer = layer_create(bounds);
  layer_set_update_proc(hands_layer, hands_update_proc);
  layer_add_child(window_layer, hands_layer);

}

static void window_unload(Window *window) {
  layer_destroy(simple_bg_layer);
//  layer_destroy(date_layer);

  layer_destroy(hands_layer);
  bitmap_layer_destroy(background);
}

/***************************************************************
*                       INT and DE INT
***************************************************************/
static void init(void) {
  //Build Windows
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  currentWindow = CLOCK;
  windowValue=0;
  //Other Windows
  cageWindow = cage_init();  
  powerWindow = power_init();
  musicWindow =  music_init(); 
  //add Listeners to Buttons
  window_set_click_config_provider(window, (ClickConfigProvider) config_provider);
  window_set_click_config_provider(cageWindow, (ClickConfigProvider) config_provider);
  window_set_click_config_provider(powerWindow, (ClickConfigProvider) config_provider);
  window_set_click_config_provider(musicWindow, (ClickConfigProvider) config_provider);  
  
  num_buffer[0] = '\0';

  hourbuttonimage = gbitmap_create_with_resource(RESOURCE_ID_HOUR_BUTTON);
  minutebuttonimage = gbitmap_create_with_resource(RESOURCE_ID_MINUTE_BUTTON);
  secondbuttonimage = gbitmap_create_with_resource(RESOURCE_ID_SECOND_HAND);

  hourbutton = rot_bitmap_layer_create(hourbuttonimage);
  minutebutton = rot_bitmap_layer_create(minutebuttonimage);
  secondbutton = rot_bitmap_layer_create(secondbuttonimage);
  // init clock face paths
  for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
    tick_paths[i] = gpath_create(&ANALOG_BG_POINTS[i]);
  }

  // Push the window onto the stack
  const bool animated = true;
  window_stack_push(window, animated);

  tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
  
  //Dictionary Stuff
   app_message_register_outbox_sent(out_handler);
   app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());    //Largest possible input and output buffer sizes
   
   app_message_outbox_begin(&iter);
}

static void deinit(void) {
  cage_deinit();
  music_deinit();
  power_deinit();
  rot_bitmap_layer_destroy(minutebutton);
  rot_bitmap_layer_destroy(hourbutton);

  for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
    gpath_destroy(tick_paths[i]);
  }  
  tick_timer_service_unsubscribe();
  accel_tap_service_unsubscribe();
  window_destroy(window);

}

/***************************************************************
*                           Main
***************************************************************/
int main(void) {
  init();
  app_event_loop();
  deinit();
}