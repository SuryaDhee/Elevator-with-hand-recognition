

#include "opencv2/opencv.hpp"
#include "Elevator.h"
#include <math.h>
#include<stdio.h>


#define SENSIB 20
using namespace std;
using namespace cv;

const Rect a=Rect(701,51,200,200);
const Rect q=Rect(150,51,200,200);

string i2s(int i)
{
  string s="";
  string st;
  while(i>0)
    {
      s+=((i%10)+'0');
      i/=10;
    }
  if (s=="") s="0";
  st=s;
  for(int i=0;i<s.length();i++)  st[s.length()-1-i]=s[i];
  return st;
}
void filterpixels(Mat &bin)//erode reducuction followed by enlargement by dilate.
{
    
  Mat erode_1 = getStructuringElement( MORPH_RECT,Size(3,3));
  Mat dilate_1 = getStructuringElement( MORPH_RECT,Size(4,4));
  for (int i=0;i<4;++i) erode(bin,bin,erode_1);
  for (int i=0;i<4;++i) dilate(bin,bin,dilate_1);
}
float distance(Point a, Point b)
{
  return sqrt(((a.x-b.x)*(a.x-b.x))+((a.y-b.y)*(a.y-b.y)));
}
void binary(Mat& im,Mat bg)//binary
{
  //the background remains
  cvtColor(im,im,COLOR_BGR2GRAY);
  absdiff(im,bg,im); //saves the result of the remains
  threshold(im,im,SENSIB,255,THRESH_BINARY); //intensify
}
void colourfill(Point c,Mat& image,float t1)// Circles
{
  Scalar colour1=Scalar(0,255,127);
  float v=180*t1;// 90 degrees per second
  //  image, center, radius, color, thickness, type of line
  circle(image,c,100,colour1,8,8);//interior
  circle(image,c,122,colour1,4,4);//exterior
  // image, center, size (axis to, axis b), angle of inclination, initial angle, final angle, color,
  ellipse(image,c,Size(112,112),270,0,v,colour1, 8); //Loading
}
int applyContours(Mat bin1, Mat &ma)//apply Contours
{
  Mat temp;
  int j=0,amax=0,x,y;
  bin1.copyTo(temp);
  //To find contours
  vector< vector<Point> > contours;
  vector<Vec4i> nested;//Vector of 4 dimensions of int type
  //find contours in the filtered image with findContours of opencv
  findContours(temp,contours,nested,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
    
  double refArea = 0;
  bool found = false;
  if (nested.size() > 0) {
    int numObj = nested.size();
    if(numObj<40){
      for (int index = 0; index >= 0; index = nested[index][0])
	{
	  Moments moment = moments((Mat)contours[index]);
	  double area = moment.m00;
                
	  //If the area is large enough
	  if(area>400 && area>refArea  )
	    {
	      x = moment.m10/area;//Centroid
	      y = moment.m01/area;//Centroid
	      found = true;
	      refArea = area;
	      if (amax<area) j=index;
	    }
	  else found = false;
	}
      //found?
            
      if(found)
	{
	  //draw the green outline in green
	  drawContours( ma, contours, j, Scalar(0,250,0), 2, 8, nested, 0, Point() );
	  //Find the polygon that holds all the points
	  vector<vector<Point> > hulls(1);
	  convexHull(Mat(contours[j]),hulls[0],false);
	  //draw polygon
	  drawContours(ma,hulls,-1,Scalar(0,0,255),2);
	  //Search for the minimum area rectangle that includes the hand
	  RotatedRect rect=minAreaRect(Mat(contours[j]));
	  Point2f rect_points[4]; rect.points( rect_points );
	  //draws it
	  for( int k = 0; k < 4; k++ )
	    line( ma, rect_points[k], rect_points[(k+1)%4], Scalar(255,0,0), 2, 8 );
	  circle(ma,Point(x,y),3,Scalar(0,215,255),-3);
                
	  //To find the fingers, I would:
                
	  vector<Point> punts;
	  for(int k=0; k<hulls[0].size();++k)//Fix your fingers
	    {
	      if(y+10>hulls[0][k].y and distance(hulls[0][k],Point(x,y))>65 and distance(hulls[0][k],hulls[0][(k+1)%hulls[0].size()])>30)
		{
		  punts.push_back(hulls[0][k]);
		}
	    }
	  for(int k=0; k<punts.size();++k)
	    {
	      circle( ma, punts[k], 4, Scalar(255,255,255), -4 );
	      line( ma,punts[k], Point(x,y), Scalar(255,255,255), 2, 8 );
	    }
	  return punts.size();
	}
      cvtColor(bkg1,bkg1,COLOR_BGR2GRAY);
    cvtCol
      if (time1==0)t1+=0.1;
    if (time1>30)
      {
	time1=0;
           
        if (time2==0)t2+=0.1;
        if (time2>30)
	  {
            ti   }
      }
        
    if(f1==1)
      {
	f2+=1;
      }
        
    if(f2>25)
      {
	print  cvtColor(bkg1,bkg1,COLOR_BGR2GRAY);
	break;
            
   
