/*
 * 该文件定义请求相关函数
 */


/*
 * 解析请求的请求行
 * @param request: 请求报文  method: 方法    path: 请求资源路径
 */
void parse_request_header_row(char *request, char *method, char *path);

void parse_request_header_row(char *request, char *method, char *path) {
    char version[1024];
    sscanf(request, "%s %s %s", method, path, version);
}