#!/usr/bin/python3
import pwn
import sys
from datetime import datetime
import time
import ctypes
if __name__ == '__main__':
    localMode = len(sys.argv) != 3
    attack = b"88"
    target = None

    if localMode:
        target = pwn.process(sys.argv[1])  # Use local file
    else:
        server = sys.argv[1]
        port = int(sys.argv[2])
        target = pwn.remote(server, port)  # Connect to the server
        #target = pwn.process("./project4-source/ra", shell=False)
    s = target.recv().decode().split(' ')[0]
    current_time = str(datetime.now())
    first = current_time.split(' ')[0].split('-')
    second = s.split(':')
    specific_time = datetime(int(first[0]), int(first[1]),int(first[2]), int(second[0]), int(second[1]), int(second[2]))  # Year, Month, Day, Hour, Minute, Second
    timestamp = specific_time.timestamp()
    timestamp = int(timestamp)
    libc = ctypes.CDLL('libc.so.6')
    libc.srand(timestamp)
    s = libc.rand()
    s = str(s)
    target.sendline(s.encode())
    print(target.recvall().decode("utf-8", "ignore").strip())
