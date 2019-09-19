#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>

#include <alchemy/task.h>
#include <alchemy/timer.h>
#include <alchemy/sem.h>

#include <xenomai/init.h>

//#include <rtdk.h>
RT_TASK demo_task;

void demo(void *arg)
{
    RT_TASK *curtask;
    RT_TASK_INFO curtaskinfo;

    rt_printf("Hello World!\n");

    curtask=rt_task_self();
    rt_task_inquire(curtask, &curtaskinfo);

    rt_printf("Task name: %s \n", curtaskinfo.name);
}

int main(int argc, char* argv[])
{
    char str[10];

//    rt_print_auto_init(1);
    mlockall(MCL_CURRENT|MCL_FUTURE);

    rt_printf("start task\n");

    sprintf(str,"hello");
    rt_task_create(&demo_task, str, 0, 50, 0);
    rt_task_start(&demo_task, &demo, 0);
}