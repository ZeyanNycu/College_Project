import numpy as np
import argparse
import os
import pandas as pd
from Dataset import *
from tqdm import tqdm
import pickle
def projection_matrix(K, R, T):
    """
    Create a projection matrix from intrinsic and extrinsic parameters.

    Parameters:
    K (numpy.ndarray): Intrinsic matrix (3x3).
    R (numpy.ndarray): Rotation matrix (3x3).
    T (numpy.ndarray): Translation vector (3x1).

    Returns:
    numpy.ndarray: Projection matrix (3x4).
    """
    # Combine rotation and translation into an extrinsic matrix [R | T]
    ext_matrix = np.hstack((R, T[:,None]))

    # Compute the projection matrix P = K * [R | T]
    P = np.dot(K, ext_matrix)

    return np.dot(K, np.dot(R, np.hstack((np.identity(3), -T.reshape(3,1)))))
def construct_architecture(feature_data_dir,scene):
    result = {}
    result['K_gt'] = []
    result['R_gt'] = []
    result['T_gt'] = []
    result['M'] = []
    result['Ps_gt'] = []
    result['Ns'] = []
    result['nameList'] = []

    feature_scene = [ os.path.join(os.path.join(feature_data_dir,scene),ind) for ind in os.listdir(os.path.join(feature_data_dir,scene))] 
    feature_scene.sort()
    
    return result

def look_up_wreck(wreck_information_set,inform):
    offset = 0
    for i in wreck_information_set:
        if(inform > i):
            offset += 1
        else:
            break
    return inform - offset

if __name__ == "__main__":
    '''
    namesList is the path to image location -> (1,m)
    K_gt is the ground truth of instrinct of camera -> (m,3,3)
    R_gt is the ground truth of Rotation matrix of camera -> (m,3,3)
    T_gt is the ground truth of Translation matrix of camera -> (m,3)
    M is the track matrix -> (2*m, n)
    Ps_gt is the projection matrix of camera -> (m,3,4)
    Ns is the normalization matrix of projection matrix (it normalize the point of pixel) -> (m,3,3)
    -------------------------------------------------------------------
    m is the number of image
    n is the number of 3d points
    '''
    SIFT = False
    knn = True
    parser = argparse.ArgumentParser(description='This is the code about convert kaggle dataset into other format')
    parser.add_argument('--phase', type=str, help='The phase of dataset we want to convert',default="train")
    parser.add_argument('--root_dir', type=str, help='The root dirctory where the dataset exists', default="/media/user/data1/zeyan/Equivariant-SFM/datasets/kaggle")
    parser.add_argument('--feature_root_dir', type=str, help='The root dirctory where the dataset exists', default="/media/user/data1/zeyan/Equivariant-SFM/datasets/kaggle/kaggle/working/results_new")
    parser.add_argument('--output_dir', type=str, help='The output directory', default="/media/user/data1/zeyan/Equivariant-SFM/datasets/Euclidean")
    args = parser.parse_args()
    dataset_dir = os.path.join(args.root_dir,args.phase)
    feature_data_dir = args.feature_root_dir
    scenes = [ os.path.join(dataset_dir,scene_name) for scene_name in os.listdir(dataset_dir) if(".csv" not in scene_name and "_SIFT" not in scene_name) ]
    train_labels_path = os.path.join(dataset_dir,'train_labels.csv')
    train_labels_csv = pd.read_csv(train_labels_path)

    for scene in scenes:
        if('transp_obj_glass_cup' not in scene):
            continue
        scenes_instance = Scene(scene,train_labels_csv)
        result = construct_architecture(feature_data_dir,scene.split("/")[-1])
        '''
        keypoints -> (m,n',c_k)
        descriptor -> (m,n',c_d)
        ---------------------------------------------
        m means the number of camera
        n' means the number of feature of image
        c_* means the channel of feature
        '''
        point3d_coordinates = []
        m = scenes_instance.m
        wreck_information_set = []
        for idx in tqdm(range(scenes_instance.m)):
            tmp = scenes_instance.image_lists[idx]
            information = [scene.split("/")[-1],tmp.split("/")[-1]]
            R,T,K = scenes_instance.look_up_table(information)
            if(R is None):
                wreck_information_set.append(idx+1)
                continue
            result['nameList'].append(scenes_instance.image_lists[idx])
            if(np.sum(K) != 0):
                N = np.linalg.inv(K)
            else:
                N = np.eye(3)
            
            Ps = projection_matrix(K, R, T)
            result['K_gt'].append(K)
            result['R_gt'].append(R)
            result['T_gt'].append(T)
            result['Ps_gt'].append(Ps)
            result['Ns'].append(N)
        if(SIFT):
            matching_global = os.path.join(scene,'matching_global_SIFT.pkl')
        elif(knn):
            matching_global = os.path.join(scene,'matching_global_knn.pkl')
        else:
            matching_global = os.path.join(scene,'matching_global.pkl')
        with open(matching_global,'rb') as f:
            corresponding_matrix = pickle.load(f)
        wreck_information_set.sort()
        for key in tqdm(corresponding_matrix):
            N_points = np.zeros((2*(m-len(wreck_information_set))))
            inform = key.split("_")
            inform[0] = int(inform[0])
            inform[1] = np.float64(inform[1])
            inform[2] = np.float64(inform[2])
            if(inform[0] in wreck_information_set):
                continue
            inform[0] = look_up_wreck(wreck_information_set,inform[0])
            N_points[2*inform[0]] = np.float64(inform[1])
            N_points[2*inform[0]+1] = np.float64(inform[2])
            for jj in range(len(corresponding_matrix[key])):
                if(len(corresponding_matrix[key][jj]) == 0):
                    continue
                if(jj in wreck_information_set):
                    continue
                new_jj = look_up_wreck(wreck_information_set,jj)
                N_points[2*new_jj] = corresponding_matrix[key][jj][0]
                N_points[2*new_jj+1] = corresponding_matrix[key][jj][1]
            result['M'].append(N_points)
        '''
        K_gt indicates the ground truth of intrinct matrix.
        R_gt indicates the ground truth of rotation matrix.
        T_gt indicates the ground truth of translation matrix.
        Ps_gt indicates the ground truth of projection matrix
        Ns indicates the normalize matrix
        M indicates the input matrix we put it into model to process
        nameList indicates the path of image.
        '''
                        
        result['K_gt'] = np.array(result['K_gt'],dtype=np.float64)
        result['R_gt'] = np.array(result['R_gt'],dtype=np.float64)
        result['T_gt'] = np.array(result['T_gt'],dtype=np.float64)
        result['Ps_gt'] = np.array(result['Ps_gt'],dtype=np.float64)
        result['Ns'] = np.array(result['Ns'],dtype=np.float64)
        result['M'] = np.array(result['M'],dtype=np.float64).T
        result['nameList'] = np.array(result['nameList']).T[:,None]
        
        print(result['K_gt'].shape,result['K_gt'].dtype)
        print(result['R_gt'].shape,result['R_gt'].dtype)
        print(result['T_gt'].shape,result['T_gt'].dtype)
        print(result['Ps_gt'].shape,result['Ps_gt'].dtype)
        print(result['Ns'].shape,result['Ns'].dtype)
        print(result['M'].shape,result['M'].dtype)
        print(result['nameList'].shape)
        if(SIFT):
            output_path = os.path.join(args.output_dir,f'{scene.split("/")[-1]}_SIFT.npz')
        elif(knn):
            output_path = os.path.join(args.output_dir,f'{scene.split("/")[-1]}_knn.npz')
        else:
            output_path = os.path.join(args.output_dir,f'{scene.split("/")[-1]}.npz')
        np.savez(output_path, K_gt=result['K_gt'], R_gt=result['R_gt'], T_gt=result['T_gt'], Ps_gt=result['Ps_gt'],Ns=result['Ns'],M=result['M'],nameList=result['nameList'] )
        