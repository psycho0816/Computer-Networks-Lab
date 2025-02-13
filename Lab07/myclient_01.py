import socket
import threading

"""
    传输函数：该函数根据用户选择的输入方式（文件或键盘）进行消息的发送。
    如果选择的是文件输入，用户提供文件路径后，函数读取文件内容并将其发送至服务器。
    如果选择的是键盘输入，用户在终端输入消息，消息被分割成不超过MAX_LENMAX_LEN字节的块并发送给服务器。
""" 
    
def file_transfer():
    print("message is from file, you don't need to input anything.\n")
    while True:
            path = input("Please input the file path:\n").rstrip()
            # 输入为空时，询问是否关闭传输
            # 如果连续两次输入为空，就关闭传输
            if path == "":
                if input("Try >Enter< again to close") == "":
                    break
                else:
                    continue
                
            try:
                f = open(path, encoding = "UTF-8")
                message = f.readlines()
                f.close()
                for line in message:
                    if line == "":
                        continue
                    i = 0
                    while len(line) - i >= MAX_LEN:
                        s.sendall(line[i:i + MAX_LEN].encode())
                        i += MAX_LEN
                    s.sendall(line[i:].encode())
            except FileNotFoundError:
                print(f"File not found: {path}")
                continue
    

def terminal_transfer():
    print("Now the way is standard input")
    while True:
        message = input("Please input the message you want to send:\n").rstrip()
        if message == "":
            if input("Enter again to close") == "":
                break
            else:
                continue
        i = 0
        while len(message) - i >= MAX_LEN:
            s.sendall(message[i:i + MAX_LEN].encode())
            i += MAX_LEN
        s.sendall(message[i:].encode())

def transfer():
    if way == "F":
        file_transfer()
    elif way == "T":
        terminal_transfer()
    else:
        print("Invalid way!")
        exit(1)
    s.close()

"""
接收函数：该函数用于从服务器接收并打印消息。它通过`recv`方法不断监听服务器的广播信息，
如果接收到消息，打印出该消息内容。如果发生异常，则退出接收循环。
"""
def receive():
    while True:
        try:
            s.settimeout(1000)  # 设置接收消息的超时限制
            message = s.recv(MAX_LEN).decode()  # 接收服务器发来的消息
            print("-----------------------------------")
            print(f"Broadcast:\n{message}")  # 打印接收到的广播消息
            print("-----------------------------------\n")
        except:
            break

port = 7701
MAX_LEN  = 20480        # 每次最多传输的字节数 —— 20KB
way = input("Please input the way to transfer.\n'F'' for file ;  'T' for terminal\n")
address = input("Please input the address of server\n")
host = (address, port)  # 将地址和端口组合成元组

try:
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # 创建套接字
    s.settimeout(5)
    s.connect(host)
except socket.error as err:
    print(err)
    exit(1)
print("Connected to server successfully!")

threading.Thread(target = transfer, args = ()).start()
threading.Thread(target = receive, args = ()).start()