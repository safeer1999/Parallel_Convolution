from PIL import Image
import numpy as np


def rgb2gray(rgb) :
	r,g,b = rgb[:,:,0],rgb[:,:,1],rgb[:,:,2]
	gray = 0.2989*r + 0.5870*g + 0.1140*b
	return gray

img = Image.open('index.jpeg') #sample image
img = np.asarray(img)
img = rgb2gray(img)#convert to grayscale

print(img.shape)

file=open('index.txt',"w")
max = float('-inf')
file.write(str(img.shape)+'\n')
for i in range(img.shape[0]) :
	#print(len(str(img[i,:].tolist())[1:-1]))
	if len(str(img[i,:].tolist())[1:-1]) > max :
		max = len(str(img[i,:].tolist())[1:-1])

	file.write(str(img[i,:].tolist())[1:-1]+'\n')

print(max)