import numpy as np

class Homography:
    def __init__(self):
        pass
    
    def solve_homography(self, kps_1, kps_2):
        A = []
        for i in range(len(kps_1)):
            A.append([kps_1[i, 0], kps_1[i, 1], 1, 0, 0, 0, -kps_1[i, 0] * kps_2[i, 0], -kps_1[i, 1] * kps_2[i, 0], -kps_2[i, 0]])
            A.append([0, 0, 0, kps_1[i, 0], kps_1[i, 1], 1, -kps_1[i, 0] * kps_2[i, 1], -kps_1[i, 1] * kps_2[i, 1], -kps_2[i, 1]])

        # Solve system of linear equations Ah = 0 using SVD
        u, sigma, vt = np.linalg.svd(A)
        
        # pick H from last line of vt
        H = np.reshape(vt[8], (3, 3))
        
        # normalization, let H[2,2] equals to 1
        H = (1/H.item(8)) * H
        
        return H
