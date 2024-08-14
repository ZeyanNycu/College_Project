
import pandas as pd
import nltk
from gensim.models import Word2Vec
import gensim.downloader as api
import os
import numpy as np
from Dataset import Training_Dataset,Testing_Dataset
from torch.utils.data import Dataset, DataLoader
from tqdm import tqdm
from torch.nn.utils.rnn import pad_sequence, pack_padded_sequence
from Model import Multi_label_Model,BiRNN,BiAtt_RNN
import torch.nn as nn
import torch.optim as optim
import torch
from torch.utils.tensorboard import SummaryWriter
import argparse
import pickle
# nltk.download('punkt')
input_dim = 100
hidden_dim = 256
num_heads = 1
output_dim=12
test_path = 'HW1_dataset/test.json'
train_path = 'HW1_dataset/train.json'
val_path = 'HW1_dataset/val.json'
sample_path = 'HW1_dataset/sample_submission.csv'
embedding_dir = 'glove.6B'
embedding_weight = f'glove.6B.{input_dim}d.txt'
batch_size=16
epochs = 15
learning_rate = 0.01
writer = SummaryWriter("logs")
Name="Default"
threshold = 0.7
args_dict = {
        "batch_size":batch_size,
        "hidden_size":hidden_dim,
        "epochs":epochs,
        "embed_size":input_dim,
        "drop":0.1,
        "learning_rate":0.001,
        "vocab_size":10000,
        "num_classes":12,
        "embeddings":np.array([]),
        "seq_model":"lstm",
        "drop_embed":0.1,
        "drop_fc":0.1,
        "drop_hidden":0.1,
        "train_embed":False,
        "weights": [1.0795518,  0.82139814, 1.1678787 ],
        'attention':'softmax',
        'max_length':70,
        "train_att": False,
        "att_lambda": 1.0,
        "embeddings": None,
        }
def metric(output,GT):
    TP,FN,FP = torch.zeros(12),torch.zeros(12),torch.zeros(12)
    for i in range(GT.shape[1]):
        for j in range(GT.shape[0]):
            if(output[j][i]!=0 and GT[j][i]!=0):
                TP[i]+=1
            if(output[j][i]==0 and GT[j][i]!=0):
                FN[i]+=1
            if(output[j][i]!=0 and GT[j][i]==0):
                FP[i]+=1
        if(TP[i] == 0):
            TP[i] = 0.001
        if(FN[i] == 0):
            FN[i] = 0.001
        if(FP[i] == 0):
            FP[i] = 0.001
    return  TP,FN,FP

def main():
    #Step 1 construct training dataset
    print("Construct training Dataset")
    from torch.utils.data import WeightedRandomSampler

    # 根据类别不平衡情况设置样本权重

    training_dataset = Training_Dataset(train_path,sample_path,embedding_weight,embedding_dir)
    training_dataloader = DataLoader(training_dataset, batch_size=batch_size, shuffle=True)
    val_dataset = Training_Dataset(val_path,sample_path,embedding_weight,embedding_dir)
    val_dataloader = DataLoader(val_dataset, batch_size=batch_size, shuffle=True)
    #Model = BiRNN(args_dict)
    Model = BiAtt_RNN(args_dict)
    optimizer = optim.Adam(Model.parameters(), lr=learning_rate)
    best_Macro_F1 = -np.inf
    for epoch in tqdm(range(epochs)):
        Model.train()
        Training_loss,Evaluation_loss = torch.zeros(1),torch.zeros(1)
        for text,GT in training_dataloader:
            optimizer.zero_grad()
            loss,output = Model(text,labels=GT)
            Training_loss += loss
            loss.backward()
            optimizer.step()
        writer.add_scalar(f"Training_Loss/{Name}", Training_loss/len(training_dataloader), epoch)
        Model.eval()
        TP,FN,FP = torch.zeros(12),torch.zeros(12),torch.zeros(12)
        Macro_F1 = 0
        for text,GT in val_dataloader:
            loss,output = Model(text,labels=GT)
            Evaluation_loss += loss
            output[output<threshold] = 0
            output[output>=threshold] = 1
            a,b,c = metric(output,GT)
            TP += a
            FN += b
            FP += c
        pre = TP/(TP+FP)
        rec = TP/(TP+FN)
        pre[TP<0.1] = 0
        rec[TP<0.1] = 0
        for i in range(pre.shape[0]):
            if(pre[i]+rec[i] <= 0.1):
                Macro_F1 += 0
            else:
                Macro_F1 += (2*pre[i]*rec[i]/(pre[i]+rec[i]))/pre.shape[0]
        writer.add_scalar(f"Evaluation_Loss/{Name}", Evaluation_loss/len(val_dataloader), epoch)
        writer.add_scalar(f"F1-Macro/{Name}", Macro_F1, epoch)
        if(Macro_F1>best_Macro_F1):
            best_Macro_F1 = Macro_F1
            torch.save(Model.state_dict(), f'models/{Name}.pt')
def val():
    #Step 1 construct training dataset
    val_dataset = Training_Dataset(val_path,sample_path,embedding_weight,embedding_dir)
    val_dataloader = DataLoader(val_dataset, batch_size=batch_size, shuffle=False)
    #step 2 construct model 
    # Model =  Multi_label_Model(input_dim,hidden_dim,num_heads,output_dim,threshold)
    # criterion = nn.BCEWithLogitsLoss()
    # criterion = nn.MultiLabelSoftMarginLoss()
    result = {}
    for i in range(10):
        threshold = 0.6
        #Model = BiRNN(args_dict)
        Model = BiAtt_RNN(args_dict)
        Model.load_state_dict(torch.load(f'models/{Name}.pt'))
        Model.eval()
        Macro_F1 = 0
        TP,FN,FP = torch.zeros(12),torch.zeros(12),torch.zeros(12)
        words = []
        atts = []
        ori = []
        for ind,data in tqdm(enumerate(val_dataloader)):
            text,GT,w,origin = data
            loss,output,att = Model(text,labels=GT)
            output[output<threshold] = 0
            output[output>=threshold] = 1
            a,b,c = metric(output,GT)
            TP += a
            FN += b
            FP += c
            for i in range(len(att)):
                words.append(w[i])
                atts.append(att[i])
                ori.append(origin[i])
        pre = TP/(TP+FP)
        rec = TP/(TP+FN)
        pre[TP<0.1] = 0
        rec[TP<0.1] = 0
        M_F1 = []
        for i in range(pre.shape[0]):
            if(pre[i]+rec[i] <= 0.1):
                Macro_F1 += 0
            else:
                Macro_F1 += (2*pre[i]*rec[i]/(pre[i]+rec[i]))/(pre.shape[0])
                M_F1.append(2*pre[i]*rec[i]/(pre[i]+rec[i]))
        # print(Macro_F1)
        # print(pre)
        # print(rec)
        # print(M_F1)
        with open('words.pkl', 'wb') as file:
            pickle.dump(words, file)
        with open('att.pkl', 'wb') as file:
            pickle.dump(atts, file)
        with open('ori.pkl', 'wb') as file:
            pickle.dump(ori, file)
        break
def test():
    test_dataset = Testing_Dataset(test_path,sample_path,embedding_weight,embedding_dir)
    test_dataloader = DataLoader(test_dataset, batch_size=1)
    #Model = BiRNN(args_dict)
    model = BiAtt_RNN(args_dict)
    model.load_state_dict(torch.load(f'models/{Name}.pt'))
    model.eval()
    submission = pd.read_csv(sample_path)
    submission = submission.iloc[[0]].copy()
    cnt = 0
    threshold = 0.6
    for text in tqdm(test_dataloader):
        output = model(text)[0]
        output[output<threshold] = 0
        output[output>=threshold] = 1
        ans = output.detach().numpy()
        for line in ans:
            if(cnt == 0):
                print(line)
            mapping = {}
            ind = np.array(cnt)
            line = np.append(cnt,line)
            tmp = 0
            for i in submission.keys():
                if(cnt != 0):
                    mapping[i]= int(line[tmp])
                else:
                    submission[i]= int(line[tmp])
                tmp += 1
            if(cnt != 0):
                ans = pd.DataFrame(mapping,index=[0])
                submission = pd.concat([submission, ans], ignore_index=True)
            cnt += 1
    submission.to_csv("109550150.csv",index=False)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='A simple command-line tool.')

    # 添加位置参数
    parser.add_argument('n', type=str, help='First argument')
    parser.add_argument('mode',type=str)
    args = parser.parse_args()
    Name = args.n
    if(args.mode == 'train'):
        main()
    elif(args.mode == 'valid'):
        val()
    else:
        test()