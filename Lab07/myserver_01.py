import socket
import threading

def broadcast():
    while True:
        message = input().rstrip()
        if message == "":
            continue
        for c in client_list:
            i = 0
            while len(message) - i >= MAX_LEN:
                c[2].sendall(message[i : i + MAX_LEN].encode())
                i += MAX_LEN
            c[2].sendall(message[i:].encode())

def reception():
    client = (address[0], address[1], connection)
    client_list.append(client)
    print(f">>>{client[0:2]} have connected.")
    while True:
        try:
            message = connection.recv(MAX_LEN).decode()
        except Exception:
            lk.acquire()
            client_list.remove(client)
            lk.release()
            print(f"{client[0:2]} have disconnected.")
        if message == '':
            print(f"{client[0:2]} have disconnected.")
            break
        else:
            print("-----------------------------------")
            print(f"{client[0:2]}:\n{message}")
            print("-----------------------------------")
    lk.acquire()
    client_list.remove(client)
    lk.release()
    connection.close()


client_list = []        # 存储所有已连接的客户端信息
lk = threading.Lock()   # 用于同步客户端列表的访问
MAX_CLIENTS = 10

port = 7701             # 端口号
MAX_LEN  = 20480        # 每次最多传输的字节数 —— 20KB

try:
    # 创建一个套接字 s，该套接字使用 IPv4 地址（AF_INET）和 TCP 协议（SOCK_STREAM）
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    host = ('', port)
    s.bind(host)
    # 最大客户端数
    s.listen(MAX_CLIENTS)
except socket.error as err:
    print(err)
    exit(1)
print("Server is ready!")
print("Waiting for connection...")


# Thread 类用于启动一个新的线程，每个线程都是程序中并行执行的一部分，可以在同一时刻执行多个任务。
# target 是 Thread 类的一个参数，指定了线程执行的目标函数。在这里，目标函数是 broadcast。
threading.Thread(target = broadcast, args = ()).start()
while True:
    (connection, address) = s.accept()
    threading.Thread(target = reception, args = ()).start()