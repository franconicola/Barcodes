#include "Barcodes.h"



int main(int argc, char* argv[]){

	//set in img_path the path to the images that you want to load
	string img_path = "/Users/nicolafranco/Documents/website/Barcodes/codici-lineari-dati/";

	// Load the images path
	vector<string> images;
    for (const auto & entry : std::__fs::filesystem::directory_iterator(img_path))
        images.push_back(entry.path());

	//Create a vector that contains all the parameters, in order to be printed
	vector<vector<vector<float>>> parameters(images.size(), vector<vector<float>>(20,vector<float>(10)));

	for(unsigned int image_num = 0; image_num < images.size(); image_num++){

		cout << "===========================================" << endl;

		Barcodes * image = new Barcodes(images[image_num]);

		/* Exceptions: 2 must be equalize
		if(img == 25 || img == 26){
			gammaCorrection(source_image,source_image);
		}*/
		image->showSourceImage(image_num);



	//Apply the binarization and dilation
	    Mat source_image = image->getSourceImage();
		Mat binarization_image = Mat::zeros(source_image.size(), source_image.type());
		Mat element = getStructuringElement(MORPH_RECT, Size(3,1),Point(-1,-1));

		image->binarization(source_image, binarization_image);
		dilate(binarization_image, binarization_image, element, Point(-1,-1), 15);

		//if(i == 25)
		//	dilate(binarization_image, binarization_image, element, Point(-1,-1), 10);

	//Show the binarized image
	    image->showImage("binarization_image", binarization_image, image_num);

	//Find BarCode with Flood-fill approach
		Mat labeling_image = binarization_image;
		unsigned int label = 1;

		for ( int i = 0; i < labeling_image.rows; i++)
			for ( int j = 0; j < labeling_image.cols; j++)
				if (labeling_image.at<uchar>(i,j) == 255)
				{
					label++;
					image->mark(labeling_image,i,j,label);
				}

	//If there are more labels, we compute the area of each of them,
	//Then we define the barcode as the label with the biggest in size
		Mat barcode_image = Mat::zeros(source_image.size(), source_image.type());

		image->position(labeling_image,labeling_image,label);
		image->orientation(labeling_image,source_image,barcode_image, image_num, parameters.at(image_num));

	//Show image
	    image->showImage("Barcode 1", barcode_image, image_num);

	//Now I compute the lenght of smallest bar in the barcode, and I cut the barcode of that lenght
		image->binarization(barcode_image, binarization_image);

		labeling_image = binarization_image;
		label = 0;

		for ( int i = 0; i < labeling_image.rows; i++)
			for ( int j = 0; j < labeling_image.cols; j++)
				if (labeling_image.at<uchar>(i,j) == 255)
				{
					label++;
					image->mark(labeling_image,i,j,label);
				}

        image->showImage("Barcode 2", labeling_image, image_num);

		image->smallBar(labeling_image, barcode_image, barcode_image, parameters.at(image_num));

	//Show the image restricted to barcode area
        image->showImage("Barcode", barcode_image, image_num);

		vector<int> scanProfile(barcode_image.cols);

		image->scan(barcode_image, scanProfile, parameters.at(image_num));
		image->scanProfileVisualization(scanProfile);
		image->overalSymbol(parameters.at(image_num));

        delete image;
	}

	print(images, parameters);
	std::cout << "Bye Bye..." << std::endl;
}
