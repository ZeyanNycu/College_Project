import torch
import os
from utils import geo_utils, ba_functions
import numpy as np
import pycolmap
from scipy.spatial.transform import Rotation 
from scipy.optimize import least_squares
from scipy.sparse import lil_matrix

def getEuler(R2):
    euler = Rotation.from_matrix(R2)
    return euler.as_rotvec()
def getRotation(Q, type_ = 'q'):
    if type_ == 'q':
        R = Rotation.from_quat(Q)
        return R.as_matrix()
    elif type_ == 'e':
        R = Rotation.from_rotvec(Q)
        return R.as_matrix()

def getCameraPointIndices(Xs):
    camera_indices = []
    point_indices = []
    h, w,_ = Xs.shape
    for i in range(h):
        for j in range(w):
            if np.sum(Xs[i,j]) != 0:
                camera_indices.append(i)
                point_indices.append(j)

    return np.array(camera_indices).reshape(-1), np.array(point_indices).reshape(-1)

def project(points_3d, camera_params, K, camera_indices, point_indices,interval):
    def projectPoint_(R, C, pt3D, K):
        P2 = np.dot(K, np.dot(R, np.hstack((np.identity(3), -C.reshape(3,1)))))
        x3D_4 = np.hstack((pt3D, 1))
        x_proj = np.dot(P2, x3D_4.T)
        x_proj /= x_proj[-1]
        return x_proj

    x_proj = []
    for i in range(len(camera_params)):
        R = getRotation(camera_params[i, :3], 'e')
        C = camera_params[i, 3:].reshape(3,1)
        pt3D = points_3d[i]
        k_idx = 0
        for idx,value in enumerate(interval):
            if(point_indices[i]<value):
                k_idx = idx
                break
        pt_proj = projectPoint_(R, C, pt3D, K[k_idx])[:2]
        x_proj.append(pt_proj)    
    return np.array(x_proj)

def bundle_adjustment_sparsity(Xs,n_points , number_of_cam):
    
    """
    To create the Sparsity matrix
    """
    tmp_version = np.sum(Xs,axis=2)
    n_observations = np.sum((tmp_version != 0))

    m = n_observations * 2
    n = number_of_cam * 6 + n_points * 3
    A = lil_matrix((m, n), dtype=int)
    print(m, n)


    i = np.arange(n_observations)
    camera_indices, point_indices = getCameraPointIndices(Xs)

    for s in range(6):
        A[2 * i, camera_indices * 6 + s] = 1
        A[2 * i + 1, camera_indices * 6 + s] = 1

    for s in range(3):
        A[2 * i, (number_of_cam - 1)* 6 + point_indices * 3 + s] = 1
        A[2 * i + 1, (number_of_cam - 1) * 6 + point_indices * 3 + s] = 1

    return A


def fun(x0, Xs, number_of_cam, n_points, camera_indices, point_indices, points_2d, Ks,interval):
    """Compute residuals.
    
    `params` contains camera parameters and 3-D coordinates.
    """
    camera_params = x0[:number_of_cam * 6].reshape((number_of_cam, 6))
    points_3d = x0[number_of_cam * 6:].reshape((n_points, 3))
    points_proj = project(points_3d[point_indices], camera_params[camera_indices], Ks,camera_indices, point_indices,interval)
    error_vec = (points_proj - points_2d).ravel()
    
    return error_vec

def euc_ba_pycolmap(Xs,Rs,Ts,Ks,pts3D_pred):
    '''
    Xs -> is the track of images points -> (m,n,2)
    Rs -> is the rotation matrix -> (m, 3, 3)
    ts -> is the translation matrix -> (m, 3)
    Ks -> is the intrinct matrix of ks -> (m, 3, 3)
    pts3D_pred -> is the 3D point that we predict -> (n, 4)
    m means the number of camera
    n means the number of 3D points
    '''
    '''
    The mathematical theory behind bundle adjustment.
    '''
    
    m,n,_ = Xs.shape
    RC_lists = []
    for i in range(m):
        # How to construct X0
        t,r = Ts[i],Rs[i]
        Q = getEuler(r)
        RC = [Q[0], Q[1], Q[2], t[0], t[1], t[2]]
        RC_lists.append(RC)
    RC_list = np.array(RC_lists).reshape(-1, 6)
    points_3d = pts3D_pred[:,:-1]
    x0 = np.hstack((RC_list.ravel(), points_3d.ravel()))
    camera_indices, point_indices = getCameraPointIndices(Xs)
    points_2d = Xs.reshape(-1,2)
    points_2d = points_2d[np.sum(points_2d,axis=1) != 0]
    A = bundle_adjustment_sparsity(Xs,n,m)
    visibility = np.sum(Xs,axis=2)
    interval = np.sum(visibility != 0, axis =1)
    res = least_squares(fun, x0, jac_sparsity=A, verbose=2, x_scale='jac', ftol=1e-10, method='trf',
                        args=(Xs,m,n,camera_indices, point_indices, points_2d, Ks,interval))
    # We need to deconde output into the following form
    '''
        Xs -> n,4
        Rs -> m,3,3
        ts -> m,3
        Ps -> m,3,4
    '''
    x1 = res.x
    number_of_cam = m
    n_points = n
    optimized_camera_params = x1[:number_of_cam * 6].reshape((number_of_cam, 6))
    optimized_C_set, optimized_R_set = [], []
    for i in range(len(optimized_camera_params)):
        R = getRotation(optimized_camera_params[i, :3], 'e')
        C = optimized_camera_params[i, 3:].reshape(3,1)
        optimized_C_set.append(C)
        optimized_R_set.append(R)
 
    optimized_points_3d = x1[number_of_cam * 6:].reshape((n_points, 3))
    optimized_points_3d = np.concatenate((optimized_points_3d, np.ones((optimized_points_3d.shape[0],1))), axis=1)
    
    ba_res = {}
    
    ba_res['Rs'] = np.array(optimized_R_set)
    ba_res['ts'] = np.array(optimized_C_set)
    ba_res['ts'] = ba_res['ts'][:,:,0]
    ba_res['Xs'] = optimized_points_3d # 4,n
    Ps_lists = []
    for i in range(m):
        # np.dot(K, np.dot(R, np.hstack((np.identity(3), -C.reshape(3,1)))))
        Ps_lists.append(np.dot(Ks[i], np.dot(optimized_R_set[i], np.hstack((np.identity(3), -optimized_C_set[i].reshape(3,1))))))
        # Ps_lists.append(Ks[i] @ np.hstack((optimized_R_set[i],optimized_C_set[i])))
    ba_res['Ps'] = np.array(Ps_lists)
    
    
    return ba_res
        
    

def prepare_predictions(data, pred_cam, conf, bundle_adjustment):
    # Take the inputs from pred cam and turn to ndarray
    outputs = {}
    outputs['scan_name'] = data.scan_name
    calibrated = conf.get_bool('dataset.calibrated')

    Ns = data.Ns.cpu().numpy()
    Ns_inv = data.Ns_invT.transpose(1, 2).cpu().numpy()  # Ks for calibrated, a normalization matrix for uncalibrated
    M = data.M.cpu().numpy()
    xs = geo_utils.M_to_xs(M)

    Ps_norm = pred_cam["Ps_norm"].cpu().numpy()  # Normalized camera!!
    Ps = Ns_inv @ Ps_norm  # unnormalized cameras
    pts3D_pred = geo_utils.pflat(pred_cam["pts3D"]).cpu().numpy()

    pts3D_triangulated = geo_utils.n_view_triangulation(Ps, M=M, Ns=Ns)

    outputs['xs'] = xs  # to compute reprojection error later
    outputs['Ps'] = Ps
    outputs['Ps_norm'] = Ps_norm
    outputs['pts3D_pred'] = pts3D_pred  # 4,m
    outputs['pts3D_triangulated'] = pts3D_triangulated  # 4,n

    if calibrated:
        Ks = Ns_inv  # data.Ns.inverse().cpu().numpy()
        outputs['Ks'] = Ks
        Rs_gt, ts_gt = geo_utils.decompose_camera_matrix(data.y.cpu().numpy(), Ks)  # For alignment and R,t errors
        outputs['Rs_gt'] = Rs_gt
        outputs['ts_gt'] = ts_gt

        Rs_pred, ts_pred = geo_utils.decompose_camera_matrix(Ps_norm)
        outputs['Rs'] = Rs_pred
        outputs['ts'] = ts_pred

        Rs_fixed, ts_fixed, similarity_mat = geo_utils.align_cameras(Rs_pred, Rs_gt, ts_pred, ts_gt, return_alignment=True) # Align  Rs_fixed, tx_fixed
        outputs['Rs_fixed'] = Rs_fixed
        outputs['ts_fixed'] = ts_fixed
        outputs['pts3D_pred_fixed'] = (similarity_mat @ pts3D_pred)  # 4,n
        outputs['pts3D_triangulated_fixed'] = (similarity_mat @ pts3D_triangulated)

        if bundle_adjustment:
            repeat = conf.get_bool('ba.repeat')
            triangulation = conf.get_bool('ba.triangulation')
            # ba_res = ba_functions.euc_ba(xs, Rs=Rs_pred, ts=ts_pred, Ks=np.linalg.inv(Ns),
            #                              Xs_our=pts3D_pred.T, Ps=None,
            #                              Ns=Ns, repeat=repeat, triangulation=triangulation, return_repro=True) #    Rs, ts, Ps, Xs
            ba_res = euc_ba_pycolmap(xs,Rs_pred,ts_pred,np.linalg.inv(Ns),pts3D_pred.T)
            # Xs -> n,4
            # Rs -> m,3,3
            # ts -> m,3
            # Ps -> m,12
            
            outputs['Rs_ba'] = ba_res['Rs']
            outputs['ts_ba'] = ba_res['ts']
            outputs['Xs_ba'] = ba_res['Xs'].T  # 4,n
            outputs['Ps_ba'] = ba_res['Ps']

            R_ba_fixed, t_ba_fixed, similarity_mat = geo_utils.align_cameras(ba_res['Rs'], Rs_gt, ba_res['ts'], ts_gt,
                                                                       return_alignment=True)  # Align  Rs_fixed, tx_fixed
            outputs['Rs_ba_fixed'] = R_ba_fixed
            outputs['ts_ba_fixed'] = t_ba_fixed
            outputs['Xs_ba_fixed'] = (similarity_mat @ outputs['Xs_ba'])

    else:
        if bundle_adjustment:
            repeat = conf.get_bool('ba.repeat')
            triangulation = conf.get_bool('ba.triangulation')
            ba_res = ba_functions.proj_ba(Ps=Ps, xs=xs, Xs_our=pts3D_pred.T, Ns=Ns, repeat=repeat,
                                          triangulation=triangulation, return_repro=True, normalize_in_tri=True)   # Ps, Xs
            outputs['Xs_ba'] = ba_res['Xs'].T  # 4,n
            outputs['Ps_ba'] = ba_res['Ps']

    return outputs


def compute_errors(outputs, conf, bundle_adjustment):
    model_errors = {}
    calibrated = conf.get_bool('dataset.calibrated')
    Ps = outputs['Ps']
    pts3D_pred = outputs['pts3D_pred']
    xs = outputs['xs']
    pts3D_triangulated = outputs['pts3D_triangulated']

    model_errors["our_repro"] = np.nanmean(geo_utils.reprojection_error_with_points(Ps, pts3D_pred.T, xs))
    model_errors["triangulated_repro"] = np.nanmean(geo_utils.reprojection_error_with_points(Ps, pts3D_triangulated.T, xs))
    if calibrated:
        Rs_fixed = outputs['Rs_fixed']
        ts_fixed = outputs['ts_fixed']
        Rs_gt = outputs['Rs_gt']
        ts_gt = outputs['ts_gt']
        Rs_error, ts_error = geo_utils.tranlsation_rotation_errors(Rs_fixed, ts_fixed, Rs_gt, ts_gt)
        model_errors["ts_mean"] = np.mean(ts_error)
        model_errors["ts_med"] = np.median(ts_error)
        model_errors["Rs_mean"] = np.mean(Rs_error)
        model_errors["Rs_med"] = np.median(Rs_error)

    if bundle_adjustment:
        Xs_ba = outputs['Xs_ba']
        Ps_ba = outputs['Ps_ba']
        model_errors['repro_ba'] = np.nanmean(geo_utils.reprojection_error_with_points(Ps_ba, Xs_ba.T, xs))
        if calibrated:
            Rs_fixed = outputs['Rs_ba_fixed']
            ts_fixed = outputs['ts_ba_fixed']
            Rs_gt = outputs['Rs_gt']
            ts_gt = outputs['ts_gt']
            Rs_ba_error, ts_ba_error = geo_utils.tranlsation_rotation_errors(Rs_fixed, ts_fixed, Rs_gt, ts_gt)
            model_errors["ts_ba_mean"] = np.mean(ts_ba_error)
            model_errors["ts_ba_med"] = np.median(ts_ba_error)
            model_errors["Rs_ba_mean"] = np.mean(Rs_ba_error)
            model_errors["Rs_ba_med"] = np.median(Rs_ba_error)
    # Rs errors mean, ts errors mean, ba repro, rs ba mean, ts ba mean

    projected_pts = geo_utils.get_positive_projected_pts_mask(Ps @ pts3D_pred, conf.get_float('loss.infinity_pts_margin'))
    valid_pts = geo_utils.xs_valid_points(xs)
    unprojected_pts = np.logical_and(~projected_pts, valid_pts)
    part_unprojected = unprojected_pts.sum() / valid_pts.sum()

    model_errors['unprojected'] = part_unprojected

    return model_errors





