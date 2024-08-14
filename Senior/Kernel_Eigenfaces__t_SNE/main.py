import argparse
import os
from PIL import Image
import numpy as np
import random
import cv2
from tqdm import tqdm
from scipy.spatial.distance import cdist
def gaussian_kernel(x, y, sigma=1.0):
    distance = np.linalg.norm(x - y)
    return np.exp(-distance**2 / (2 * sigma**2))
def construct_simple_lda(train_x,train_label_dataset):
    
    all_mean = np.mean(train_x,axis=0)
    class_set = set([x for x in train_label_dataset])
    class_mean = []
    for c in class_set:
        class_mean.append(np.mean(train_x[train_label_dataset == c],axis=0))
    class_mean = np.array(class_mean)
    
    # construct S_b
    S_b = np.zeros((train_x.shape[1],train_x.shape[1]))

    for idx,c in enumerate(class_set):
        n = np.sum(train_label_dataset == c)
        diff = (class_mean[idx]-all_mean).reshape(train_x.shape[1],1)
        S_b += n * (diff) @ (diff.T) 
    # construct S_w
    S_w = np.zeros((train_x.shape[1],train_x.shape[1]))
    for idx,c in enumerate(class_set):
        diff = train_x[train_label_dataset == c] - class_mean[idx]
        S_w += ( diff.T @  diff )
    
    return np.linalg.pinv(S_w) @ S_b

def construct_kernel_lda(train_x,train_label_dataset,kernel,gamma = 0.000001):
    class_set = set([x for x in train_label_dataset])
    num_of_classes = len(class_set)
    num_of_images = len(train_x.shape[0])
    if (kernel == "gaussian"):
        # Linear
        kernel_of_each_class = np.zeros((num_of_classes, 29 * 24, 29 * 24))
        for idx,c in enumerate(class_set):
            images = train_x[training_labels == c]
            kernel_of_each_class[idx] = gaussian_kernel(images.T,images.T)
        kernel_of_all = gaussian_kernel(train_x.T,train_x.T)
    elif (kernel == "RBF"):
        # RBF
        kernel_of_each_class = np.zeros((num_of_classes, 29 * 24, 29 * 24))
        for idx,c in enumerate(class_set):
            images = train_x[training_labels == c]
            kernel_of_each_class[idx] = np.exp(-gamma * cdist(images.T, images.T, 'sqeuclidean'))
        kernel_of_all = np.exp(-gamma * cdist(train_x.T, train_x.T, 'sqeuclidean'))

    # Compute N
    matrix_n = np.zeros((29 * 24, 29 * 24))
    identity_matrix = np.eye(29 * 24)
    for idx, num in enumerate(num_of_each_class):
        matrix_n += kernel_of_each_class[idx].dot(identity_matrix - num * identity_matrix).dot(
            kernel_of_each_class[idx].T)

    # Compute M
    matrix_m_i = np.zeros((num_of_classes, 29 * 24))
    for idx, kernel in enumerate(kernel_of_each_class):
        for row_idx, row in enumerate(kernel):
            matrix_m_i[idx, row_idx] = np.sum(row) / num_of_each_class[idx]
    matrix_m_star = np.zeros(29 * 24)
    for idx, row in enumerate(kernel_of_all):
        matrix_m_star[idx] = np.sum(row) / num_of_images
    matrix_m = np.zeros((29 * 24, 29 * 24))
    for idx, num in enumerate(num_of_each_class):
        difference = (matrix_m_i[idx] - matrix_m_star).reshape((29 * 24, 1))
        matrix_m += num * difference.dot(difference.T)

    # Get N^(-1) * M
    info_log('=== Calculate inv(N) * M ===')
    matrix = np.linalg.pinv(matrix_n).dot(matrix_m)

    return matrix

def LDA(train_dataset,train_label_dataset,n=25,kernel="no"):
    train_x = train_dataset.reshape(135,-1).copy()
    if(kernel == "no"):
        S = construct_simple_lda(train_x,train_label_dataset)
    elif(kernel == "gaussian"):
        kernel_result = kdf(train_x_transpose, kernel='gaussian')
        S = construct_kernel_lda(train_x,train_label_dataset,kernel)
    elif(kernel == "rbf"):
        S = construct_kernel_lda(train_x,train_label_dataset,kernel)
    
    eigenvalues, eigenvectors = np.linalg.eig(S)
    sorted_indices = np.argsort(eigenvalues)[::-1]  # Indices of sorted eigenvalues in descending order
    top_n_indices = sorted_indices[:n]  # Indices of the top n eigenvalues
    top_n_eigenvectors = eigenvectors[:, top_n_indices].real# Get the top n eigenvectors
    
    # We need to store the result of top_n_eigenvector
    
    target_eigenvectors = find_target_eigenvectors(S)
    
    return top_n_eigenvectors


def get_data_path(args,mode,output_path="debug.png"):
    data_root = os.path.join(args.data_root,mode)
    data_path_list = [ os.path.join(data_root,file_path) for file_path in os.listdir(data_root) ]
    data_path_list.sort()
    data_list = [ np.array(Image.open(file_path).resize((24,29))) for file_path in data_path_list ]
    label_list = [ int(file_path.split("/")[-1][7:9]) for file_path in data_path_list]
    
    return np.array(data_list),np.array(label_list)
def find_target_eigenvectors(matrix: np.ndarray) -> np.ndarray:
    """
    Find 25 first largest eigenvectors
    :param matrix: given matrix
    :return: 25 first largest eigenvectors
    """
    # Compute eigenvalues and eigenvectors
    eigenvalues, eigenvectors = np.linalg.eig(matrix)

    # Get 25 first largest eigenvectors
    target_idx = np.argsort(eigenvalues)[::-1][:25]
    target_eigenvectors = eigenvectors[:, target_idx].real

    return target_eigenvectors

def kdf(x, kernel='gaussian',bandwidth=1,gamma = 0.000001):
    """
    計算給定數據的KDF(核密度函數)值。
    
    參數:
    x (array-like): 計算KDF值的自變量。
    data (array-like): 樣本數據。
    kernel (str, optional): 核函數的類型,可選 'gaussian', 'rbf'。默認為 'gaussian'。
    bandwidth (float, optional): 核函數的帶寬。默認為 1.0。
    
    返回:
    array-like: KDF值。
    """
    
    
    if kernel == 'gaussian':
        n = len(x)
        kernel_func = lambda u: (1 / np.sqrt(2 * np.pi)) * np.exp(-u**2 / 2)    
        kdf_values = np.zeros_like(x)
        for i, t in enumerate(x):
            distances = (x - t) / bandwidth
            kdf_values[i] = np.mean([kernel_func(d) for d in distances])
        
        return kdf_values
    elif kernel == "rbf":
        kernel = np.exp(-gamma * cdist(x, x, 'sqeuclidean'))
        matrix_n = np.ones((29 * 24, 29 * 24), dtype=float) / (29 * 24)
        matrix = kernel - matrix_n.dot(kernel) - kernel.dot(matrix_n) + matrix_n.dot(kernel).dot(matrix_n)

        return matrix

def PCA(train_dataset,n=25,kernel="no"):
    # Doing Principle Components Analysis
    '''
    Step 1. Construct S matrix (1/N(x-mean)(x-mean)T)
    Step 2. Find its eigenvector
    '''
    train_x = train_dataset.reshape(135,-1).copy()
    mean_train_x = np.mean(train_x, axis=0)[:,None] # (c,1)
    train_x_transpose = train_x.T
    if(kernel == "no"):
        S = (train_x_transpose - mean_train_x) @ (train_x_transpose - mean_train_x).T * (1/train_x.shape[0]) # S (number of data points, channel)
    elif(kernel == "gaussian"):
        kernel_result = kdf(train_x_transpose, kernel='gaussian')
        S = kernel_result @ kernel_result.T * (1/train_x.shape[0])
    elif(kernel == "rbf"):
        kernel_result = kdf(train_x_transpose, kernel='rbf')
        S = kernel_result @ kernel_result.T * (1/train_x.shape[0])
    
    eigenvalues, eigenvectors = np.linalg.eig(S)
    sorted_indices = np.argsort(eigenvalues)[::-1]  # Indices of sorted eigenvalues in descending order
    top_n_indices = sorted_indices[:n]  # Indices of the top n eigenvalues
    top_n_eigenvectors = eigenvectors[:, top_n_indices].real# Get the top n eigenvectors
    
    # We need to store the result of top_n_eigenvector
    
    target_eigenvectors = find_target_eigenvectors(S)
    
    return top_n_eigenvectors

def store_data(path,reconstruct_data):
    parent_directory = os.path.dirname(path)
    os.makedirs(parent_directory, exist_ok=True)
    cv2.imwrite(path,reconstruct_data)

def construct_faces(num_of_images: int, training_images: np.ndarray, target_eigenvectors: np.ndarray) -> None:
    """
    Reconstruct faces from eigenfaces/fisherfaces
    :param num_of_images: number of images
    :param training_images: training images
    :param target_eigenvectors: 25 first largest eigenvectors
    :return: None
    """
    reconstructed_images = np.zeros((10, 29 * 24))
    choice = np.random.choice(num_of_images, 10)
    for idx in range(10):
        reconstructed_images[idx, :] = training_images[choice[idx], :].dot(target_eigenvectors).dot(
            target_eigenvectors.T)
    fig = plt.figure(2)
    fig.canvas.set_window_title('Reconstructed faces')
    for idx in range(10):
        # Original image
        plt.subplot(10, 2, idx * 2 + 1)
        plt.axis('off')
        plt.imshow(training_images[choice[idx], :].reshape((29, 24)), cmap='gray')
        # Reconstructed image
        plt.subplot(10, 2, idx * 2 + 2)
        plt.axis('off')
        plt.imshow(reconstructed_images[idx, :].reshape((29, 24)), cmap='gray')
    plt.savefig("train_result.png")
    
def compute_distribution(eigen_matrix,train_dataset):
    train_x = train_dataset.reshape(train_dataset.shape[0],-1).copy()
    return train_x @ eigen_matrix

def knn(test_distribution,train_distribution,train_label_dataset,test_label_dataset,k=5):
    '''
    test_distribution -> (n,c)
    train_distribution -> (n,c)
    ---------------------------------------------
    Use k nearest neighbor to vote for the label that the testcase belong to
    '''
    err = 0
    for idx,testcase in enumerate(tqdm(test_distribution)):
        testcase = testcase[None]
        dis = (train_distribution - testcase) * (train_distribution - testcase) # (n,c)
        dis = np.sum(dis,axis=1) #(n)
        smallest_k_idx = np.argsort(dis)[:k]
        bin_class = {}
        for neighbor in train_label_dataset[smallest_k_idx]:
            if(neighbor not in bin_class.keys()):
                bin_class[neighbor] = 0
            else:
                bin_class[neighbor] += 1
        max_key = max(bin_class, key=bin_class.get)
        if(test_label_dataset[idx] != max_key):
            err += 1
    print(f"The number of error is: {err}")
    print(f"The number of error rate is {err/test_distribution.shape[0]}")
    
    

if __name__ == "__main__":
    # Step 1: Create the parser
    parser = argparse.ArgumentParser(description="Machine Learning HW7")

    # Step 2: Add arguments
    parser.add_argument('-a', '--analysis', type = str, default="LDA", help='Specific the analysis we want to use')
    parser.add_argument('-m', '--mode', type = str, default="part3", help='Specific the mode of this program')
    parser.add_argument('-d', '--data_root', type = str, default="data/Yale_Face_Database", help='Root of the dataset')
    parser.add_argument('-k', '--kernel', type = str, default="gaussian", help='The type of kernel I want to use')
    # Step 3: Parse the arguments
    args = parser.parse_args()


    '''
    Load the data set into datalist
    train_dataset (number of image,h,w) -> (135,231,195)
    test_dataset (number of image,h,w) -> (30,231,195)
    '''
    train_dataset,train_label_dataset = get_data_path(args,"Training")
    test_dataset,test_label_dataset = get_data_path(args,"Testing")
    if(args.analysis == "PCA"):
        if(args.mode == "part1"):
            # In this part, I try to finish part 1
            '''
            Use PCA and LDA to show the first 25 eigenfaces and fisherfaces, and 
            randomly pick 10 images to show their reconstruction. (please refer to the lecture 
            slides).
            '''
            print("Start to do part1")
            eigen_matrix = PCA(train_dataset)
            random_data = random.choices(train_dataset,k=10)
            for idx,data in enumerate(random_data):
                gt_data = data.copy()
                reconstruct_data = data.reshape(-1) @ eigen_matrix @ eigen_matrix.T
                reconstruct_data = reconstruct_data.reshape(29,24) # I misunderstand the width x height
                store_data(f"part1/randomly_select_{idx}_{args.analysis}.png",reconstruct_data)
                store_data(f"part1/randomly_select_{idx}_{args.analysis}_gt.png",gt_data)
        if(args.mode == "part2"):
            '''
            Use PCA and LDA to do face recognition, and compute the performance. You 
            should use k nearest neighbor to classify which subject the testing image belongs to. 
            '''
            print("Start to do part2")
            eigen_matrix = PCA(train_dataset)
            # I should construct the distribution of training data in dimension=25
            train_distribution = compute_distribution(eigen_matrix,train_dataset)
            test_distribution = compute_distribution(eigen_matrix,test_dataset)
            
            knn(test_distribution,train_distribution,train_label_dataset,test_label_dataset)
        if(args.mode == "part3"):
            '''
            Use kernel PCA and kernel LDA to do face recognition, and compute the 
            performance. (You can choose whatever kernel you want, but you should try different 
            kernels in your implementation.) Then compare the difference between simple LDA/PCA 
            and kernel LDA/PCA, and the difference between different kernels. 
            '''
            print("Start to do part3")
            eigen_matrix = PCA(train_dataset,kernel=args.kernel)
            # I should construct the distribution of training data in dimension=25
            train_distribution = compute_distribution(eigen_matrix,train_dataset)
            test_distribution = compute_distribution(eigen_matrix,test_dataset)\
            
            knn(test_distribution,train_distribution,train_label_dataset,test_label_dataset)
    elif(args.analysis == "LDA"):
        if(args.mode == "part1"):
            # In this part, I try to finish part 1
            '''
            Use PCA and LDA to show the first 25 eigenfaces and fisherfaces, and 
            randomly pick 10 images to show their reconstruction. (please refer to the lecture 
            slides).
            '''
            print("Start to do part1")
            eigen_matrix = LDA(train_dataset,train_label_dataset)
            random_data = random.choices(train_dataset,k=10)
            for idx,data in enumerate(random_data):
                gt_data = data.copy()
                reconstruct_data = data.reshape(-1) @ eigen_matrix @ eigen_matrix.T
                reconstruct_data = reconstruct_data.reshape(29,24) # I misunderstand the width x height
                store_data(f"part1/randomly_select_{idx}_{args.analysis}.png",reconstruct_data)
                store_data(f"part1/randomly_select_{idx}_{args.analysis}_gt.png",gt_data)
        if(args.mode == "part2"):
            '''
            Use PCA and LDA to do face recognition, and compute the performance. You 
            should use k nearest neighbor to classify which subject the testing image belongs to. 
            '''
            print("Start to do part2")
            eigen_matrix = LDA(train_dataset,train_label_dataset)
            # I should construct the distribution of training data in dimension=25
            train_distribution = compute_distribution(eigen_matrix,train_dataset)
            test_distribution = compute_distribution(eigen_matrix,test_dataset)\
            
            knn(test_distribution,train_distribution,train_label_dataset,test_label_dataset)
        if(args.mode == "part3"):
            '''
            Use kernel PCA and kernel LDA to do face recognition, and compute the 
            performance. (You can choose whatever kernel you want, but you should try different 
            kernels in your implementation.) Then compare the difference between simple LDA/PCA 
            and kernel LDA/PCA, and the difference between different kernels. 
            '''
            print("Start to do part3")
            eigen_matrix = PCA(train_dataset,kernel=args.kernel)
            # I should construct the distribution of training data in dimension=25
            train_distribution = compute_distribution(eigen_matrix,train_dataset)
            test_distribution = compute_distribution(eigen_matrix,test_dataset)\
            
            knn(test_distribution,train_distribution,train_label_dataset,test_label_dataset)
        
    
        
        
        