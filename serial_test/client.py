import socket

s = socket.socket()
host = 'DESKTOP-5D3LTMV' # needs to be in quote
port = 1247
s.connect((host, port))
print(s.recv(1024).decode('utf-8'))
inpt = input('type anything and click enter... ')
s.send(bytes(inpt, 'utf-8'))
print("the message has been sent")
