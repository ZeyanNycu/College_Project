import json
import matplotlib.pyplot as plt
import numpy as np
from tqdm import tqdm

def univariate_data_generator(config,num=100000):
    data_points = []
    std = np.sqrt(config['variance'])
    for i in range(num):
        data_points.append( config['mean']+std*(np.sum(np.random.uniform(size=12))-6) )
    return data_points

if __name__ == "__main__":
    print("This is hw3-2")
    config_path = "hw3-2.json"
    with open(config_path,'r') as f:
        config = json.load(f)
    # print(config)
    m,s = float(config['mean']),float(config['variance'])
    estimate_m,estimate_s = 0,0
    pre_estimate_m,pre_estimate_s = 0,0
    print(f"Data point source function: N({m}, {s})")
    n = 0
    while(True):
        data_points = univariate_data_generator(config,num=1)[0]
        print(f"Add data point: {data_points}")
        estimate_m = (pre_estimate_m * n + data_points)/(n+1)
        estimate_s = pre_estimate_s + (data_points - pre_estimate_m) * (data_points - estimate_m)
        print(f"Mean = {estimate_m} Variance = {estimate_s/(n+1)}")
        
        if(abs(estimate_m-pre_estimate_m)<1e-5 and abs(estimate_s-pre_estimate_s)<1e-5):
            break
        pre_estimate_m = estimate_m
        pre_estimate_s = estimate_s
        n += 1
        
        
    
        