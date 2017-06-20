import Image
from scipy.ndimage.filters import generic_gradient_magnitude, sobel
import numpy
from math import fabs
import sys

def gradient_filter ( im ):
	
	"""
	Takes a grayscale img and retuns the Sobel operator on the image. Fast thanks to Scipy/Numpy. See slow_gradient_filter for
	an implementation of what the Sobel operator is doing
	@im: a grayscale image represented in floats
	"""
	print_fn("Computing energy function using the Sobel operator")
	im_width, im_height = im.size 
	im_arr = numpy.reshape( im.getdata( ), (im_height, im_width) )
	sobel_arr = generic_gradient_magnitude( im, derivative=sobel )
	gradient = Image.new("F", im.size)
	gradient.putdata( list( sobel_arr.flat ) )
	return gradient