#include <ros/ros.h>
#include <tf/transform_listener.h>
#include <visualization_msgs/Marker.h>
/// 112, 113, 114 
#include <cmath>

#include <stdio.h>		
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <X11/Xlibint.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/keysymdef.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include <iostream>
#include <iomanip>
#ifdef HAVE_CONFIG
#include "config.h"
#endif


using namespace std;
#define PROG "xmacroplay"
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


const int DefaultDelay = 10;
const float DefaultScale = 1.0;

int   Delay = DefaultDelay;
float Scale = DefaultScale;
char * Remote;

int sequence[16]={HEAD,NECK,
LEFT_SHOULDER,LEFT_ELBOW,
LEFT_ELBOW,LEFT_HAND,
RIGHT_SHOULDER,RIGHT_ELBOW,
RIGHT_ELBOW,RIGHT_HAND,
LEFT_SHOULDER,TORSO,
RIGHT_SHOULDER,TORSO,
LEFT_SHOULDER,RIGHT_SHOULDER};
float x_value[9]; 
float y_value[9]; 
float z_value[9]; 
char array[10][20]={"/openni_depth_frame","/head_1","/neck_1","/torso_1","/left_shoulder_1","/left_elbow_1","/left_hand_1","/right_shoulder_1","/right_elbow_1","/right_hand_1"} ;







void parseCommandLine (int argc, char * argv[]) {

  int Index = 2;
    // check the number of arguments
  if ( argc < 2 ) {
	// oops, too few arguments, go away
	ROS_ERROR(" oops, too few arguments, go away\n"); 
	exit ( EXIT_SUCCESS );
  }

  
	// is this the last parameter?
	else if ( Index == argc) {
	  // yep, we assume it's the display, store it
	  Remote = argv [ Index ];
	}

	else {
	  // we got this far, the parameter is no good...
	  cerr << "Invalid parameter '" << argv[Index] << "'." << endl;
	  ROS_ERROR(" Invalid parameter "); 
		exit ( EXIT_SUCCESS );
	}	
  
}


Display * remoteDisplay (const char * DisplayName) {

  int Event, Error;
  int Major, Minor;  

  // open the display
  Display * D = XOpenDisplay ( DisplayName );

  // did we get it?
  if ( ! D ) {
	// nope, so show error and abort
	cerr << PROG << ": could not open display \"" << XDisplayName ( DisplayName )
		 << "\", aborting." << endl;
	exit ( EXIT_FAILURE );
  }

  // does the remote display have the Xtest-extension?
  if ( ! XTestQueryExtension (D, &Event, &Error, &Major, &Minor ) ) {
	// nope, extension not supported
	cerr << PROG << ": XTest extension not supported on server \""
		 << DisplayString(D) << "\"" << endl;

	// close the display and go away
	XCloseDisplay ( D );
	exit ( EXIT_FAILURE );
  }

  // print some information
  cerr << "XTest for server \"" << DisplayString(D) << "\" is version "
	   << Major << "." << Minor << "." << endl << endl;;

  // execute requests even if server is grabbed 
  XTestGrabControl ( D, True ); 

  // sync the server
  XSync ( D,True ); 

  // return the display
  return D;
}



void action_mouse_position (Display * RemoteDpy, int RemoteScreen, int x,int y)
{
XTestFakeMotionEvent(RemoteDpy,RemoteScreen ,x,y,Delay); 
	XFlush ( RemoteDpy );
} 
void action_mouse_button (Display * RemoteDpy, int RemoteScreen, int button) 
{
	  XTestFakeButtonEvent ( RemoteDpy, button, True, Delay );
	  XTestFakeButtonEvent ( RemoteDpy, button, False, Delay );
	  XFlush ( RemoteDpy );
} 
	
void action_keyboard (Display * RemoteDpy, int RemoteScreen, int key_code) 
{
	XTestFakeKeyEvent ( RemoteDpy, key_code, True, Delay );
	XFlush ( RemoteDpy );		  
	XTestFakeKeyEvent ( RemoteDpy, key_code, False, Delay );
	XFlush ( RemoteDpy );
} 
	

int main( int argc, char** argv )
{ int i=0;int UP_pressed=0,LEFT_pressed=0,RIGHT_pressed=0; 
  ros::init(argc, argv, "points_and_lines");
  ros::NodeHandle n,node;
  tf::TransformListener listener;
parseCommandLine ( argc, argv );
// open the remote display or abort
	Display * RemoteDpy = remoteDisplay ( Remote );

// get the screens too
  int RemoteScreen = DefaultScreen ( RemoteDpy );
  
  XTestDiscard ( RemoteDpy );
listener.waitForTransform("/openni_depth_frame", "/head_1", ros::Time(), ros::Duration(5.0));

ros::Publisher marker_pub = n.advertise<visualization_msgs::Marker>("visualization_marker", 10);

ROS_INFO(" will start in 10 sec "); 
sleep(10); 
action_mouse_position( RemoteDpy, RemoteScreen ,100,100); 
action_mouse_button( RemoteDpy, RemoteScreen ,1);

  ros::Rate r(30);
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
		if(LEFT_HAND==i+1)
			ROS_INFO("LEFT HAND = %f %f %f ",x_value[i],y_value[i],z_value[i]); 
		if(RIGHT_HAND==i+1)
			ROS_INFO("RIGHT HAND = %f %f %f ",x_value[i],y_value[i],z_value[i]); 
		
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
	for (i=0; i <=8; i++)
	{	
	p.x = x_value[i];
	p.y = y_value[i];
	p.z = z_value[i];
	points.points.push_back(p);
	}
	for(i=0;i<16;i++)
	{
	p.x = x_value[(sequence[i]-1)];
	p.y = y_value[(sequence[i]-1)];
	p.z = z_value[(sequence[i]-1)];
	line_list.points.push_back(p);
	}


   marker_pub.publish(points);
   marker_pub.publish(line_list);
	//z_value[LEFT_HAND-1] 
	if( (x_value[LEFT_HAND-1]+0.2)<=x_value[NECK-1]) /// only when hand if fwd 
	{	if(UP_pressed==0)
		{ 
		XTestFakeKeyEvent ( RemoteDpy,111, True, Delay );  //UP
		XFlush ( RemoteDpy );	  
		ROS_INFO("UP");
		UP_pressed=1;
		} 
 		if (z_value[LEFT_HAND-1]>=(z_value[RIGHT_HAND-1]+0.2))
		{   	if(LEFT_pressed==0)
			{ XTestFakeKeyEvent ( RemoteDpy,113, True, Delay );  //LEFT
			XFlush ( RemoteDpy );	  
			ROS_INFO("LEFT"); 
			LEFT_pressed=1;
			}	
		}
		else 
		{ LEFT_pressed=0;
			XTestFakeKeyEvent ( RemoteDpy,113, False, Delay );  //LEFT
		XFlush ( RemoteDpy );	
		} 
			
 		if (z_value[LEFT_HAND-1]<=(z_value[RIGHT_HAND-1]-0.2))
		{ if(RIGHT_pressed==0)
			{ XTestFakeKeyEvent ( RemoteDpy,114, True, Delay );  //LEFT
			XFlush ( RemoteDpy );	  
			ROS_INFO("RIGHT"); 
			RIGHT_pressed=1;
			}	
		}
		else 
		{ RIGHT_pressed=0;
			XTestFakeKeyEvent ( RemoteDpy,114, False, Delay );  //RIGHT
		XFlush ( RemoteDpy );	
		} 
	}   
	else 
	{ UP_pressed=0; LEFT_pressed=0;RIGHT_pressed=0;
		XTestFakeKeyEvent ( RemoteDpy,111, False, Delay );  //UP
	XFlush ( RemoteDpy );	
		XTestFakeKeyEvent ( RemoteDpy,113, False, Delay );  //LEFT
	XFlush ( RemoteDpy );	
		XTestFakeKeyEvent ( RemoteDpy,114, False, Delay );  //RIGHT
	XFlush ( RemoteDpy );	} 

    r.sleep();

    
  }
XTestDiscard ( RemoteDpy );
  XFlush ( RemoteDpy ); 

  // we're done with the display
  XCloseDisplay ( RemoteDpy );

  cerr << PROG << ": pointer and keyboard released. " << endl;
  

}
