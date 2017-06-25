cv::Mat

Mat M(2,2, CV_8UC3, Scalar(0,0,255));

CV_[The number of bits per item][Signed or Unsigned][Type Prefix]C[The channel number]

Mat E = Mat::eye(4, 4, CV_64F);
cout << "E = " << endl << " " << E << endl << endl;
Mat O = Mat::ones(2, 2, CV_32F);
cout << "O = " << endl << " " << O << endl << endl;
Mat Z = Mat::zeros(3,3, CV_8UC1);


If Mat.at<type> chosen is a wrong type, then the returned number is meaningless. Often it is too big or small. You should check it.

