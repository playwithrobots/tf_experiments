#include <ros/ros.h>
#include <tf/transform_listener.h>
#include<P3DX_sbpl/start_msg.h>
/// 112, 113, 114 
#include <cmath>
#include <stdio.h>		
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

float x_value[9]; 
float y_value[9]; 
float z_value[9]; 
char array[10][20]={"/openni_depth_frame","/head_1","/neck_1","/torso_1","/left_shoulder_1","/left_elbow_1","/left_hand_1","/right_shoulder_1","/right_elbow_1","/right_hand_1"} ;

float x1,x2;
float Y1,y2;
float z1,z2; 

float t;
float new_x;
float new_y;
int goalx,goaly;




int main( int argc, char** argv )
{ int i=0; int flag=0;
  ros::init(argc, argv, "pointing");
  ros::NodeHandle n,node,pub;
	ros::Publisher sbpl_xy_pub;
	sbpl_xy_pub = pub.advertise<P3DX_sbpl::start_msg>("start_sbpl",1);
	P3DX_sbpl::start_msg msg; 
  tf::TransformListener listener;
listener.waitForTransform("/openni_depth_frame", "/head_1", ros::Time(), ros::Duration(5.0));

ROS_INFO(" will start in 5 sec "); 
sleep(5); 
  ros::Rate r(10);
  while (node.ok())
  {
   tf::StampedTransform transform;
	for(i=0;i<=8;i++)
		{ try{
		      listener.lookupTransform(array[0], array[i+1],  
		                       ros::Time(0), transform);
		     }
	   	 catch (tf::TransformException ex)
	     	{  ROS_ERROR("%s",ex.what());} 
		x_value[i]=transform.getOrigin().x();
		y_value[i]=transform.getOrigin().y();
		z_value[i]=transform.getOrigin().z();
		//if(LEFT_HAND==i+1)
		//	ROS_INFO("LEFT HAND = %f %f %f ",x_value[i],y_value[i],z_value[i]); 
		//if(RIGHT_HAND==i+1)
			//ROS_INFO("RIGHT HAND = %f %f %f ",x_value[i],y_value[i],z_value[i]); 
		
		//ROS_INFO(" %f %f %f ",x_value[i],z_value[i],z_value[i]); 
		} 
		if(z_value[RIGHT_HAND-1]>(z_value[NECK-1])&&flag==0)
		{ flag=1; 
		x1=x_value[LEFT_SHOULDER-1];
		x2=x_value[LEFT_HAND-1];
		Y1=y_value[LEFT_SHOULDER-1];
		y2=y_value[LEFT_HAND-1];
		z1=z_value[LEFT_SHOULDER-1];
		z2=z_value[LEFT_HAND-1];
		t=-z1/(z2-z1);
		new_x=x1+(x2-x1)*t;
		new_y=Y1+(y2-Y1)*t;
		ROS_INFO(" x=%f y=%f ", new_x,new_y); 
		// let grid is of 21 by 21
		msg.width=21;
		msg.height=21;
		goalx=(new_x*100)/40;		
		goaly=(new_y*100)/40;
		msg.goalx=goalx+10;
		msg.goaly=goaly+10;
		msg.startx=10;
		msg.starty=10;
		if(goalx>10)
			msg.goalx=10;
		if(goalx<-10)
			msg.goalx=-10;
		if(goaly>10)
			msg.goaly=10;
		if(goaly<-10)
			msg.goaly=-10;
		msg.obsthresh=1;
		sbpl_xy_pub.publish(msg);		

		}
		if(z_value[RIGHT_HAND-1]<(z_value[NECK-1]))
		flag=0;
 r.sleep();

    
  }

}
