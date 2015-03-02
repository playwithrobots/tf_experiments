##This is not under development and was tested on ROS Electric Emys. 
www.PlaywithRobots.com

Playing game with kinect   

Package : openni\_tracker , tf\_experiments

working 
The basic concept used is to send fake keyboard event signal on doing a particular gesture. For example if my both hands are in front of my neck it will send a signal that UP arrow key is pressed. Similarly when my left hand is below right hand by 20 cm and my hands are still in front of my neck it will send a signal as LEFT arrow key pressed.   

Usage : 

Run openni\_tracker node in openni\_kinect package, this node publishes transforms of various body joints.  ( You can download this package from www.ros.org) 

After this run your game, and during the game pause it, open terminal , run nfs node from package tf\_experiments, its syntax is : 
rosrun tf\_experiments nfs “$DISPLAY” 

Now you have 10sec to bring the game on the top of the screen. 

Precautions: Only one user at a time. 

UP command: right + left hand in horizontal position 30cm in front of neck( 30 cm in terms of depth). 

RIGHT command: Right hand above left hand by 25cm , while keeping both hand 30cm in front of neck.

LEFT command: Left hand above right hand by 25cm , while keeping both hand 30cm in front of neck.

No signal: when both hands are within 30cm close to neck in terms of depth. 


