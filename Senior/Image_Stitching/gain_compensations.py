import numpy as np
import cv2



def calculateI(image,overlap):
    I = (
            np.sum(
                image * np.repeat(overlap[:, :, np.newaxis], 3, axis=2),
                axis=(0, 1),
            )
            / overlap.sum()
        )
    return I


def get_gain_compensations(
    warp1,warp2,sigma_n: float = 10.0, sigma_g: float = 0.1
):
    """
    Compute the gain compensation for each image, and save it into the images objects.

    Args:
        images: Images of the panorama
        pair_matches: Pair matches between the images
        sigma_n: Standard deviation of the normalized intensity error
        sigma_g: Standard deviation of the gain
    """
    coefficients = []
    results = []
    (hl, wl) = warp1.shape[:2]
    (hr, wr) = warp2.shape[:2]
    img_left_value = np.sum(warp1,axis=2)
    img_right_value = np.sum(warp2,axis=2)
    img_left_mask = np.zeros((hl, wl), dtype="int")
    img_right_mask = np.zeros((hr, wr), dtype="int")
    img_left_mask[img_left_value != 0] = 1
    img_right_mask[img_right_value != 0] = 1
    overlap = np.zeros((hr, wr), dtype="int")
    mask = np.logical_and(img_left_mask!=0,img_right_mask!=0)
    overlap[mask] = 1
    
    pair_match_I_0 = calculateI(warp1,overlap)
    pair_match_I_1 = calculateI(warp2,overlap)

    coefs = [ np.zeros(3) for _ in range(2) ]
    result = np.zeros(3)
    coefs[0] = (
        (pair_match_I_0 ** 2 / sigma_n ** 2) + (1 / sigma_g ** 2)
    ) 
    coefs[1] -= (
        (1 / sigma_n ** 2) * pair_match_I_0 * pair_match_I_1
    )
    result += 1 / sigma_g ** 2
    coefficients.append(coefs)
    results.append(result)


    coefs = [ np.zeros(3) for _ in range(2) ]
    result = np.zeros(3)
    coefs[0] -= (
        (1 / sigma_n ** 2) * pair_match_I_0 * pair_match_I_1
    ) 
    coefs[1] = (
        (pair_match_I_1 ** 2 / sigma_n ** 2) + (1 / sigma_g ** 2)
    )
    result += 1 / sigma_g ** 2
    coefficients.append(coefs)
    results.append(result)



    coefficients = np.array(coefficients)
    results = np.array(results)

    gains = np.zeros_like(results)

    for channel in range(coefficients.shape[2]):
        coefs = coefficients[:, :, channel]
        res = results[:, channel]

        gains[:, channel] = np.linalg.solve(coefs, res)
    images = [warp1,warp2]
    max_pixel_value = np.max([image for image in images])

    if gains.max() * max_pixel_value > 255:
        gains = gains / (gains.max() * max_pixel_value) * 255

    return gains[0],gains[1]