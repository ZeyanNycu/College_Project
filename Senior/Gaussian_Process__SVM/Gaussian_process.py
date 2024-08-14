import numpy as np
from scipy.spatial import distance
import matplotlib.pyplot as plt
from scipy.optimize import minimize

def calculate_rational_quatratic_kenerl(x1,x2,alpha,length_scale):
    return np.power((1 + distance.cdist(x1,x2,'euclidean'))/(2*alpha*length_scale*length_scale), -alpha)

def marginal_log_likihood(theta):
    global X,Y
    covaraince = calculate_rational_quatratic_kenerl(X,X,theta[0],theta[1])
    value = -(np.log(np.linalg.det(covaraince)))/2
    value -= (Y.T @ np.linalg.inv(covaraince) @ Y)/2
    value -= (X.shape[0] * np.log(2*np.pi) )/2
    return -value


if __name__ == "__main__":
    path = "D:\\2024_Fall_Nycu\\Machine_Learning\\hw5\\data\\input.data"
    mode = "optimized"
    alpha = 1
    length = 1
    beta = 5

    
    X = []
    Y = []
    with open(path,'r') as f:
        for line in f:
            line = line[:-1]
            X.append(float(line.split(" ")[0]))
            Y.append(float(line.split(" ")[1]))

    X_test = np.linspace(-60,60,1000)[:,None]
    X = np.array(X)[:,None]
    Y = np.array(Y)[:,None]

    if(mode == "optimized"):
        initial_array = np.random.rand(2)
        res = minimize(marginal_log_likihood,initial_array)

        alpha = res.x[0]
        length = res.x[1]
    
    # Construct Gaussian distribution
    # Follow the equation to calculate the Multivariate Gaussian
    covariance = calculate_rational_quatratic_kenerl(X,X,alpha,length)
    covariance_train_test = calculate_rational_quatratic_kenerl(X,X_test,alpha,length)
    covariance_test_test = calculate_rational_quatratic_kenerl(X_test,X_test,alpha,length) + (np.eye(1000)/5)

    conditional_mean = covariance_train_test.T @ np.linalg.inv(covariance) @ Y
    conditional_variance = covariance_test_test - covariance_train_test.T @ np.linalg.inv(covariance) @ covariance_train_test

    # Start to visualize result
    '''
    Show all training data points.
    - Draw a line to represent mean of f in range [-60,60].
    - Mark the 95% confidence interval of f.
    '''
    upper_bound = conditional_mean + 1.96 * conditional_variance.diagonal()[:,None]
    lower_bound = conditional_mean - 1.96 * conditional_variance.diagonal()[:,None]

    plt.plot(X_test, conditional_mean, color='red', linestyle='-')
    plt.scatter(X.T, Y.T, color='blue', marker='o')  # 'o' specifies points as circles
    plt.fill_between(X_test[:,0], upper_bound[:,0], lower_bound[:,0], color='r', alpha=0.5)
    plt.xlabel('X axis')
    plt.ylabel('Y axis')
    plt.title('Gaussian Process regression')
    plt.xlim(-60, 60)  # Limits x-axis from 0 to 6
    plt.ylim(-5, 5)  # Limits y-axis from 0 to 12
    plt.grid(True)
    plt.savefig("Gaussian_process_optimized")
    plt.show()




