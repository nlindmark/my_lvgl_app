#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <lvgl.h>


#define LOG_LEVEL LOG_LEVEL_INF
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);



static lv_style_t style_btn;
static lv_style_t style_title;

static lv_obj_t *main_screen = NULL;
static lv_obj_t *second_screen = NULL;
static lv_obj_t *third_screen = NULL;

// Function declarations
static void switch_screen_cb(lv_event_t *e);
static void create_main_screen(void);
static void create_second_screen(void);
static void create_third_screen(void);


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
	//lv_style_set_text_font_scale(&style_title, 150);  // 150% scaling
    lv_style_set_text_color(&style_title, lv_color_make(40, 40, 40));
}

// Animation for screen switching
static void start_screen_transition(lv_obj_t *target_screen)
{
    lv_scr_load_anim(target_screen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 500, 0, false);
}



static void switch_screen_cb(lv_event_t *e)
{
    lv_obj_t *target = (lv_obj_t *)lv_event_get_user_data(e);
    
    LOG_INF("Switch screen requested");

    // Create screens just before switching to them
    if (target == second_screen && second_screen == NULL) {
        LOG_INF("Creating second screen on demand");
        create_second_screen();
    }
    else if (target == third_screen && third_screen == NULL) {
        LOG_INF("Creating third screen on demand");
        create_third_screen();
    }
    
    LOG_INF("Starting screen transition");
    start_screen_transition(target);
}


static void create_spinning_label(lv_obj_t *parent)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, "*");
    lv_obj_center(label);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, label);
    // Use y position animation instead of rotation
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&a, lv_obj_get_y(label) - 20, lv_obj_get_y(label) + 20);  // Move up and down
    lv_anim_set_time(&a, 1000);  // 1 second
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_playback_time(&a, 1000);  // Smooth return
    lv_anim_start(&a);
}
// Create main screen
static void create_main_screen(void)
{
    main_screen = lv_obj_create(NULL);

    // Title
    lv_obj_t *title = lv_label_create(main_screen);
    lv_label_set_text(title, "Main Screen");
    lv_obj_add_style(title, &style_title, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);

    // Navigation button
    lv_obj_t *btn = lv_btn_create(main_screen);
    lv_obj_add_style(btn, &style_btn, 0);
    lv_obj_set_size(btn, 140, 50);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_add_event_cb(btn, switch_screen_cb, LV_EVENT_CLICKED, second_screen);

    lv_obj_t *btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Next Screen");
    lv_obj_center(btn_label);

    // Add spinning animation
    create_spinning_label(main_screen);
}

// Create second screen with slider
static void create_second_screen(void)
{
    second_screen = lv_obj_create(NULL);

    // Title
    lv_obj_t *title = lv_label_create(second_screen);
    lv_label_set_text(title, "Second Screen");
    lv_obj_add_style(title, &style_title, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);

    // Create a slider
    lv_obj_t *slider = lv_slider_create(second_screen);
    lv_obj_set_size(slider, 200, 10);
    lv_obj_center(slider);
    
    // Navigation buttons
    lv_obj_t *btn_prev = lv_btn_create(second_screen);
    lv_obj_add_style(btn_prev, &style_btn, 0);
    lv_obj_set_size(btn_prev, 100, 50);
    lv_obj_align(btn_prev, LV_ALIGN_BOTTOM_LEFT, 20, -20);
    lv_obj_add_event_cb(btn_prev, switch_screen_cb, LV_EVENT_CLICKED, main_screen);

    lv_obj_t *btn_next = lv_btn_create(second_screen);
    lv_obj_add_style(btn_next, &style_btn, 0);
    lv_obj_set_size(btn_next, 100, 50);
    lv_obj_align(btn_next, LV_ALIGN_BOTTOM_RIGHT, -20, -20);
    lv_obj_add_event_cb(btn_next, switch_screen_cb, LV_EVENT_CLICKED, third_screen);

    lv_obj_t *label_prev = lv_label_create(btn_prev);
    lv_label_set_text(label_prev, "Previous");
    lv_obj_center(label_prev);

    lv_obj_t *label_next = lv_label_create(btn_next);
    lv_label_set_text(label_next, "Next");
    lv_obj_center(label_next);
}

// Create third screen with chart
static void create_third_screen(void)
{
    third_screen = lv_obj_create(NULL);

    // Title
    lv_obj_t *title = lv_label_create(third_screen);
    lv_label_set_text(title, "Third Screen");
    lv_obj_add_style(title, &style_title, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);

    // Create a chart
    lv_obj_t *chart = lv_chart_create(third_screen);
    lv_obj_set_size(chart, 200, 150);
    lv_obj_center(chart);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    
    lv_chart_series_t *ser = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
    
    // Set some sample data
    for(int i = 0; i < 10; i++) {
        lv_chart_set_next_value(chart, ser, lv_rand(10, 90));
    }
    
    // Navigation button
    lv_obj_t *btn = lv_btn_create(third_screen);
    lv_obj_add_style(btn, &style_btn, 0);
    lv_obj_set_size(btn, 140, 50);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_add_event_cb(btn, switch_screen_cb, LV_EVENT_CLICKED, main_screen);

    lv_obj_t *btn_label = lv_label_create(btn);
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

    display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
    if (!device_is_ready(display_dev)) {
        LOG_ERR("Device %s not ready", display_dev->name);
        return;
    }

    LOG_INF("Initializing styles...");
    init_theme_styles();

    LOG_INF("Creating initial screen");
    create_screens_on_demand();  // Only creates main screen

    LOG_INF("Loading main screen...");
    lv_scr_load(main_screen);

    LOG_INF("Entering main loop...");
    display_blanking_off(display_dev);

    while (1) {
        lv_timer_handler();
        k_msleep(5);  // 5ms delay between updates
        
        // Optional: Add some debug logging periodically
        static int count = 0;
        if (++count >= 200) {  // Every second (200 * 5ms = 1000ms)
            LOG_INF("Main loop running...");
            count = 0;
        }
    }
}