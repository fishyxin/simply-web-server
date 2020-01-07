/*
 * 该文件用来定义响应相关函数
 */


/*
 * 设置响应报文首部的状态行
 * @param header: 首部  status: 状态码
 */
void set_response_header_status(int client_fd, char *header, int status);

void set_response_header_status(int client_fd, char *header, int status) {
    // 只列举常用状态码，默认为200
    switch (status) {
        case 204:
            sprintf(header, "HTTP/1.0 %d %s\r\n", 204, "No Content");
            break;
        case 400:
            sprintf(header, "HTTP/1.0 %d %s\r\n", 400, "Bad Request");
            break;
        case 401:
            sprintf(header, "HTTP/1.0 %d %s\r\n", 401, "Unauthorized");
            break;
        case 403:
            sprintf(header, "HTTP/1.0 %d %s\r\n", 403, "Forbidden");
            break;
        case 404:
            sprintf(header, "HTTP/1.0 %d %s\r\n", 404, "Not Found");
            break;
        case 500:
            sprintf(header, "HTTP/1.0 %d %s\r\n", 500, "Internal Server Error");
            break;
        default:
            sprintf(header, "HTTP/1.0 %d %s\r\n", 200, "OK");
            break;
    }
    write(client_fd, header, strlen(header));
}

/*
 * 设置响应报文的Content-type
 * @parma header: 首部  value: content-type的值
 */
void set_response_header_content_type(int client_fd, char *header, char *value);

void set_response_header_content_type(int client_fd, char *header, char *value) {
    sprintf(header, "%sContent-type: %s\r\n\r\n", header, value);
    write(client_fd, header, strlen(header));
}