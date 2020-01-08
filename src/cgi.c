#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#define SIZE 1024

static void math_data(char* data)
{
//	assert(data);

	char* arr[3];
	int i=0;
	char* start = data;
	//METHOD=...&...&...&...
	while(*start)
	{
	perror("math_data !");
		if(*start == '=')
		{
			arr[i] = start+1;
			i++;
		}
		else if(*start == '&')
		{
			*start = '\0';
		}
		else{
			//DO NOTHING!!
		}
		start++;
	}
	arr[i] = NULL;

	int num1 = atoi(arr[0]);
	int num2 = atoi(arr[1]);

	perror("data1 and data2!");
	printf("<html>\n");
	printf("<body>\n");
	printf("<h1>data1 + data2 = %d</h1>\n",num1 + num2);
	printf("<h1>data1 - data2 = %d</h1>\n",num1 - num2);
	printf("<h1>data1 * data2 = %d</h1>\n",num1 * num2);
	printf("<h1>data1 / data2 = %d</h1>\n",num1 / num2);
	printf("</body>\n");
	printf("</html>\n");
}

int main()
{
//	printf("hello world!\n");
	//获取环境变量
	//GET->query_string    POST->CONTENT-LENGTH
	char method[SIZE];
	char content_len[SIZE];
	char data[SIZE];

	strcpy(data, getenv("DATA"));
	math_data(data);

	return 0;
}
