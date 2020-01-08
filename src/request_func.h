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

/*
 * 判断是否为get请求
 * @param  method: 方法
 */
int is_get_request(char *method);

int is_get_request(char *method) {
    if (strcmp(method, "GET") == 0) {
        return 1;
    } else {
        return 0;
    }
}

/*
 * 判读是否为post请求
 * @param  method: 方法
 */
int is_post_request(char *method);

int is_post_request(char *method) {
    if (strcmp(method, "POST") == 0) {
        return 1;
    } else {
        return 0;
    }
}

/*
 * 判断路径是否等于指定路径
 * @param path1  path2
 */
int path_equal(char *path1, char *path2);

int path_equal(char *path1, char *path2) {
    if (strcmp(path1, path2) == 0) {
        return 1;
    } else {
        return 0;
    }
}

/*
 * 解析uri中的文件路径和查询字符串
 */
void parse_path_and_querystring(char *uri, char *path, char *querystring);
void parse_path_and_querystring(char *uri, char *path, char *querystring) {
    char *result = NULL;
    result = strtok(uri, "?");
    if (result != NULL) {
        strcpy(path, result);
    } else {
        strcpy(path, "/");
    }

    result = strtok(NULL, "?");
    if (result != NULL) {
        strcpy(querystring, result);
    } else {
        strcpy(querystring, "");
    }
}