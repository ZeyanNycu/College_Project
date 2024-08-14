import torch
import torch.nn as nn
import argparse
debug = False
class Attention(nn.Module):
    def __init__(self, feature_dim, step_dim, bias=True, **kwargs):
        super(Attention, self).__init__(**kwargs)
        
        self.supports_masking = True

        self.bias = bias
        self.feature_dim = feature_dim
        self.step_dim = step_dim
        self.features_dim = 0
        
        weight = torch.zeros(feature_dim, 1)
        nn.init.xavier_uniform_(weight)
        self.weight = nn.Parameter(weight)
        
        if bias:
            self.b = nn.Parameter(torch.zeros(step_dim))
        
    def forward(self, x, mask=None):
        feature_dim = self.feature_dim
        step_dim = self.step_dim

        temp=x.contiguous().view(-1, feature_dim)
        eij = torch.mm(temp, self.weight)
        eij = eij.view(-1, step_dim)
        if self.bias:
            eij = eij + self.b
        
        eij = torch.tanh(eij)
        
        a=torch.softmax(eij, dim=1)
        
#         a = torch.exp(eij)
#         if(debug==True):
#             print("a shape",a.shape)
#             print("mask shape",mask.shape)
#         if mask is not None:
#             a = a * mask

#         a = a /(torch.sum(a, 1, keepdim=True) + 1e-10)
        
        
        
        
        weighted_input = x * torch.unsqueeze(a, -1)
        
        return torch.sum(weighted_input, 1),a


class Attention_LBSA(nn.Module):
    def __init__(self, feature_dim, step_dim, bias=True, **kwargs):
        super(Attention_LBSA, self).__init__(**kwargs)
        
        self.supports_masking = True

        self.bias = bias
        self.feature_dim = feature_dim
        self.step_dim = step_dim
        self.features_dim = 0
        
        weight = torch.zeros(feature_dim, feature_dim)
        nn.init.xavier_uniform_(weight)
        self.weight = nn.Parameter(weight)
        context=torch.zeros(feature_dim, 1)
        nn.init.xavier_uniform_(context)
        self.context_vector=nn.Parameter(context)
        if bias:
            self.b = nn.Parameter(torch.zeros(feature_dim))
        
    def forward(self, x, mask=None):
        feature_dim = self.feature_dim
        step_dim = self.step_dim

        temp=x.contiguous().view(-1, feature_dim)
        eij = torch.mm(temp, self.weight)
        if self.bias:
            eij = eij + self.b
        eij = torch.tanh(eij)
        
        ### changedstep
        eij = torch.mm(eij, self.context_vector)
        eij = eij.view(-1, step_dim)

#         a = torch.exp(eij)
#         if(debug==True):
#             print("a shape",a.shape)
#             print("mask shape",mask.shape)
#         if mask is not None:
#             a = a * mask

#         a = a /(torch.sum(a, 1, keepdim=True) + 1e-10)

        a=torch.softmax(eij, dim=1)
        
        
        weighted_input = x * torch.unsqueeze(a, -1)
        
        return torch.sum(weighted_input, 1),a

class Multi_label_Model(nn.Module):
    def __init__(self, input_dim, hidden_dim, num_heads,output_dim,threshold):
        super(Multi_label_Model, self).__init__()
        self.threshold = threshold
        self.input_dim = input_dim
        self.hidden_dim = hidden_dim

        # 其他模型组件
        self.embedding = nn.Linear(input_dim, hidden_dim)
        self.relu0 = nn.ReLU()
        self.dropout0 = nn.Dropout(0.1)
        hidden_dim = input_dim
        self.self_attention = SelfAttention(hidden_dim, num_heads)
        self.dropout1 = nn.Dropout(0.1)
        self.fc1 = nn.Linear(hidden_dim, hidden_dim//2)
        self.relu1 = nn.ReLU()
        self.dropout2 = nn.Dropout(0.1)
        self.fc2 = nn.Linear(hidden_dim//2, output_dim)
        self.sigmoid = nn.Sigmoid()

    def forward(self, src, mask=None):
        outputs = self.self_attention(src, src, src, mask)
        outputs = torch.mean(outputs, dim=1)

        outputs = self.dropout1(outputs)
        outputs = self.fc1(outputs)
        outputs = self.relu1(outputs)

        outputs = self.dropout2(outputs)
        outputs = self.fc2(outputs)
        outputs = self.sigmoid(outputs)
        # outputs = self.bn1(outputs)
        
        # 在下游任务中使用自注意力的输出
        # ...
        
        return outputs
class Attention_LBSA_sigmoid(Attention_LBSA):
    def __init__(self, feature_dim, step_dim, bias=True, **kwargs):
        super().__init__(feature_dim, step_dim, bias, **kwargs)
        
    def forward(self, x, mask=None):
        feature_dim = self.feature_dim
        step_dim = self.step_dim

        temp=x.contiguous().view(-1, feature_dim)
        if(debug):
            print("temp",temp.shape)
            print("weight",self.weight.shape)
        eij = torch.mm(temp, self.weight)
        if(debug):
            print("eij step 1",eij.shape)
        #eij = eij.view(-1, step_dim)
        if(debug):
            print("eij step 2",eij.shape)
        if self.bias:
            eij = eij + self.b
        eij = torch.tanh(eij)
        
        ### changedstep
        eij = torch.mm(eij, self.context_vector)
        if(debug):
            print("eij step 3",eij.shape)
            print("context_vector",self.context_vector.shape)
        eij = eij.view(-1, step_dim)
        sigmoid = nn.Sigmoid()
        a=sigmoid(eij)
           
        if(debug==True):
            print("a shape",a.shape)
            print("mask shape",mask.shape)
        if mask is not None:
            a = a * mask

        if(debug):
            print("attention",a.shape)
        
        weighted_input = x * torch.unsqueeze(a, -1)
        if(debug):
            print("weighted input",weighted_input.shape)
        
        return torch.sum(weighted_input, 1),a


class BiRNN(nn.Module):  
    def __init__(self,args):
        super(BiRNN, self).__init__()
        self.hidden_size = args['hidden_size']
        self.batch_size = args['batch_size']
        self.drop_embed=args['drop_embed']
        self.drop_fc=args['drop_fc']
        self.embedsize=args["embed_size"]
        self.drop_hidden=args['drop_hidden']
        self.seq_model_name=args["seq_model"]
        self.weights =args["weights"]
        # self.embedding = nn.Embedding(args["vocab_size"], self.embedsize)
        # self.embedding.weight = nn.Parameter(torch.tensor(embeddings.astype(np.float32), dtype=torch.float32))
        # self.embedding.weight.requires_grad = args["train_embed"]
        if(args["seq_model"]=="lstm"):
            self.seq_model = nn.LSTM(args["embed_size"], self.hidden_size, bidirectional=True, batch_first=True,dropout=self.drop_hidden)
        elif(args["seq_model"]=="gru"):
            self.seq_model = nn.GRU(args["embed_size"], self.hidden_size, bidirectional=True, batch_first=True,dropout=self.drop_hidden) 
        self.linear1 = nn.Linear(2 * self.hidden_size, self.hidden_size)
        self.linear2 = nn.Linear(self.hidden_size, args['num_classes'])
        self.dropout_embed = nn.Dropout2d(self.drop_embed)
        self.dropout_fc = nn.Dropout(self.drop_fc)
        self.num_labels=args['num_classes']
        
        
        
    def forward(self,input_ids=None,attention_mask=None,attention_vals=None,labels=None,device='cpu'):
        batch_size=input_ids.size(0)
        # h_embedding = self.embedding(input_ids)
        # h_embedding = torch.squeeze(self.dropout_embed(torch.unsqueeze(h_embedding, 0))).view(batch_size,input_ids.shape[1],self.embedsize)
        if(self.seq_model_name=="lstm"):
            _, hidden = self.seq_model(input_ids)
            hidden=hidden[0]
        else:
            _, hidden = self.seq_model(input_ids)
            
        # if(debug):
        #     print(hidden.shape)
        hidden = hidden.transpose(0, 1).contiguous().view(batch_size, -1) 
        hidden = self.dropout_fc(hidden)
        hidden = torch.relu(self.linear1(hidden))  #batch x hidden_size
        hidden = self.dropout_fc(hidden)
        logits = self.linear2(hidden)
        if labels is not None:
            loss_funct = torch.nn.CrossEntropyLoss(reduction='mean')
            #loss_funct = torch.nn.MultiLabelMarginLoss(reduction='mean')
            loss_logits =  loss_funct(logits, labels) 
            return (loss_logits,logits)  
        return (logits,)
    
    
    
    def init_hidden(self, batch_size):
        return cuda_available(torch.zeros(2, self.batch_size, self.hidden_size))
class BiAtt_RNN(BiRNN):
    def __init__(self,args):
        super().__init__(args)
        embeddings = None
        if(args['attention']=='sigmoid'):
            self.seq_attention = Attention_LBSA_sigmoid(args["embed_size"], args['max_length'])
        else:
            self.linear0 = nn.Linear(args["embed_size"],args["embed_size"])
            self.relu0 = nn.ReLU()
            self.seq_attention = Attention_LBSA(args["embed_size"], args['max_length'])
        self.linear = nn.Linear(args["embed_size"], args["batch_size"])
        self.relu = nn.ReLU()
        self.out = nn.Linear(args["batch_size"], args["num_classes"])
        self.return_att=False
        self.lam=args['att_lambda']
        self.train_att =args['train_att']
            
            
    def forward(self, input_ids=None,attention_mask=None,attention_vals=None,labels=None,device=None):
        h_seq_atten,att = self.seq_attention(input_ids,attention_mask)
        
        conc=h_seq_atten
        conc=self.dropout_fc(conc)
        conc = self.relu(self.linear(conc))
        conc = self.dropout_fc(conc)
        outputs = self.out(conc)
        
        if labels is not None:
            loss_funct = torch.nn.CrossEntropyLoss(reduction='mean')
            loss_logits =  loss_funct(outputs, labels)
            loss= loss_logits
        
            if(self.train_att):
                loss_atts = self.lam*masked_cross_entropy(att,attention_vals,attention_mask)
                
                loss = loss+loss_atts
            return loss_logits,outputs,att

        return (outputs,)