#pragma once

#include "../kernel/task.h"

// Start the first task from kmain (never returns)
void resume_new_task(task_t* task);