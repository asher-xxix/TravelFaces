#include <pebble.h>
//#define LOCATION 0
#include "netdownload.h"
#include "png.h"
#include "file_receiver.h"
// #ifdef PBL_PLATFORM_APLITE
// #include "png.h"
// #endif

#define KEY_DATA 4
#define STRING_KEY_DATA 7

#define COMMAND_KEY 0
#define COLOR_KEY 1

#define ID_KEY 47000
#define BYTES_KEY 47001
#define TOTAL_SIZE_KEY 47002
#define OFFSET_KEY 47003

#define RANDOM_PHOTO_COMMAND 0

// byte array to store transferred bitmaps
// each part of the bitmap is 100 bytes
static const uint8_t *bytes[] = {};
FileReceiver *s_filereceiver;

static Window *s_main_window;

static TextLayer *s_time_layer;
static GFont s_time_font;

static TextLayer *android_layer;

static GBitmap *s_example_bitmap;
static BitmapLayer *s_bitmap_layer;

static TextLayer *s_location_layer;
static GFont s_location_font;

static GBitmap *neu_bitmap;

static GBitmap *s_current_bitmap;
static TextLayer *s_status_text_layer;

static TextLayer *text_layer;
static BitmapLayer *bitmap_layer;
static GBitmap *current_bmp;

// static char *images[] = {
//     // this needs to be changed to be images that are gathered
//     "http://miriadna.com/desctopwalls/images/max/Thick-clouds-layer.jpg"
//     "http://assets.getpebble.com.s3-website-us-east-1.amazonaws.com/pebble-faces/cherie.png",
//     "http://assets.getpebble.com.s3-website-us-east-1.amazonaws.com/pebble-faces/mtole.png",
//     "http://assets.getpebble.com.s3-website-us-east-1.amazonaws.com/pebble-faces/chris.png",
//     "http://assets.getpebble.com.s3-website-us-east-1.amazonaws.com/pebble-faces/heiko.png",
//     "http://assets.getpebble.com.s3-website-us-east-1.amazonaws.com/pebble-faces/thomas.png",
//     "http://assets.getpebble.com.s3-website-us-east-1.amazonaws.com/pebble-faces/matt.png",
//     "http://assets.getpebble.com.s3-website-us-east-1.amazonaws.com/pebble-faces/katharine.png",
//     "http://assets.getpebble.com.s3-website-us-east-1.amazonaws.com/pebble-faces/katherine.png",
//     "http://assets.getpebble.com.s3-website-us-east-1.amazonaws.com/pebble-faces/alex.png",
//     "http://assets.getpebble.com.s3-website-us-east-1.amazonaws.com/pebble-faces/lukasz.png"
// };

//static unsigned long image = 0;

#define MAX_STATUS_TEXT_LENGTH 255

static char s_status_text[MAX_STATUS_TEXT_LENGTH + 1];

static void set_status(const char *text) {
    if (text) {
        text_layer_set_text(s_status_text_layer, text);
        layer_set_hidden(text_layer_get_layer(s_status_text_layer), false);
    }
    else {
        layer_set_hidden(text_layer_get_layer(s_status_text_layer), true);
    }
}

static void load_image() {
    APP_LOG(APP_LOG_LEVEL_INFO, "sending load random image command");

    filereceiver_reset(s_filereceiver);

    DictionaryIterator *outbox;
    app_message_outbox_begin(&outbox);
    dict_write_uint8(outbox, COMMAND_KEY, RANDOM_PHOTO_COMMAND);
#ifdef PBL_COLOR
    dict_write_uint8(outbox, COLOR_KEY, 1);
#else
    dict_write_uint8(outbox, COLOR_KEY, 0);
#endif
    app_message_outbox_send();

    app_comm_set_sniff_interval(SNIFF_INTERVAL_REDUCED);
    set_status("Loading");
}

static void clear_image() {
    if (s_current_bitmap) {
        bitmap_layer_set_bitmap(s_bitmap_layer, NULL);
        gbitmap_destroy(s_current_bitmap);
        s_current_bitmap = NULL;
    }
}

static void image_loaded(GBitmap *bitmap) {
    layer_set_hidden(text_layer_get_layer(s_status_text_layer), true);

    clear_image();

    bitmap_layer_set_bitmap(s_bitmap_layer, bitmap);
    s_current_bitmap = bitmap;

    set_status(NULL);
}

static void handle_download_progress(uint32_t file_id, size_t downloaded, size_t total) {
    snprintf(s_status_text, MAX_STATUS_TEXT_LENGTH, "%d / %d", downloaded, total);
    set_status(s_status_text);
}

static void handle_download_error(int error) {
    app_comm_set_sniff_interval(SNIFF_INTERVAL_NORMAL);

    snprintf(s_status_text, MAX_STATUS_TEXT_LENGTH, "Error %d", error);
    set_status(s_status_text);
}

static void handle_image_received(uint32_t image_id, unsigned char *data, size_t size) {
    app_comm_set_sniff_interval(SNIFF_INTERVAL_NORMAL);

#ifdef PBL_PLATFORM_APLITE
    // Take over ownership of the data pointer
    filereceiver_disown_data(s_filereceiver, data);

    // Use upng decoder (this will free the data buffer)
    GBitmap *bitmap = gbitmap_create_with_png_data(data, size);
#else
    GBitmap *bitmap = gbitmap_create_from_png_data(data, size);
#endif

    if (!bitmap) return;

    image_loaded(bitmap);

    set_status(NULL);
}

static void single_click_handler(ClickRecognizerRef recognizer, void *context) {
    clear_image();
    load_image();
}

// static void click_config_provider(void *context) {
//     window_single_click_subscribe(BUTTON_ID_SELECT, single_click_handler);
//     window_single_click_subscribe(BUTTON_ID_UP, single_click_handler);
//     window_single_click_subscribe(BUTTON_ID_DOWN, single_click_handler);
// }


// This function is actually depreciated now
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Get the first pair
  Tuple *t = dict_read_first(iterator);

  // Process all pairs present
  while (t != NULL) {
  //while (true) {
    // Long lived buffer
    static char s_buffer[64];


    // Process this pair's key
    switch (t->key) {
        case KEY_DATA:

            //get image and display it
            //bitmap_layer_set_bitmap(s_bitmap_layer, neu_bitmap);
            //break;
            // copy value of bitmap chunks
            // byte_array
            // t -> value -> bytes
            // t -> length
            vibes_short_pulse();
            //clear_image();
            //load_image();


            //t->value->bytes[];
            break;

        case STRING_KEY_DATA:
            // this is for basic values, and it needs to be phased out
            // Copy value and display
            snprintf(s_buffer, sizeof(s_buffer), "Received '%s'", t->value->cstring);
            vibes_short_pulse();
            text_layer_set_text(android_layer, s_buffer);
            load_image();
            break;
    }
    // Get next pair, if any
    t = dict_read_next(iterator);
  }
}


static void inbox_dropped_callback(AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}


// void show_next_image() {
//     // at first, no image to show that it is working
//     bitmap_layer_set_bitmap(bitmap_layer, NULL);
//     text_layer_set_text(text_layer, "Loading...");
//
//     // Unload the current image if we had one and save a pointer to this one
//     if(current_bmp) {
//         gbitmap_destroy(current_bmp);
//         current_bmp = NULL;
//     }
//
//     netdownload_request(images[image]);
//
//     image++;
//     if(image >= sizeof(images)/sizeof(char*)) {
//         image = 0;
//     }
// }

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    // Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
}

static void window_load(Window *window) {

    // Create bitmap
    s_example_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NUM_2);
    s_bitmap_layer = bitmap_layer_create(GRect(-10, 0, 144, 168));
    //s_bitmap_layer = bitmap_layer_create(layer_get_bounds(window_layer));
    bitmap_layer_set_bitmap(s_bitmap_layer, s_example_bitmap);
#ifdef PBL_PLATFORM_APLITE
    bitmap_layer_set_compositing_mode(s_bitmap_layer, GCompOpAssign);
#elif PBL_PLATFORM_BASALT
    bitmap_layer_set_compositing_mode(s_bitmap_layer, GCompOpSet);
#endif
    //layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmap_layer));
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_bitmap_layer));

    // Start background window
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    // text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
    // text_layer_set_text(text_layer, "Shake");
    // text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
    // layer_add_child(window_layer, text_layer_get_layer(text_layer));

    bitmap_layer = bitmap_layer_create(bounds);
    layer_add_child(window_layer, bitmap_layer_get_layer(bitmap_layer));
    // current_bmp = NULL;


    // Create time TextLayer
    s_time_layer = text_layer_create(GRect(0, 10, 144, 50));
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text_color(s_time_layer, GColorBlack);
    //text_layer_set_text(s_time_layer, "Nyanpassu!");
    //text_layer_set_text(s_time_layer, "00.00");

    // Create GFont
    s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_SAN_FRANCISCO_42));

    // Apply to time TextLayer
    text_layer_set_font(s_time_layer, s_time_font);
    //text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
    //text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

    // Add it as a child layer to the Window's root layer
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));

    //creating android message Layer
    android_layer = text_layer_create(GRect(0, 130, 144, 25));
    text_layer_set_background_color(android_layer, GColorClear);
    text_layer_set_text_color(android_layer, GColorBlack);
    text_layer_set_text_alignment(android_layer, GTextAlignmentCenter);
    text_layer_set_text(android_layer, "Waiting!"); //pass in found message
    text_layer_set_overflow_mode(android_layer, GTextOverflowModeWordWrap);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(android_layer));


    // apply the weather font
    // s_location_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_SAN_FRANCISCO_21));
    // text_layer_set_font(s_location_layer, s_location_font);
    // layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_location_layer));

    //make sure time is initially displayed
    update_time();
    //load_image();
}

static void window_unload(Window *window) {
    // Unload GFont
    fonts_unload_custom_font(s_time_font);

    //destroy GBitmap
    //    gbitmap_destroy(s_example_bitmap);

    // destroy bitmap layer
    //    bitmap_layer_destroy(s_bitmap_layer);

    // destroy the text layer
    text_layer_destroy(s_time_layer);

    // destroy location layer
    //text_layer_destroy(s_location_layer);
    //fonts_unload_custom_font(s_location_font);

    text_layer_destroy(android_layer);

    // text_layer_destroy(text_layer);
    // bitmap_layer_destroy(bitmap_layer);
    // gbitmap_destroy(current_bmp);
}

// this is only for bitmaps
static void handle_incoming_message(DictionaryIterator *iter, void *context) {
  filereceiver_handle_message(s_filereceiver, iter);
}


// void download_complete_handler(NetDownload *download) {
//     printf("Loaded image with %lu bytes", download->length);
//     printf("Heap free is %u bytes", heap_bytes_free());
//
// #ifdef PBL_PLATFORM_APLITE
//     GBitmap *bmp = gbitmap_create_with_png_data(download->data, download->length);
// #else
//     GBitmap *bmp = gbitmap_create_from_png_data(download->data, download->length);
// #endif
//     bitmap_layer_set_bitmap(bitmap_layer, bmp);
//
//     // Save pointer to currently shown bitmap (to free it)
//     if (current_bmp) {
//         gbitmap_destroy(current_bmp);
//     }
//     current_bmp = bmp;
//
//     // Free the memory now
// #ifdef PBL_PLATFORM_APLITE
//     // gbitmap_create_with_png_data will free download->data
// #else
//     free(download->data);
// #endif
//     // We null it out now to avoid a double free
//     download->data = NULL;
//     netdownload_destroy(download);
// }

// void tap_handler(AccelAxisType accel, int32_t direction) {
//     show_next_image();
// }

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
}



static void init() {
    // app_message_register_inbox_received(in_received_handler);
    //app_message_open(512, 512);    //Large input and output buffer sizes

    //app_message_register_inbox_received((AppMessageInboxReceived) in_received_handler);
    //app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

    // Register callbacks
    app_message_register_inbox_received(inbox_received_callback);
    app_message_register_inbox_dropped(inbox_dropped_callback);



    //accel_tap_service_subscribe(tap_handler);

    // Register with TickTimerService
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);


    s_main_window = window_create();
// #ifdef PBL_SDK_2
//     window_set_fullscreen(s_main_window, true);
// #endif
    //window_set_background_color(s_main_window, COLOR_FALLBACK(GColorBlue, GColorBlack));
    //window_set_background_color(s_main_window, GColorClear);
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload
    });

    app_message_register_inbox_received(handle_incoming_message);
    //app_message_register_inbox_dropped(handle_incoming_message);
    app_message_register_outbox_failed(outbox_failed_callback);
    app_message_register_outbox_sent(outbox_sent_callback);

    // Open AppMessage
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

    s_filereceiver = filereceiver_create((FileReceiverMessageKeys) {
        .id_key = ID_KEY,
        .size_key = TOTAL_SIZE_KEY,
        .offset_key = OFFSET_KEY,
        .bytes_key = BYTES_KEY
    });

    filereceiver_set_callbacks(s_filereceiver, (FileReceiverCallbacks) {
        .error = handle_download_error,
        .file_progress = handle_download_progress,
        .file_received = handle_image_received
    });

    window_stack_push(s_main_window, true);

}

static void deinit() {
    netdownload_deinitialize();
    window_destroy(s_main_window);
}


int main(void) {
    init();

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", s_main_window);

    app_event_loop();
    deinit();
}
