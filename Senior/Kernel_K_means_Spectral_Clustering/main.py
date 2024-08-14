import argparse
import os
import cv2
import numpy as np
from scipy.spatial.distance import cdist
import numpy as np
import matplotlib.pyplot as plt
from PIL import Image
from tqdm import tqdm
import torch

class parameter:
    def __init__(self):
        self.y_s = 1 / (100*100)
        self.y_c = 1 / (255*255)
        self.root_dir = "/kaggle/input/image-data"
        self.n = 2
        self.mode = "spectral_clustering"
        self.choose_mode = "kmeans++"
        self.iteration = 1000
        self.cut = 0

def visualize_result(arr):
    arr_vis = arr.copy()
    arr_vis = arr_vis.reshape((100,100))
    cv2.imwrite("test.png",(arr_vis==1)*255)
    plt.figure()
    plt.title('Clustering Result')
    plt.imshow(arr_vis)
    plt.savefig(f'Clustering_Result.png')


def calculate_gramm_matrix(args,img):
    gramm_matrix = np.zeros((img.shape[0]*img.shape[1],img.shape[0]*img.shape[1]))
    img_copy = img.copy()
    img_copy = img_copy.reshape((-1,3))
    color_distance = cdist(img_copy, img_copy,'sqeuclidean')

    grid = np.indices((img.shape[0],img.shape[1]))
    row_indices,col_indices = grid[0],grid[1]
    final_indices = np.hstack((row_indices.reshape(-1,1),col_indices.reshape(-1,1)))
    location_distance = cdist(final_indices,final_indices,'sqeuclidean')
    
    # return (np.exp(location_distance*(-args.y_s)) * np.exp(color_distance*(-args.y_c)))
    return (np.exp(location_distance*(-args.y_s)) * np.exp(color_distance*(-args.y_c)))


def initial_clustering(initial_points,gramm_matrix):
    '''
    Need to output clustering (number of points) -> each put the label of each points
    '''
    calculate_distance = np.zeros((gramm_matrix.shape[0],initial_points.shape[0]))
    
    for idx,center_point in enumerate(initial_points):
        calculate_distance[:,idx] = np.diag(gramm_matrix)
        calculate_distance[:,idx] += gramm_matrix[center_point,center_point]
        calculate_distance[:,idx] -= 2*gramm_matrix[:,center_point]
    result = np.argmin(calculate_distance,axis=1)
    
    return result
def calculate_pair_distance(cluster,gramm_matrix,args,num_cluster):

    pairwise = np.zeros(args.n)
    for i in range(args.n):
        tmp_kernel = gramm_matrix.copy()
        for p in range(cluster.shape[0]):
            # Set distance to 0 if the point doesn't belong to the cluster
            if cluster[p] != i:
                tmp_kernel[p, :] = 0
                tmp_kernel[:, p] = 0
        pairwise[i] = np.sum(tmp_kernel)
    
    num_cluster[num_cluster == 0] = 1
    
    return pairwise / (num_cluster ** 2)
           

def calculate_clsutering(cluster,gramm_matrix,args):
    
    # number of cluster in each class
    num_cluster = np.array([ np.sum(cluster == c) for c in range(args.n) ])
    new_cluster = np.zeros(gramm_matrix.shape[0],dtype=int)
    pair_distance = calculate_pair_distance(cluster,gramm_matrix,args,num_cluster)
    
    for p in range(gramm_matrix.shape[0]):
        dis = np.zeros(args.n)
        for idx in range(args.n):
            dis[idx] = gramm_matrix[p,p] + pair_distance[idx]
            tmp = gramm_matrix[p,:].copy()
            tmp[cluster != idx] = 0
            dis[idx] -= ( 2/num_cluster[idx] ) * ( np.sum(tmp) ) 
        
        new_cluster[p] = np.argmin(dis)
    return new_cluster
        
    

def k_means(args,img_lists):
    # How did kernel k_means work?
    '''
    1. Initialize the place of center points set
        a. Need to construct gramm matrix to calculate the distance between each points
        b. Need to assign label to each point
    2. Recalculate the center point based on the result of step 1
    3. Repeat step 1 if the program do not satisfy the condition
    '''
    num_col = 100
    num_row = 100
    for idx,img in enumerate(img_lists):
        initial_points = choose_initial_points(args,img)
        gramm_matrix = calculate_gramm_matrix(args,img)
        current_cluster = initial_clustering(initial_points,gramm_matrix)
        colors = np.array([[255,0,0],
                       [0,255,0],
                       [0,0,255]])
        if args.n > 3:
            colors = np.append(colors, np.random.choice(256, (args.n - 3, 3)), axis=0)
        imgs = [ capture_current_state(100,100,current_cluster,colors) ]
        count = 0
        for i in tqdm(range(args.iteration)):
            new_cluster = calculate_clsutering(current_cluster,gramm_matrix,args)
            
            imgs.append(capture_current_state(100, 100, new_cluster, colors))
            if np.linalg.norm((new_cluster - current_cluster), ord=2) < 0.001 or count >= args.iteration:
                break

            current_cluster = new_cluster.copy()
            count += 1
        print()
        print(count)
        filename = f'{args.mode}_{args.n}_{args.choose_mode}_image_{idx+1}.gif'
        imgs[0].save(filename, save_all=True, append_images=imgs[1:], optimize=False, loop=0, duration=100)
        
        
def capture_current_state(num_of_rows: int, num_of_cols: int, cluster: np.ndarray, colors: np.ndarray) -> Image:
    """
    Capture current clustering
    :param num_of_rows: number of rows
    :param num_of_cols: number of columns
    :param cluster: clusters from kernel k-means
    :param colors: color of each cluster
    :return: an image of current clustering
    """
    state = np.zeros((num_of_rows * num_of_cols, 3))

    # Give every point a color according to its cluster
    for p in range(num_of_rows * num_of_cols):
        state[p, :] = colors[cluster[p], :]

    state = state.reshape((num_of_rows, num_of_cols, 3))

    return Image.fromarray(np.uint8(state))

def choose_initial_points(args,img):
    if(args.choose_mode == "random"):
        return np.random.randint(low=0, high=10000, size=(args.n))
    else:
        # We want to choose the centroid the far the better
        grid = np.indices((img.shape[0],img.shape[1]))
        grid = np.stack(grid, axis=-1)
        indices = np.array([ i for i in range(10000) ])
        center = [ np.random.randint(low=0, high=10000) ]
        for i in range(args.n-1):
            dis = np.zeros((img.shape[0],img.shape[1]))
            min_distance = np.Inf
            arr = np.full((100, 100), 100000,dtype=float)
            for c in center:
                x = c //img.shape[0]
                y = c %img.shape[1]
                dis = np.sum((grid - np.array([x,y]))*(grid - np.array([x,y])),axis=2)
                dis = np.sqrt(dis)
                arr[dis<arr] = dis[dis<arr]
            arr = arr.reshape(100*100)
            arr /= np.sum(arr)
            
            center.append(indices[np.random.choice(100*100, 1, p=arr)[0]].tolist())
        
        
        return np.array(center)

    
def kmeans_initial_points(args,img,matrix_u,num_of_rows,num_of_cols):
    if args.choose_mode == "random":
        # Random strategy
        return matrix_u[np.random.choice(num_of_rows * num_of_cols, args.n)]
    else:
        # k-means++ strategy
        # Construct indices of a grid
        grid = np.indices((num_of_rows, num_of_cols))
        row_indices = grid[0]
        col_indices = grid[1]

        # Construct indices vector
        indices = np.hstack((row_indices.reshape(-1, 1), col_indices.reshape(-1, 1)))

        # Randomly pick first center
        num_of_points = num_of_rows * num_of_cols
        centers = [indices[np.random.choice(num_of_points, 1)[0]].tolist()]

        # Find remaining centers
        for _ in range(args.n - 1):
            # Compute minimum distance for each point from all found centers
            distance = np.zeros(num_of_points)
            for idx, point in enumerate(indices):
                min_distance = np.Inf
                for cen in centers:
                    dist = np.linalg.norm(point - cen)
                    min_distance = dist if dist < min_distance else min_distance
                distance[idx] = min_distance
            # Divide the distance by its sum to get probability
            distance /= np.sum(distance)
            # Get a new center
            centers.append(indices[np.random.choice(num_of_points, 1, p=distance)[0]].tolist())

        # Change from index to feature index
        for idx, cen in enumerate(centers):
            centers[idx] = matrix_u[cen[0] * num_of_rows + cen[1], :]

        return np.array(centers)
    
def construct_Umatrix(args,gramm_matrix):
    '''
    First need to construct D - W = L maxtrix
    W =  gramm_matrix
    '''
    matrix_w = gramm_matrix.copy()
    matrix_d = np.zeros_like(matrix_w)
    for idx,row in enumerate(matrix_w):
        matrix_d[idx,idx] = np.sum(row)
    matrix_l = matrix_d - matrix_w
    if args.cut:
        # Normalized cut
        # Compute normalized Laplacian
        for idx in range(len(matrix_d)):
            matrix_d[idx, idx] = 1.0 / np.sqrt(matrix_d[idx, idx])
        matrix_l = matrix_d.dot(matrix_l).dot(matrix_d)
    # Find eigenvalues and eigenvectors
    eigenvalues, eigenvectors = np.linalg.eig(matrix_l)
    
    eigenvectors = eigenvectors.T

    # Sort eigenvalues and find indices of nonzero eigenvalues
    sort_idx = np.argsort(eigenvalues)
    sort_idx = sort_idx[eigenvalues[sort_idx] > 0]

    return eigenvectors[sort_idx[:args.n]].T
    
    
def calculate_normal_kmeans(current_center,matrix_u,args,img):
    new_cluster = np.zeros(matrix_u.shape[0], dtype=int)
    for p in range(matrix_u.shape[0]):
        # Find minimum distance from data point to centers
        distance = np.zeros(args.n)
        for idx, cen in enumerate(current_center):
            distance[idx] = np.linalg.norm((matrix_u[p] - cen), ord=2)
        # Classify data point into cluster according to the closest center
        new_cluster[p] = np.argmin(distance)

    return new_cluster
def calculate_new_center(num_of_clusters: int, matrix_u: np.ndarray, current_cluster: np.ndarray) -> np.ndarray:
    """
    Recompute centers according to current cluster
    :param num_of_clusters: number of clusters
    :param matrix_u: matrix U containing eigenvectors
    :param current_cluster: current cluster
    :return: new centers
    """
    new_centers = []
    for c in range(num_of_clusters):
        points_in_c = matrix_u[current_cluster == c]
        new_center = np.average(points_in_c, axis=0)
        new_centers.append(new_center)

    return np.array(new_centers)
    
    
    
def spectral_clustering(args,img_lists):
    #This part is about implementing spectral clustering
    '''
    1. First we need to construct U matrix (indicates which nodes is connected)
    2. We need to use k-means
    '''
    num_col = 100
    num_row = 100
    for idx,img in enumerate(img_lists):
        gramm_matrix = calculate_gramm_matrix(args,img)
        U_matrix = construct_Umatrix(args,gramm_matrix)
#         U_matrix = np.random.rand(10000, 4)
        if args.cut:
            # Normalized cut
            sum_of_each_row = np.sum(U_matrix, axis=1)
            for j in range(len(U_matrix)):
                U_matrix[j, :] /= sum_of_each_row[j]
        current_center = kmeans_initial_points(args,img,U_matrix,img.shape[0],img.shape[1])
        colors = np.array([[255,0,0],
                       [0,255,0],
                       [0,0,255]])
        if args.n > 3:
            colors = np.append(colors, np.random.choice(256, (args.n - 3, 3)), axis=0)
        imgs = []
        
        for i in tqdm(range(args.iteration)):
            new_cluster = calculate_normal_kmeans(current_center,U_matrix,args,img)
            new_center =  calculate_new_center(args.n,U_matrix,new_cluster)
            imgs.append(capture_current_state(100, 100, new_cluster, colors))
            if np.linalg.norm((new_center - current_center), ord=2) < 0.001 :
                break

            current_center = new_center.copy()
        
        print()
        filename = f'{args.mode}_{args.n}_{args.choose_mode}_image_{idx+1}.gif'
        imgs[0].save(filename, save_all=True, append_images=imgs[1:], optimize=False, loop=0, duration=100)
        
        
        



args = parameter()
img_paths = [ "image1.png", "image2.png" ]
img_lists = [ cv2.imread(os.path.join(args.root_dir,img_path)) for img_path in img_paths ]
if(args.mode == "kernel_k_means"):
    k_means(args,img_lists)
elif(args.mode == "spectral_clustering"):
    spectral_clustering(args,img_lists)
    
