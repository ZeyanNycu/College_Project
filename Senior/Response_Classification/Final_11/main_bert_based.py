from torch.utils.data import Dataset, DataLoader, TensorDataset
import pandas as pd
import torch
import re
import numpy as np
import os
from torch.nn.utils.rnn import pad_sequence
import numpy as np
from data_class import * 
from transformers import AutoTokenizer, AutoModelForSequenceClassification
import torch
from tqdm import tqdm
from torch.utils.tensorboard import SummaryWriter
import torch.nn as nn
import argparse
import torch.nn.functional as F
writer = SummaryWriter("logs")
def metric(predictions,labels):
    TP_1,FP_1 = 0,0
    TP_0,FP_0 = 0,0
    for pre,GT in zip(predictions,labels):
        if(pre == 1):
            if(GT == 1):
                TP_1 += 1
            else:
                FP_1 += 1
        else:
            if(GT == 0):
                TP_0 += 1
            else:
                FP_0 += 1
    return TP_1,FP_1,TP_0,FP_0
def print_information(TP_1,FP_1,TP_0,FP_0):
    print(f"Predicted | TP |  FP | total")
    print(f"   1      | {TP_1} | {FP_1} | {TP_1/(TP_1+FP_1)}")
    print(f"   0      | {TP_0} | {FP_0} | {TP_0/(TP_0+FP_0)}")

def train(args):
    num_classes = 2
    epochs=3
    batch_size=16
    learning_rate=2e-5
    num_epochs = args.epoch
    device="cuda"
    max_len=512
    train_datapath="/media/user/1e157ea6-6726-4f42-9dcc-80f030cff0dc/scenario_retrieval/Assess_Subjective_Risk/NLP/dataset/train.json"
    eval_datapath="/media/user/1e157ea6-6726-4f42-9dcc-80f030cff0dc/scenario_retrieval/Assess_Subjective_Risk/NLP/dataset/val.json"
    if(args.mode == "final"):
        train_dataset = construct_dataset(train_datapath)
        eval_dataset = construct_dataset(eval_datapath)
    elif(args.mode == "gold"):
        train_dataset = construct_dataset_golden(train_datapath)
        eval_dataset = construct_dataset_golden(eval_datapath)
    elif(args.mode == "gold_v2"):
        train_dataset = construct_dataset_golden_v2(train_datapath)
        eval_dataset = construct_dataset_golden_v2(eval_datapath)
    elif(args.mode == "ideal"):
        train_dataset = construct_dataset_ideal(train_datapath)
        eval_dataset = construct_dataset_ideal(eval_datapath)
    elif(args.mode == "predicted"):
        tokenizer = AutoTokenizer.from_pretrained(args.predicted)
        model = AutoModelForSequenceClassification.from_pretrained(args.predicted).to(device)
        train_dataset = construct_dataset_predict(train_datapath,tokenizer,model)
        eval_dataset = construct_dataset_predict(eval_datapath,tokenizer,model)
    
    model_name = args.origin#"roberta-base"
    tokenizer = AutoTokenizer.from_pretrained(model_name)
    model = AutoModelForSequenceClassification.from_pretrained(model_name,num_labels=num_classes).to(device)
    if tokenizer.pad_token is None:
        tokenizer.add_special_tokens({'pad_token': '[PAD]'})
        model.resize_token_embeddings(len(tokenizer))
    
    train_tokens = tokenizer.batch_encode_plus(
                    train_dataset.data,
                    padding=True,
                    truncation=True,
                    return_tensors='pt',
                    max_length=max_len
    )
    train_labels = torch.tensor(train_dataset.GT)
    train_data = TensorDataset(train_tokens['input_ids'], train_tokens['attention_mask'], train_labels)
    train_loader = DataLoader(train_data, batch_size=batch_size, shuffle=True)
    optimizer = torch.optim.AdamW(model.parameters(), lr=learning_rate)
    eval_tokens = tokenizer.batch_encode_plus(
                    eval_dataset.data,
                    padding=True,
                    truncation=True,
                    return_tensors='pt',
                    max_length=max_len
    )
    eval_labels = torch.tensor(eval_dataset.GT)
    eval_data = TensorDataset(eval_tokens['input_ids'],eval_tokens['attention_mask'], eval_labels)
    eval_loader = DataLoader(eval_data, batch_size=batch_size, shuffle=True)
    loss_min = np.inf
    for epoch in tqdm(range(num_epochs)):
        model.train()
        loss_all = 0
        for batch in tqdm(train_loader):
            optimizer.zero_grad()
            input_ids, attention_mask, labels = batch
            input_ids, attention_mask, labels = input_ids.to(device), attention_mask.to(device), labels.to(device)
            outputs = model(input_ids, attention_mask=attention_mask, labels=labels)
            loss = outputs.loss
            loss_all += loss
            loss.backward()
            optimizer.step()
        writer.add_scalar(f"Training_Loss/{args.finetune}", loss_all/len(train_loader), epoch)
        model.eval()
        loss_all = 0
        num = 0
        T_1,F_1,T_0,F_0 = 0,0,0,0
        with torch.no_grad():
            for batch in eval_loader:
                input_ids, attention_mask, labels = batch
                input_ids, attention_mask, labels = input_ids.to(device), attention_mask.to(device), labels.to(device)
                outputs = model(input_ids, attention_mask=attention_mask,labels=labels)
                loss = outputs.loss
                loss_all += loss
                softmax_output = F.softmax(outputs.logits, dim=1)
                tmp = softmax_output[:,0]-softmax_output[:,1]
                predictions = torch.argmax(outputs.logits, dim=1)
                # predictions[tmp>0.2] = 0
                # predictions[tmp<=0.2] = 1
                TP_1,FP_1,TP_0,FP_0 = metric(predictions,labels)
                T_1 += TP_1
                F_1 += FP_1
                T_0 += TP_0
                F_0 += FP_0
                correct = predictions[predictions==labels]
                num += len(correct)
            print_information(T_1,F_1,T_0,F_0)
            if(loss_all<loss_min):
                loss_min = loss_all
                model.save_pretrained(f"weights/{args.finetune}_weights")
                tokenizer.save_pretrained(f"weights/{args.finetune}_weights")
        writer.add_scalar(f"Evaluation_Loss/{args.finetune}", loss_all/len(eval_loader), epoch)
        writer.add_scalar(f"Accuracy/{args.finetune}", num/len(eval_loader)/batch_size, epoch)
    # text = "Your input text goes here."
    # inputs = tokenizer(text, return_tensors="pt")

    # outputs = model(**inputs)
    # predictions = torch.nn.functional.softmax(outputs.logits, dim=-1)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="A simple command-line program")

    # Step 3: Define command-line arguments
    parser.add_argument('-e', '--epoch', type=int, help='How much epoch you want to use to train?')
    parser.add_argument('-m', '--mode', type=str, help='What kind of task we want to use?')
    parser.add_argument('-o', '--origin', type=str, help='Pretrained weight')
    parser.add_argument('-f', '--finetune', type=str, help='Finetune the weight we want')
    parser.add_argument('-p', '--predicted', type=str, help='Specific the model we want to use to predict')

    # Step 4: Parse the command-line arguments
    args = parser.parse_args()
    train(args)