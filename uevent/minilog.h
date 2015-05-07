#ifndef MINI_LOG_H
#define MINI_LOG_H

void mini_log_printf(const char *fmt, ...);
#define LOGE mini_log_printf
#define LOG_VOL LOGE

#endif