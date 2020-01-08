#include "request_func.h"
#include "response_func.h"

/*
 * 处理http请求
 * @param client_fd: http请求文件描述符
 */
void handle_request(int client_fd);

void handle_request(int client_fd) {
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

