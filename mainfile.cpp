#include<iostream>
#include "opencv2/objdetect/objdetect.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

#define GVSIZE 30		//Ghost Vector Size
#define pi pair<int,int>
#define F first
#define S second
#define mp make_pair
RNG rng(12345);

int main()
{
	VideoCapture cap(0);
	if(!cap.isOpened())
	{
		cout<<"Error in Capturing"<<endl;
		return -1;
	}

	//A sample image captured to know webcam dimensions
	Mat src;
	cap>>src;

	//Ghost Effect

	cout<<"Press q to quit"<<endl;

	//Taking In Video Feed
	int i=0;
	bool f=false;
	vector<Mat> v(GVSIZE);
	Mat output(src);
	int fr=5;
	while(1)
	{
		cap>>v[i];
		flip(v[i],v[i],1);		//Flip so that view on video becomes natural.
		
		if(f)
		{
			for(int i1=0;i1<output.rows;i1++)
			{
				for(int j1=0;j1<output.cols;j1++)
				{
					//Ghost effect takes 2 images (one current and one old), gives the weighted average intensity of the two images.
					//The fraction of current image can be set by trackbar available to user.
					//v[i] is the current image,  v[(i+(GVSIZE/2))%GVSIZE] is an old image.
					output.at<Vec3b>(i1,j1)[0] = (v[i].at<Vec3b>(i1,j1)[0]*fr + v[(i+(GVSIZE/2))%GVSIZE].at<Vec3b>(i1,j1)[0]*(10-fr))/10;
					output.at<Vec3b>(i1,j1)[1] = (v[i].at<Vec3b>(i1,j1)[1]*fr + v[(i+(GVSIZE/2))%GVSIZE].at<Vec3b>(i1,j1)[1]*(10-fr))/10;
					output.at<Vec3b>(i1,j1)[2] = (v[i].at<Vec3b>(i1,j1)[2]*fr + v[(i+(GVSIZE/2))%GVSIZE].at<Vec3b>(i1,j1)[2]*(10-fr))/10;
				}
			}

			cvNamedWindow("Ghost",CV_WINDOW_AUTOSIZE);
			//If fr=0, Only old image is showed. Terefore, all the transitions will be delayed.
			//If fr=10, No Ghost effect is shown.
			//Inititally fr=5.
			createTrackbar("Fraction","Ghost",&fr,10);
			imshow("Ghost",output);
			if(cvWaitKey(30)=='q') break;
		}

		i=(i+1)%GVSIZE;
		if(i>GVSIZE*3/4)
			f=true;
	}


	//Old Movie Effect
	Mat img;
	Mat inp;
	cap>>inp;
	i=0;
	int bigtime=0;
	int colno;
	cout<<"Press q to quit"<<endl;
	//Kernel values to convert iamhe to sepia.
	Mat kern = (cv::Mat_<float>(4,4) <<  0.272, 0.534, 0.131, 0,
                                             0.349, 0.686, 0.168, 0,
                                             0.393, 0.769, 0.189, 0,
                                             0, 0, 0, 1);
	while(1)
	{
		cap>>inp;
		flip(inp,inp,1);
		
		//cvtColor(img,img,CV_BGR2GRAY);
		//Convert Image to Sepia
		transform(inp, img, kern);
		//Selecting Column randomly for line
		if(i==0)
			colno = img.cols/10 + rand()%(img.cols/5);

		for(int i1=0;i1<img.rows;i1++)
		{
			img.at<Vec3b>(i1,colno)[0] = 0;
			img.at<Vec3b>(i1,colno)[1] = 0;
			img.at<Vec3b>(i1,colno)[2] = 0;
		}

		//Creating Black spots
		//vector<pi> sopts(50);

		int bigposts = rand()%6;
		for(int nspots=0;nspots<bigposts;nspots++)
		{
			Point ctr;
			ctr.x = 5+rand()%(img.rows-5);
			ctr.y = 5+rand()%(img.cols-5);
			circle(img,ctr,2,Scalar(0,0,0),-1);
		}

		for(int nspots=0;nspots<50;nspots++)
		{
			//50 small spots
			img.at<Vec3b>(rand()%img.rows,rand()%img.cols)[0] = 0;
			img.at<Vec3b>(rand()%img.rows,rand()%img.cols)[1] = 0;
			img.at<Vec3b>(rand()%img.rows,rand()%img.cols)[2] = 0;
		}
		

		//Displaying some rectangles at an interval of 150 (relative to time taken for 1 loop).
		if(bigtime==0)
		{
			Point p1,p2;
			p1.x = rand()%(img.rows*38/40);
			p1.y = rand()%(img.cols*38/40);
			int size1 = rand()%(img.rows/40);
			int size2 = rand()%(img.cols/40);
			//int sign = pow(-1,rand()%2);
			p2.x = p1.x + size1;
			p2.y = p2.y + size2;
			rectangle(img,p1,p2,Scalar(0,0,0),-1);
		}

		cvNamedWindow("Old_Movie",CV_WINDOW_AUTOSIZE);
		imshow("Old_Movie",img);
		if(cvWaitKey(30)=='q') break;

		i=(i+1)%4;
		bigtime = (bigtime+1)%150;

	}


	//Snow Effect
	
	i=0;
	int nsnow = img.cols/4;
	vector<pair<Point,pair<int,long long int> > > snow(nsnow);
	for(int j=0;j<nsnow;j++)
	{
		snow[j].F.y=0;
		snow[j].F.x=rand()%img.cols;
		snow[j].S.F = 1 + rand()%10;  //Speed of falling
		//Removed feature as it was creating problems //snow[j].S.S = 0;   //Count in case it has been obstructed to fall autmotically afetr some time
	}

	cout<<"Press q to quit\nPress s for more snow"<<endl;
	
	while(1)
	{
		cap>>img;
		flip(img,img,1);
		int thresh=50;

		Mat img2(img);
		cvNamedWindow("Snow",CV_WINDOW_AUTOSIZE);		

		vector<vector<Point> > contours;
  		vector<Vec4i> hierarchy;
  		
  		Mat canny_out(img2);
  		Canny(img2,canny_out,thresh,thresh*2);

  		findContours(canny_out,contours,hierarchy,CV_RETR_TREE,CV_CHAIN_APPROX_SIMPLE,Point(0,0));
  		
  		Scalar color = Scalar(255,255,255);
  		//drawContours(img,contours,-1,color,0,8,hierarchy,1);

  		for(int k=0;k<nsnow;k++)
  		{
  			circle(img,snow[k].F,2,Scalar(255,255,255),-1);
  			for(int i1=0;i1<contours.size();i1++)
  			{
  				for(int j1=0;j1<contours[i1].size();j1++)
  				{
  					if(snow[k].F.x==contours[i1][j1].x && snow[k].F.y<contours[i1][j1].y)
  					{
  						
  							snow[k].F.y+=snow[k].S.F;
  							//snow[k].S.S++;	
  							//checksnow[k]=true;				
  					
  					}

  					//Tried that obstructed snow continues moving automatically after sometime, but measuring time was creating problems 
  					//as the no of times loop runs is not constant and depends on size of contours, so does the time taken.

  					/*else if(snow[k].F.x==contours2[i1][j1].x && snow[k].F.y>=contours2[i1][j1].y)
  					{
  						
  							
  							//checksnow[k]=true;
  						
  					}*/
  					/*if(snow[k].S.S>100000)
  					{
  						snow[k].S.S=0;
  						snow[k].F.y+=snow[k].S.F;
  					}*/
  					/*if(snow[k].F.y>=img.cols)
  					{
  						snow[k].F.y=0;
						snow[k].F.x=rand()%img.rows;
						snow[k].S.F = 1 + rand()%3;  
						snow[k].S.S = 0;
  					}*/
  				}
  			}
  		}

		imshow("Snow",img);
		int res = cvWaitKey(30);
		if(res=='q') break;
		
		//If s is pressed, snow starts falling again.
		else if(res=='s')
		{
			for(int j=0;j<nsnow;j++)
			{
				snow[j].F.y=0;
				snow[j].F.x=rand()%img.cols;
				snow[j].S.F = 1 + rand()%5;  //Speed of falling
			}
		}
	}


	//Holi
	CascadeClassifier face_cascade;
	String face_cascade_name = "/usr/share/opencv/haarcascades/haarcascade_frontalface_alt.xml";
 	if( !face_cascade.load( face_cascade_name ) ){ cout<<"--(!)Error loading\n"; return -1; };
	while(1)
	{
		cap>>img;
		flip(img,img,1);
		vector<Rect> faces;
		Mat grayimg;
		cvtColor( img, grayimg, CV_BGR2GRAY );
  		equalizeHist( grayimg, grayimg );
  		face_cascade.detectMultiScale( grayimg, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );
  		for(size_t j=0;j<faces.size();j++)
  		{
  			Point p;
  			vector<int> colval(3,0);
  			for(int ct = 1; ct < 8; ct++)
		    {
		    	int ctcpy=ct;
		      for(int k=0;k<3;k++)
		      {

		      	colval[k]=255*(ctcpy%2);
		      	ctcpy/=2;
		      }
		      p.x = faces[j].x + rand()%faces[j].width;
	  		  p.y = faces[j].y + rand()%faces[j].height;
	  		  Scalar colors = (colval[0],colval[1],colval[2]);
	  		  circle(	img,p,5+rand()%5,colors,-1);
		    }
  			
  			cvNamedWindow("Holi",CV_WINDOW_AUTOSIZE);
  			imshow("Holi",img);
  			if(cvWaitKey(30)=='q') break;
  		}

	}
	

	


}