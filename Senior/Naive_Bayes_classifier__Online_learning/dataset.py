import numpy as np
import cv2
from tqdm import tqdm

class MINIST():
    def __init__(self,path,mode,label_path):
        with open(path,'rb') as f:
            self.dataset = f.read()
        with open(label_path,'rb') as f:
            self.label_dataset = f.read()
        # This dataset is big endian
        self.mode = mode
        self.magic_num = int.from_bytes(self.dataset[:4], "big")
        self.len = int.from_bytes(self.dataset[4:4+4], "big")
        self.num_row = int.from_bytes(self.dataset[8:8+4], "big")
        self.num_col = int.from_bytes(self.dataset[12:12+4], "big")
        self.raw_dataset = self.dataset[16:]
        self.raw_label = self.label_dataset[8:]
        self.image_data = []
        self.label_data = []
        for i in tqdm(range(self.len)):
            self.image_data.append(self.parse_image(i))
            self.label_data.append(self.raw_label[i])

        self.image_data = np.array(self.image_data)
        self.label_data = np.array(self.label_data)
    def parse_image(self,idx):
        str_bytes = idx*28*28
        self.ans = []
        for i in range(28):
            self.tmp = []
            for j in range(28):
                if(self.mode == "discrete"):
                    self.tmp.append(self.raw_dataset[str_bytes+i*28+j]//8)
                else:
                    self.tmp.append(self.raw_dataset[str_bytes+i*28+j])
            self.ans.append(self.tmp)
        return self.ans
    def visualization(self,bytes_data):
        cv2.imwrite("mask.png",bytes_data)
        
