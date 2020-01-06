/*
 * 启动http服务
 * 1. 打开socket
 * 2. 绑定ip，端口
 * 3. 监听socket
 * @param port: 端口  concurrency_num: 最大并发数
 * @return socket_fd: 套接字的文件描述符
 */
int start_server(int port, int concurrency_num);

int start_server(int port, int concurrency_num) {
    int socket_fd = 0;
    struct sockaddr_in address;

    // 打开socket，AF_INET表示IPV4，SOCK_STREAM表示TCP协议
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd == -1) {
        printf("Socket error!");
        return -1;
    }

    // 地址的协议(IPV4)
    address.sin_family = AF_INET;
    // INADDR_ANY表示本机的任意地址
	address.sin_addr.s_addr = htonl(INADDR_ANY);
    // 端口
	address.sin_port = htons(port);

    // 绑定地址(ip和端口)
    if (bind(socket_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        printf("Bind error!");
        return -1;
    }

    // 监听socket
    if (listen(socket_fd, concurrency_num) == -1) {
        printf("listen error!");
        return -1;
    }

    return socket_fd;
}