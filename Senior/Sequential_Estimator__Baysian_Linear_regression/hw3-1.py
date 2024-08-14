import json
import matplotlib.pyplot as plt
import numpy as np
from tqdm import tqdm

def univariate_data_generator(config,num=100000):
    data_points = []
    std = np.sqrt(config['variance'])
    for i in tqdm(range(num)):
        data_points.append( config['mean']+std*(np.sum(np.random.uniform(size=12))-6) )
    return data_points
def polynomial_data_generator(config,num=100000):
    w = [ 1 for i in range(config['n']) ]
    config_fake = {"mean":0,"variance":config['variance']}
    constant = univariate_data_generator(config_fake,num=num)
    xs,ys = [],[]
    for i in tqdm(range(num)):
        x = np.random.uniform(low=-1, high=1, size=1)
        y = constant[i]
        y += sum([ w[ii]*(x**ii) for ii in range(config['n'])])
        xs.append(x)
        ys.append(y)
    return xs,ys

def visualization(data_points,config):
    plt.hist(data_points, bins=30, edgecolor='black')  # Using 30 bins, you can adjust this value as needed
    plt.title('Histogram of Random Data')
    plt.xlabel('Value')
    plt.ylabel('Frequency')
    plt.grid(True)  # Add grid lines
    plt.savefig(f"hw3-1-{config['method']}.png")
def visualization_plot_points(xs,ys,config):
    plt.scatter(xs, ys, color='red', marker='o')
    plt.title('Scatter Plot of Points')
    plt.xlabel('X Values')
    plt.ylabel('Y Values')
    plt.grid(True)
    plt.savefig(f"hw3-1-{config['method']}.png")


if __name__=="__main__":
    print("This is homework 3-1")
    config_path = "hw3-1.json"
    with open(config_path,'r') as f:
        config = json.load(f)
    print(config)
    if(config['method'] == "univariate"):
        data_points = univariate_data_generator(config)
        visualization(data_points,config)
    if(config['method'] == "polynomial"):
        xs,ys = polynomial_data_generator(config,num=1000)
        visualization_plot_points(xs,ys,config)
    