from sklearn import svm
import numpy as np
from libsvm.svmutil import *

def linear_kernel(x, y):
    return np.dot(x, y.T)

# Define the RBF kernel
def rbf_kernel(x, y, gamma=0.1):
    return np.exp(-gamma * np.linalg.norm(x - y) ** 2)


def Implemmet_part_1(train_data,train_label,test_data,test_label):
    kernels = ['Linear', 'Polynomial', 'RBF']

    # Get performance of each kernel
    for idx, name in enumerate(kernels):
        param = svm_parameter(f"-t {idx} -q")
        prob = svm_problem(train_label, train_data)

        print(f'# {name}')

        model = svm_train(prob, param)
        svm_predict(test_label, test_data, model)

def grid_search_cv(train_data, train_label, parameters, isKernel = False):
    # We use cross validate to estimate the performance of the model
    # Devide data and label into 5 pieces
    param = svm_parameter(parameters + ' -v 3 -q')
    prob = svm_problem(train_label, train_data,isKernel=isKernel)
    model = svm_train(prob, param)
    return model

def Implemmet_part_2(train_data,train_label,test_data,test_label):
    kernels = ['Linear', 'Polynomial', 'RBF']

    # Parameters
    cost = [np.power(10.0, i) for i in range(-1, 2)]
    gamma = [1.0 / 784] + [np.power(10.0, i) for i in range(-1, 1)]
    degree = [i for i in range(0, 3)]
    constant = [i for i in range(-1, 2)]

    # Best parameters and max accuracies
    best_parameter = []
    max_accuracy = []

    # Find best parameters of each kernel
    for idx, name in enumerate(kernels):
        best_para = ''
        max_acc = 0.0
        if name == 'Linear':
            print('# Linear')
            for c in cost:
                parameters = f'-t {idx} -c {c}'
                acc = grid_search_cv(train_data, train_label, parameters)

                if acc > max_acc:
                    max_acc = acc
                    best_para = parameters
            best_parameter.append(best_para)
            max_accuracy.append(max_acc)
        elif name == 'Polynomial':
            print('# Polynomial')
            for c in cost:
                for d in degree:
                    for g in gamma:
                        for const in constant:
                            parameters = f'-t {idx} -c {c} -d {d} -g {g} -r {const}'
                            acc = grid_search_cv(train_data, train_label, parameters)

                            if acc > max_acc:
                                max_acc = acc
                                best_para = parameters
            best_parameter.append(best_para)
            max_accuracy.append(max_acc)
        else:
            print('# RBF')
            for c in cost:
                for g in gamma:
                    parameters = f'-t {idx} -c {c} -g {g}'
                    acc = grid_search_cv(train_data, train_label, parameters)

                    if acc > max_acc:
                        max_acc = acc
                        best_para = parameters
            best_parameter.append(best_para)
            max_accuracy.append(max_acc)

    # Print results and prediction
    prob = svm_problem(train_label,train_data)
    print('-------------------------------------------------------------------')
    for idx, name in enumerate(kernels):
        print(f'# {name}')
        print(f'\tMax accuracy: {max_accuracy[idx]}%')
        print(f'\tBest parameters: {best_parameter[idx]}')

        model = svm_train(prob, svm_parameter(best_parameter[idx] + ' -q'))
        svm_predict(test_label, test_data, model)
        print()


def Implement_part_3(train_data,train_label,test_data,test_label):
    cost = [np.power(10.0, i) for i in range(-2, 3)]
    gamma = [1.0 / 784] + [np.power(10.0, i) for i in range(-2, 3)]
    rows, _ = train_data.shape

    # Use grid search to find best parameters
    linear = linear_kernel(train_data, train_data)
    best_parameter = ''
    best_gamma = 1.0
    max_accuracy = 0.0
    for c in cost:
        for g in gamma:
            rbf = rbf_kernel(train_data, train_data, g)

            # The combination is linear + RBF, but np.arange is the required serial number from the library
            combination = np.hstack((np.arange(1, rows + 1).reshape(-1, 1), linear + rbf))

            parameters = f'-t 4 -c {c}'
            acc = grid_search_cv(combination, train_label, parameters, True)
            if acc > max_accuracy:
                max_accuracy = acc
                best_parameter = parameters
                best_gamma = g

    # Print best parameters and max accuracy
    print('-------------------------------------------------------------------')
    print('# Linear + RBF')
    print(f'\tMax accuracy: {max_accuracy}%')
    print(f'\tBest parameters: {best_parameter} -g {best_gamma}\n')

    # Train the model using best parameters
    rbf = rbf_kernel(train_data, train_data, best_gamma)
    combination = np.hstack((np.arange(1, rows + 1).reshape(-1, 1), linear + rbf))
    model = svm_train(svm_problem(train_label, combination, isKernel=True), svm_parameter(best_parameter + ' -q'))

    # Make prediction using best parameters
    rows, _ = test_data.shape
    linear = linear_kernel(test_data, test_data)
    rbf = rbf_kernel(test_data, test_data, best_gamma)
    combination = np.hstack((np.arange(1, rows + 1).reshape(-1, 1), linear + rbf))
    svm_predict(test_label, combination, model)


if __name__ == "__main__":
    train_data_path = "D:\\2024_Fall_Nycu\\Machine_Learning\\hw5\\data\\X_train.csv"
    train_label_path = "D:\\2024_Fall_Nycu\\Machine_Learning\\hw5\\data\\Y_train.csv"
    test_data_path = "D:\\2024_Fall_Nycu\\Machine_Learning\\hw5\\data\\X_test.csv"
    test_label_path = "D:\\2024_Fall_Nycu\\Machine_Learning\\hw5\\data\\Y_test.csv"

    train_data = np.loadtxt(train_data_path, delimiter=',')
    train_label = np.loadtxt(train_label_path, dtype=int, delimiter=',')
    test_data = np.loadtxt(test_data_path, delimiter=',')
    test_label = np.loadtxt(test_label_path, dtype=int, delimiter=',')

    mode = "part3"

    if(mode == "part1"):
        # Use different kernel functions (linear, polynomial, and RBF kernels) 
        # and have comparison between their performance.
        Implemmet_part_1(train_data,train_label,test_data,test_label)
    if(mode == "part2"):
        Implemmet_part_2(train_data,train_label,test_data,test_label)
    
    if(mode == "part3"):
        Implement_part_3(train_data,train_label,test_data,test_label)
