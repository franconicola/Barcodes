#include "../include/functions.h"



int main(int argc, char* argv[]){

	//set in img_path the path to the image that you want to load
	string img_path = "/home/nicola/Desktop/Computer Vision/codici-lineari-dati/";

	vector<string> images = {"C39_4.4LOW.BMP","C39_4.4UP.BMP", "C39_7.5LOW.BMP", "C39_7.5UP.BMP", "C128_4.4LOW.BMP",
	"C128_4.4UP.BMP", "C128_7.5LOW.BMP", "C128_7.5UP.BMP", "EAN128-CONTRAST IMGB.BMP", "EAN128-DEFECTS IMGB.BMP",
	"EAN128-LOW DECODABILITY IMGB.BMP", "EAN128-MASTER IMGB.BMP", "EAN-UPC-CONTRAST IMGB.BMP","EAN-UPC-DECODABILITY IMGB.BMP",
	"EAN-UPC-DEFECTS IMGB.BMP","EAN-UPC-EAN-13 MASTER GRADE IMGB.BMP","EAN-UPC-UPC-A MASTER GRADE IMGB.BMP",
	"I25-CONTRAST IMGB.BMP", "I25-DEFECTS IMGB.BMP", "I25-LOW DECODABILITY IMGB.BMP", "I25-MASTER GRADE IMGB.BMP",
	"UPC#01.BMP", "UPC#03.BMP", "UPC#04.BMP", "UPC#05.BMP", "UPC#06.BMP", "UPC#07.BMP", "UPC#08.BMP", "UPC#09.BMP",
	"UPC#10.BMP", "UPC#11.BMP", "UPC#12.BMP", "UPC#13.BMP", "UPC#14.BMP", "UPC#15.BMP", "UPC#16.BMP", "UPC#17.BMP",
	"UPC#18.BMP", "UPC#19.BMP", "UPC#20.BMP", "UPC#21.BMP", "UPC#22.BMP", "UPC#23.BMP", "UPC#24.BMP", "UPC#25.BMP",
	"UPC#26.BMP", "UPC#27.BMP", "UPC#28.BMP", "UPC#29.BMP", "UPC#30.BMP", "UPC#31.BMP", "UPC#32.BMP"};

	//Create a vector that contains all the parameters, that has to be printed
	vector<vector<vector<float>>> parameters(52, vector<vector<float>>(20,vector<float>(10)));

	for(int img = 0; img < 52; img++){

		cout << "===========================================" << endl;

		//load source image
		Mat source_image = imread(img_path + images[img], CV_LOAD_IMAGE_GRAYSCALE);

		//Check if the image is properly loaded
		if (source_image.data == NULL){
			cout << "Unable to load image with file path: " << img_path + images[img] << endl;
			cout << "Ending execution..." << endl;
			return 0;
		}

		//Exceptions: 2 images must be rotated of 90 degrees, others 2 must be equalize
		if(img == 14 || img == 16)
		{
			Mat rotation = getRotationMatrix2D(Point2f(source_image.cols/2,source_image.rows/2), -90, 1);
			warpAffine(source_image,source_image, rotation, Size(source_image.cols-100,source_image.rows), INTER_LINEAR + WARP_INVERSE_MAP);
		}
		if(img == 25 || img == 26){
			gammaCorrection(source_image,source_image);
		}


		//Show the original image and print some information
		namedWindow("Original", WINDOW_NORMAL);
		resizeWindow("Original", source_image.cols/2, source_image.rows/2);
		imshow("Original", source_image);
		cout << "Image " << img << " : " 	<< images[img] << " succesfully loaded." << endl;
		cout << "Dimension: " << source_image.cols << "X" << source_image.rows << endl;
		cout << "Step: " << source_image.step << endl;
		waitKey(0);
		destroyWindow("Original");

	//Apply the binarization and dilation
		Mat binarization_image = Mat::zeros(source_image.size(), source_image.type());
		Mat element = getStructuringElement(MORPH_RECT, Size(3,1),Point(-1,-1));

		binarization(source_image, binarization_image);
		dilate(binarization_image, binarization_image, element, Point(-1,-1), 15);

		if(img == 25)
			dilate(binarization_image, binarization_image, element, Point(-1,-1), 10);

	//Show the binarized image
		namedWindow("binarization_image", WINDOW_NORMAL);
		imshow("binarization_image", binarization_image);
		resizeWindow("binarization_image", binarization_image.cols/2, binarization_image.rows/2);
		waitKey(0);
		destroyWindow("binarization_image");

	//Find BarCode with Flood-fill approach
		Mat labeling_image = binarization_image;
		unsigned int label = 1;

		for ( int i = 0; i < labeling_image.rows; i++)
			for ( int j = 0; j < labeling_image.cols; j++)
				if (labeling_image.at<uchar>(i,j) == 255)
				{
					label++;
					mark(labeling_image,i,j,label);
				}

	//If there are more labels, we compute the area of each of them,
	//Then we define the barcode as the label with the biggest in size
		Mat barcode_image = Mat::zeros(source_image.size(), source_image.type());

		position(labeling_image,labeling_image,label);
		orientation(labeling_image,source_image,barcode_image, img, parameters.at(img));

	//Show image
		namedWindow("Barcode 1", WINDOW_NORMAL);
		imshow("Barcode 1", barcode_image);
		resizeWindow("Barcode 1", barcode_image.cols/2, barcode_image.rows/2);
		waitKey(0);
		destroyWindow("Barcode 1");

	//Now I compute the lenght of smallest bar in the barcode, and I cut the barcode of that lenght
		binarization(barcode_image, binarization_image);

		labeling_image = binarization_image;
		label = 0;

		for ( int i = 0; i < labeling_image.rows; i++)
			for ( int j = 0; j < labeling_image.cols; j++)
				if (labeling_image.at<uchar>(i,j) == 255)
				{
					label++;
					mark(labeling_image,i,j,label);
				}

		namedWindow("Barcode 2", WINDOW_NORMAL);
		resizeWindow("Barcode 2", labeling_image.cols/2, labeling_image.rows/2);
		imshow("Barcode 2",labeling_image);
		waitKey(0);
		destroyWindow("Barcode 2");


		smallBar(labeling_image, barcode_image, barcode_image, parameters.at(img));

	//Show the image restricted to barcode area
		namedWindow("Barcode", WINDOW_NORMAL);
		resizeWindow("Barcode", barcode_image.cols/2, barcode_image.rows/2);
		imshow("Barcode", barcode_image);
		waitKey(0);
		destroyWindow("Barcode");


		vector<int> scanProfile(barcode_image.cols);

		scan(barcode_image, scanProfile, parameters.at(img));
		scanProfileVisualization(scanProfile);
		overalSymbol(parameters.at(img));

	}

	print(images, parameters);
	std::cout << "Bye Bye..." << std::endl;
}
