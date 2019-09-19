#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <getopt.h>
#include <boilerplate/ancillaries.h>
//#include <alchemy/task.h>
//#include <alchemy/timer.h>
#include <rtdm/can.h>
//#include <net/if.h>
//#include <linux/can.h>
//#include <linux/can/raw.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <sys/ioctl.h>
//extern int optind, opterr, optopt;
//static void print_usage(char *prg)
//{
//    fprintf(stderr,
//            "Usage: %s <can-interface> [Options] <can-msg>\n"
//            "<can-msg> can consist of up to 8 bytes given as a space separated list\n"
//            "Options:\n"
//            " -i, --identifier=ID   CAN Identifier (default = 1)\n"
//            " -r  --rtr             send remote request\n"
//            " -e  --extended        send extended frame\n"
//            " -l  --loop=COUNT      send message COUNT times\n"
//            " -c, --count           message count in data[0-3]\n"
//            " -d, --delay=MS        delay in ms (default = 1ms)\n"
//            " -s, --send            use send instead of sendto\n"
//            " -t, --timeout=MS      timeout in ms\n"
//            " -L, --loopback=0|1    switch local loopback off or on\n"
//            " -v, --verbose         be verbose\n"
//            " -p, --print=MODULO    print every MODULO message\n"
//            " -h, --help            this help\n",
//            prg);
//}
//RT_TASK rt_task_desc;
//static int s=-1, dlc=0, rtr=0, extended=0, verbose=0, loops=1;
//static SRTIME delay=1000000;
//static int count=0, print=1, use_send=0, loopback=-1;
//static nanosecs_rel_t timeout = 0;
static struct can_frame frame;
static int s;
//static struct sockaddr_can to_addr;
//static void cleanup(void)
//{
//    int ret;
//    if (verbose)
//        printf("Cleaning up...\n");
//    usleep(100000);
//    if (s >= 0) {
//        ret = close(s);
//        s = -1;
//        if (ret) {
//            fprintf(stderr, "close: %s\n", strerror(-ret));
//        }
//        exit(EXIT_SUCCESS);
//    }
//}
//static void cleanup_and_exit(int sig)
//{
//    if (verbose)
//        printf("Signal %d received\n", sig);
//    cleanup();
//    exit(0);
//}
//static void rt_task(void)
//{
//    int i, j, ret;
//    for (i = 0; i < loops; i++) {
//        rt_task_sleep(rt_timer_ns2ticks(delay));
//        if (count)
//            memcpy(&frame.data[0], &i, sizeof(i));
//        /* Note: sendto avoids the definiton of a receive filter list */
//        if (use_send)
//            ret = send(s, (void *)&frame, sizeof(can_frame_t), 0);
//        else
//            ret = sendto(s, (void *)&frame, sizeof(can_frame_t), 0,
//                         (struct sockaddr *)&to_addr, sizeof(to_addr));
//        if (ret < 0) {
//            switch (ret) {
//                case -ETIMEDOUT:
//                    if (verbose)
//                        printf("send(to): timed out");
//                    break;
//                case -EBADF:
//                    if (verbose)
//                        printf("send(to): aborted because socket was closed");
//                    break;
//                default:
//                    fprintf(stderr, "send: %s\n", strerror(-ret));
//                    break;
//            }
//            i = loops;          /* abort */
//            break;
//        }
//        if (verbose && (i % print) == 0) {
//            if (frame.can_id & CAN_EFF_FLAG)
//                printf("<0x%08x>", frame.can_id & CAN_EFF_MASK);
//            else
//                printf("<0x%03x>", frame.can_id & CAN_SFF_MASK);
//            printf(" [%d]", frame.can_dlc);
//            for (j = 0; j < frame.can_dlc; j++) {
//                printf(" %02x", frame.data[j]);
//            }
//            printf("\n");
//        }
//    }
//}

int main(int argc, char **argv)
{
//    int i, opt, ret;
    struct can_ifreq ifr;
//    char name[32];

    static struct sockaddr_can addr;
//    static struct ifreq ifr;
    static const char *ifname = "slcan0";

    if ((s=socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0){
        perror(("Error while opening socket"));
        return -1;
    }

    strcpy(ifr.ifr_name, ifname);
    ioctl(s, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

//    memset(&addr, 0, sizeof(addr));
    if(bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0){
        perror("Error in socket bind");
        return -2;
    }

    frame.can_id = 0x000;
    frame.can_dlc = 2;
    frame.data[0] = 0x81;
    frame.data[1] = 0x00;
    write(s, &frame, sizeof(struct can_frame));
    usleep(10000000);

    frame.can_id = 0x000;
    frame.can_dlc = 2;
    frame.data[0] = 0x01;
    frame.data[1] = 0x00;
    write(s, &frame, sizeof(struct can_frame));
    usleep(1000000);

    frame.can_id = 0x000;
    frame.can_dlc = 2;
    frame.data[0] = 0x82;
    frame.data[1] = 0x00;
    write(s, &frame, sizeof(struct can_frame));
    usleep(1000000);



//
//
//
//
//
//    if (optind == argc) {
//        print_usage(argv[0]);
//        exit(0);
//    }
//    if (argv[optind] == NULL) {
//        fprintf(stderr, "No Interface supplied\n");
//        exit(-1);
//    }
//    if (verbose)
//        printf("interface %s\n", argv[optind]);
//    ret = socket(PF_CAN, SOCK_RAW, CAN_RAW);
//    if (ret < 0) {
//        fprintf(stderr, "socket: %s\n", strerror(-ret));
//        return -1;
//    }
//    s = ret;
//
//    if (loopback >= 0) {
//        ret = setsockopt(s, SOL_CAN_RAW, CAN_RAW_LOOPBACK,
//                         &loopback, sizeof(loopback));
//        if (ret < 0) {
//            fprintf(stderr, "setsockopt: %s\n", strerror(-ret));
//            goto failure;
//        }
//        if (verbose)
//            printf("Using loopback=%d\n", loopback);
//    }
//    namecpy(ifr.ifr_name, argv[optind]);
//    if (verbose)
//        printf("s=%d, ifr_name=%s\n", s, ifr.ifr_name);
//    ret = ioctl(s, SIOCGIFINDEX, &ifr);
//    if (ret < 0) {
//        fprintf(stderr, "ioctl: %s\n", strerror(-ret));
//        goto failure;
//    }
//    memset(&to_addr, 0, sizeof(to_addr));
//    to_addr.can_ifindex = ifr.ifr_ifindex;
//    to_addr.can_family = AF_CAN;
//    if (use_send) {
//        /* Suppress definiton of a default receive filter list */
////        ret = setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);
////        if (ret < 0) {
////            fprintf(stderr, "setsockopt: %s\n", strerror(-ret));
////            goto failure;
////        }
//        ret = bind(s, (struct sockaddr *)&to_addr, sizeof(to_addr));
//        if (ret < 0) {
//            fprintf(stderr, "bind: %s\n", strerror(-ret));
//            goto failure;
//        }
//    }
//    if (count)
//        frame.can_dlc = sizeof(int);
//    else {
//        for (i = optind + 1; i < argc; i++) {
//            frame.data[dlc] = strtoul(argv[i], NULL, 0);
//            dlc++;
//            if( dlc == 8 )
//                break;
//        }
//        frame.can_dlc = dlc;
//    }
//    if (rtr)
//        frame.can_id |= CAN_RTR_FLAG;
//    if (extended)
//        frame.can_id |= CAN_EFF_FLAG;
//    if (timeout) {
//        if (verbose)
//            printf("Timeout: %lld ns\n", (long long)timeout);
//        ret = ioctl(s, RTCAN_RTIOC_SND_TIMEOUT, &timeout);
//        if (ret) {
//            fprintf(stderr, "ioctl SND_TIMEOUT: %s\n", strerror(-ret));
//            goto failure;
//        }
//    }
//    snprintf(name, sizeof(name), "rtcansend-%d", getpid());
//    ret = rt_task_shadow(&rt_task_desc, name, 1, 0);
//    if (ret) {
//        fprintf(stderr, "rt_task_shadow: %s\n", strerror(-ret));
//        goto failure;
//    }
//    rt_task();
//    cleanup();
    return 0;
//    failure:
//    cleanup();
//    return -1;
}