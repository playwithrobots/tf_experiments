#include "ros/ros.h"
#include "sensor_msgs/Imu.h"
#include "tf/tf.h"
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

#define PROG "xmacroplay"
using namespace std;

const int DefaultDelay = 1;
const float DefaultScale = 1.0;

int   Delay = DefaultDelay;
float Scale = DefaultScale;
char * Remote;
double roll, pitch, yaw;

void quaternion_fun(const sensor_msgs::Imu::ConstPtr& msg)
{
  // Convert quaternion to RPY.
    btQuaternion q;

    tf::quaternionMsgToTF(msg->orientation, q);
    btMatrix3x3(q).getRPY(roll, pitch, yaw);
    ROS_INFO("RPY = (%lf, %lf, %lf)", roll*180.0/3.14, pitch*180.0/3.14, yaw*180.0/3.14);
}





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
	

int main(int argc, char **argv)
{	int i=0;int UP_pressed=0,LEFT_pressed=0,RIGHT_pressed=0,REVERSE_pressed=0; 
  ros::init(argc, argv, "android");

  ros::NodeHandle n;

  ros::Subscriber quaternion_sub = n.subscribe("android/imu", 1, quaternion_fun);
parseCommandLine ( argc, argv );
// open the remote display or abort
	Display * RemoteDpy = remoteDisplay ( Remote );

// get the screens too
  int RemoteScreen = DefaultScreen ( RemoteDpy );
  
  XTestDiscard ( RemoteDpy );
action_mouse_position( RemoteDpy, RemoteScreen ,100,100); 
action_mouse_button( RemoteDpy, RemoteScreen ,1);

  ros::Rate r(30);
  while (ros::ok())
  {	roll=roll*180.0/3.14;
	pitch=pitch*180.0/3.14;
	yaw=yaw*180.0/3.14;
	//z_value[LEFT_HAND-1] 
	if((pitch>-20)&&(pitch<60)) /// only when hand if fwd 
	{	if(REVERSE_pressed==1)
		{ 
		XTestFakeKeyEvent ( RemoteDpy,116, False, Delay );  //REVERSE
		XFlush ( RemoteDpy );	  
		REVERSE_pressed=0;
		}
		if(UP_pressed==0)
		{ 
		XTestFakeKeyEvent ( RemoteDpy,111, True, Delay );  //UP
		XFlush ( RemoteDpy );	  
		ROS_INFO("UP");
		UP_pressed=1;
		} 
 		if (roll<-30)
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
			
 		if (roll>30)
		{ if(RIGHT_pressed==0)
			{ XTestFakeKeyEvent ( RemoteDpy,114, True, Delay );  //RIGHT
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
	XFlush ( RemoteDpy );	
	} 
	if(pitch>60)			// REVERSE
	{ if(REVERSE_pressed==0)
		{ 
		XTestFakeKeyEvent ( RemoteDpy,116, True, Delay );  //REVERSE
		XFlush ( RemoteDpy );	  
		ROS_INFO("REVERSE");
		REVERSE_pressed=1;
		}
	} 

    r.sleep();
ros::spinOnce();
    
  }
XTestDiscard ( RemoteDpy );
  XFlush ( RemoteDpy ); 

  // we're done with the display
  XCloseDisplay ( RemoteDpy );

  cerr << PROG << ": pointer and keyboard released. " << endl;
  

}
