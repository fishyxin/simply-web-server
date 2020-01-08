#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>

#include "constant.h"
#include "start_server.h"
#include "handle_request.h"

int main(void) {
    int http_fd = 0;
    int create_thread_res = 0;
    pthread_t thread;

    // 调用启动函数
    http_fd = start_server(SERVER_PORT, MAX_CONCURENCY_NUM);

    if (http_fd == -1) {
        printf("Server start fail!");
        return 0;
    }

    while (1) {
        // 接收请求(因为无需关心客户端地址，后面两个参数为NULL)
        int client_fd = accept(http_fd, NULL, NULL);

        // 遇到无法处理的请求
        if (client_fd == -1) {
            printf("Accept request fail!");
            continue ;
        }

        // 开始新线程处理请求(成功返回0)
        create_thread_res = pthread_create(&thread, NULL, handle_request, client_fd);
        if (create_thread_res != 0) {
            printf("Create thread fail!");
            continue ;
        } 
    }

    close(http_fd);

    return 0;
}