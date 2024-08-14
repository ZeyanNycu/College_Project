param1=$1

# python single_scene_optimization.py --conf Optimization_Proj.conf # Uncalibrated
CUDA_VISIBLE_DEVICES=$param1 python single_scene_optimization.py --conf Optimization_Euc.conf # Calibrated