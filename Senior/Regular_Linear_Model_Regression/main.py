import argparse
import json
import numpy as np
import matplotlib.pyplot as plt
from tqdm import tqdm
def LU_decompose(result_A,y_tmp):
    L,U = np.zeros(result_A.shape),np.zeros(result_A.shape)
    #Construct first column of L and construct first raw of U
    L[:,0] = result_A[:,0]
    U[0,:] = result_A[0,:]/result_A[0,0]
    for i in range(1,result_A.shape[0]):
        U[i,i] = 1
    for j in range(1,result_A.shape[0]-1):
        # calculate the value of l(i,j)
        for i in range(j,result_A.shape[0]):
            s = 0
            for k in range(0,j):
                s += L[i,k]*U[k,j]
            L[i,j] = (result_A[i,j] - s)
        for k in range(j+1,result_A.shape[0]):
            s = 0
            for i in range(0,j):
                s += L[j,i]*U[i,k] 
            U[j,k] = (result_A[j,k] - s)/L[j,j]
    n = result_A.shape[0]-1
    s = 0
    for k in range(0,n):
        s += L[n,k]*U[k,n]
    L[n,n] = result_A[n,n] - s
    d,x = np.zeros(y_tmp.shape),np.zeros(y_tmp.shape)
    d[0] = (y_tmp[0])/(L[0,0])
    for i in range(1,d.shape[0]):
        s = 0
        for j in range(0,i):
            s += L[i,j]*d[j]
        d[i] = (y_tmp[i] - s)/L[i,i]
    x[-1] = d[-1]
    for i in range(y_tmp.shape[0]-2,-1,-1):
        s = 0
        for j in range(i+1,y_tmp.shape[0]):
            s += U[i,j]*x[j]
        x[i] = d[i] - s

    return x


def closed_formed(data):
    A,y = [],[] 
    with open(data['datapoints_path'],'r') as f:
        for i in f:
            i = i[:-1]
            A.append(float(i.split(',')[0]))
            y.append(float(i.split(',')[1]))
    I = np.identity(data['n'])
    mat_A = []
    for base in A:
        mat_A.append([1])
        mat_A[-1].extend([pow(base,i+1) for i in range(data['n']-1)])
    mat_A,y = np.array(mat_A),np.array(y)
    result_A = mat_A.T@mat_A+data['lamda']*I
    y_tmp = mat_A.T @ y
    
    return LU_decompose(result_A,y_tmp),mat_A,y
def newton_method(data):
    A,y = [],[] 
    with open(data['datapoints_path'],'r') as f:
        for i in f:
            i = i[:-1]
            A.append(float(i.split(',')[0]))
            y.append(float(i.split(',')[1]))
    I = np.identity(data['n'])
    mat_A = []
    for base in A:
        mat_A.append([1])
        mat_A[-1].extend([pow(base,i+1) for i in range(data['n']-1)])
    mat_A,y = np.array(mat_A),np.array(y)
    result_A = mat_A.T@mat_A
    y_tmp = mat_A.T @ y
    
    return LU_decompose(result_A,y_tmp),mat_A,y

def get_gradient(prim_y,y,x,mat_A):
    l2_d = np.sum(2*(prim_y-y))
    x_value = np.zeros(x.shape)
    for i in range(y.shape[0]):
        x_value += mat_A[i]
    l2_d = l2_d * x_value

    mask = np.zeros(prim_y.shape)
    mask[prim_y>y] = 1
    mask[prim_y<y] = -1
    l1_d = np.zeros(x.shape)
    for i in range(y.shape[0]):
        l1_d += mat_A[i] * mask[i]
    loss = 0.5*np.sum((prim_y - y)*(prim_y - y)) + 0.5*np.sum(np.abs(prim_y-y))
    # l1_d = np.sum(prim_y[prim_y>y])
    # tmp =  prim_y[prim_y<y]*(-1)
    # l1_d = np.sum(tmp)
    # return l2_d + l1_d
    return l2_d+l1_d,loss

def steepest_method(data):
    A,y = [],[] 
    with open(data['datapoints_path'],'r') as f:
        for i in f:
            i = i[:-1]
            A.append(float(i.split(',')[0]))
            y.append(float(i.split(',')[1]))
    I = np.identity(data['n'])
    mat_A = []
    for base in A:
        mat_A.append([1])
        mat_A[-1].extend([pow(base,i+1) for i in range(data['n']-1)])
    mat_A,y = np.array(mat_A),np.array(y)
    x = np.zeros((data['n']))
    loss = 100
    lr = data['learning_rate']
    jug = 1000
    losses = []
    loss = 100000
    idx = 0
    x_idx = []
    max_iter = 1000000
    for i in tqdm(range(max_iter)):
        prim_y = mat_A @ x
        d,loss = get_gradient(prim_y,y,x,mat_A)
        x = x - lr * d
        losses.append(loss)
        x_idx.append(idx)
        idx += 1
    fig, ax = plt.subplots()
    ax.plot(x_idx,losses,color='black')

    # Set axes limits if desired
    # ax.set_xlim(-6, 6)
    # ax.set_ylim(0, 10)

    # # Add labels and title
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_title('Plot loss curve')

    # Display the figure
    plt.show()
    
    return x,mat_A,y

def calculate_loss(data,parameter,A,y):
    approximation = A @ parameter
    loss = approximation - y
    loss = loss * loss
    loss = np.sum(loss)
    s = ""
    tmp = ""
    for i in range(len(parameter)):
        if(i == 0):
            tmp = f" {parameter[i]}"
        else:
            if("-" in tmp):
                tmp = f" {parameter[i]}X^{i} "
            else:
                tmp = f" {parameter[i]}X^{i} +"
        s = tmp + s
    print(f"Fitting line:{s}")
    print(f"Total error: {loss}")
def visualization(data,parameter,A,y):
    fig, ax = plt.subplots()
    # Plot the point
    x = [ i[1] for i in A]
    x = np.array(x)
    y_prime = A @ parameter
    ax.scatter(x, y, marker='o', color='red')
    ax.plot(x,y_prime,color='black')

    # Set axes limits if desired
    # ax.set_xlim(-6, 6)
    # ax.set_ylim(0, 10)

    # # Add labels and title
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_title('Plot Point')

    # Display the figure
    plt.show()

def main():
    with open('config.json', 'r') as f:
        # Load the JSON data from the file into a Python dictionary
        data = json.load(f)
    if(data['function'] == 'closed_formed' or data['function'] == 'all'):
        parameter,A,y = closed_formed(data)
        print("LSE:")
        calculate_loss(data,parameter,A,y)
        visualization(data,parameter,A,y)
    if(data['function'] == 'newton' or data['function'] == 'all'):
        parameter,A,y = newton_method(data)
        print("Newton's Method:")
        calculate_loss(data,parameter,A,y)
        visualization(data,parameter,A,y)
    if(data['function'] == 'steepest' or data['function'] == 'all'):
        parameter,A,y = steepest_method(data)
        print("Steepest's Method:")
        calculate_loss(data,parameter,A,y)
        visualization(data,parameter,A,y)

if __name__ == '__main__':
    main()