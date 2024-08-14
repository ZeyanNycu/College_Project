import cv2
import numpy as np
import random
import math
import sys
import os
from tqdm import tqdm
from blender import *
from homography import *
from image import *
from gain_compensations import *

def Convert_xy(x, y):
    global center, f

    xt = ( f * np.tan( (x - center[0]) / f ) ) + center[0]
    yt = ( (y - center[1]) / np.cos( (x - center[0]) / f ) ) + center[1]
    
    return xt, yt


def ProjectOntoCylinder(InitialImage):
    global w, h, center, f
    h, w = InitialImage.shape[:2]
    center = [w // 2, h // 2]
    f = 1100       # 1100 field; 1000 Sun; 1500 Rainier; 1050 Helens
    
    # Creating a blank transformed image
    TransformedImage = np.zeros(InitialImage.shape, dtype=np.uint8)
    
    # Storing all coordinates of the transformed image in 2 arrays (x and y coordinates)
    AllCoordinates_of_ti =  np.array([np.array([i, j]) for i in range(w) for j in range(h)])
    ti_x = AllCoordinates_of_ti[:, 0]
    ti_y = AllCoordinates_of_ti[:, 1]
    
    # Finding corresponding coordinates of the transformed image in the initial image
    ii_x, ii_y = Convert_xy(ti_x, ti_y)

    # Rounding off the coordinate values to get exact pixel values (top-left corner)
    ii_tl_x = ii_x.astype(int)
    ii_tl_y = ii_y.astype(int)

    # Finding transformed image points whose corresponding 
    # initial image points lies inside the initial image
    GoodIndices = (ii_tl_x >= 0) * (ii_tl_x <= (w-2)) * \
                  (ii_tl_y >= 0) * (ii_tl_y <= (h-2))

    # Removing all the outside points from everywhere
    ti_x = ti_x[GoodIndices]
    ti_y = ti_y[GoodIndices]
    
    ii_x = ii_x[GoodIndices]
    ii_y = ii_y[GoodIndices]

    ii_tl_x = ii_tl_x[GoodIndices]
    ii_tl_y = ii_tl_y[GoodIndices]

    # Bilinear interpolation
    dx = ii_x - ii_tl_x
    dy = ii_y - ii_tl_y

    weight_tl = (1.0 - dx) * (1.0 - dy)
    weight_tr = (dx)       * (1.0 - dy)
    weight_bl = (1.0 - dx) * (dy)
    weight_br = (dx)       * (dy)
    
    TransformedImage[ti_y, ti_x, :] = ( weight_tl[:, None] * InitialImage[ii_tl_y,     ii_tl_x,     :] ) + \
                                      ( weight_tr[:, None] * InitialImage[ii_tl_y,     ii_tl_x + 1, :] ) + \
                                      ( weight_bl[:, None] * InitialImage[ii_tl_y + 1, ii_tl_x,     :] ) + \
                                      ( weight_br[:, None] * InitialImage[ii_tl_y + 1, ii_tl_x + 1, :] )


    # Getting x coorinate to remove black region from right and left in the transformed image
    min_x = min(ti_x)

    # Cropping out the black region from both sides (using symmetricity)
    TransformedImage = TransformedImage[:, min_x : -min_x, :]

    return np.array(TransformedImage), ti_x-min_x, ti_y

# read the image file & output the color & gray image
def read_img(path):
    # opencv read image in BGR color space
    img = cv2.imread(path)
    img_gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    return img, img_gray

# the dtype of img must be "uint8" to avoid the error of SIFT detector
def img_to_gray(img):
    if img.dtype != "uint8":
        print("The input image dtype is not uint8 , image type is : ",img.dtype)
        return
    img_gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    return img_gray

# create a window to show the image
# It will show all the windows after you call im_show()
# Remember to call im_show() in the end of main
def creat_im_window(window_name,img):
    cv2.imshow(window_name,img)

# show the all window you call before im_show()
# and press any key to close all windows
def im_show():
    cv2.waitKey(0)
    cv2.destroyAllWindows()

def feature_matching_debug(kts1, dcpt1,kts2, dcpt2):
    bf = cv2.BFMatcher()
    ans = {}
    # Match descriptors
    matches = bf.knnMatch(dcpt1, dcpt2, k=2)
    # Apply ratio test
    good_matches = []
    for m, n in matches:
        if m.distance < 0.75 * n.distance:
            good_matches.append(m)
 
    good_matches = np.array(good_matches)
    return good_matches

def feature_matching_implement(kts1, dcpt1,kts2, dcpt2):

    # Match descriptors
    good_matches = []
    for ii,des1 in enumerate(tqdm(dcpt1)):
        min_value_1_distance = 100000000000001
        min_value_1_idx = -1
        min_value_2_distance = 100000000000000
        min_value_2_idx = -1
        for jj,des2 in enumerate(dcpt2):
            distance = (des1 - des2) * (des1 - des2)
            distance  = sum(distance)
            distance = math.sqrt(distance)
            if(distance < min_value_1_distance):
                min_value_2_distance = min_value_1_distance
                min_value_2_idx = min_value_1_idx
                min_value_1_distance = distance
                min_value_1_idx = jj
            elif(distance < min_value_2_distance and distance >= min_value_1_distance):
                min_value_2_distance = distance
                min_value_2_idx = jj
        if(min_value_1_distance < min_value_2_distance*0.75):
            m = cv2.DMatch()
            m.queryIdx = ii
            m.trainIdx = min_value_1_idx
            m.distance = min_value_1_distance
            m.imgIdx = 0
            good_matches.append(m)

    return good_matches

def find_homography(img_SIFT,matches):
    src_pts = np.float32([img_SIFT[0][1][m.queryIdx].pt for m in matches['0_1']]).reshape(-1, 1, 2)
    dst_pts = np.float32([img_SIFT[1][1][m.trainIdx].pt for m in matches['0_1']]).reshape(-1, 1, 2)

    # Find homography matrix
    H, _ = cv2.findHomography(src_pts, dst_pts, cv2.RANSAC)

    return H

def find_homography_imp(kts1, dcpt1, kts2, dcpt2,matches):

    best_H = {}
    src_pts = np.float32([kts1[m.queryIdx].pt for m in matches]).reshape(-1, 2)
    dst_pts = np.float32([kts2[m.trainIdx].pt for m in matches]).reshape(-1, 2)

    best_H = None
    s_pts = src_pts.copy()
    s_pts = np.concatenate((s_pts,np.ones((s_pts.shape[0],1))),axis=1)
    d_pts = dst_pts.copy()
    d_pts = np.concatenate((d_pts,np.ones((d_pts.shape[0],1))),axis=1)
    # Find homography matrix
    # H, _ = cv2.findHomography(src_pts, dst_pts, cv2.RANSAC)
    max_iteration = 2000
    S = -1
    final_H = None
    for ii in range(max_iteration):
        # Random select 4 points
        points_idx = np.random.choice(src_pts.shape[0], 4, replace=False)
        src_points = src_pts[points_idx]
        dst_points = dst_pts[points_idx]
        # # Start to construct A

        A = []
        for idx in range(len(src_points)):
            A.append([src_points[idx, 0], src_points[idx, 1], 1, 0, 0, 0, -src_points[idx, 0] * dst_points[idx, 0], -src_points[idx, 1] * dst_points[idx, 0], -dst_points[idx, 0]])
            A.append([0, 0, 0, src_points[idx, 0], src_points[idx, 1], 1, -src_points[idx, 0] * dst_points[idx, 1], -src_points[idx, 1] * dst_points[idx, 1], -dst_points[idx, 1]])
        # Solve system of linear equations Ah = 0 using SVD
        u, sigma, vt = np.linalg.svd(A)
        # pick H from last line of vt
        H = np.reshape(vt[8], (3, 3))
        # normalization, let H[2,2] equals to 1
        H = (1/H.item(8)) * H
        inliner_num = 0
        x = s_pts.copy()
        pred = H @ x.T
        pred = pred.T
        for i in range(pred.shape[0]):
            if pred[i,2] <= 1e-8: 
                continue
            pred[i] /= pred[i,2]
        criteria = np.linalg.norm(d_pts - pred,axis=1)
        value = np.sum(criteria<5)
        if(value>S):
            S = value
            best_H = H
    return best_H
    

def stitch_image_imp(final_H,img1, img2,bl,initial):
    H = final_H
    left_down = np.hstack(([0], [0], [1]))
    left_up = np.hstack(([0], [img1.shape[0]-1], [1]))
    right_down = np.hstack(([img1.shape[1]-1], [0], [1]))
    right_up = np.hstack(([img1.shape[1]-1], [img1.shape[0]-1], [1]))
    
    warped_left_down = H @ left_down.T
    warped_left_up = H @ left_up.T
    warped_right_down =  H @ right_down.T
    warped_right_up = H @ right_up.T

    warped_left_down /= warped_left_down[-1]
    warped_left_up /= warped_left_up[-1]
    warped_right_down /= warped_right_down[-1]
    warped_right_up /= warped_right_up[-1]
    
    x1 = int(min(min(min(warped_left_down[0],warped_left_up[0]),min(warped_right_down[0], warped_right_up[0])), 0))
    y1 = int(min(min(min(warped_left_down[1],warped_left_up[1]),min(warped_right_down[1], warped_right_up[1])), 0))

    x1_max = int(max(max(max(warped_left_down[0],warped_left_up[0]),max(warped_right_down[0], warped_right_up[0])), img2.shape[1]))
    y1_max = int(max(max(max(warped_left_down[1],warped_left_up[1]),max(warped_right_down[1], warped_right_up[1])), img2.shape[0]))
    size = (abs(x1) + x1_max , abs(y1) + y1_max)
    A = np.float32([[1, 0, -(x1)], [0, 1, -(y1)], [0, 0, 1]])

    

    warped1 = cv2.warpPerspective(src=img1, M=A@H, dsize=size)
    warped2 = cv2.warpPerspective(src=img2, M=A, dsize=size)

    gain1,gain2 = get_gain_compensations(warped1,warped2)

    for i in range(3):
        warped1[:,:,i] = warped1[:,:,i] * gain1[i]
        warped2[:,:,i] = warped2[:,:,i] * gain2[i]

    cv2.imwrite("test1.png",warped1)
    cv2.imwrite("test2.png",warped2)
    warp_img1 = Image("test1.png")
    warp_img2 = Image("test2.png")

    # if(initial == False):
    #     result = bl.multi_band_blending([warp_img2, warp_img1],5,1)
    #     cv2.imwrite("blend_img.png",result)
    #     result = bl.simple_blending_main([warped2, warped1],result)
    # else:
    #     result = bl.multi_band_blending([warp_img1, warp_img2],5,1)
    #     cv2.imwrite("blend_img.png",result)
    #     result = bl.simple_blending_main([warped1, warped2],result)
    
    # if(initial == False):
    #     result = bl.simple_blending([warp_img2, warp_img1])
    #     cv2.imwrite("blend_img.png",result)
    #     result = bl.simple_blending_main([warped2, warped1],result)
    # else:
    #     result = bl.simple_blending([warp_img1, warp_img2])
    #     cv2.imwrite("blend_img.png",result)
    #     result = bl.simple_blending_main([warped1, warped2],result)


    # if(initial == False):
    #     result = bl.linearBlendingWithConstantWidth([warped2, warped1])
    # else:
    #     result = bl.linearBlendingWithConstantWidth([warped1, warped2])
    
    if(initial == False):
        result = bl.linearBlending([warped2, warped1])
    else:
        result = bl.linearBlending([warped1, warped2])
    cv2.imwrite("intermediate_result.png",result)
    return result

if __name__ == '__main__':
    root_dir = "D:\\Downloads\\CV_HW2\\Photos\\Challenge"
    focal_length = 500
    img_path_lists = [ os.path.join(root_dir,i) for i in os.listdir(root_dir) ]
    img_SIFT = []
    SIFT_detector = cv2.SIFT_create()
    bl = Blender()
    img1,img_gray1 = read_img(img_path_lists[0])
    img2,img_gray2 = read_img(img_path_lists[1])
    kts1, dcpt1 = SIFT_detector.detectAndCompute(img_gray1, None)
    kts2, dcpt2 = SIFT_detector.detectAndCompute(img_gray2, None)
    # matches = feature_matching_debug(kts1, dcpt1,kts2, dcpt2)
    matches = feature_matching_implement(kts1, dcpt1,kts2, dcpt2)
    final_H = find_homography_imp(kts1, dcpt1, kts2, dcpt2,matches)
    stitch_image = stitch_image_imp(final_H,img1, img2,bl,True)
    img_path_lists = img_path_lists[2:]

    for idx,img_path in enumerate(img_path_lists):
        stitch_image_gray = img_to_gray(stitch_image)
        img,img_gray = read_img(img_path)
        kts1, dcpt1 = SIFT_detector.detectAndCompute(img_gray, None)
        kts2, dcpt2 = SIFT_detector.detectAndCompute(stitch_image_gray, None)
        
        # matches = feature_matching_debug(kts1, dcpt1,kts2, dcpt2)
        matches = feature_matching_implement(kts1, dcpt1,kts2, dcpt2)
        final_H = find_homography_imp(kts1, dcpt1, kts2, dcpt2,matches)
        stitch_image = stitch_image_imp(final_H,img,stitch_image,bl,False)
    cv2.imwrite("result.png",stitch_image)
    # Need to do KNN to match the feature
    # We use some function to help me debug
    
