import socket
import threading


def transfer():
    """
    传输函数：该函数根据用户选择的输入方式（文件或键盘）进行消息的发送。
    如果选择的是文件输入，用户提供文件路径后，函数读取文件内容并将其发送至服务器。
    如果选择的是键盘输入，用户在终端输入消息，消息被分割成不超过8192字节的块并发送给服务器。
    """
    if way == "file":
        print("Now the way is file,input nothing to stop.")
        while True:
            path = input("Please input the file path:\n").rstrip()  # 获取文件路径
            # 输入为空时，询问是否关闭传输
            if path == "":
                if input("Enter again to close") == "":
                    break  # 用户决定停止
                else:
                    continue  # 否则继续获取文件路径
            # 读取文件内容并发送
            try:
                file = open(path, encoding="UTF-8")
                message = file.readlines()  # 读取文件的每一行
                file.close()
                for line in message:  # 遍历文件的每一行
                    if line == "":  # 如果行为空，则跳过
                        continue
                    a = 0
                    while len(line) - a >= 8192:  # 按最大8192字节切割发送
                        s.sendall(line[a:a + 8192].encode())
                        a += 8192
                    s.sendall(line[a:].encode())  # 发送剩余部分
            except FileNotFoundError:
                print(f"File not found: {path}")
                continue  # 如果文件不存在，则跳过该文件，继续循环
    elif way == "keyboard":
        print("Now the way is standard input")
        while True:
            message = input("Please input the message:\n").rstrip()  # 获取键盘输入
            if message == "":  # 输入为空时，询问是否关闭传输
                if input("Enter again to close") == "":
                    break
                else:
                    continue
            # 按最大8192字节切割消息并发送
            a = 0
            while len(message) - a >= 8192:
                s.sendall(message[a:a + 8192].encode())
                a += 8192
            s.sendall(message[a:].encode())
    else:
        print("ERROR:Wrong way!")  # 输入无效方式
        exit(1)
    s.close()  # 关闭连接


def receive():
    while True:
        try:
            s.settimeout(1000)  # 设置接收消息的超时限制
            message = s.recv(8192).decode()  # 接收服务器发来的消息
            print("——————————————————————")
            print(f"Broadcast:\n{message}")  # 打印接收到的广播消息
            print("——————————————————————")
        except:
            break  # 如果接收发生异常（如连接关闭），退出接收循环


# 主程序部分
way = input("The way you want to input is file or keyboard?\n")  # 选择输入方式：文件或键盘
address = input("The address you want to connect is?\n")  # 输入服务器地址
port = input("The port you want to connect is?\n")  # 输入服务器端口号
host = (address, int(port))  # 将地址和端口组合成元组

# 尝试建立连接
try:
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # 创建一个TCP套接字
    s.settimeout(3)  # 设置连接超时为3秒
    s.connect(host)  # 连接到服务器
except socket.error as msg:
    print(msg)  # 如果连接失败，打印错误消息
    exit()  # 退出程序
print("Connected successfully.")  # 连接成功

# 启动两个线程：一个用于消息发送，另一个用于接收消息
threading.Thread(target=transfer, args=()).start()  # 启动传输线程
threading.Thread(target=receive, args=()).start()  # 启动接收线程
