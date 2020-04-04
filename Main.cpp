//
//  Final2.cpp
//  opencvtest2
//
//  Created by Surya Dheeshjith on 14/11/18.
//  Copyright © 2018 Surya Dheeshjith. All rights reserved.
//





#include "opencv2/opencv.hpp"
#include "lift_helper.h"
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
        }
    }
    return -1;
}
int main()
{
    
    queue_k dq, uq,tempq;
    dq.front = -1;
    dq.rear = -1;
    uq.front= -1;
    uq.rear = -1;
    tempq.front = -1;
    tempq.rear = -1;
    uq.flag = 0;
    dq.flag = 0;
    tempq.flag=-1;
    tempq.empty=1;
    dq.empty=1;
    uq.empty=1;
    
    int f1=0,f2=0,lfno;
    
    char ch='a',ch2='a';
    
    Point c=Point(800,150);
    Point b=Point(250,150);
    float t1=0;
    float t2=0;
    int n1,n2,time1=0,time2=0;
    int fingers1=-1,fingers2=-1;
    string s;
    VideoCapture cam;
    Mat img,hull_square1,hand_square1,bkg1,bin1,hull_square2,hand_square2,bkg2,bin2;
    cam.open(0);
    cam>>bkg1;
    cam>>bkg2;
    flip(bkg1,bkg1, 1);
    flip(bkg2,bkg2, 1);
    bkg1=bkg1(a);
    bkg2=bkg2(q);
    cvtColor(bkg1,bkg1,COLOR_BGR2GRAY);
    cvtColor(bkg2,bkg2,COLOR_BGR2GRAY);
    
    while(1)
    {
        cam>>img;
        flip(img,img, 1);   //It mirrors the first and saves it in the second
        img.copyTo(hull_square1);
        img.copyTo(hand_square1);
        hand_square1= hand_square1(a);
        hand_square1.copyTo(bin1);//Copy a fraction of the image
        binary(bin1,bkg1);
        filterpixels(bin1);
        n1=applyContours(bin1,hand_square1);//He makes the drawing and returns his fingers
        if(n1<0)
        {
            t1=0;
        }
        colourfill(c,hull_square1,t1);
        
        addWeighted(hull_square1,0.5, img, 1 - 0.5, 0, img);
        if (n1>-1 and t1>0)
        {
            putText(hand_square1,i2s(n1),Point(5,30),7,1,Scalar(0,0,0),0,8,false);
        }
        else
        {
            t1=0;
        }
        
        if(t1>2 )
        {
            fingers1=n1;
          //  printf("%d",fingers1);
            t1=0;
            time1=25;

        }
        if (time1==0)t1+=0.1;
        if (time1>30)
        {
            time1=0;
            
        }
        else if (time1>0)  ++time1;
        
        
        img.copyTo(hull_square2);
        img.copyTo(hand_square2);
        hand_square2= hand_square2(q);
        hand_square2.copyTo(bin2);//Copy a fraction of the image
        binary(bin2,bkg2);
        filterpixels(bin2);///Filter Pixels
        n2=applyContours(bin2,hand_square2);//drawing and returns fingers
        if(n2<0)
        {
            t2=0;
        }
        colourfill(b,hull_square2,t2);
        
        addWeighted(hull_square2,0.5, img, 1 - 0.5, 0, img);
        if (n2>-1 and t2>0)
        {
            putText(hand_square2,i2s(n2),Point(5,30),7,1,Scalar(0,0,0),0,8,false);
        
        }
        else
        {
            t2=0;
        }
        
        
        if(t2>2 )
        {
        
            fingers2=n2;
          //  printf("%d\n",fingers2);
            t2=0;
            time2=25;
            
        }
        if (time2==0)t2+=0.1;
        if (time2>30)
        {
            time2=0;
            
        }
        else if (time2>0)  ++time2;
        
        
        if(fingers1!=-1&&fingers2==-1)
        {
            putText(img,i2s(fingers1),Point(50,50),7,2,Scalar(0,255,0),3,8,false);
            f1=1;
            lfno=fingers1;
            
          
        }
        
        else if(fingers1!=-1&&fingers2!=-1)
        {
            
            if(fingers2==0)
            {
                putText(img,i2s(fingers1+5),Point(50,50),7,2,Scalar(0,255,0),3,8,false);
                f1=1;
                lfno=fingers1+5;
                
           
                //Function to lift
            }
            
            else{
                
                putText(img,i2s(fingers1+fingers2+9),Point(50,50),7,2,Scalar(0,255,0),3,8,false);
                f1=1;
                lfno=fingers1+fingers2+9;
                
               
                //Function to lift
                
            }
        }
        
        if(f1==1)
        {
            f2+=1;
        }
        
        if(f2>50)
        {
            printf("Destination Floor --- %d",lfno);
            lift_me(&dq,&uq,&tempq,lfno);
            fingers1=-1;
            fingers2=-1;
            f1=0;
            f2=0;
        }
        imshow("binary1",bin1);
        moveWindow("binary1",700,40);
        imshow("hull1",hand_square1);
        moveWindow("hull1",1000,40);
        imshow("Image",img);
        moveWindow("Image",40,40);
        imshow("binary2",bin2);
        moveWindow("binary2",700,40);
        imshow("hull2",hand_square2);
        moveWindow("hull2",1000,40);
        switch(waitKey(10))
        {
            case 27://exit //ESC
                return 0;
            case 98://refresh background
                
                cam>>bkg2;
                flip(bkg2,bkg2, 1);
                bkg2=bkg2(q);
                cvtColor(bkg2,bkg2,COLOR_BGR2GRAY);
                cam>>bkg1;
                flip(bkg1,bkg1, 1);
                bkg1=bkg1(a);
                cvtColor(bkg1,bkg1,COLOR_BGR2GRAY);
                break;
                
                
            case 102://restart values
                fingers1=-1;
                fingers2=-1;
                f1=0;f2=0;
                
        }
        
    }
    
    return 0;
}

