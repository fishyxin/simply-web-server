## 概述

Linux的课程作业，实现一个简易的web服务器。

学习Linux和计算机网络基础后，最佳的实践就是写一个简易的web服务器。

因为个人对C++了解不多，本项目采用C编写。

支持GET、POST两种方法。

关于并发访问本项目没有采用多路复用而是采用传统的多线程方式。


## 项目目录
```
    /----
       |----error 存放错误页面
          |----400.html
          |----401.html
          |----403.html
          |----404.html
          |----405.html
          |----500.html
       |----resource 资源目录
          |----homepage.html 主页
          |----test.html
       |----src 源码文件
          |----cgi.c cgi源码
          |----constant.h 定义常量
          |----handle_request.h 定义处理请求函数
          |----main.c 主入口文件(主函数)
          |----request_func.h 定义解析请求的函数
          |----response_func.h  定义响应的函数
          |----start_server.h  定义启动函数
       |----.gitignore git忽略配置
       |----cgi 已经编译cgi程序
       |----Makefile makefile
```
## 设计思路

#### http协议

http协议是应用层协议，基于TCP协议。

http协议格式为三段

*   报文主体
*   空行
*   报文主体

建立TCP服务，可以接收客户端(浏览器)发送的http报文。

解析http报文，获取请求的数据。

按照http报文格式，响应给客户端(浏览器)。

#### 建立TCP连接

1.  调用```socket()```

2.  ```bind()```绑定地址信息(ip和端口)

3.  ```listen()```监听

4.  ```accept()```接收请求

#### 多线程

调用```accept()```接收到客户端连接后，可调用```pthread_create()```将请求交给子线程处理，不阻塞主线程，实现并发访问。



## 主要函数

#### main方法

调用函数后，在死循环中等待请求，接收到客户端请求就开启一个新线程去处理

``````c
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
``````



#### 启动函数

按照tcp流程，打开socket，绑定地址参数，监听，成功返回socket的文件描述符

```c
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
```



#### 请求处理函数

读取请求报文，解析报文首部的请求行，可读取方法与请求uri，如果方法非get和post，返回405(方法不支持)错误页面。分割uri为请求路径和查询字符串。

如果请求路径为/，则返回主页。

如果路径为/cgi，则读取查询字符串或请求体中的数据后，交给执行函数处理，返回执行结果。

其他情况，判断请求的资源存不存在，不存在，返回404页面，存在则读取静态文件返回。

```c
oid handle_request(int client_fd) {
    char request[BUF_MAXSIZE];
    char method[BUF_MAXSIZE], uri[BUF_MAXSIZE];
    char response_header[BUF_MAXSIZE];

    // 读取请求报文
    read(client_fd, request, sizeof(request));

    // 解析请求行
    parse_request_header_row(request, method, uri);
    
    // 非get和post请求，返回405
    if (!is_get_request(method) && !is_post_request(method)) {
        // 设置405响应头
        set_response_header_status(client_fd, response_header, 405);
        set_response_header_content_type(client_fd, response_header, "text/html");
        // 响应405静态文件
        set_response_body_static_file(client_fd, "./error/405.html");
        close(client_fd);
        return ;
    }

    // 解析资源路径和查询字符串
    char request_path[BUF_MAXSIZE], querystring[BUF_MAXSIZE];
    parse_path_and_querystring(uri, request_path, querystring);

    // 访问根路径，直接返回主页
    if (path_equal(request_path, "/")) {
        set_response_header_status(client_fd, response_header, 200);
        set_response_header_content_type(client_fd, response_header, "text/html");
        set_response_body_static_file(client_fd, "./resource/homepage.html");
        close(client_fd);
        return ;
    }

    // 请求路径为cgi
    if (path_equal(request_path, "/cgi")) {
        if (is_get_request(method)) {
            // 执行cgi
            excute_cgi(client_fd, querystring);
        } else {
            char request_body[BUF_MAXSIZE];
            // 读取请求体
            parse_request_body(request, request_body);
            // 执行cgi
            excute_cgi(client_fd, request_body);
        }
        return ;
    }

    // 文件路径(request_path加上目录)
    char file_path[BUF_MAXSIZE];
    sprintf(file_path, "%s%s", SERVER_DIR, request_path);

    // 获文件状态
    struct stat sbuf;
    int file_status = stat(file_path, &sbuf);

    // 文件不存在
    if (file_status < 0) {
        set_response_header_status(client_fd, response_header, 404);
        set_response_header_content_type(client_fd, response_header, "text/html");
        set_response_body_static_file(client_fd, "./error/404.html");
        close(client_fd);
        return ;
    }

    // 设置响应头
    set_response_header_status(client_fd, response_header, 200);
    set_response_header_content_type(client_fd, response_header, "text/html");
    
    // 响应静态
    set_response_body_static_file(client_fd, file_path);

    close(client_fd);
}
```



## Makefile

```makefile
http: src/main.c src/cgi.c src/constant.h src/handle_request.h src/request_func.h src/response_func.h src/start_server.h
	gcc -o app src/main.c -lpthread 

clean: 
	rm app

run: 
	./app
```

编译项目

```makefile
make http
```

运行
```makefile
make run
```
