#include "Barcodes.h"

const int DIMENSION_X = 0;
const int HEIGHT = 1;
const int POS_X1 = 2;
const int POS_X2 = 3;
const int POS_Y1 = 4;
const int POS_Y2 = 5;
const int ANGLE = 6;
const int EDGES = 7;
const int SYMBOL = 9;
const int Rmin = 10;
const int ECmin = 11;
const int SC = 12;
const int MOD = 13;
const int Defects = 14;
const int OVERALL = 15;
const int ERN = 16;
const int Rmax = 17;
const int NUMBER_OF_BARS = 18;

const int N = 10;	//Line of scans
const float CONV = 0.392156863; 	// 100/255

const int MA_X = 30;	//Margin in X
const int MA_Y = 20;	//Margin in Y

Barcodes::Barcodes(std::string & input){
    //load source image
    cv::Mat source_image = cv::imread(input, cv::ImreadModes::IMREAD_GRAYSCALE);

    //Check if the image is properly loaded
    if (source_image.data == NULL){
        cout << "Unable to load image with file path: " << input << endl;
    }
    Barcodes::adress = input;
    Barcodes::source = source_image;
}

Barcodes::Barcodes(Mat & input){
    //load source image
    Barcodes::source = input;
}

Barcodes::~Barcodes(){};

//Show the image and print some information
void Barcodes::showImage(string name, Mat & image, unsigned int & image_number){
    namedWindow(name, WINDOW_NORMAL);
    resizeWindow(name, image.cols/2, image.rows/2);
    imshow(name, image);
    cout << "Wait key ...." << endl;
    waitKey(0);
    destroyWindow(name);
}

//Show the image and print some information
void Barcodes::showSourceImage(unsigned int & image_number){
    namedWindow("Original", WINDOW_NORMAL);
    resizeWindow("Original", Barcodes::source.cols/2, Barcodes::source.rows/2);
    imshow("Original", Barcodes::source);
    cout << "Name" << image_number << " : " << Barcodes::adress << " succesfully loaded." << endl;
    cout << "Dimension: " << Barcodes::source.cols << "X" << Barcodes::source.rows << endl;
    cout << "Step: " << Barcodes::source.step << endl;
    cout << "Wait key ...." << endl;
    waitKey(0);
    destroyWindow("Original");
}

cv::Mat Barcodes::getSourceImage() {
    return Barcodes::source;
}

void Barcodes::gammaCorrection(Mat & input, Mat & output){
	for(int i = 0; i < input.rows; i++)
		for(int j = 0; j < input.cols; j++)
		{
			int pixelValue = (int) input.at<uchar>(i,j);
			output.at<uchar>(i,j) = pow(pixelValue, 1.1);
		}
}

void Barcodes::binarization(const Mat & input, cv::Mat & output){

	Mat binarization_image = Mat::zeros(input.size(), input.type());
	Mat kernel(100, 3, CV_8U, Scalar(1));

	threshold(input, binarization_image, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);
	morphologyEx(binarization_image, output, MORPH_OPEN, kernel);
}

void Barcodes::mark(Mat & input, int & iseed, int & jseed, unsigned int & label){
	int i, j, m, n;
	int again;
	input.at<uchar>(iseed,jseed)=label;
	do{
		again = 0;

		for (i = 0; i < input.rows; i++)
		for (j = 0; j < input.cols; j++)
				if(input.at<uchar>(i,j) == label)
					for (m = i-1; m <= i+1; m++)
					for (n = j-1; n <=j+1; n++)
							if(input.at<uchar>(m,n) == 255)
							{
								input.at<uchar>(m,n) = label;
								again = 1;
							}
		for (i = input.rows-1; i >= 0; i--)
		for (j = input.cols-1; j >= 0; j--)
			if(input.at<uchar>(i,j) == label)
					for (m = i-1; m <= i+1; m++)
					for (n = j-1; n <=j+1; n++)
							if( input.at<uchar>(m,n) == 255)
							{
								input.at<uchar>(m,n) = label;
								again = 1;
							}
	} while (again);

}

void Barcodes::position(Mat & input, Mat & output, unsigned int & labels){

	//If there are more labels, we compute the area of each of them,
	//Then we define the barcode as the label with the biggest in size

	int areas[labels+1];
	int barcodeArea = 0;
	unsigned int barcode = 1;

	for(unsigned int i = 0; i<= labels; i++)
		areas[i] = 0;

	if(labels > 1){
		for (int i = 0; i < input.rows; i++)
				for ( int j = 0; j < input.cols; j++)
					areas[input.at<uchar>(i,j)]++;

		for (unsigned int i = 1; i <= labels; i++)
				if(areas[i] > barcodeArea){
					barcode = i;
					barcodeArea = areas[i];
				}
	}
	for ( int i = 0; i < input.rows; i++)
			for ( int j = 0; j < input.cols; j++)
					if(input.at<uchar>(i,j) == barcode)
						output.at<uchar>(i,j) = 255;
					else
						output.at<uchar>(i,j) = 0;
}

void Barcodes::orientation(Mat & input_labeling, Mat & input_source, Mat & output, unsigned int & index, vector<vector<float>> & parameters){

	//variables and constants
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	Mat barcode_image = input_source;

	// Find contours
	findContours(input_labeling, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE, Point(0,0) );

	// Find the rectangle
	RotatedRect minRect = minAreaRect(contours[0]);
	Point2f rect_points[4];
	float angle = 0;
	int x0 = input_source.cols;
	int x1 = 0;
	int y0 = input_source.rows;
	int y1 = 0;

	minRect.points(rect_points);

	RNG rng( 0xFFFFFFFF );

	/// Draw contours + rotated rects + ellipses
	 Mat drawing = Mat::zeros(input_labeling.size(), CV_8UC3 );

	   Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
	   // contour
	   drawContours( drawing, contours, 0, color, 1, 8, vector<Vec4i>(), 0, Point() );

	   // rotated rectangle
	   for( int j = 0; j < 4; j++ )
		  line( drawing, rect_points[j], rect_points[(j+1)%4], color, 1, 8 );


	  /// Show in a window
	  namedWindow( "Contours", WINDOW_NORMAL);
	  resizeWindow("Contours", drawing .cols/2, drawing .rows/2);
	  imshow( "Contours", drawing );
	  waitKey(0);
	  destroyWindow("Contours");


	for(int i = 0; i < 4; i++)
	{
		if(rect_points[i].x < x0)
			x0 = rect_points[i].x;
		if(rect_points[i].x > x1)
			x1 = rect_points[i].x;
		if(rect_points[i].y < y0)
			y0 = rect_points[i].y;
		if(rect_points[i].y > y1)
			y1 = rect_points[i].y;

		int diff_y = rect_points[i+1].y - rect_points[i].y;
		if((abs(diff_y) > abs(angle)) && (abs(diff_y)<20))
			angle = diff_y;
	}

	//Cut of original image
	barcode_image = input_source(Range((int)y0- MA_Y,(int)y1 + MA_Y),Range((int)x0-MA_X,x1+MA_X));

	//exceptions
	if(index == 17 || index == 25)
		for(int i = 0; i < 3; i++)
		{
			int difference = rect_points[i+1].x - rect_points[i].x;
			if((abs(difference) > abs(angle)) && (abs(difference)<20))
				angle = difference*2;
		}

	if(index == 12 || index ==13 || index == 22 || index == 23 || index > 25)
		angle = -angle;

	if(index == 8 || index == 18 || index == 20)
		angle = angle/2;

	if(index == 6)
		angle = angle*(5/3);
	if(index == 10 || index == 25)
		angle = (angle*3)/5;

	if(index < 14 || index > 20 || index == 17)
			parameters[ANGLE][0] = -angle/10;
		else
			parameters[ANGLE][0] = angle/10;

	//Rotation
	if(angle != 0){
		Mat rotation = getRotationMatrix2D(Point2f(barcode_image.cols/2,barcode_image.rows/2), angle/10, 1);
		warpAffine(barcode_image,barcode_image, rotation, Size(barcode_image.cols,barcode_image.rows));
	}

	output = barcode_image(Range(10,barcode_image.rows -10),Range(10,barcode_image.cols -10));

}

void Barcodes::smallBar(Mat & input, Mat & source, Mat & output, vector<vector<float>> & parameters){

	//Variables
	vector<float> smallBarPosition;
	int smallBarArea = source.cols*source.rows;
	int heightX = source.rows;
	int widthX = source.cols;

	//Initialization of smallBarPosition
	for(int i = 0; i < 4; i++)
		smallBarPosition.push_back(0);
	smallBarPosition[2] = source.cols;

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	// Find contours
	findContours(input, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE, Point(0,0) );

	// Find rectangle
	vector<Rect> boundRect(contours.size());


	RNG rng( 0xFFFFFFFF );

	/// Draw contours + rotated rects + ellipses
	 Mat drawing = Mat::zeros(input.size(), CV_8UC3 );


	for(unsigned int i = 0; i< contours.size(); i++ )
	{
		//Create bounded rectangles for every bars in barcode
		boundRect.at(i) = boundingRect(contours.at(i));

		 Scalar color = Scalar( rng.uniform(0, 256), rng.uniform(0,256), rng.uniform(0,256) );

		 // contour
		drawContours(drawing, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
		rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2 );


		//Variables defined in order to compare with the desired ones
		int height = boundRect.at(i).height;
		int width = boundRect.at(i).width;
		int area = height*width;

		//Find the y-length
		if( height < heightX && height > input.rows/2)
		{
			heightX = height;
			smallBarPosition[0] = boundRect.at(i).y;
			smallBarPosition[1] = boundRect.at(i).y + heightX;
		}
		//Find the area
		if( area < smallBarArea)
			smallBarArea = area;
		//Find the X_DIMENSION
		if( width < widthX)
			widthX = width;

		//Find the position of barcode in x-axis
		if(boundRect.at(i).x < smallBarPosition[2])
			smallBarPosition[2] = boundRect.at(i).x;
		if(boundRect.at(i).x > smallBarPosition[3])
			smallBarPosition[3] = boundRect.at(i).x + width;

	 }

	 /// Show in a window
	  namedWindow( "Contours", WINDOW_NORMAL);
	  resizeWindow("Contours", drawing .cols/2, drawing .rows/2);
	  imshow( "Contours", drawing );
	  waitKey(0);
	  destroyWindow("Contours");


	//Cut the barcode
	smallBarPosition[2] = smallBarPosition[2] - widthX*10;
	smallBarPosition[3] = smallBarPosition[3] + widthX*10;
	if (smallBarPosition[2]<0)
		smallBarPosition[2] = 0;
	if(smallBarPosition[3]>source.cols)
		smallBarPosition[3] = source.cols;

	//Outputs of parameters
	parameters[DIMENSION_X][0] = widthX;
	parameters[HEIGHT][0] = heightX;
	parameters[POS_Y1][0] = smallBarPosition[0];
	parameters[POS_Y2][0] = smallBarPosition[1];
	parameters[POS_X1][0] = smallBarPosition[2];
	parameters[POS_X2][0] = smallBarPosition[3];


	output = source(Range(smallBarPosition[0],smallBarPosition[1]),Range(smallBarPosition[2],smallBarPosition[3]));
}

void Barcodes::scan(Mat & input, vector<int> & scanProfile, vector<vector<float>> & parameters){

	//CONSTANTS
	const int PARALLEL_LINES = (int) input.rows/(N+1);

	//Scan profile
	float scan_profile[N][input.cols];

	//cycle of 10 rows to producing the scan_profile and the parameters
	for(int i = 0; i < N; i++)
	{
		//inizialization of arrays
		parameters[Rmin][i] = 50;
		parameters[Rmax][i] = 0;
		parameters[ECmin][i] = 100;
		parameters[SC][i] = 0;
		parameters[MOD][i] = 0;
		parameters[ERN][i] = 0;
		parameters[Defects][i] = 0;

		// # number of row to check
		int row = PARALLEL_LINES*(i+1);

		for(int j = 0; j < input.cols; j++)
		{
			//I am scanning each pixel in the row and then compute the percentage
			scan_profile[i][j] = (int) input.at<uchar>(row,j)*CONV;

			if(scan_profile[i][j] < parameters[Rmin][i])
				parameters[Rmin][i] = scan_profile[i][j];
			if(scan_profile[i][j] > parameters[Rmax][i])
				parameters[Rmax][i] = scan_profile[i][j];
		}
		parameters[SC][i] = parameters[Rmax][i] - parameters[Rmin][i];
	}

	//Compute the ECmin parameters and the number of edges
	for(int i = 0; i < N; i++)
	{
		//variables
		float edge, edge_up, edge_down, ern, ern_up, ern_down;
		//counters
		int count1, count2, count_ern;
		//mean value of scan used to find the edge
		float median = parameters[SC][i]/2;
		//mean value between three pixels
		float foreground, background;
		//Used if I find an edge or not
		bool flag = false;
		//dimension X o the bar
		int X_dim = parameters[DIMENSION_X][0];

		for(int j = 2; j < input.cols-2; j++)
		{
			//When I match the mean, there are 2 situations: ascendent or descendet edge:
			//Sobel
			foreground = (scan_profile[i][j] + pow(2,0.5)*scan_profile[i][j+1] + scan_profile[i][j+2])/(2+pow(2,0.5));
			background = (scan_profile[i][j-2] + pow(2,0.5)*scan_profile[i][j-1] + scan_profile[i][j])/(2+pow(2,0.5));

			//The ascendent
			if(background < median && foreground > median && scan_profile[i][j-1] < scan_profile[i][j+1])
			{
				parameters[EDGES][i]++;
				flag = true;
				count1 = j;
				count2 = j;

				//Compute the upper part
				do{
					edge_up = scan_profile[i][count1];
					count1++;
				}while(scan_profile[i][count1-1] < scan_profile[i][count1]);

				//Compute the down part
				do{
					edge_down = scan_profile[i][count2];
					count2--;
				}while(scan_profile[i][count2] < scan_profile[i][count2+1]);


								//Compute Defects
								count_ern = count1;
								ern_down = 100;
								ern_up = 0;

								//Compute until we don't overtake the threshold
								do{
									if(scan_profile[i][count_ern] > ern_up)
										ern_up = scan_profile[i][count_ern];

									//Update until we don't find the defect
									while(scan_profile[i][count_ern] > scan_profile[i][count_ern+1])
										count_ern++;

									if(scan_profile[i][count_ern] > ern_down && scan_profile[i][count_ern] > median)
										ern_down = scan_profile[i][count_ern];

									count_ern++;
								}while(scan_profile[i][count_ern] > median);

				j = count1-1;

			}

			//The descendet
			else if(background > median && foreground < median && scan_profile[i][j-1] > scan_profile[i][j+1])
			{
				parameters[EDGES][i]++;
				flag = true;
				count1 = j;
				count2 = j;
				count_ern = j;

				//Compute the upper part
				do{
					edge_up = scan_profile[i][count1];
					count1--;
				}while(scan_profile[i][count1] > scan_profile[i][count1+1]);

				//Compute the down part
				do{
					edge_down = scan_profile[i][count2];
					count2++;
				}while(scan_profile[i][count2-1] > scan_profile[i][count2]);

				//Compute Defects
				count_ern = count2;
				ern_down = median;
				ern_up = 0;

				//Compute until we don't overtake the threshold
							do{
								if(scan_profile[i][count_ern] < ern_down)
									ern_down = scan_profile[i][count_ern];

								//Update until we don't find the defect
								while(scan_profile[i][count_ern] < scan_profile[i][count_ern+1])
									count_ern++;

								if(scan_profile[i][count_ern] > ern_up && scan_profile[i][count_ern] < median)
									ern_up = scan_profile[i][count_ern];

								count_ern++;
							}while(scan_profile[i][count_ern] < median);

				j = count2-1;
			}

			if(flag)
			{
				//I compute the edge size and then I store in parameters vector
				edge = edge_up - edge_down;
				if(edge < parameters[ECmin][i] && edge > N)
						parameters[ECmin][i] = edge;

				// I store the defect value
				ern = ern_up - ern_down;
				if(ern > parameters[ERN][i] && ern < N*5)
					parameters[ERN][i] = ern;
			}

			flag = false;
		}
		parameters[MOD][i] = (parameters[ECmin][i]/parameters[SC][i])*100;
		parameters[Defects][i] = (parameters[ERN][i]/parameters[SC][i])*100;
	}

	//In order to rapresent the scan profile, I need to compute the mean between the 10's profiles
	for(int j = 0; j < input.cols; j++)
	{
		float mean = 0;
		for(int i = 0; i < N-1; i++)
			mean = mean + scan_profile[i][j];

		scanProfile[j] = (int) mean/N;
	}
}

void Barcodes::scanProfileVisualization(vector<int> & scanProfile){

	int outMargin = 30;
		int binw = 1;
		int binh = 3;
		int wout = (scanProfile.size() / binw) + (outMargin * 2);
		int hout = (100 * binh) + (outMargin * 2) ;
		Size size;
		size.width = wout;
		size.height = hout;
		Mat image_out = Mat(size, CV_8UC3);
		image_out = Scalar(255, 255, 255);

		//draw axes
		line(image_out, Point(outMargin, outMargin), Point(outMargin, hout - outMargin), Scalar(0, 0, 0), 2);
		line(image_out, Point(wout - outMargin, hout - outMargin), Point(outMargin, hout - outMargin), Scalar(0, 0, 0), 2);

		//draw histogram bin
		int max = 100;

		for (int i = 0; i < (int) scanProfile.size(); i++) {
			if (scanProfile.at(i) > 0){
				Point pt1(outMargin + (i*binw), hout - outMargin);
				Point pt2(pt1.x + binw, pt1.y - ((scanProfile.at(i) * 100 / max)*binh));
				rectangle(image_out, pt1, pt2, Scalar(255 - i, i / 3, i), cv::FILLED);
			}
		}
		//add notes
		string left = "0";
		string right =  to_string(scanProfile.size());
		putText(image_out, left, Point(outMargin, hout - (outMargin / 2)), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 2);
		putText(image_out, right, Point(wout - (1.5*outMargin), hout - (outMargin / 2)), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 2);

		imshow("histogram", image_out);
		cout << "Press any key to continue..." << std::endl;
		waitKey();
		destroyWindow("histogram");

}

void Barcodes::overalSymbol(vector<vector<float>> & parameters){

	//CONSTANTS of Grades
	const int ECmin_GRADE = 15;
	const int SC_GRADE_A = 70;
	const int SC_GRADE_B = 55;
	const int SC_GRADE_C = 40;
	const int SC_GRADE_D = 20;
	const int MOD_GRADE_A = 70;
	const int MOD_GRADE_B = 60;
	const int MOD_GRADE_C = 50;
	const int MOD_GRADE_D = 40;
	const int Defects_GRADE_A = 15;
	const int Defects_GRADE_B = 20;
	const int Defects_GRADE_C = 25;
	const int Defects_GRADE_D = 30;

	//Variables
	int Symbol, Overall = 0;

	for(int i = 0; i < N; i++)
	{
		Symbol = 4;

		//Rmin
		if(parameters[Rmin][i] <= parameters[Rmax][i]/2)
			Symbol = 4;
		else
			Symbol = 0;

		//ECmin
		if(parameters[ECmin][i] >= ECmin_GRADE	&& Symbol > 3)
			Symbol = 4;
		else
			Symbol = 0;

		//Symbol Contrast
		if(parameters[SC][i] >= SC_GRADE_A && Symbol > 3 )
			Symbol = 4;
		else if (parameters[SC][i] >= SC_GRADE_B && parameters[SC][i] < SC_GRADE_A && Symbol > 2)
			Symbol = 3;
		else if (parameters[SC][i] >= SC_GRADE_C && parameters[SC][i] < SC_GRADE_B && Symbol > 1)
			Symbol = 2;
		else if (parameters[SC][i] >= SC_GRADE_D && parameters[SC][i] < SC_GRADE_C && Symbol != 0)
			Symbol = 1;
		else if (parameters[SC][i] < SC_GRADE_D)
			Symbol = 0;

		//Modulation
		if(parameters[MOD][i] >= MOD_GRADE_A && Symbol > 3)
			Symbol = 4;
		else if (parameters[MOD][i] >= MOD_GRADE_B && parameters[MOD][i] < MOD_GRADE_A && Symbol > 2)
			Symbol = 3;
		else if (parameters[MOD][i] >= MOD_GRADE_C && parameters[MOD][i] < MOD_GRADE_B && Symbol > 1)
			Symbol = 2;
		else if (parameters[MOD][i] >= MOD_GRADE_D && parameters[MOD][i] < MOD_GRADE_C && Symbol != 0)
			Symbol = 1;
		else if (parameters[MOD][i] < MOD_GRADE_D)
			Symbol = 0;

		//Defects
		if(parameters[Defects][i] <= Defects_GRADE_A && Symbol > 3)
			Symbol = 4;
		else if (parameters[Defects][i] <= Defects_GRADE_B && parameters[Defects][i] > Defects_GRADE_A && Symbol > 2)
			Symbol = 3;
		else if (parameters[Defects][i] <= Defects_GRADE_C && parameters[Defects][i] > Defects_GRADE_B && Symbol > 1)
			Symbol = 2;
		else if (parameters[Defects][i] <= Defects_GRADE_D && parameters[Defects][i] > Defects_GRADE_C && Symbol > 0)
			Symbol = 1;
		else if (parameters[Defects][i] >= Defects_GRADE_D )
			Symbol = 0;

		parameters[SYMBOL][i] = Symbol;
		Overall = Overall + Symbol;
	}
	parameters[OVERALL][0] = Overall/N;
}

void print(vector<string> & image, vector<vector<vector<float>>> & parameters){

	//CONSTANTS
	const int DIFF_PARAM = 16;
	const int SAME_PARAM = 10;
	const int IMAGES = 52;

	ofstream excelFile;
	excelFile.open("/home/nicola/Desktop/Computer Vision/codici-lineari-dati/Datas.xls");

	excelFile << "FileName (*.BMP), Dim. X, Height, X1, X2, Y1, Y2, Angle, Edges, Bars Dim.,"
				"Symbol Grade, Min Reflectance, Min Edge Contrast, Symbol Contrast, Modulation, Defects, "
				"Overall Symbol Grade, Bars and white spaces dimension respect to X\n" << endl;

	for ( int index = 0; index < IMAGES; index++){

		excelFile << image.at(index) << endl;

		for (int j = 0; j < SAME_PARAM; j++){

			excelFile << ",";

			for (int i = 0; i < DIFF_PARAM; i++)
			{
				if( i == 9 || ( i == 15 && j == 0))
				{
					if((int)parameters[index][i][j] == 4)
						excelFile << "   A ";
					else if((int)parameters[index][i][j] == 3)
						excelFile << "   B ";
					else if((int)parameters[index][i][j] == 2)
						excelFile << "   C ";
					else if((int)parameters[index][i][j] == 1)
						excelFile << "   D ";
					else
						excelFile << "   F ";

					excelFile << ",";
				}

				else
					excelFile << fixed << setprecision(1) <<  parameters[index][i][j] << "," ;

			}
			excelFile << endl;
		}
	}

	excelFile.close();

}







