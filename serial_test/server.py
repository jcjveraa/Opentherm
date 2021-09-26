import socket

s = socket.socket()
host = socket.gethostname()
port = 1247
s.bind((host,port))
print(host)
s.listen(5)
while True:
    c, addr = s.accept()
    # print("Connection accepted from " + repr(addr[1]))
    # c.send(bytes("Server approved connection\n", 'utf-8'))
    print(repr(addr[1]) + ": " + c.recv(1026).decode('utf-8'))
    c.close()
