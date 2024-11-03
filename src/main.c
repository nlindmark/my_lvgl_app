#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <lvgl.h>

#define LOG_LEVEL LOG_LEVEL_INF
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);

// Style definitions
static lv_style_t style_btn;
static lv_style_t style_title;

// Screen pointers
static lv_obj_t *main_screen = NULL;
static lv_obj_t *second_screen = NULL;
static lv_obj_t *third_screen = NULL;

// Function declarations
static void switch_screen_cb(lv_event_t *e);
static void create_main_screen(void);
static void create_second_screen(void);
static void create_third_screen(void);
static void log_memory_status(void);

// Memory monitoring function
static void log_memory_status(void)
{
    lv_mem_monitor_t mon;
    lv_mem_monitor(&mon);
    LOG_INF("Memory: used: %d, free: %d, frag: %d%%",
            (int)mon.total_size - (int)mon.free_size,
            (int)mon.free_size,
            (int)mon.frag_pct);
}

// Custom theme setup
static void init_theme_styles(void)
{
    // Initialize button style
    lv_style_init(&style_btn);
    lv_style_set_bg_color(&style_btn, lv_color_make(0, 100, 180));
    lv_style_set_bg_grad_color(&style_btn, lv_color_make(0, 150, 220));
    lv_style_set_bg_grad_dir(&style_btn, LV_GRAD_DIR_VER);
    lv_style_set_radius(&style_btn, 10);
    lv_style_set_shadow_width(&style_btn, 5);
    lv_style_set_shadow_ofs_y(&style_btn, 5);
    lv_style_set_shadow_opa(&style_btn, LV_OPA_50);

    // Initialize title style
    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, &lv_font_montserrat_14);
    lv_style_set_text_color(&style_title, lv_color_make(40, 40, 40));
}

static void start_screen_transition(lv_obj_t *target_screen)
{
    if (target_screen == NULL) {
        LOG_ERR("Cannot transition to NULL screen");
        return;
    }

    // Get current screen
    lv_obj_t *current_screen = lv_scr_act();
    if (current_screen == NULL) {
        LOG_ERR("Current screen is NULL");
        return;
    }

    LOG_INF("Transitioning from %p to %p", current_screen, target_screen);
    
    // Try simple transition first without animation
    lv_scr_load(target_screen);
}


static void switch_screen_cb(lv_event_t *e)
{
    lv_obj_t **target_ptr = (lv_obj_t **)lv_event_get_user_data(e);
    
    LOG_INF("Switch screen requested, target_ptr: %p", target_ptr);
    LOG_INF("Current screen pointers - main: %p, second: %p, third: %p", 
            main_screen, second_screen, third_screen);

    // Create screens just before switching to them
    if (target_ptr == &second_screen && second_screen == NULL) {
        LOG_INF("Creating second screen on demand");
        create_second_screen();
        if (second_screen == NULL) {
            LOG_ERR("Failed to create second screen");
            return;
        }
        LOG_INF("Second screen created at %p", second_screen);
    }
    else if (target_ptr == &third_screen && third_screen == NULL) {
        LOG_INF("Creating third screen on demand");
        create_third_screen();
        if (third_screen == NULL) {
            LOG_ERR("Failed to create third screen");
            return;
        }
        LOG_INF("Third screen created at %p", third_screen);
    }

    // Get the actual screen pointer
    lv_obj_t *target_screen = *target_ptr;
    if (target_screen == NULL) {
        LOG_ERR("Target screen is NULL after creation attempt");
        return;
    }

    LOG_INF("Starting screen transition to screen at %p", target_screen);
    start_screen_transition(target_screen);
}


static void create_spinning_label(lv_obj_t *parent)
{
    if (parent == NULL) {
        LOG_ERR("Cannot create spinning label: parent is NULL");
        return;
    }

    lv_obj_t *label = lv_label_create(parent);
    if (label == NULL) {
        LOG_ERR("Failed to create spinning label");
        return;
    }

    lv_label_set_text(label, "*");
    lv_obj_center(label);

    // Create x position animation
    lv_anim_t a_x;
    lv_anim_init(&a_x);
    lv_anim_set_var(&a_x, label);
    lv_anim_set_exec_cb(&a_x, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_values(&a_x, lv_obj_get_x(label) - 20, lv_obj_get_x(label) + 20);
    lv_anim_set_time(&a_x, 1000);
    lv_anim_set_path_cb(&a_x, lv_anim_path_ease_in_out);
    lv_anim_set_repeat_count(&a_x, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_playback_time(&a_x, 1000);

    // Create y position animation
    lv_anim_t a_y;
    lv_anim_init(&a_y);
    lv_anim_set_var(&a_y, label);
    lv_anim_set_exec_cb(&a_y, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&a_y, lv_obj_get_y(label) - 20, lv_obj_get_y(label) + 20);
    lv_anim_set_time(&a_y, 800);
    lv_anim_set_path_cb(&a_y, lv_anim_path_ease_in_out);
    lv_anim_set_repeat_count(&a_y, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_playback_time(&a_y, 800);

    lv_anim_start(&a_x);
    lv_anim_start(&a_y);
}

static void create_main_screen(void)
{
    if (main_screen != NULL) {
        LOG_WRN("Main screen already exists");
        return;
    }

    LOG_INF("Creating main screen");
    main_screen = lv_obj_create(NULL);
    if (main_screen == NULL) {
        LOG_ERR("Failed to create main screen");
        return;
    }

    // Title
    lv_obj_t *title = lv_label_create(main_screen);
    if (title == NULL) {
        LOG_ERR("Failed to create title label");
        return;
    }
    
    lv_label_set_text(title, "Main Screen");
    lv_obj_add_style(title, &style_title, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);

    // Navigation button
    lv_obj_t *btn = lv_btn_create(main_screen);
    if (btn == NULL) {
        LOG_ERR("Failed to create button");
        return;
    }
    
    lv_obj_add_style(btn, &style_btn, 0);
    lv_obj_set_size(btn, 140, 50);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -20);
    
    // Store second_screen pointer separately for the callback
    lv_obj_add_event_cb(btn, switch_screen_cb, LV_EVENT_CLICKED, &second_screen);

    lv_obj_t *btn_label = lv_label_create(btn);
    if (btn_label == NULL) {
        LOG_ERR("Failed to create button label");
        return;
    }
    
    lv_label_set_text(btn_label, "Next Screen");
    lv_obj_center(btn_label);

    // Add spinning animation
    create_spinning_label(main_screen);
}

static void create_second_screen(void)
{
    if (second_screen != NULL) {
        LOG_WRN("Second screen already exists");
        return;
    }

    LOG_INF("Creating second screen");
    second_screen = lv_obj_create(NULL);
    if (second_screen == NULL) {
        LOG_ERR("Failed to create second screen");
        return;
    }

    // Title
    lv_obj_t *title = lv_label_create(second_screen);
    if (title == NULL) {
        LOG_ERR("Failed to create title");
        return;
    }
    
    lv_label_set_text(title, "Second Screen");
    lv_obj_add_style(title, &style_title, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);

    // Create a slider
    lv_obj_t *slider = lv_slider_create(second_screen);
    if (slider == NULL) {
        LOG_ERR("Failed to create slider");
        return;
    }
    
    lv_obj_set_size(slider, 200, 10);
    lv_obj_center(slider);
    
    // Navigation buttons
    lv_obj_t *btn_prev = lv_btn_create(second_screen);
    if (btn_prev == NULL) {
        LOG_ERR("Failed to create previous button");
        return;
    }
    
    lv_obj_add_style(btn_prev, &style_btn, 0);
    lv_obj_set_size(btn_prev, 100, 50);
    lv_obj_align(btn_prev, LV_ALIGN_BOTTOM_LEFT, 20, -20);
    lv_obj_add_event_cb(btn_prev, switch_screen_cb, LV_EVENT_CLICKED, &main_screen);

    lv_obj_t *btn_next = lv_btn_create(second_screen);
    if (btn_next == NULL) {
        LOG_ERR("Failed to create next button");
        return;
    }
    
    lv_obj_add_style(btn_next, &style_btn, 0);
    lv_obj_set_size(btn_next, 100, 50);
    lv_obj_align(btn_next, LV_ALIGN_BOTTOM_RIGHT, -20, -20);
    lv_obj_add_event_cb(btn_next, switch_screen_cb, LV_EVENT_CLICKED, &third_screen);

    // Button labels
    lv_obj_t *label_prev = lv_label_create(btn_prev);
    if (label_prev == NULL) {
        LOG_ERR("Failed to create previous button label");
        return;
    }
    
    lv_label_set_text(label_prev, "Previous");
    lv_obj_center(label_prev);

    lv_obj_t *label_next = lv_label_create(btn_next);
    if (label_next == NULL) {
        LOG_ERR("Failed to create next button label");
        return;
    }
    
    lv_label_set_text(label_next, "Next");
    lv_obj_center(label_next);
}

static void create_third_screen(void)
{
    if (third_screen != NULL) {
        LOG_WRN("Third screen already exists");
        return;
    }

    LOG_INF("Creating third screen");
    third_screen = lv_obj_create(NULL);
    if (third_screen == NULL) {
        LOG_ERR("Failed to create third screen");
        return;
    }

    // Title
    lv_obj_t *title = lv_label_create(third_screen);
    if (title == NULL) {
        LOG_ERR("Failed to create title");
        return;
    }
    
    lv_label_set_text(title, "Third Screen");
    lv_obj_add_style(title, &style_title, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);

    // Create a chart
    lv_obj_t *chart = lv_chart_create(third_screen);
    if (chart == NULL) {
        LOG_ERR("Failed to create chart");
        return;
    }
    
    lv_obj_set_size(chart, 200, 150);
    lv_obj_center(chart);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    
    lv_chart_series_t *ser = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
    if (ser == NULL) {
        LOG_ERR("Failed to create chart series");
        return;
    }
    
    // Set some sample data
    for(int i = 0; i < 10; i++) {
        lv_chart_set_next_value(chart, ser, lv_rand(10, 90));
    }
    
    // Navigation button
    lv_obj_t *btn = lv_btn_create(third_screen);
    if (btn == NULL) {
        LOG_ERR("Failed to create button");
        return;
    }
    
    lv_obj_add_style(btn, &style_btn, 0);
    lv_obj_set_size(btn, 140, 50);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_add_event_cb(btn, switch_screen_cb, LV_EVENT_CLICKED, &main_screen);

    lv_obj_t *btn_label = lv_label_create(btn);
    if (btn_label == NULL) {
        LOG_ERR("Failed to create button label");
        return;
    }
    
    lv_label_set_text(btn_label, "Back to Start");
    lv_obj_center(btn_label);
}

static void create_screens_on_demand(void)
{
    // Only create main screen initially
    LOG_INF("Creating main screen");
    create_main_screen();
}

void main(void)
{
    const struct device *display_dev;

    LOG_INF("Starting application...");
    k_msleep(1000);  // 1 second delay to ensure we see initial logs

    // Initialize display
    display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
    if (!device_is_ready(display_dev)) {
        LOG_ERR("Device %s not ready", display_dev->name);
        return;
    }

    // Log initial memory status
    log_memory_status();

    LOG_INF("Initializing styles...");
    init_theme_styles();

    LOG_INF("Creating initial screen");
    create_screens_on_demand();  // Only creates main screen

    if (main_screen == NULL) {
        LOG_ERR("Failed to create main screen");
        return;
    }

    LOG_INF("Loading main screen...");
    lv_scr_load(main_screen);

    LOG_INF("Entering main loop...");
    display_blanking_off(display_dev);

    while (1) {
        lv_task_handler();
        k_msleep(5);  // 5ms delay between updates
        
        // Optional: Add some debug logging periodically
        static int count = 0;
        if (++count >= 200) {  // Every second (200 * 5ms = 1000ms)
            LOG_INF("Main loop running...");
            log_memory_status();  // Log memory status periodically
            count = 0;
        }
    }
}