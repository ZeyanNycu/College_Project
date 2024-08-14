from torch.utils.data import Dataset, DataLoader
import pandas as pd
import torch
import re
import numpy as np
import os
from torch.nn.utils.rnn import pad_sequence

class Training_Dataset(Dataset):
    def __init__(self, Data_path,sample_path,embedding_weight,embedding_dir):
        self.data = []
        self.GT_mapping={}
        self.embedding = self.construct_embedding(embedding_weight,embedding_dir)
        cnt = 0
        data = pd.read_json(Data_path)
        GT_header = pd.read_csv(sample_path)
        self.PE = self.construct_PE()
        for i in GT_header:
            if(i == 'index'):
                continue
            self.GT_mapping[i] = cnt
            cnt+=1
        context = []
        GT = []
        for i in range(data['tweet'].size):
            a,b,w = self.data_preprocessing(data['tweet'][i],data['labels'][i])
            if(i == 2):
                print(w)
                asd
            self.data.append([a,b,w,data['tweet'][i]])
    def construct_PE(self):
        PE = []
        for i in range(1,71):
            PE.append(self.embedding[str(i)])
        return PE
    def data_preprocessing(self,s,GT):
        filtered_string = re.sub(r'[^a-zA-Z ]', '',s)
        filtered_string = filtered_string.lower()
        tokens = filtered_string.split()
        words = []
        cnt = 0
        while(True):
            if(tokens[cnt] in self.embedding):
                tmp = self.embedding[tokens[cnt]]
                embedding = np.expand_dims(tmp,axis=0)
                words.append(tokens[cnt])
                break
            cnt += 1
        cnt+=1
        for i in range(cnt,len(tokens)):
            if(tokens[i] in self.embedding):
                tmp = self.embedding[tokens[i]]
                words.append(tokens[i])
                embedding = np.append(embedding,np.expand_dims(tmp,axis=0),axis=0)
                cnt += 1
            elif ('covid' in tokens[i]):
                tmp = self.embedding['virus']
                words.append(tokens[i])
                embedding = np.append(embedding,np.expand_dims(tmp,axis=0),axis=0)
                cnt += 1
        tmp = np.zeros(12)
        for i in GT:
            tmp[self.GT_mapping[i]] = 1
        for i in range(embedding.shape[0],70):
            embedding = np.append(embedding,np.zeros((1,self.embedding['use'].shape[0])),axis=0)
            words.append("$")
        return embedding,tmp,words
    def __len__(self):
        return len(self.data)

    def __getitem__(self, idx):
        train, GT,w,origin = self.data[idx]
        return torch.tensor(train).float(), torch.tensor(GT).float(),w,origin
    def construct_embedding(self,embedding_weight,embedding_dir):
        embeddings_index = {}
        f = open(os.path.join(embedding_dir, embedding_weight), encoding='utf8')
        for line in f:
            values = line.split()
            word = values[0]
            coefs = np.asarray(values[1:], dtype='float32')
            embeddings_index[word] = coefs
        f.close()
        return embeddings_index 

class Testing_Dataset(Dataset):
    def __init__(self, Data_path,sample_path,embedding_weight,embedding_dir):
        self.data = []
        self.GT_mapping={}
        self.embedding = self.construct_embedding(embedding_weight,embedding_dir)
        cnt = 0
        data = pd.read_json(Data_path)
        context = []
        for i in range(data['tweet'].size):
            a = self.data_preprocessing(data['tweet'][i])
            self.data.append(a)
    def data_preprocessing(self,s):
        filtered_string = re.sub(r'[^a-zA-Z ]', '',s)
        filtered_string = filtered_string.lower()
        tokens = filtered_string.split()
        cnt = 0
        while(True):
            if(tokens[cnt] in self.embedding):
                embedding = np.expand_dims(self.embedding[tokens[cnt]],axis=0)
                break
            cnt += 1
        cnt+=1
        for i in range(cnt,len(tokens)):
            if(tokens[i] in self.embedding):
                embedding = np.append(embedding,np.expand_dims(self.embedding[tokens[i]],axis=0),axis=0)
        for i in range(embedding.shape[0],70):
            embedding = np.append(embedding,np.zeros((1,self.embedding['use'].shape[0])),axis=0)
        return embedding
    def __len__(self):
        return len(self.data)

    def __getitem__(self, idx):
        train = self.data[idx]
        return torch.tensor(train).float()
    def construct_embedding(self,embedding_weight,embedding_dir):
        embeddings_index = {}
        f = open(os.path.join(embedding_dir, embedding_weight), encoding='utf8')
        for line in f:
            values = line.split()
            word = values[0]
            coefs = np.asarray(values[1:], dtype='float32')
            embeddings_index[word] = coefs
        f.close()
        return embeddings_index 