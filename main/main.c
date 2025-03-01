#include "esp_lvgl_port.h"
#include "lvgl.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_err.h>

static const char *TAG = "MAIN";

void app_main(void)
{
    esp_err_t ret;
    ESP_LOGI(TAG, "Initializing LVGL port...");

    const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    esp_err_t err = lvgl_port_init(&lvgl_cfg);

    /* 
     * Initialisiert den LVGL-Port mit der Standardkonfiguration.
     * Die Macro ESP_LVGL_PORT_INIT_CONFIG() liefert ein Default-Configuration-Objekt,
     * das in der esp_lvgl_port-Komponente definiert ist. Über Menuconfig kannst du
     * hier auch Anpassungen vornehmen.
     */
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize LVGL port");
        return;
    }

    static lv_disp_t * disp_handle;

    /* LCD IO */
	esp_lcd_panel_io_handle_t io_handle = NULL;
	ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t) 1, &io_config, &io_handle));

    /* LCD driver initialization */
    esp_lcd_panel_handle_t lcd_panel_handle;
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &lcd_panel_handle));

    /* Add LCD screen */
    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io_handle,
        .panel_handle = lcd_panel_handle,
        .buffer_size = DISP_WIDTH*DISP_HEIGHT,
        .double_buffer = true,
        .hres = DISP_WIDTH,
        .vres = DISP_HEIGHT,
        .monochrome = false,
        .mipi_dsi = false,
        .color_format = LV_COLOR_FORMAT_RGB565,
        .rotation = {
            .swap_xy = false,
            .mirror_x = false,
            .mirror_y = false,
        },
        .flags = {
            .buff_dma = true,
            .swap_bytes = false,
        }
    };
    disp_handle = lvgl_port_add_disp(&disp_cfg);

    /* ... the rest of the initialization ... */

    /* If deinitializing LVGL port, remember to delete all displays: */
    lvgl_port_remove_disp(disp_handle);

    
    
    /* 
     * Erstelle ein einfaches LVGL Widget, z.B. ein Label,
     * das den Text anzeigt. 
     */
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Hello, esp_lvgl_port & ST7735!");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    ESP_LOGI(TAG, "LVGL port initialized. Entering main loop...");
    while (1) {
        lv_timer_handler();  // Bearbeitet LVGL Timers, Animationen und Rendering
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
