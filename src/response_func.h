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

/*
 * 响应静态文件(响应体设置为静态文件)
 * @param filename 文件名
 */
void set_response_body_static_file(int client_fd, char *filename);

void set_response_body_static_file(int client_fd, char *filename) {
    struct stat sbuf;

    // 获取文件状态
    int file_status = stat(filename, &sbuf);
    // 文件存在，放在外层判断，同步设置状态行


    // 打开文件
    int static_file_fd = open(filename, O_RDONLY);
    char *srcp = mmap(0, sbuf.st_size, PROT_READ, MAP_PRIVATE, static_file_fd, 0);

    // 关闭文件
    close(static_file_fd);

    // 写到到响应体
    write(client_fd, srcp, sbuf.st_size);

    // 清空空间
    munmap(srcp, sbuf.st_size);
}

/* 
 * 执行cgi程序
 *
 */
void excute_cgi(int client_fd, char *data);

void excute_cgi(int client_fd, char *data) {
    int cgi_output[2];
    int cgi_input[2];
    pid_t pid;
    int status;
    char header[1024];

    if (pipe(cgi_output) < 0 || pipe(cgi_output) < 0 || (pid = fork()) < 0) {
        // 返回服务器器内部错误
        set_response_header_status(client_fd, header, 500);
        set_response_header_content_type(client_fd, header, "text/html");
        set_response_body_static_file(client_fd, "./error/500.html");
        return ;
    }

    if (pid == 0) { // 子进程(执行cgi程序，将结果写回管道)
        char env_content[BUF_MAXSIZE];

        // 重定向管道输入输出
        dup2(cgi_output[1], 1);
        dup2(cgi_input[0], 0);
        close(cgi_output[0]);
        close(cgi_input[1]);

        // 将数据写入环境变量
        sprintf(env_content, "DATA=%s", data);
        putenv(env_content);

        // 执行cgi文件
        execl("./cgi", "./cgi", NULL);
        // 退出子进程
        exit(0);
    } else {    // 父进程
        close(cgi_output[1]);
        close(cgi_input[0]);

        set_response_header_status(client_fd, header, 200);
        set_response_header_content_type(client_fd, header, "text/html");

        char buf[BUF_MAXSIZE];
        int n;
        
        // 将管道的输出写入到响应
        while ((n = read(cgi_output[0], &buf, BUF_MAXSIZE)) > 0) {
            write(client_fd, buf, n);
        }

        close(cgi_output[0]);
        close(cgi_input[1]);
        waitpid(pid, &status, 0);
        close(client_fd);
    }

}