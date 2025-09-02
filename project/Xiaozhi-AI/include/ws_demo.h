#ifndef __WS_DEMO_H__
#define __WS_DEMO_H__

#include "xiaozhi_api.h"
#include "stdbool.h"
typedef void (*message_recive_callback_t)(void *arg, char *data);

extern bool USE_VAD;
int mic_record_report_callback(char *data, int len);
void start_listen();
int xiaozhi_active(active_info_t *activeInfo);
void delete_xiaozhi_task(void);
#endif