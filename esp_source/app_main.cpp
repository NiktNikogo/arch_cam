#include "who_camera.h"
#include "app_wifi.h"
#include "app_httpd.hpp"
#include "app_model.hpp"

static QueueHandle_t xQueueFrame1 = NULL;
static QueueHandle_t xQueueFrame2 = NULL;
static QueueHandle_t xQueueResult = NULL;

extern "C" void app_main()
{
    app_wifi_main();

    xQueueFrame1 = xQueueCreate(1, sizeof(camera_fb_t *));
    xQueueFrame2 = xQueueCreate(1, sizeof(camera_fb_t *));
    xQueueResult = xQueueCreate(1, sizeof(float));

    float result = 0.0f;
    xQueueOverwrite(xQueueResult, &result);

    register_camera(PIXFORMAT_GRAYSCALE, FRAMESIZE_QVGA, 1, xQueueFrame1);
    register_model(xQueueFrame1, xQueueFrame2, xQueueResult);
    register_my_httpd(xQueueFrame2, NULL, xQueueResult, true);
}
