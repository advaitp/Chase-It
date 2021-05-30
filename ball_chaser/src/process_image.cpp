#include"ros/ros.h"
#include"ball_chaser/DriveToTarget.h"
#include<sensor_msgs/Image.h>
#include <cv_bridge/cv_bridge.h>
#include "opencv2/opencv.hpp"
#include <iostream>
#include <sensor_msgs/image_encodings.h>
#include <bits/stdc++.h>

ros::ServiceClient client ;
using namespace cv;
using namespace std ;

void drive_robot(float lin_x, float ang_z)
{
    ROS_INFO("Moving the robot car");

    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    // Call the command_robot service and pass the requested velocities
    if (!client.call(srv))
        ROS_ERROR("Failed to call service command_robot");
}

void process_image_callback(const sensor_msgs::Image img)
{
	ROS_INFO("Starting process_image_callback") ;
	int white_pixel = 255 ;

	cv_bridge::CvImagePtr cv_ptr ;

	try
	{
		cv_ptr = cv_bridge::toCvCopy(img, sensor_msgs::image_encodings::BGR8);
	}

	catch (cv_bridge::Exception& e)
  	{
	    ROS_ERROR("cv_bridge exception: %s", e.what());
	    return;
  	}

  	bool spotted = false ;
  	vector<int> pos ;
  	Mat im = cv_ptr->image ;
  	
  	for(int i = 0 ; i < im.rows ; i++)
  	{
  		for(int j = 0 ; j < im.cols ; j++)
  		{
  			Vec3b pixel = im.at<Vec3b>(i, j) ;
  			if(pixel.val[0] == white_pixel && pixel.val[1] == white_pixel && pixel.val[2] == white_pixel)
  			{
  				spotted = true ;
  				pos = {i, j} ;
  			}
  		}
  	}

  	if(!spotted) drive_robot(0.0, 0.0) ;

  	else
  	{
  		int row = pos[0] ;
	  	int col = pos[1] ;
	  	// GO LEFT
	  	if(col < im.cols/3)
		{
			ROS_INFO("Moving LEFT");
			drive_robot(0.0, 0.1) ;
		}

		// GO STRAIGHT
		else if(col > im.cols/3 && col< (2*im.cols/3))
		{
			ROS_INFO("Moving STRAIGHT");
			drive_robot(0.3, 0.0) ;
		}

		// GO RIGHT
		else
		{
			ROS_INFO("Moving RIGHT") ;
			drive_robot(0.0, -0.1) ;
		}
  	}
	
}

int main(int argc, char** argv)
{
	ROS_INFO("Starting process_image node") ;
	ros::init(argc, argv, "process_image") ;
	ros::NodeHandle n ;

	client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");
	ROS_INFO("Starting process_image node 2") ;
    
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 1, process_image_callback);

    ros::spin();

    drive_robot(0.0 , 0.0);
    return 0;
}