#include<ros/ros.h>
#include<ball_chaser/DriveToTarget.h>
#include<geometry_msgs/Twist.h>

ros::Publisher motor_command_publisher ;

bool handle_drive_request(ball_chaser::DriveToTarget::Request &req, ball_chaser::DriveToTarget::Response &res)
{
	ROS_INFO("Starting Service");

	static int counter = 0;

	ros::Rate loop_rate(10) ;
	geometry_msgs::Twist motor_command ;
	motor_command.linear.x = req.linear_x ;
	motor_command.angular.z = req.angular_z ;

	res.msg_feedback = "Linear x set as : " + std::to_string(req.linear_x) + " Angular z set as : " + std::to_string(req.angular_z);
	ROS_INFO_STREAM(res.msg_feedback);
	motor_command_publisher.publish(motor_command) ;
	loop_rate.sleep() ;
	counter++ ;

	if(counter == 5)
	{
		counter = 0 ;
		motor_command.linear.x = 0 ;
		motor_command.angular.z = 0 ;

		motor_command_publisher.publish(motor_command) ;
		ros::Duration(0.1).sleep();
	}

}

int main(int argc, char** argv)
{
	ros::init(argc, argv, "drive_bot") ;
	ros::NodeHandle n ;
	motor_command_publisher = n.advertise<geometry_msgs::Twist>("/cmd_vel", 2);

	ros::ServiceServer service = n.advertiseService("/ball_chaser/command_robot", handle_drive_request);
	ROS_INFO("Ready to send commands") ;

	while(ros::ok())
	{
    	ros::spinOnce();
    }
	return 0 ;
}