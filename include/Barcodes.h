#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core.hpp"

#include <cstdio>
#include <vector>
#include <math.h>
#include <bits/stdc++.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>

using namespace std;
using namespace cv;

class Barcodes{

    private:
        Mat source;
        string adress;

    public:
        Barcodes(Mat & input);

        Barcodes(string & input);
        ~Barcodes();

        void showImage(string name, Mat & image, unsigned int & image_number);
        void showSourceImage(unsigned int & image_number);
        cv::Mat getSourceImage();
        void gammaCorrection(Mat & input, Mat & output);
        void binarization(const Mat & input, Mat & output);
        void mark(Mat & input, int & iseed, int & jseed, unsigned int & label);
        void position(Mat & input, Mat & output, unsigned int & labels);
        void orientation(Mat & input_labeling, Mat & input_source, Mat & output, unsigned int & index, vector<vector<float>> & parameters);
        void smallBar(Mat & input, Mat & source, Mat & output, vector<vector<float>> & parameters);
        void scan(Mat & input, vector<int> & scanProfile, vector<vector<float>> & parameters);
        void scanProfileVisualization(vector<int> & scanProfile);
        void overalSymbol(vector<vector<float>> & parameters);
};

void print(vector<string> & image, vector<vector<vector<float>>> & parameters);