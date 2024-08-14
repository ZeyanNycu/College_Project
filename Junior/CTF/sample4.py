#!/usr/bin/python3
import pwn
import sys
import base64
import os
import subprocess
import zipfile
import re
import sys 
if __name__ == '__main__':
  filename = "project4-source/meow.jpg"
  filename = sys.argv[1]
  f = open(filename,'rb')
  s = f.read()
  for i in range(len(s)-4):
    if(s[i:i+4] == b'\x50\x4b\x03\x04'):
        want = s[i:]
        with open('test.zip','wb') as f:
            f.write(want)
        with zipfile.ZipFile('test.zip', 'r') as zip_file:
            # Get a list of all the files in the ZIP archive
            file_list = zip_file.namelist()
            # Print information about each file in the ZIP archive
            for file_info in zip_file.infolist():
                specific_file = file_info.filename
                zip_file.extract(specific_file, path='./')
                with open(file_info.filename,'r') as f:
                    s_1 = f.read()
                    ans = re.findall("FLAG{.*}",s_1)[0]
                    print(ans)
                os.remove(file_info.filename)
        os.remove('test.zip')
  #image_data = f.read().decode('ascii',"ignore")
  