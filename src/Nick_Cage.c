//IMPORTS
#include "pebble.h"
#include "string.h"
#include "stdlib.h"

//WINDOWS
Window *cageWindow;
//MISC
GBitmap *radioImage;

/***************************************************************
*                      Graphics Misc
***************************************************************/
static void cage_bg_update_proc(Layer *layer, GContext *ctx) {
  Layer *window_layer = window_get_root_layer(cageWindow);
  GRect bounds = layer_get_bounds(window_layer);  
 
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
  graphics_fill_rect(ctx, bounds, 11, GCornerNone);
  graphics_context_set_fill_color(ctx, GColorWhite);

}
/***************************************************************
*                       LOAD and UNLOAD
***************************************************************/
static void cage_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  //init layers
  Layer *simple_bg_layer = layer_create(bounds);
  layer_set_update_proc(simple_bg_layer, cage_bg_update_proc);
  layer_add_child(window_layer, simple_bg_layer);
  
  //background 
  BitmapLayer *background = bitmap_layer_create(bounds);
  radioImage = gbitmap_create_with_resource(RESOURCE_ID_NICK_CAGE);
  bitmap_layer_set_bitmap(background, radioImage);
  bitmap_layer_set_alignment(background, GAlignCenter);
  bitmap_layer_set_compositing_mode(background, GCompOpAssign);
  layer_add_child(window_layer, bitmap_layer_get_layer(background));
}

static void cage_window_unload(Window *window) {
}
/***************************************************************
*                       INT and DE INT
***************************************************************/
static Window* cage_init(void) {
  cageWindow = window_create();
  window_set_window_handlers(cageWindow, (WindowHandlers) {
    .load = cage_window_load,
    .unload = cage_window_unload,
  });
  return cageWindow;
}


static void cage_deinit(void) {

 window_destroy(cageWindow);
}






























