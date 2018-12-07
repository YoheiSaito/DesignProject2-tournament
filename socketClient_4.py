# ソケット通信クライアント

import socket
import re
import time

BUFSIZE = 1024

serverName = "localhost"
serverPort = 4444

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((serverName, serverPort))
print(s.recv(BUFSIZE).rstrip().decode())

while True:
    line = input("")

    if re.match(r"^q\s*$", line):
        break

    s.send((line + "\n").encode())
    time.sleep(0.1)
    msg = s.recv(BUFSIZE)
    msg = msg.rstrip()
    
    print(msg.decode())

print("bye")

s.close()

