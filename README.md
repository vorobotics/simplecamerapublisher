# simplecamerapublisher

The `simplecamerapublisher` is a ROS package designed to capture images from CSI cameras using GStreamer, process them with OpenCV, and publish them as ROS Image messages.

It essentially just reads from GStreamer and passes the output to the OpenCV ROS bridge.

## Dependencies

- ROS (Robot Operating System) - Tested on ROS Noetic, but should be compatible with other versions.
- OpenCV - Used for image processing.
- GStreamer - For capturing images from CSI cameras.
- cv_bridge - For converting between ROS Image messages and OpenCV image formats.
