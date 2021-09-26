import socket

def sendAndPrint(conn, msg):
    print(msg)
    conn.send(bytes(msg, 'utf-8'))

from time import time, sleep
def infinite_repeat():
    while True:
        sendAndPrint(conn, "BC01C1A66\r\n")
        sleep(1)
        
        # thing to run
    

HOST = '127.0.0.1'                 # Symbolic name meaning all available interfaces
PORT = 50007              # Arbitrary non-privileged port
while True:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT))
        s.listen(1)
        conn, addr = s.accept()
        with conn:
            print('Connected by', addr)
            while True:
                data = conn.recv(1024)
                if not data: break
                rec_str = data.decode("utf-8")
                print(rec_str)
                if "PR=A" in rec_str:
                    sendAndPrint(conn, "PR:A\r")
                if "PS=0"  in rec_str:
                    sendAndPrint(conn, "PS:0\r")
                    infinite_repeat()
                # conn.sendall(data)