# Barcodes


### Dependencies

OpenCV

------

Several images of linear barcodes are used as dataset to:
- Find the ROI (Region Of Interest) with the Barcode and extract some characteristics
- Estimate quality parameters of Barcode according to the specific ISO/IEC 15416

In particolar, the following quality parameters are computed:
1. Symbol Contrast
2. Reflectance
3. Min edge contrast
4. Modulation
5. Defects
6. Overall Symbol Grade

![Example](/codici-lineari-dati/images/1.png)

## ROI

### Binarization
In order to produce a correct binarization of the image, I've used Otsu's Threshold, then I've applied a morphology operator composed by a vertical line dimension kernel, in opening configuration, to eliminate other objects not related to the barcode.
After, a new morphology operation, a dilation by a small rectangular kernel (3 rows, 1 col), several times, to connect all the regions of the barcode and creating a big one rectangle.

![Example](/codici-lineari-dati/images/2.png)

### Labeling
According to the Flood-fill approach, I can find all the labels. Furthermore, to find exactly the barcode into the image, I've used all object's areas and I've taken the biggest one. Then I've eliminated all the other labels, with areas less than the barcode.

### Position and Orientation
In this section, I've used the function RotatedRect that it gives me back the minimum enclosing rectangle(MER) and In order to find the minimum and maximum position of the barcode, I've checked all the corners of the MER.
While, to find angle, I've computed the difference between the right and left corner along the x direction, i.e. the same side of the rectangle.
At the end, by using warpAffine function, I've created a rotation matrix with the angle founded before, and I've rotated and cutted the image.

![Example](/codici-lineari-dati/images/4.png)

### X-dimension
To get a precise estimation of my barcode, I've just cutted my original image around the region of my barcode, with the previous steps. Then, I've applied again the binarization with the Otsu's threshold and the morphology open operation with a vertical line element.
After, I've created for each bar a bounded rect that contains the bar, after the estimation of the contours.
Then to find the X dimensions requested of smallest bar of barcode, I've evaluated all the bars in a for loop and I've extracted the height, width and area.
At the end, I've stored all the parameters in my parameter's vector and I've cutted again the image in the specific ROI.

![Example](/codici-lineari-dati/images/6.png)

## BARCODE PARAMETERS

I've started to create 10 parallel lines, by dividing the number of rows of my ROI, then I've stored pixels values for each line into an scan_profile vector, with a for loop.
I've founded the first two parameters: Minimum Reflectance and Symbol Contrast.
Then in another loop, I've defined the median line equal than the half of the Symbol Contrast SC/2. After that, I've computed frei-chen operator, to evaluate forward and backward pixels along the scan, in order to reduce noise. So, when I am in proximity of mean line, with two thresholds, one for upper part and the other for lower part of the edge, I've computed edge contrast and updated the number of edges founded. A similar procedure I've adopted to find defects. 
At the end, if I've founded an edge I could compared parameters with the desired ones and I've stored parameters into parameter's vector.

![Example](/codici-lineari-dati/images/8.png)

### Overall Symbol Grade
By using comparision between all parameters, for all parallel scans, I've computed the Overall Symbol Grade, then the mean value.
In conclusion, I've printed all parameters, founded previously, to a excel file.


