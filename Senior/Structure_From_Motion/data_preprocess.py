import os
import pickle
from tqdm import tqdm
import pickle


results = {}
# Read the setting of Data Preprocess
config = Config()
data_dict = parse_sample_submission(config.base_path)
datasets = list(data_dict.keys())
root_dir = '/kaggle/input/image-matching-challenge-2024/train'
scene_path_lists = [ os.path.join(root_dir,scene_name) for scene_name in os.listdir(root_dir) if('.csv' not in scene_name) ]
dtype = torch.float32 # ALIKED has issues with float16
device = 'cuda'
SIFT = False
visualize = True
Knn = True
# Load pretrained ALIKED model 
extractor = ALIKED(
    max_num_keypoints=4096, 
    detection_threshold=0.01, 
    resize=1024
).eval().to('cuda', dtype)
# Create SIFT model to extract image feature.
SIFT_extractor = cv2.SIFT_create()
output_dir_root: Path = Path.cwd() / "results_new"
matcher_params = {
        "width_confidence": -1,
        "depth_confidence": -1,
        "mp": True,
}
device = torch.device("cuda")
FLANN_INDEX_KDTREE = 1
index_params = dict(algorithm=FLANN_INDEX_KDTREE, trees=5)
search_params = dict(checks=50)  # or pass an empty dictionary
# Create Knn model to match feature fo the paired image 
matcher = KF.LightGlueMatcher("aliked", matcher_params).eval().to(device)
flann = cv2.FlannBasedMatcher(index_params, search_params)

# Perform KNN matching
# matches = flann.knnMatch(descriptors1, descriptors2, k=2)


for scene_path in scene_path_lists:
    output_dir = output_dir_root / scene_path.split("/")[-1]
    output_dir.mkdir(parents=True, exist_ok=True)
    scene_path = os.path.join(scene_path,'images')
    img_path_lists = [ os.path.join(scene_path,img_path) for img_path in os.listdir(scene_path)  ]
    img_path_lists.sort()
    if(len(img_path_lists)>40):
        continue
    for idx,img_path in enumerate(tqdm(img_path_lists)):
        img_num = img_path.split("/")[-1]
        img_num = img_num.split(".")[0]
        output_path = output_dir / img_num
        output_path.mkdir(parents=True, exist_ok=True)
        if(SIFT == False):
            image = load_torch_image(img_path, device=device).to(dtype)
            features = extractor.extract(image)
            with open(f'{output_path}/keypoints.pkl', 'wb') as f:
                pickle.dump(features['keypoints'].detach().cpu().numpy(), f)
            with open(f'{output_path}/descriptors.pkl', 'wb') as f:
                pickle.dump(features['descriptors'].detach().cpu().numpy(), f)
        else:
            image = cv2.imread(img_path)
            keypoints, descriptors = SIFT_extractor.detectAndCompute(image, None)
            # We need to organize it as the following structure
            # keypoints -> [ number of batch, number of feature, pixel ] (1,4096,2)
            # descriptors -> [ number of batch, number of feature, channel ] (1,4096,128)'
            keypoints = np.array([x.pt for x in keypoints])[None]
            descriptors = np.array([x for x in descriptors])[None]
            with open(f'{output_path}/keypoints.pkl', 'wb') as f:
                pickle.dump(keypoints, f)
            with open(f'{output_path}/descriptors.pkl', 'wb') as f:
                pickle.dump(descriptors, f)

# Start to match the keypoints
for scene_path in scene_path_lists:
    output_dir = output_dir_root / scene_path.split("/")[-1]
    output_dir.mkdir(parents=True, exist_ok=True)
    scene_path = os.path.join(scene_path,'images')
    img_path_lists = [ os.path.join(scene_path,img_path) for img_path in os.listdir(scene_path)  ]
    img_path_lists.sort()
    bool_map = {}
    corresponding_matrix = {}
    for idx,img_path in enumerate(tqdm(img_path_lists)):
        img_num = img_path.split("/")[-1]
        img_num = img_num.split(".")[0]
        source_path = output_dir / img_num
        # Load the pickle file
        with open(f'{source_path}/keypoints.pkl', 'rb') as file:
            source_keypoints = pickle.load(file)
        with open(f'{source_path}/descriptors.pkl', 'rb') as file:
            source_descriptors = pickle.load(file)
        
        for jdx,target_img_path in enumerate(img_path_lists):
            if(jdx<=idx):
                continue
            
            target_img_num = target_img_path.split("/")[-1]
            target_img_num = target_img_num.split(".")[0]
            target_path = output_dir / target_img_num
            # Load keypoints and descriptors of target image
            with open(f'{target_path}/keypoints.pkl', 'rb') as file:
                target_keypoints = pickle.load(file)
            with open(f'{target_path}/descriptors.pkl', 'rb') as file:
                target_descriptors = pickle.load(file)
            
            if(jdx not in bool_map.keys()):
                bool_map[jdx] = np.ones((target_keypoints.shape[1]))
            if(Knn):
                '''
                Descriptor should be (number of feature,channel)
                '''
                descriptors1,descriptors2 = source_descriptors,target_descriptors
                keypoints1 = source_keypoints[0]
                keypoints2 = target_keypoints[0]
                matches = flann.knnMatch(descriptors1[0], descriptors2[0], k=2)
                good_matches = []
                # We filter some feature that is ambiguish
                for m, n in matches:
                        if m.distance < 0.75 * n.distance:
                            good_matches.append(m)
                # We need to organize output as Glue Liked format
                
                '''
                The data structure of indices is (number of match,2) -> (source idx),(target idx)
                The data structure of distance is (number of match) -> give it 0 (Already use some method to filter) 
                '''
                indices,distances = [],[]
                for match in good_matches:
                    distances.append(0)
                    indices.append([match.queryIdx,match.trainIdx])
            else:
                device = torch.device("cuda")
                keypoints1 = torch.from_numpy(source_keypoints).to(torch.float16).to(device)
                keypoints2 = torch.from_numpy(target_keypoints).to(torch.float16).to(device)
                descriptors1 = torch.from_numpy(source_descriptors).to(torch.float16).to(device)
                descriptors2 = torch.from_numpy(target_descriptors).to(torch.float16).to(device)

                with torch.inference_mode():
                    # Use matcher to match the feature
                    distances, indices = matcher(
                        descriptors1[0], 
                        descriptors2[0], 
                        KF.laf_from_center_scale_ori(keypoints1),
                        KF.laf_from_center_scale_ori(keypoints2),
                    )
                n_matches = len(indices)
                if(n_matches == 0):
                    continue
                # We have 2100 Possible 3D points We construct a bool map to cover with its
                # If bool map is true means it is still available
                # If bool map is untrue means it is unavailable
                '''
                The data structure of distance & indices
                distances -> (match_points,1)
                indices -> (match_points,2)
                '''
                keypoints1 = keypoints1.detach().cpu().numpy()[0]
                keypoints2 = keypoints2.detach().cpu().numpy()[0]
                descriptors1 = descriptors1.detach().cpu().numpy()
                descriptors2 = descriptors2.detach().cpu().numpy()
                distances, indices = distances.detach().cpu().numpy(), indices.detach().cpu().numpy()
            if(visualize):
                # This section help us to visualize the final results.
                img1 = cv2.imread('/kaggle/input/image-matching-challenge-2024/train/transp_obj_glass_cup/images/00.png')
                img2 = cv2.imread('/kaggle/input/image-matching-challenge-2024/train/transp_obj_glass_cup/images/01.png')
                # construct A big array to indicate images
                # image shape -> (h,w,3)
                # A (some space) B
                image = np.zeros(img1.shape)
                image = np.concatenate((image,np.zeros((img1.shape[0],200,3))),axis=1)
                image = np.concatenate((image,np.zeros(img2.shape)),axis=1)
                image[:,:img1.shape[1]] = img1
                image[:,img1.shape[1]+200:] = img2
                '''
                Use indices to draw the pictures
                '''
                for distance,indice in zip(distances,indices):
                    if(distance>0.5):
                        continue
                    '''
                    start_point = (100, 100)
                    end_point = (400, 400)

                    color = (0, 255, 0)  # Green color
                    thickness = 5
                    '''
                    start_point = (int(keypoints1[indice[0]][1]),int(keypoints1[indice[0]][0]))
                    end_point = (int(keypoints2[indice[1]][1]+img1.shape[1]+200),int(keypoints2[indice[1]][0]))
                    color = (0, 255, 0)  # Green color
                    thickness = 2
                    cv2.line(image, start_point, end_point, color, thickness)
                cv2.imwrite('testing_result.png',image)
                asd
            
            for distance,indice in zip(distances,indices):
                # Construct the matrix whose format is
                # (m,n,2) -> m indicate the number of camera
                # (m,n,2) -> n indicate the number of data points
                # (m,n,2) -> the coordinates of image
                if(distance>0.5):
                    continue
                if( f'{idx}_{keypoints1[indice[0]][0]}_{keypoints1[indice[0]][1]}' in bool_map.keys()):
                    continue
                if( f'{idx}_{keypoints1[indice[0]][0]}_{keypoints1[indice[0]][1]}' not in corresponding_matrix.keys() ):
                    corresponding_matrix[f'{idx}_{keypoints1[indice[0]][0]}_{keypoints1[indice[0]][1]}'] = [ [] for _ in img_path_lists ]    
                corresponding_matrix[f'{idx}_{keypoints1[indice[0]][0]}_{keypoints1[indice[0]][1]}'][jdx] = [keypoints2[indice[1]][0],keypoints2[indice[1]][1]]
                # We need to update the bool map
                bool_map[f'{jdx}_{keypoints2[indice[1]][0]}_{keypoints2[indice[1]][1]}'] = 0
    with open(f'{output_dir}/matching_global.pkl', 'wb') as f:
        pickle.dump(corresponding_matrix, f)