import os
import numpy as np

class Scene():
    def __init__(self,path,label_csv):
        image_lists = [ os.path.join(os.path.join(path,'images'),img_path) for img_path in os.listdir(os.path.join(path,'images')) ]
        image_lists.sort()
        self.image_lists = np.array(image_lists)
        self.m = self.image_lists.shape[0]
        self.label_csv = label_csv
    
    def look_up_table(self,information):
        query = self.label_csv[self.label_csv['scene'] == information[0]]
        query = query[query['image_name'] == information[1]]
        if(len(query['rotation_matrix'].values) < 1):
            return None,None,None
        else:
            R = query['rotation_matrix'].values[0]
            R = R.split(";")
            for i in range(len(R)):
                R[i] = np.float64(R[i])
            R = np.array(R).reshape(3,3)
            
            
            T = query['translation_vector'].values[0]
            T = T.split(";")
            for i in range(len(T)):
                T[i] = np.float64(T[i])
            T = np.array(T).reshape(3)
            
            calibration_matrix = query['calibration_matrix'].values[0]
            calibration_matrix = calibration_matrix.split(";")
            for i in range(len(calibration_matrix)):
                calibration_matrix[i] = np.float64(calibration_matrix[i])
            calibration_matrix = np.array(calibration_matrix).reshape(3,3)
            
            return R,T,calibration_matrix
        