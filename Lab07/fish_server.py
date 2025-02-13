import socket
import threading

clients = []  # 用于存储所有已连接的客户端信息
lock = threading.Lock()  # 用于同步客户端列表的访问


def broadcast():
    while True:
        message = input().rstrip()  # 获取用户输入的消息并去掉末尾的空白字符
        if message == '':  # 如果输入为空，则忽略该消息
            continue
        for client in clients:  # 遍历每个连接的客户端
            a = 0  # 初始化切割起始位置
            while len(message) - a >= 8192:  # 如果消息长度超过8192字节，则分段发送
                client[2].sendall(message[a:a + 8192].encode())  # 发送当前段
                a += 8192  # 更新切割的起始位置
            client[2].sendall(message[a:].encode())  # 发送消息的剩余部分


def reception():
    client = (address[0], address[1], connection)  # 将客户端的IP地址、端口号和连接对象存储在元组中
    clients.append(client)  # 将当前客户端加入客户端列表
    print(f">>>{client[0:2]} have connected.")  # 打印客户端连接信息
    while True:
        try:
            message = connection.recv(8192).decode()  # 从客户端接收消息，最多接收8192字节
        except Exception:
            # 如果接收发生异常，说明客户端断开了连接
            lock.acquire()  # 获取锁，避免多线程同时修改客户端列表
            clients.remove(client)  # 从列表中移除该客户端
            lock.release()  # 释放锁
            print(f"{client[0:2]} have disconnected.")  # 打印客户端断开信息
        if message == '':  # 如果接收到的消息为空，客户端已断开
            print(f"{client[0:2]} have disconnected.")
            break
        else:
            print("——————————————————————")
            print(f"{client[0:2]}:\n{message}")  # 打印客户端发送的消息
            print("——————————————————————")
    lock.acquire()  # 获取锁
    clients.remove(client)  # 移除该客户端
    lock.release()  # 释放锁
    connection.close()  # 关闭与该客户端的连接


# 主程序部分，创建服务器并开始监听客户端连接
port = input("Input the port you want to bind.\n")  # 用户输入端口号
try:
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # 创建TCP套接字
    host = ('', int(port))  # 绑定到指定的端口号，空字符串表示绑定所有可用的网络接口
    s.bind(host)  # 绑定套接字
    s.listen(10)  # 设置最大连接数为10
except socket.error as msg:
    print(msg)  # 打印错误信息
    exit(1)  # 如果绑定失败，则退出程序
print("Server start!")
print('Waiting connection...')

threading.Thread(target=broadcast, args=()).start()  # 启动广播线程
while True:
    (connection, address) = s.accept()  # 等待客户端连接
    threading.Thread(target=reception, args=()).start()  # 启动接收线程