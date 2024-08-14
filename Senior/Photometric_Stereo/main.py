import cv2
import numpy as np
import open3d as o3d
import matplotlib.pyplot as plt
import json
import os
import scipy
from sklearn.preprocessing import normalize
from scipy.ndimage import gaussian_filter


image_row = 120
image_col = 120

# visualizing the mask (size : "image width" * "image height")
def mask_visualization(M,config):
    mask = np.copy(np.reshape(M, (image_row, image_col)))
    plt.figure()
    plt.imshow(mask, cmap='gray')
    plt.title('Mask')
    plt.savefig(f'Mask_{config["target"]}.png')

# visualizing the unit normal vector in RGB color space
# N is the normal map which contains the "unit normal vector" of all pixels (size : "image width" * "image height" * 3)
def normal_visualization(N,config):
    # converting the array shape to (w*h) * 3 , every row is a normal vetor of one pixel
    N_map = np.copy(np.reshape(N, (image_row, image_col, 3)))
    N_map = (N_map + 1.0) / 2.0
    plt.figure()
    plt.title('Normal map')
    plt.imshow(N_map)
    plt.savefig(f'Normal_map_{config["target"]}.png')
    

# visualizing the depth on 2D image
# D is the depth map which contains "only the z value" of all pixels (size : "image width" * "image height")
def depth_visualization(D,config):
    D_map = np.copy(np.reshape(D, (image_row,image_col)))
    # D = np.uint8(D)
    plt.figure()
    plt.imshow(D_map)
    plt.colorbar(label='Distance to Camera')
    plt.title('Depth map')
    plt.xlabel('X Pixel')
    plt.ylabel('Y Pixel')
    plt.savefig(f'Depth_map_{config["target"]}.png')

# convert depth map to point cloud and save it to ply file
# Z is the depth map which contains "only the z value" of all pixels (size : "image width" * "image height")
def save_ply(Z,filepath):
    Z_map = np.reshape(Z, (image_row,image_col)).copy()
    data = np.zeros((image_row*image_col,3),dtype=np.float32)
    # let all point float on a base plane 
    baseline_val = np.min(Z_map)
    Z_map[np.where(Z_map == 0)] = baseline_val
    for i in range(image_row):
        for j in range(image_col):
            idx = i * image_col + j
            data[idx][0] = j
            data[idx][1] = i
            data[idx][2] = Z_map[image_row - 1 - i][j]
    # output to ply file
    pcd = o3d.geometry.PointCloud()
    pcd.points = o3d.utility.Vector3dVector(data)
    o3d.io.write_point_cloud(filepath, pcd,write_ascii=True)

# show the result of saved ply file
def show_ply(filepath):
    pcd = o3d.io.read_point_cloud(filepath)
    o3d.visualization.draw_geometries([pcd])

# read the .bmp file
def read_bmp(filepath,config):
    global image_row
    global image_col
    image = cv2.imread(filepath,cv2.IMREAD_GRAYSCALE)
    if(config['target'] == "noisy_venus"):
        sigma = 2
        image= gaussian_filter(image, sigma=sigma)
    image_row , image_col = image.shape
    return image


def construct_pre_matrix(config):
    file = open(os.path.join(config['root_dir'],config['target'],'LightSource.txt'),'r')
    target_dir = os.path.join(config['root_dir'],config['target'])
    l_lists = []
    for i in file:
        tmp = i[:-2]
        tmp = tmp.split("(")[1]
        l_lists.append(tmp.split(','))
    bmp_lists = [os.path.join(target_dir,i) for i in os.listdir(target_dir) if(".bmp" in i)]
    bmp_lists.sort()
    bmp_files = []
    for path in bmp_lists:
        bmp = read_bmp(path,config)
        mask = bmp>10
        bmp = bmp.reshape(bmp.shape[0]*bmp.shape[1])
        bmp_files.append(bmp)
    bmp_files = np.array(bmp_files,dtype=np.float32)
    l = np.array(l_lists,dtype=np.float32)
    L = normalize(l, axis = 1)
    return bmp_files,L,mask
    
def construct_normal_vector(intensity,light,config):
    light_T = light.T
    product = light_T @ light
    inverse_matrix = np.linalg.inv(product)
    product = inverse_matrix @ light_T
    normal_vector_scale = product @ intensity

    matrix = normalize(normal_vector_scale,axis=0)
    normal_visualization(matrix.T,config)
    return matrix

def fill_parameter_into_matrix(M,V,mask,s,Z):
    nonzero_h, nonzero_w = np.where(mask!=0)
    index_mapping = np.zeros((image_row,image_col)).astype(np.int16)
    for i in range(s):
        index_mapping[nonzero_h[i],nonzero_w[i]] = i
    for i in range(s):
        h,w = nonzero_h[i],nonzero_w[i]
        nx = Z[0,h*image_col+w]
        ny = Z[1,h*image_col+w]
        nz = Z[2,h*image_col+w]
        # fill M matrix
        # z(x+1,y) - z(x,y) = -nx/nz
        j = i*2
        if(mask[h,w+1] != 0):
            k = index_mapping[h,w+1]
            M[j,i] = -1
            M[j,k] = 1
            V[j] = -nx/nz
        elif(mask[h,w-1] != 0):
            k = index_mapping[h,w-1]
            M[j,i] = 1
            M[j,k] = -1
            V[j] = -nx/nz
        # z(x,y+1) - z(x,y) = -ny/nz
        j = i*2+1
        if(mask[h+1,w] != 0):
            k = index_mapping[h+1,w]
            M[j,i] = 1
            M[j,k] = -1
            V[j] = -ny/nz
        elif(mask[h-1,w] != 0):
            k = index_mapping[h,w-1]
            M[j,i] = -1
            M[j,k] = 1
            V[j] = -ny/nz
        # z(x+1,y+1) - z(x,y) = -nx/nz + -ny'/nz'
        
    return M,V


def get_surface_reconstruct(z,mask,s):
    ans = np.zeros((image_row,image_col))
    nonzero_h, nonzero_w = np.where(mask!=0)
    normalized_z = (z-np.mean(z))/np.std(z)
    outliner_idx = np.abs(normalized_z) > 2

    max_num = np.max(z[~outliner_idx])
    min_num = np.min(z[~outliner_idx])

    for i in range(s):
        h,w = nonzero_h[i],nonzero_w[i]
        if(max_num<z[i]):
            ans[h,w] = max_num
        elif(min_num>z[i]):
            ans[h,w] = min_num
        else:
            ans[h,w] = z[i]
    return ans




def reconstruct_depth(Z,mask):
    '''
    I decide to use the second method
    Z would be the normal vector
    construct the M matrix with mask ( (2S-mask)X(S) )
    '''
    '''
    I want to add different path and its summation should be zero 
    '''
    # N = np.reshape(Z.T, (image_row, image_col, 3))
    s = np.size(np.where(mask != 0)[0])
    M = scipy.sparse.lil_matrix((2*s,s))
    V = np.zeros((2*s,1))
    M,V = fill_parameter_into_matrix(M,V,mask,s,Z)
    # M, V = fill_value_into_matrix(M, V, s, N, mask)
    z = scipy.sparse.linalg.spsolve(M.T @ M, M.T @ V)
    
    return get_surface_reconstruct(z,mask,s)







if __name__ == '__main__':
    # Specify the path to your JSON file
    file_path = 'config.json'
    with open(file_path, 'r') as file:
        config = json.load(file)
    filepath=f"{config['target']}.ply"
    intensity,light,mask = construct_pre_matrix(config)
    mask_visualization(mask,config)
    normal_matrix = construct_normal_vector(intensity,light,config)
    Z = reconstruct_depth(normal_matrix,mask)

    depth_visualization(Z,config)
    save_ply(Z,filepath)
    show_ply(filepath)

    # showing the windows of all visualization function
    plt.show()