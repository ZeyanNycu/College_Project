import numpy as np
from  sklearn.neighbors import NearestNeighbors
import scipy.sparse
import warnings
import matplotlib.pyplot as plt
import cv2
def find_Knn_image(fi,K):
    print("Start to find the nearest Knn")
    h,w,c = fi.shape
    i, j = np.unravel_index(np.arange(h*w), (h, w))
    feature_vec = np.append(np.transpose(fi.reshape(h*w,c)), [ i, j]/np.sqrt(h*h + w*w), axis=0).T
    knn = NearestNeighbors(n_neighbors=K,n_jobs=8)
    knn.fit(feature_vec)
    distances, indices = knn.kneighbors(feature_vec)
    return indices,feature_vec
def computing_parameter(img,knn,K,feature_vec,constraints,foreground,mylambda):
    h,w,c = img.shape
    print("Start to compute the affinity matrix A and all other stuff needed")
    row_inds = np.repeat(np.arange(h*w),K)
    col_inds = knn.reshape(h*w*K)
    vals = 1 - np.linalg.norm(feature_vec[row_inds]-feature_vec[col_inds],axis=1)/(c+2)
    A = scipy.sparse.coo_matrix((vals, (row_inds, col_inds)),shape=(h*w, h*w))
    
    D_value = scipy.sparse.diags((np.ravel(A.sum(axis=1))))
    L = D_value-A
    M = scipy.sparse.diags(np.ravel(constraints[:,:]))
    v = np.ravel(foreground[:,:])
    c = 2*mylambda*np.transpose(v)
    H = 2*(L + mylambda*M)

    return H,c
def knn_matting(image, trimap, my_lambda=100):
    K = 20
    [h, w, c] = image.shape
    image, trimap = image / 255.0, trimap / 255.0
    foreground = (trimap == 1.0).astype(int)
    background = (trimap == 0.0).astype(int)
    constraints = foreground + background
    ####################################################
    # TODO: find KNN for the given image
    ####################################################
    knn,feature_vec = find_Knn_image(image,K)
    ####################################################
    # TODO: compute the affinity matrix A
    #       and all other stuff needed
    ####################################################
    H,c = computing_parameter(image,knn,K,feature_vec,constraints,foreground,my_lambda)
    ####################################################
    # TODO: solve for the linear system,
    #       note that you may encounter en error
    #       if no exact solution exists
    ####################################################
    print("Start to compute through linear system")
    warnings.filterwarnings('error')
    alpha = []
    try:
        alpha = np.minimum(np.maximum(scipy.sparse.linalg.spsolve(H, c), 0), 1).reshape(h, w)
    except Warning:
        x = scipy.sparse.linalg.lsqr(H, c)
        alpha = np.minimum(np.maximum(x[0], 0), 1).reshape(h, w)

    return alpha


if __name__ == '__main__':
    target = "woman"
    image = cv2.imread(f'./image/{target}.png')
    trimap = cv2.imread(f'./trimap/{target}.png', cv2.IMREAD_GRAYSCALE)

    alpha = knn_matting(image, trimap)
    #scipy.misc.imsave('./result/alpha.png', alpha)
    # alpha = alpha[:, :, np.newaxis]

    # ####################################################
    # # TODO: pick up your own background image, 
    # #       and merge it with the foreground
    # ####################################################
    print("Start to composite image")
    background = cv2.imread('./image/Background.jpg')
    alpha = np.expand_dims(alpha,axis=2)
    alpha = np.repeat(alpha,3,axis=2)
    background[:alpha.shape[0],:alpha.shape[1]] = np.multiply(image[:alpha.shape[0],:alpha.shape[1]],alpha)+ \
                                                    np.multiply((1-alpha),background[:alpha.shape[0],:alpha.shape[1]])
    # result = []
    cv2.imwrite(f'./result/{target}.png', background)
