#include "app_model.hpp"

#include "who_camera.h"
#include "my_model.hpp"
#include "esp_log.h"

static QueueHandle_t xQueueFrameI = NULL;
static QueueHandle_t xQueueFrameO = NULL;
static QueueHandle_t xQueueResultO = NULL;
static QueueHandle_t xQueueFrameExecute = NULL;

static MyModel *model;

static void task_model_execute_handler(void *arg)
{
    camera_fb_t *frame = NULL;

    while (true)
    {
        if (xQueueReceive(xQueueFrameExecute, &frame, portMAX_DELAY))
        {
            ESP_LOGI("MODEL", "Start");
            dl::tool::Latency latency;
            latency.start();
            float result = model->execute(frame->buf + ((240 - 160) / 2 * 320)) / 100.0f;
            latency.end();
            latency.print("MODEL", "execute");
            ESP_LOGI("MODEL", "Result: '%f'", result);
            xQueueOverwrite(xQueueResultO, &result);
        }
    }
}

static void task_model_process_handler(void *arg)
{
    camera_fb_t *frame = NULL;

    while (true)
    {
        if (xQueueReceive(xQueueFrameI, &frame, portMAX_DELAY))
        {
            xQueueSend(xQueueFrameExecute, &frame, 0);
            xQueueSend(xQueueFrameO, &frame, portMAX_DELAY);
        }
    }
}

void register_model(const QueueHandle_t frame_i, const QueueHandle_t frame_o, const QueueHandle_t result_o) {
    xQueueFrameI = frame_i;
    xQueueFrameO = frame_o;
    xQueueResultO = result_o;
    xQueueFrameExecute = xQueueCreate(1, sizeof(camera_fb_t *));

    model = new MyModel();

    xTaskCreatePinnedToCore(task_model_execute_handler, "model_execute", 4 * 1024, NULL, 5, NULL, 0);
    xTaskCreate(task_model_process_handler, "model_process", 4 * 1024, NULL, 3, NULL);
}