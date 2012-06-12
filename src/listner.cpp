#include <ros/ros.h>
#include <tf/transform_listener.h>
#include <visualization_msgs/Marker.h>

#include <cmath>
#define ODF 0
#define HEAD 1
#define NECK 2
#define TORSO 3
#define LEFT_SHOULDER 4
#define LEFT_ELBOW 5
#define LEFT_HAND 6
#define RIGHT_SHOULDER 7
#define RIGHT_ELBOW 8
#define RIGHT_HAND 9
#define LEFT_HIP 10
#define LEFT_KNEE 11
#define LEFT_FOOT 12
#define RIGHT_HIP 13
#define RIGHT_KNEE 14
#define RIGHT_FOOT 15
int sequence[34]={HEAD,NECK,
LEFT_SHOULDER,LEFT_ELBOW,
LEFT_ELBOW,LEFT_HAND,
RIGHT_SHOULDER,RIGHT_ELBOW,
RIGHT_ELBOW,RIGHT_HAND,
LEFT_HIP,LEFT_KNEE,
LEFT_KNEE,LEFT_FOOT,
RIGHT_HIP,RIGHT_KNEE,
RIGHT_KNEE,RIGHT_FOOT,
LEFT_SHOULDER,TORSO,
RIGHT_SHOULDER,TORSO,
LEFT_SHOULDER,LEFT_HIP,
RIGHT_SHOULDER,RIGHT_HIP,
LEFT_SHOULDER,RIGHT_SHOULDER,
RIGHT_HIP,LEFT_HIP,
TORSO,RIGHT_HIP,
TORSO,LEFT_HIP};
float x_value[15]; 
float y_value[15]; 
float z_value[15]; 
char array[16][20]={"/camera_depth_frame","/head_1","/neck_1","/torso_1","/left_shoulder_1","/left_elbow_1","/left_hand_1","/right_shoulder_1","/right_elbow_1","/right_hand_1","/left_hip_1","/left_knee_1","/left_foot_1","/right_hip_1","/right_knee_1","/right_foot_1"} ;
int main( int argc, char** argv )
{ int i=0;
  ros::init(argc, argv, "points_and_lines");
  ros::NodeHandle n,node;
  tf::TransformListener listener;
  listener.waitForTransform("/camera_depth_frame", "/head_1", ros::Time(), ros::Duration(5.0));

ros::Publisher marker_pub = n.advertise<visualization_msgs::Marker>("visualization_marker", 10);

  ros::Rate r(30);
  while (node.ok())
  {
   tf::StampedTransform transform;
	for(i=0;i<=14;i++)
		{ try{
		      listener.lookupTransform(array[0], array[i+1],  
		                       ros::Time(0), transform);
		     }
	   	 catch (tf::TransformException ex)
	     	{  ROS_ERROR("%s",ex.what());} 
		x_value[i]=transform.getOrigin().x();
		y_value[i]=-transform.getOrigin().y();
		z_value[i]=transform.getOrigin().z();
		//ROS_INFO(" %f %f %f ",x_value[i],z_value[i],z_value[i]); 
		} 



    visualization_msgs::Marker points,line_list;
    points.header.frame_id = line_list.header.frame_id = array[0];
    points.header.stamp = line_list.header.stamp = ros::Time::now();
    points.ns = line_list.ns = "points_and_lines";
    points.action =line_list.action = visualization_msgs::Marker::ADD;
    points.pose.orientation.w = line_list.pose.orientation.w = 1.0;
    points.id = 0;
    line_list.id = 1;
    points.type = visualization_msgs::Marker::POINTS;
    line_list.type = visualization_msgs::Marker::LINE_LIST;



    // POINTS markers use x and y scale for width/height respectively
    points.scale.x = 0.04;
    points.scale.y = 0.04;

line_list.scale.x = 0.04;
    // Points are green
    points.color.g = 1.0f;
    points.color.a = 1.0;
    // Line list is red
    line_list.color.r = 1.0f;
    line_list.color.a = 1.0;
    // Create the vertices for the points and lines
geometry_msgs::Point p;    
	for (i=0; i <=14; i++)
	{	
	p.x = x_value[i];
	p.y = y_value[i];
	p.z = z_value[i];
	points.points.push_back(p);
	}
	for(i=0;i<34;i++)
	{
	p.x = x_value[(sequence[i]-1)];
	p.y = y_value[(sequence[i]-1)];
	p.z = z_value[(sequence[i]-1)];
	line_list.points.push_back(p);
	}


   marker_pub.publish(points);
   marker_pub.publish(line_list);

    r.sleep();

    
  }
}
