# Barcodes

Several images of linear barcodes are given, and it is asked to:
- Find the ROI (Region Of Interest) with the Barcode and extract some characteristics
- Estimate quality parameters of Barcode according to the specific ISO/IEC 15416

In particolar, it is requested to compute the following quality parameters:
1. Symbol Contrast
2. Reflectance
3. Min edge contrast
4. Modulation
5. Defects
6. Overall Symbol Grade

![Example](/codici-lineari-dati/images/1.png)

## ROI

### Binarization
In order to produce a correct binarization of the image, I have used Otsu's Threshold, then I applied a morphology operator with a vertical line dimensional kernel, in opening configuration, to eliminate from the image different objects not relates to the barcode.
After this, a new morphology operation, a dilation by a small rectangular kernel (3 rows, 1 col), several times, to connect all the regions of the barcode and creating a big one rectangle.

![Example](https://github.com/francovia/Barcodes/tree/master/codici-lineari-dati/images/2.png)

### Labeling
According to the Flood-fill approach, I can find all the labels. Furthermore, to find exactly my barcode I compute all the areas and I estimate the biggest one. in this manner the biggest has to be the area of the barcode. Then I want to eliminate all the other labels, with areas less than my barcode.

### Position and Orientation
In this section, I have used the function RotatedRect that it gives me back the minimum enclosing rectangle(MER). And In order to find the minimum and maximum x and y position of the barcode, I have checked all the corners of my MER.
While, to find angle, I have used the difference between two y positons of respectively, the right and left corner along the x direction, i.e. the same side of the rectangle.
At the end, by using warpAffine function, I create a rotation matrix with the angle founded before, and I rotated and cutted the image.

![Example](https://github.com/francovia/Barcodes/tree/master/codici-lineari-dati/images/4.png)

### X-dimension
To get a precise estimation of my barcode, I just cutted my original image around the region of my barcode, with the previous steps. Then, I applied again the binarization with the Otsu's threshold and the morphology open operation with a vertical line element.
Now, I create for each bar a bounded rect that contains the bar, after the estimation of the contours.
Then to find the X dimensions requested of smallest bar of barcode, I evaluated all the bars in a for loop and I extracted the height, width and area.
At the end, I store all the parameters in my parameter's vector and I cut again the image in the specific ROI.

![Example](https://github.com/francovia/Barcodes/tree/master/codici-lineari-dati/images/6.png)

## BARCODE PARAMETERS

I started to create 10 parallel lines, by dividing the number of rows of my ROI, then I stored pixels values for each line into an scan_profile vector, with a for loop.
I founded the first two parameters: Minimum Reflectance and Symbol Contrast.
Then in another loop, I have defined the median line equal than the half of the Symbol Contrast SC/2. After that, I compute the frei chen operator, to evaluate the forward and backward pixels along the scan, in order to reduce the noise. So, when I am in the proximity of my median line, with two thresholds, one for the upper part and the other for the lower part of the edge, I can compute the edge contrast and update the number of edges founded.
A similar procedure I have adopted to find the defects, at the end, if I find an edge I can compare the parameters with the desired one and I will store the parameters into the parameter's vector.

![Example](https://github.com/francovia/Barcodes/tree/master/codici-lineari-dati/images/8.png)

### Overall Symbol Grade
By using comparision between all the parameters, for all the parallel scans, I computed the Overall Symbol Grade. Then I compute the mean value between them.
In conclusion, I created a function to print all the parameters founded previously in the barcode, to a excel file.


