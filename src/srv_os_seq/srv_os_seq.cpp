#include "srv_os_seq.h"

#include "app_lab_1_2/app_lab_1_2.h"
#include "app_lab_2_1/app_lab_2_1_task_1.h" 
#include "app_lab_2_1/app_lab_2_1_task_2.h"
#include "app_lab_2_1/app_lab_2_1_task_3.h"
#include <stdio.h>

#include "timer-api.h"


void srv_os_seq_setup() {
    app_lab_1_2_setup();
    app_lab_2_1_task_1_setup();
    app_lab_2_1_task_2_setup();
    app_lab_2_1_task_3_setup();
}

void srv_os_seq_run() {
    app_lab_1_2_run();
    app_lab_2_1_task_1_run();
    app_lab_2_1_task_2_run();
    app_lab_2_1_task_3_run();
}
