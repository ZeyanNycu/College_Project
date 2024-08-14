import numpy as np
from tqdm import tqdm
from image import *
from utils import get_new_parameters, single_weights_matrix

class Blender:
    def __init__(self):
        pass
    def linearBlending(self,imgs):
        '''
        linear Blending with Constat Width, avoiding ghost region
        # you need to determine the size of constant with
        '''
        img_left, img_right = imgs
        (hl, wl) = img_left.shape[:2]
        (hr, wr) = img_right.shape[:2]
        img_left_value = np.sum(img_left,axis=2)
        img_right_value = np.sum(img_right,axis=2)
        img_left_mask = np.zeros((hl, wl), dtype="int")
        img_right_mask = np.zeros((hr, wr), dtype="int")
        constant_width = 3 # constant width
        
        # find the left image and right image mask region(Those not zero pixels)
        img_left_mask[img_left_value != 0] = 1
        img_right_mask[img_right_value != 0] = 1
                    
        # find the overlap mask(overlap region of two image)
        overlap_mask = np.zeros((hr, wr), dtype="int")
        mask = np.logical_and(img_left_mask!=0,img_right_mask!=0)
        overlap_mask[mask] = 1
        
        # compute the alpha mask to linear blending the overlap region
        alpha_mask = np.zeros((hr, wr)) # alpha value depend on left image
        for i in tqdm(range(hr)): 
            minIdx = maxIdx = -1
            for j in range(wr):
                if (overlap_mask[i, j] == 1 and minIdx == -1):
                    minIdx = j
                if (overlap_mask[i, j] == 1):
                    maxIdx = j
            
            if (minIdx == maxIdx): # represent this row's pixels are all zero, or only one pixel not zero
                continue
                
            decrease_step = 1 / (maxIdx - minIdx)
            for j in range(minIdx, maxIdx + 1):
                alpha_mask[i, j] = 1 - (decrease_step * (j - minIdx))
        cv2.imwrite("alpha.png",alpha_mask*255)
        



        # linearBlendingWithConstantWidth_img = np.copy(img_right)
        linearBlending_img = np.copy(img_left)
        com = (1 - alpha_mask)
        # linear blending with constant width
        linearBlending_img[overlap_mask>0] = alpha_mask[overlap_mask>0][:,None] * img_left[overlap_mask>0] + \
                                                                com[overlap_mask>0][:,None] * img_right[overlap_mask>0]
        mask_left = np.logical_and(overlap_mask==0,img_left_mask>0)
        mask_right = np.logical_and(overlap_mask==0,img_right_mask>0)
        linearBlending_img[mask_left>0] = img_left[mask_left>0]
        linearBlending_img[mask_right>0] = img_right[mask_right>0]
        # for i in tqdm(range(hr)):
        #     for j in range(wr):
        #         minIdx = maxIdx = -1
        #         if (overlap_mask[i, j] == 1 and minIdx == -1):
        #             for width in range(-5,5):
        #                 linearBlendingWithConstantWidth_img[i,j+width] = img_left[i,j+width]
        #             break


        
        return linearBlending_img

    def linearBlendingWithConstantWidth(self, imgs):
        '''
        linear Blending with Constat Width, avoiding ghost region
        # you need to determine the size of constant with
        '''
        img_left, img_right = imgs
        (hl, wl) = img_left.shape[:2]
        (hr, wr) = img_right.shape[:2]
        img_left_value = np.sum(img_left,axis=2)
        img_right_value = np.sum(img_right,axis=2)
        img_left_mask = np.zeros((hl, wl), dtype="int")
        img_right_mask = np.zeros((hr, wr), dtype="int")
        constant_width = 3 # constant width
        
        # find the left image and right image mask region(Those not zero pixels)
        img_left_mask[img_left_value != 0] = 1
        img_right_mask[img_right_value != 0] = 1
                    
        # find the overlap mask(overlap region of two image)
        overlap_mask = np.zeros((hr, wr), dtype="int")
        mask = np.logical_and(img_left_mask!=0,img_right_mask!=0)
        overlap_mask[mask] = 1
        
        # compute the alpha mask to linear blending the overlap region
        alpha_mask = np.zeros((hr, wr)) # alpha value depend on left image
        for i in tqdm(range(hr)):
            minIdx = maxIdx = -1
            for j in range(wr):
                if (overlap_mask[i, j] == 1 and minIdx == -1):
                    minIdx = j
                if (overlap_mask[i, j] == 1):
                    maxIdx = j
            if (minIdx == maxIdx): # represent this row's pixels are all zero, or only one pixel not zero
                continue

            decrease_step = 1 / (maxIdx - minIdx)
            
            # Find the middle line of overlapping regions, and only do linear blending to those regions very close to the middle line.
            middleIdx = int((maxIdx + minIdx) / 2)
            # left 
            for j in range(minIdx, middleIdx + 1):
                if (j >= middleIdx - constant_width):
                    alpha_mask[i, j] = 1 - (decrease_step * (j - minIdx))
                else:
                    alpha_mask[i, j] = 1
            # right
            for j in range(middleIdx + 1, maxIdx + 1):
                if (j <= middleIdx + constant_width):
                    alpha_mask[i, j] = 1 - (decrease_step * (j - minIdx))
                else:
                    alpha_mask[i, j] = 0
        cv2.imwrite("alpha_constant_width.png",alpha_mask*255)
        cv2.imwrite("overlap.png",overlap_mask*255)



        # linearBlendingWithConstantWidth_img = np.copy(img_right)
        linearBlendingWithConstantWidth_img = np.copy(img_left)
        com = (1 - alpha_mask)
        # linear blending with constant width
        linearBlendingWithConstantWidth_img[overlap_mask>0] = alpha_mask[overlap_mask>0][:,None] * img_left[overlap_mask>0] + \
                                                                com[overlap_mask>0][:,None] * img_right[overlap_mask>0]
        mask_left = np.logical_and(overlap_mask==0,img_left_mask>0)
        mask_right = np.logical_and(overlap_mask==0,img_right_mask>0)
        linearBlendingWithConstantWidth_img[mask_left>0] = img_left[mask_left>0]
        linearBlendingWithConstantWidth_img[mask_right>0] = img_right[mask_right>0]
        # for i in tqdm(range(hr)):
        #     for j in range(wr):
        #         minIdx = maxIdx = -1
        #         if (overlap_mask[i, j] == 1 and minIdx == -1):
        #             for width in range(-5,5):
        #                 linearBlendingWithConstantWidth_img[i,j+width] = img_left[i,j+width]
        #             break


        
        return linearBlendingWithConstantWidth_img
    
    def add_image(self,
        panorama: np.ndarray, image: Image, offset: np.ndarray, weights: np.ndarray
    ) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
        """
        Add a new image to the panorama using the provided offset and weights.

        Args:
            panorama: Existing panorama
            image: Image to add to the panorama
            offset: Offset already applied to the panorama
            weights: Weights matrix of the panorama

        Returns:
            panorama: Panorama with the new image
            offset: New offset matrix
            weights: New weights matrix
        """
        H = offset @ image.H
        size, added_offset = get_new_parameters(panorama, image.image, H)

        new_image = cv2.warpPerspective(image.image, added_offset @ H, size)

        if panorama is None:
            panorama = np.zeros_like(new_image)
            weights = np.zeros_like(new_image)
        else:
            panorama = cv2.warpPerspective(panorama, added_offset, size)
            weights = cv2.warpPerspective(weights, added_offset, size)

        image_weights = single_weights_matrix(image.image.shape)
        image_weights = np.repeat(
            cv2.warpPerspective(image_weights, added_offset @ H, size)[:, :, np.newaxis], 3, axis=2
        )

        normalized_weights = np.zeros_like(weights)
        normalized_weights = np.divide(
            weights, (weights + image_weights), where=weights + image_weights != 0
        )

        panorama = np.where(
            np.logical_and(
                np.repeat(np.sum(panorama, axis=2)[:, :, np.newaxis], 3, axis=2) == 0,
                np.repeat(np.sum(new_image, axis=2)[:, :, np.newaxis], 3, axis=2) == 0,
            ),
            0,
            new_image * (1 - normalized_weights) + panorama * normalized_weights,
        ).astype(np.uint8)

        new_weights = (weights + image_weights) / (weights + image_weights).max()

        return panorama, added_offset @ offset, new_weights
    
    def simple_blending(self,images):
        panorama = None
        weights = None
        offset = np.eye(3)
        for image in images:
            panorama, offset, weights = self.add_image(panorama, image, offset, weights)

        return panorama

    def simple_blending_main(self,imgs,blended_img):
        img_left, img_right = imgs
        (hl, wl) = img_left.shape[:2]
        (hr, wr) = img_right.shape[:2]
        img_left_value = np.sum(img_left,axis=2)
        img_right_value = np.sum(img_right,axis=2)
        img_left_mask = np.zeros((hl, wl), dtype="int")
        img_right_mask = np.zeros((hr, wr), dtype="int")
        constant_width = 3 # constant width
        
        # find the left image and right image mask region(Those not zero pixels)
        img_left_mask[img_left_value != 0] = 1
        img_right_mask[img_right_value != 0] = 1
                    
        # find the overlap mask(overlap region of two image)
        overlap_mask = np.zeros((hr, wr), dtype="int")
        mask = np.logical_and(img_left_mask!=0,img_right_mask!=0)
        overlap_mask[mask] = 1


        result = np.copy(img_left)
        mask_left = np.logical_and(overlap_mask==0,img_left_mask>0)
        mask_right = np.logical_and(overlap_mask==0,img_right_mask>0)
        result[mask_left>0] = img_left[mask_left>0]
        result[mask_right>0] = img_right[mask_right>0]
        result[overlap_mask>0] = blended_img[overlap_mask>0]

        return result
    
    def add_weights(self,
        weights_matrix: np.ndarray, image: Image, offset: np.ndarray
    ) -> tuple[np.ndarray, np.ndarray]:
        """
        Add the weights corresponding to the given image to the given existing weights matrix.

        Args:
            weights_matrix: Existing weights matrix
            image: New image to add to the weights matrix
            offset: Offset already applied to the weights matrix

        Returns:
            weights_matrix, offset: The updated weights matrix and the updated offset
        """
        H = offset @ image.H
        size, added_offset = get_new_parameters(weights_matrix, image.image, H)

        weights = single_weights_matrix(image.image.shape)
        weights = cv2.warpPerspective(weights, added_offset @ H, size)[:, :, np.newaxis]

        if weights_matrix is None:
            weights_matrix = weights
        else:
            weights_matrix = cv2.warpPerspective(weights_matrix, added_offset, size)

            if len(weights_matrix.shape) == 2:
                weights_matrix = weights_matrix[:, :, np.newaxis]

            weights_matrix = np.concatenate([weights_matrix, weights], axis=2)

        return weights_matrix, added_offset @ offset


    def get_max_weights_matrix(self,images: list[Image]) -> tuple[np.ndarray, np.ndarray]:
        """
        Compute the maximum weights matrix for the given images.

        Args:
            images: List of images to compute the maximum weights matrix for

        Returns:
            max_weights_matrix: Maximum weights matrix
            offset: Offset matrix
        """
        weights_matrix = None
        offset = np.eye(3)

        for image in images:
            weights_matrix, offset = self.add_weights(weights_matrix, image, offset)

        weights_maxes = np.max(weights_matrix, axis=2)[:, :, np.newaxis]
        max_weights_matrix = np.where(
            np.logical_and(weights_matrix == weights_maxes, weights_matrix > 0), 1.0, 0.0
        )

        max_weights_matrix = np.transpose(max_weights_matrix, (2, 0, 1))

        return max_weights_matrix, offset


    def get_cropped_weights(
        self,
        images: list[Image], weights: np.ndarray, offset: np.ndarray
    ) -> list[np.ndarray]:
        """
        Convert a global weights matrix to a list of weights matrices for each image,
        where each weight matrix is the size of the corresponding image.

        Args:
            images: List of images to convert the weights matrix for
            weights: Global weights matrix
            offset: Offset matrix

        Returns:
            cropped_weights: List of weights matrices for each image
        """
        cropped_weights = []
        for i, image in enumerate(images):
            cropped_weights.append(
                cv2.warpPerspective(
                    weights[i], np.linalg.inv(offset @ image.H), image.image.shape[:2][::-1]
                )
            )

        return cropped_weights


    def build_band_panorama(
        self,
        images: list[Image],
        weights: list[np.ndarray],
        bands: list[np.ndarray],
        offset: np.ndarray,
        size: tuple[int, int],
    ) -> np.ndarray:
        """
        Build a panorama from the given bands and weights matrices.
        The images are needed for their homographies.

        Args:
            images: Images to build the panorama from
            weights: Weights matrices for each image
            bands: Bands for each image
            offset: Offset matrix
            size: Size of the panorama

        Returns:
            panorama: Panorama for the given bands and weights
        """
        pano_weights = np.zeros(size)
        pano_bands = np.zeros((*size, 3))

        for i, image in enumerate(images):
            weights_at_scale = cv2.warpPerspective(weights[i], offset @ image.H, size[::-1])
            pano_weights += weights_at_scale
            pano_bands += weights_at_scale[:, :, np.newaxis] * cv2.warpPerspective(
                bands[i], offset @ image.H, size[::-1]
            )

        return np.divide(
            pano_bands, pano_weights[:, :, np.newaxis], where=pano_weights[:, :, np.newaxis] != 0
        )


    def multi_band_blending(self,images: list[Image], num_bands: int, sigma: float) -> np.ndarray:
        """
        Build a panorama from the given images using multi-band blending.

        Args:
            images: Images to build the panorama from
            num_bands: Number of bands to use for multi-band blending
            sigma: Standard deviation for the multi-band blending

        Returns:
            panorama: Panorama after multi-band blending
        """
        max_weights_matrix, offset = self.get_max_weights_matrix(images)
        size = max_weights_matrix.shape[1:]

        max_weights = self.get_cropped_weights(images, max_weights_matrix, offset)

        weights = [[cv2.GaussianBlur(max_weights[i], (0, 0), 2 * sigma) for i in range(len(images))]]
        sigma_images = [cv2.GaussianBlur(image.image, (0, 0), sigma) for image in images]
        bands = [
            [
                np.where(
                    images[i].image.astype(np.int64) - sigma_images[i].astype(np.int64) > 0,
                    images[i].image - sigma_images[i],
                    0,
                )
                for i in range(len(images))
            ]
        ]

        for k in range(1, num_bands - 1):
            sigma_k = np.sqrt(2 * k + 1) * sigma
            weights.append(
                [cv2.GaussianBlur(weights[-1][i], (0, 0), sigma_k) for i in range(len(images))]
            )

            old_sigma_images = sigma_images

            sigma_images = [
                cv2.GaussianBlur(old_sigma_image, (0, 0), sigma_k)
                for old_sigma_image in old_sigma_images
            ]
            bands.append(
                [
                    np.where(
                        old_sigma_images[i].astype(np.int64) - sigma_images[i].astype(np.int64) > 0,
                        old_sigma_images[i] - sigma_images[i],
                        0,
                    )
                    for i in range(len(images))
                ]
            )

        weights.append([cv2.GaussianBlur(weights[-1][i], (0, 0), sigma_k) for i in range(len(images))])
        bands.append([sigma_images[i] for i in range(len(images))])

        panorama = np.zeros((*max_weights_matrix.shape[1:], 3))

        for k in range(0, num_bands):
            panorama += self.build_band_panorama(images, weights[k], bands[k], offset, size)
            panorama[panorama < 0] = 0
            panorama[panorama > 255] = 255

        return panorama
    # def multi_blending(self,):
