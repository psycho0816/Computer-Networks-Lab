#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
/*添加头文件*/
#include <unistd.h>

#define BUFFER_SIZE 8192

int sockfd;  // 用于存储套接字描述符
char way[10];  // 用于存储传输方式（文件或键盘）

// 传输函数：根据用户输入的方式（文件或键盘）发送数据
void* transfer(void* arg) {
    char path[256];
    char message[BUFFER_SIZE];
    FILE *file;
    ssize_t bytes_sent;
    
    if (strcmp(way, "file") == 0) {
        printf("Now the way is file, input nothing to stop.\n");
        while (1) {
            printf("Please input the file path:\n");
            fgets(path, sizeof(path), stdin);
            path[strcspn(path, "\n")] = 0;  // 去掉路径中的换行符

            if (strlen(path) == 0) {
                char close_input[10];
                printf("Enter again to close: ");
                fgets(close_input, sizeof(close_input), stdin);
                if (strlen(close_input) == 1 && close_input[0] == '\n') {
                    break;
                } else {
                    continue;
                }
            }

            file = fopen(path, "r");
            if (file == NULL) {
                perror("File not found");
                continue;
            }

            // 读取文件内容并发送
            while (fgets(message, sizeof(message), file)) {
                size_t message_len = strlen(message);
                size_t sent = 0;
                while (sent < message_len) {
                    bytes_sent = send(sockfd, message + sent, message_len - sent, 0);
                    if (bytes_sent < 0) {
                        perror("Send failed");
                        fclose(file);
                        return NULL;
                    }
                    sent += bytes_sent;
                }
            }

            fclose(file);
        }
    } else if (strcmp(way, "keyboard") == 0) {
        printf("Now the way is standard input.\n");
        while (1) {
            printf("Please input the message:\n");
            fgets(message, sizeof(message), stdin);
            message[strcspn(message, "\n")] = 0;  // 去掉消息中的换行符

            if (strlen(message) == 0) {
                char close_input[10];
                printf("Enter again to close: ");
                fgets(close_input, sizeof(close_input), stdin);
                if (strlen(close_input) == 1 && close_input[0] == '\n') {
                    break;
                } else {
                    continue;
                }
            }

            // 发送消息
            size_t message_len = strlen(message);
            size_t sent = 0;
            while (sent < message_len) {
                bytes_sent = send(sockfd, message + sent, message_len - sent, 0);
                if (bytes_sent < 0) {
                    perror("Send failed");
                    return NULL;
                }
                sent += bytes_sent;
            }
        }
    } else {
        printf("ERROR: Wrong way!\n");
        exit(1);
    }

    close(sockfd);
    return NULL;
}

// 接收函数：不断监听服务器消息并打印
void* receive(void* arg) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;
    
    while (1) {
        bytes_received = recv(sockfd, buffer, sizeof(buffer), 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';  // 确保字符串终止
            printf("——————————————————————\n");
            printf("Broadcast:\n%s\n", buffer);
            printf("——————————————————————\n");
        } else if (bytes_received == 0) {
            break;  // 连接关闭
        } else {
            perror("Recv failed");
            break;
        }
    }

    close(sockfd);
    return NULL;
}

int main() {
    struct sockaddr_in server_addr;
    pthread_t transfer_thread, receive_thread;
    char address[100];
    int port;

    // 获取用户输入
    printf("The way you want to input is file or keyboard?\n");
    fgets(way, sizeof(way), stdin);
    way[strcspn(way, "\n")] = 0;  // 去掉换行符

    printf("The address you want to connect is?\n");
    fgets(address, sizeof(address), stdin);
    address[strcspn(address, "\n")] = 0;

    printf("The port you want to connect is?\n");
    scanf("%d", &port);
    getchar();  // 清除缓冲区中的换行符

    // 创建套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // 设置服务器地址
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, address, &server_addr.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        return 1;
    }

    // 尝试连接到服务器
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return 1;
    }

    printf("Connected successfully.\n");

    // 启动两个线程：一个用于传输数据，一个用于接收数据
    pthread_create(&transfer_thread, NULL, transfer, NULL);
    pthread_create(&receive_thread, NULL, receive, NULL);

    // 等待线程结束
    pthread_join(transfer_thread, NULL);
    pthread_join(receive_thread, NULL);

    return 0;
}
