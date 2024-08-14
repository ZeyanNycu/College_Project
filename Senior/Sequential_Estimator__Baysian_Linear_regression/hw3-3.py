import json
import matplotlib.pyplot as plt
import numpy as np
from tqdm import tqdm
from copy import deepcopy

def univariate_data_generator(config,num=100000):
    data_points = []
    std = np.sqrt(config['variance'])
    for i in range(num):
        data_points.append( config['mean']+std*(np.sum(np.random.uniform(size=12))-6) )
    return data_points
def polynomial_data_generator(config,num=100000):
    w = [ 1 for i in range(config['n']) ]
    config_fake = {"mean":0,"variance":config['variance']}
    constant = univariate_data_generator(config_fake,num=num)
    xs,ys = [],[]
    for i in range(num):
        x = np.random.uniform(low=-1, high=1, size=1)
        y = constant[i]
        y += sum([ w[ii]*(x**ii) for ii in range(config['n'])])
        xs.append(x)
        ys.append(y)
    return xs,ys
def SubplotResult(config,idx, title, x, y, m, a, lambda_inverse, err_var, ground_truth):
    plt.subplot(idx)
    plt.title(title)
    plt.xlim(-2.0, 2.0)
    plt.ylim(-20.0, 20.0)
    function = np.poly1d(np.flip(m))
    x_curve = np.linspace(-2.0, 2.0, 30)
    y_curve = function(x_curve)
    plt.plot(x_curve, y_curve, 'k')
    if ground_truth:
        plt.plot(x_curve, y_curve + err_var, 'r')
        plt.plot(x_curve, y_curve - err_var, 'r')
    else:
        plt.scatter(x, y, s=10)
        y_curve_plus_var = []
        y_curve_minus_var = []
        for i in range(0, 30):    
            X = np.array([[x_curve[i] ** j for j in range(0, config['n'])]])
            distance = (1 / a) + (X @ lambda_inverse @ X.T)[0][0]
            y_curve_plus_var.append(y_curve[i] + distance)
            y_curve_minus_var.append(y_curve[i] - distance)
        plt.plot(x_curve, y_curve_plus_var, 'r')
        plt.plot(x_curve, y_curve_minus_var, 'r')

if __name__ == "__main__":
    print("This is hw3-3")
    config_path = "hw3-3.json"
    with open(config_path,'r') as f:
        config = json.load(f)
    num = 0
    xs,ys = [], []
    m_pre = np.array([0 for i in range(config['n'])])
    a = config['variance']
    b = config['mean']
    data_points = []
    inv_variance = 1/a
    basis = config['n']
    data_x = [-0.64152,0.07122,-0.19330]
    data_y = [0.19039, 1.63175,0.24507]
    prior_mean = np.zeros((basis,1))
    prior_covariance = np.identity(basis)
    pre_marginalize_mean = 0
    pre_marginalize_variance =0

    xs,ys = [],[]
    while(True):
        x,y = polynomial_data_generator(config,num=1)
        x,y = x[0][0],y[0][0]
        xs.append(x)
        ys.append(y)
        print(f"Add data point ({x}, {y}):")
        num += 1
        
        X = np.array([[x ** idx for idx in range(config['n'])]])
        Y = np.array([[y]])
        
        if num == 1:
            posterior_covariance = np.linalg.inv(inv_variance * X.T.dot(X) + b * np.identity(basis))
            posterior_mean = inv_variance * posterior_covariance.dot(X.T) * y
        else:
            posterior_covariance = np.linalg.inv(inv_variance * X.T.dot(X) + np.linalg.inv(prior_covariance))
            posterior_mean = posterior_covariance.dot(
                inv_variance * X.T * y + np.linalg.inv(prior_covariance).dot(prior_mean))

        marginalize_mean = X.dot(prior_mean)
        marginalize_variance = inv_variance + X.dot(prior_covariance).dot(X.T)
        print("Postirior mean:")
        print(posterior_mean)
        print("")
        print("Posterior variance:")
        print(posterior_covariance)
        print("")
        print(f"Predictive distribution ~ N({marginalize_mean[0][0]}, {marginalize_variance[0][0]})")
        if np.linalg.norm(posterior_mean - prior_mean) < 0.00001 and num > 50:
            break
        if(num == 10):
            x_10,y_10 = deepcopy(xs),deepcopy(ys)
            mean_10 = deepcopy(posterior_mean)
            variance_10 = deepcopy(posterior_covariance)
        if(num == 50):
            x_50,y_50 = deepcopy(xs),deepcopy(ys)
            mean_50 = deepcopy(posterior_mean)
            variance_50 = deepcopy(posterior_covariance)

        pre_marginalize_mean = marginalize_mean
        pre_marginalize_variance = marginalize_variance
        prior_mean = posterior_mean
        prior_covariance = posterior_covariance
    SubplotResult(config,221, 'Ground truth',     None, None, config['weight'],                       None, None,                    a, True)
    SubplotResult(config,222, 'Predict result',   xs,    ys,    np.reshape(posterior_mean, config['n']), a,    posterior_covariance, None,  False)
    SubplotResult(config,223, 'After 10 incomes', x_10, y_10, np.reshape(mean_10, config['n']),    a, variance_10,       None,  False)
    SubplotResult(config,224, 'After 50 incomes', x_50, y_50, np.reshape(mean_50, config['n']),     a, variance_50,       None,  False)
    plt.tight_layout()
    plt.savefig("hw3-3.png")

    