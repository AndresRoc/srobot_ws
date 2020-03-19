#include <ros/ros.h>
#include <visualization_msgs/Marker.h>
#include "nav_msgs/Odometry.h"
#include <complex>


//Position of the Target
float pick_pose[3] = {-3.0, -5.0, 1.0};
float drop_pose[3] = {-2.0, -3.0, 1.0};
float thresh[2] = {0.3, 0.01};


//Flags
bool atpick_pose = false;
bool at_drop_pose = false;
bool picked = false;
bool dropped = false;


void chatterCallback(const nav_msgs::Odometry::ConstPtr& msg)
{ 
  
//Pick up
if (std::abs(pick_pose[0] -msg->pose.pose.position.x) < thresh[0] && std::abs(pick_pose[1] -msg->pose.pose.position.y) < thresh[0] && std::abs(pick_pose[2] -msg->pose.pose.orientation.w) < thresh[1])
   { 
    if(!atpick_pose)
    {
     atpick_pose = true;
    }
   }else{atpick_pose = false;}

//Drop off
if (std::abs(drop_pose[0] -msg->pose.pose.position.x) < thresh[0] && std::abs(drop_pose[1] -msg->pose.pose.position.y) < thresh[0] && std::abs(drop_pose[2] -msg->pose.pose.orientation.w) < thresh[1])
  { 
    if(!at_drop_pose)
    {
     at_drop_pose = true;
    }
   }else{at_drop_pose = false;}

}

int main( int argc, char** argv )
{
  ros::init(argc, argv, "add_markers");
  ros::NodeHandle n;
  ros::Rate r(1);
  ros::Publisher marker_pub = n.advertise<visualization_msgs::Marker>("virtual_obj", 1);
  ros::Subscriber odom_sub = n.subscribe("odom", 1000, chatterCallback);
  

  uint32_t shape = visualization_msgs::Marker::CUBE;

  while (ros::ok())
  {
    visualization_msgs::Marker marker;
    // Set the frame ID and timestamp.  See the TF tutorials for information on these.
    marker.header.frame_id = "map";
    marker.header.stamp = ros::Time::now();

    // Set the namespace and id for this marker.  This serves to create a unique ID
    // Any marker sent with the same namespace and id will overwrite the old one
    marker.ns = "basic_shapes";
    marker.id = 0;

    // Set the marker type.  Initially this is CUBE, and cycles between that and SPHERE, ARROW, and CYLINDER
    marker.type = shape;

    // Set the marker action.  Options are ADD, DELETE, and new in ROS Indigo: 3 (DELETEALL)
    marker.action = visualization_msgs::Marker::ADD;

    // Set the pose of the marker.  This is a full 6DOF pose relative to the frame/time specified in the header
    marker.pose.position.x = pick_pose[0];
    marker.pose.position.y = pick_pose[1];
    marker.pose.position.z = 0;
    
    marker.pose.orientation.x = 0.0;
    marker.pose.orientation.y = 0.0;
    marker.pose.orientation.z = 0.0;
    marker.pose.orientation.w = pick_pose[2];

    // Set the scale of the marker -- 1x1x1 here means 1m on a side
    marker.scale.x = 0.5;
    marker.scale.y = 0.5;
    marker.scale.z = 0.5;

    // Set the color -- be sure to set alpha to something non-zero!
    marker.color.r = 1.0f;
    marker.color.g = 0.0f;
    marker.color.b = 0.0f;
    marker.color.a = 1.0;

    marker.lifetime = ros::Duration();

    // Publish the marker
    while (marker_pub.getNumSubscribers() < 1)
    {
      if (!ros::ok())
      {
        return 0;
      }
      sleep(1);
    }
    
   marker_pub.publish(marker);
   
   //Wait for Pick-Up
   while(!atpick_pose)
   {
    ros::spinOnce();
   }
   
   if(atpick_pose && !picked)
   {
    marker.action = visualization_msgs::Marker::DELETE;
    marker_pub.publish(marker);
    picked = true;
   }  
   
   //Wait for Drop-Off
   while(!at_drop_pose)
   {
    ros::spinOnce();
   }

   if(at_drop_pose && !dropped)
   {
    marker.pose.position.x = drop_pose[0];
    marker.pose.position.y = drop_pose[1];
    marker.pose.orientation.w = drop_pose[2];;
    marker.action = visualization_msgs::Marker::ADD;
    marker_pub.publish(marker);
    dropped = true;
    ros::Duration(10.0).sleep();
   }  
    return 0;
  }
 
}
