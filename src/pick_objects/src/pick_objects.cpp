#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>

//Positions
float pick_pose[3] = {-3.0, -5.0, 1.0};
float drop_pose[3] = {-18.0, -3.0, 1.0};


// Define a client for to send goal requests to the move_base server through a SimpleActionClient
typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

int main(int argc, char** argv){
  // Initialize the simple_navigation_goals node
  ros::init(argc, argv, "pick_objects");

  //tell the action client that we want to spin a thread by default
  MoveBaseClient ac("move_base", true);

  // Wait 5 sec for move_base action server to come up
  while(!ac.waitForServer(ros::Duration(5.0))){
    ROS_INFO("Waiting for the move_base action server to come up");
  }

  move_base_msgs::MoveBaseGoal goal;

  // set up the frame parameters
  goal.target_pose.header.frame_id = "map";
  goal.target_pose.header.stamp = ros::Time::now();

  // Define a position and orientation for the robot to reach
  goal.target_pose.pose.position.x = pick_pose[0];
  goal.target_pose.pose.position.y = pick_pose[1];
  goal.target_pose.pose.orientation.w = pick_pose[2] ;

   // Send the goal position and orientation for the robot to reach
  ROS_INFO("Setting Pick-up position");
  ac.sendGoal(goal);

  // Wait an infinite time for the results
  ac.waitForResult();

  // Check if the robot reached its goal
  if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
     
    {
     ROS_INFO("Robot reached Pick-up position......");
     ros::Duration(5.0).sleep();
     //Go to drop off point
     // Define a position and orientation for the robot to reach
     goal.target_pose.pose.position.x = drop_pose[0];
     goal.target_pose.pose.position.y = drop_pose[1];
     goal.target_pose.pose.orientation.w = drop_pose[2];
     // Send the goal position and orientation for the robot to reach
     ROS_INFO("Setting Drop-off position");
     ac.sendGoal(goal);
     // Wait an infinite time for the results
     ac.waitForResult();

  if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
     ros::Duration(5.0).sleep();}
  else
     {ROS_INFO("Robot failed!");}

  return 0;
}
