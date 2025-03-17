/*
 * Copyright (C) 2025-present, Guanyou.Chen. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "monitor"

#include "log.h"
#include <sys/prctl.h>
#include <pthread.h>

static bool installed = false;
typedef void* (*monitor_pthread_entry)(void*);

static void monitor_thread(void*) {
    prctl(PR_SET_NAME, "open-monitor");
#if 0
    do {
        // todo
    } while (installed);
#endif
}

extern "C"
void __attribute__((constructor)) monitor_ctor_init() {
    JNI_LOGI("Init inject open-monitor %s environment..", __MONITOR_VERSION__);
    installed = true;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_t thread;
    pthread_create(&thread, &attr, (monitor_pthread_entry)monitor_thread, NULL);
}

extern "C"
void __attribute__((destructor)) monitor_dtor_fini() {
    JNI_LOGI("Remove open-monitor environment.");
    installed = false;
}
