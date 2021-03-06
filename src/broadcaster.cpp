#include <ros/ros.h>
#include <tf/transform_broadcaster.h>
int ran(void)
{ int j=0;
j = 1 + (int)( 3.0 * rand() / ( RAND_MAX + 1.0 ) );
return j;

} 
int main(int argc, char** argv){
  ros::init(argc, argv, "robot_tf_publisher");
  ros::NodeHandle n;

  ros::Rate r(1);

  tf::TransformBroadcaster broadcaster;

  while(n.ok()){
    broadcaster.sendTransform(
      tf::StampedTransform(
        tf::Transform(tf::Quaternion(0, 0, 0, 1), tf::Vector3(ran(), ran(), ran())),
        ros::Time::now(),"/openni_depth_frame", "/head"));


	 broadcaster.sendTransform(
      tf::StampedTransform(
        tf::Transform(tf::Quaternion(0, 0, 0, 1), tf::Vector3(ran(), ran(), ran())),
        ros::Time::now(),"/openni_depth_frame", "/neck"));
    r.sleep();
  }
}

