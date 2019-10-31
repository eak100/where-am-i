#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
    ROS_INFO_STREAM("Robot on move!");


    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z= ang_z;
    client.call(srv);


    if (!client.call(srv))
        ROS_ERROR("Failed to call service");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

int width = img.width;

    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera

    int Count_start = img.data.size() / 3;
    int Count_end = img.data.size() * 2 / 3;

  //  int total_black_pixels = 0;
    int total_white_pixels = 0;
    int x_sum = 0;

// Scan black pixels
for (int i=Count_start; i+2<Count_end; i+=3) {

    int R_value = img.data[i];
    int G_value = img.data[i+1];
    int B_value = img.data[i+2];


  if (R_value  == 255 && G_value == 255 && B_value == 255)
  {
      int x_position = (i % (width * 3)) / 3;



      x_sum = x_sum + x_position;
      //ROS_INFO("No ball ,x position :%1.2i",(int)width);
      total_white_pixels += 1;
  }
}

if (total_white_pixels == 0)
{

  drive_robot(0.0, 0.0);
}
else
{
    int mean_x_position = x_sum / total_white_pixels;
  if (mean_x_position < width / 3)
  {
      ROS_INFO_STREAM("Ball on left!");
    drive_robot(0.5, 1);
  }
  else if (mean_x_position > width * 2 / 3)
  {
    ROS_INFO_STREAM("Ball on right!");
    drive_robot(0.5, -1);
  }
  else
  {
    ROS_INFO_STREAM("Ball ahead!");
    drive_robot(1, 0.0);
  }
}
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
