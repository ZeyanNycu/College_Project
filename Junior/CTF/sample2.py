#!/usr/bin/python3
import pwn
import sys
import base64
import os
import subprocess
if __name__ == '__main__':
  filename = sys.argv[1]
  command = '/usr/bin/strings -d '+filename
  result = subprocess.run(command, shell=True, capture_output=True, text=True)
  s = result.stdout.split('\n')
  for i in s:
    try:
      decoded_bytes = base64.b64decode(i)
      s = (decoded_bytes.decode())
      if(s.find("FLAG{") != -1):
        print(s)
    except:
      continue
  #image_data = f.read().decode('ascii',"ignore")
  