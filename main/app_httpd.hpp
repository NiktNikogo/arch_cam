#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

void register_my_httpd(const QueueHandle_t frame_i, const QueueHandle_t frame_o, const QueueHandle_t result_i, const bool return_fb);
