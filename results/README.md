file name:
<operation_type>_<kernel_type>_<original_size>_<new_size>

operation_type:
by : bydirectional resizing
small : both sides decrease
enlarge : both sides increase

kernel_type:
1:3x3 Sobel
2:5x5 Sobel
3:3x3 Laplace
4:5x5 Laplace
5(Customized) : 3x3 近邻方差
6(Customized) : 3x3 预移动方差
