import json
from dataset import *
import numpy as np
from tqdm import tqdm
import math
def gaussain_prob(x,mu,var):
    return ((1/math.sqrt(2*math.pi*var))*math.exp((-(x-mu)**2)/(2*var)))

def get_mu(array):
    return np.mean(array)

def get_variance(array,eps_var):
    var=np.var(array)
    return var if var!=0 else eps_var #avoid Gaussian formula divided by zero

def construct_likelihood_discrete(test_dataset):
    likelihood = np.zeros((10,28*28,32))
    for i in tqdm(range(test_dataset.len)):
        c = test_dataset.label_data[i]
        for p in range(28*28):
            h,w = p//28,p%28
            likelihood[c][p][test_dataset.image_data[i,h,w]] += 1
    count = np.sum(likelihood,axis=2)
    likelihood /= count[:,:,None]

    return likelihood
def construct_likelihood_continous(test_dataset):
    likelihood = np.zeros((10,28*28,256))
    for c in range(10):
        specific = test_dataset.image_data[test_dataset.label_data == c]
        for p in range(28*28):
            h,w = p//28,p%28
            g_mean = get_mu(specific[:,h,w])
            g_var = get_variance(specific[:,h,w],10)
            if(g_var == 0):
                g_var = 10
            for v in range(256):
                likelihood[c,p,v] = gaussain_prob(v,g_mean,g_var)
    return likelihood

def construct_prior(test_dataset):
    prior = np.zeros(10)
    for i in tqdm(range(test_dataset.len)):
        prior[test_dataset.label_data[i]] += 1
    prior /= test_dataset.len
    return prior

def get_final_result_discrete(likelihood,prior,test_dataset):
    err = 0
    
    for i in tqdm(range(test_dataset.len)):
        prob = np.zeros(10)
        label = test_dataset.label_data[i]
        for c in range(10):
            for p in range(28*28):
                h,w = p//28,p%28
                prob[c] += np.log(max(1e-30,likelihood[c][p][test_dataset.image_data[i,h,w]//8]))
            prob[c] += np.log(prior[c])
        # We need to normalize in here
        prob = prob/np.sum(prob)
        print("Postirior (in log scale):")
        for i in range(10):
            print(f"{i}: {prob[i]}")
        prediction = np.argmin(prob)
        if(prediction != label):
            err += 1
        print(f"Prediction: {prediction}, Ans: {label}")
    for c in range(10):
        print(f"{c}:")
        for p in range(28*28):
            if(p%28 == 0):
                print(end="\n")
            interval = np.argmax(likelihood[c][p])
            if(interval>=16):
                print(" 1",end="")
            else:
                print(" 0",end="")
        print(end="\n")
    print(f"Error rate: {err/test_dataset.len}")
    print()

def get_final_result_continous(likelihood,prior,test_dataset):
    err = 0

    for i in tqdm(range(test_dataset.len)):
        prob = np.zeros(10)
        label = test_dataset.label_data[i]
        for c in range(10):
            for p in range(28*28):
                h,w = p//28,p%28
                prob[c] += np.log(max(1e-30,likelihood[c][p][test_dataset.image_data[i,h,w]]))
            prob[c] += np.log(prior[c])
        # We need to normalize in here
        prob = prob/np.sum(prob)
        print("Postirior (in log scale):")
        for i in range(10):
            print(f"{i}: {prob[i]}")
        prediction = np.argmin(prob)
        if(prediction != label):
            err += 1
        print(f"Prediction: {prediction}, Ans: {label}")
    for c in range(10):
        print(f"{c}:")
        for p in range(28*28):
            if(p%28 == 0):
                print(end="\n")
            interval = np.argmax(likelihood[c][p])
            if(interval>=128):
                print(" 1",end="")
            else:
                print(" 0",end="")
        print(end="\n")
    print(f"Error rate: {err/test_dataset.len}")
    print()

if __name__ == "__main__":
    with open('config.json','r') as f:
        config = json.load(f)
    # step1 parse the dataset
    train_dataset = MINIST(config['train_dataset_path'],config['mode'],config['train_label_path'])
    test_dataset = MINIST(config['test_dataset_path'],config['mode'],config['test_label_path'])

    if(config['mode'] == 'discrete'):
        likelihood = construct_likelihood_discrete(train_dataset)
        prior = construct_prior(train_dataset)
        get_final_result_discrete(likelihood,prior,test_dataset)
    elif(config['mode'] == "continous"):
        likelihood = construct_likelihood_continous(train_dataset)
        prior = construct_prior(train_dataset)
        get_final_result_continous(likelihood,prior,test_dataset)
    
    
    