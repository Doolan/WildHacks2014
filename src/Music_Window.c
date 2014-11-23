//IMPORTS
#include "pebble.h"
#include "string.h"
#include "stdlib.h"

//WINDOWS
Window *musicWindow;  
//MISC
GBitmap *radioImage;


/***************************************************************
*                      Graphics Misc
***************************************************************/
static void music_bg_update_proc(Layer *layer, GContext *ctx) {
  Layer *window_layer = window_get_root_layer(musicWindow);
  GRect bounds = layer_get_bounds(window_layer);
  
 
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
  graphics_fill_rect(ctx, bounds, 11, GCornerNone);
  graphics_context_set_fill_color(ctx, GColorWhite);

}
/***************************************************************
*                       LOAD and UNLOAD
***************************************************************/
static void music_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  //init layers
  Layer *simple_bg_layer = layer_create(bounds);
  layer_set_update_proc(simple_bg_layer, music_bg_update_proc);
  layer_add_child(window_layer, simple_bg_layer);
  
  //background clock
  BitmapLayer *background = bitmap_layer_create(bounds);
  radioImage = gbitmap_create_with_resource(RESOURCE_ID_MUSIC_IMAGE);
  bitmap_layer_set_bitmap(background, radioImage);
  bitmap_layer_set_alignment(background, GAlignBottom);
  bitmap_layer_set_compositing_mode(background, GCompOpAssign);
  layer_add_child(window_layer, bitmap_layer_get_layer(background));
}

static void music_window_unload(Window *window) {
}
/***************************************************************
*                       INT and DE INT
***************************************************************/
static Window* music_init(void) {
  musicWindow = window_create();
  window_set_window_handlers(musicWindow, (WindowHandlers) {
    .load = music_window_load,
    .unload = music_window_unload,
  });
  return musicWindow;
   // Push the window onto the stack
  ///window_stack_push(musicWindow, true);
}


static void music_deinit(void) {
 window_destroy(musicWindow);
}






























