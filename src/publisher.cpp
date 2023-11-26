#include <iostream>
#include <sstream>
#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <sensor_msgs/Image.h>
#include <opencv2/opencv.hpp>
#include <cv_bridge/cv_bridge.h>

/*
 * Simple camera to ROS bridge in C++
 * This just uses the OpenCV ROS bridge and gstreamer

 * Author: Christopher Vo <vo@irixmedia.com>
 */


int main(int argc, char** argv) {
    ros::init(argc, argv, "simplecamerapublisher");
    ros::NodeHandle nh("~");

    // ROS Parameters ---------------------------------------------------------

    std::string camera_topic;
    int sensor_id;
    int width;
    int height;
    int frame_rate;

    nh.param("camera_topic", camera_topic, std::string("/camera/image"));
    nh.param("sensor_id", sensor_id, 0);
    nh.param("width", width, 1280);
    nh.param("height", height, 720);
    nh.param("frame_rate", frame_rate, 15);

    // Topic publisher --------------------------------------------------------

    image_transport::ImageTransport it(nh);
    image_transport::Publisher pub = it.advertise(camera_topic, 10);

    // Build Gstreamer Pipeline -----------------------------------------------

    std::stringstream ss;
    ss << "nvarguscamerasrc sensor-id=" << sensor_id << " ! video/x-raw(memory:NVMM)" 
       << ", width=(int)" << width
       << ", height=(int)" << height
       << ", format=(string)NV12"
       << ", framerate=(fraction)" << frame_rate << "/1"
       << "! nvvidconv flip-method=2 ! video/x-raw, format=(string)BGRx ! videoconvert ! appsink sync=false"; 
    std::string pipeline = ss.str();

    ROS_INFO("Sensor ID: %d", sensor_id);
    ROS_INFO("Launching pipeline: %s", pipeline.c_str());
    ROS_INFO("Publishing to: %s", camera_topic.c_str());

    // Capture into OpenCV ROS bridge for conversion to Image message ---------

    cv::VideoCapture cap(pipeline, cv::CAP_GSTREAMER);

    if (!cap.isOpened()) {
        ROS_ERROR("Failed to open camera.");
        return -1;
    }

    cv::Mat frame;
    sensor_msgs::ImagePtr msg;

    while (nh.ok()) {
        cap >> frame;
        if (!frame.empty()) {
            msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", frame).toImageMsg();
            pub.publish(msg);
        }
        ros::spinOnce();
    }
}