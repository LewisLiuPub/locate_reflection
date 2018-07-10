//
// Created by waxz on 18-7-4.
//

#ifndef LOCATE_REFLECTION_BOARDFINDER_H
#define LOCATE_REFLECTION_BOARDFINDER_H

#include <iostream>
#include <chrono>
#include <boost/bind.hpp>
// node msg and srv

#include <cpp_utils/container.h>
#include <cpp_utils/listener.h>
#include <cpp_utils/threading.h>
#include <cpp_utils/parse.h>
#include <cpp_utils/search.h>
#include <locate_reflection/patternMatcher.h>

#include <ros/ros.h>
#include <ros/callback_queue.h> //  ros::CallbackQueue queue
#include "tf/message_filter.h"  // filter message with tf
#include <message_filters/subscriber.h>


//sensor data
#include <sensor_msgs/LaserScan.h>
#include <sensor_msgs/Image.h>

//geometry transformation
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/Point.h>
#include <geometry_msgs/PoseArray.h>
#include <tf/tf.h>
#include <geometry_msgs/PoseWithCovarianceStamped.h>

#include <Eigen/Dense>


//

#include <yaml/Yaml.hpp>
#include <kdtree/kdtree.h>
#include <vector>
#include <string>
#include <map>
#include <tuple>
#include <thread>
#include <array>

using std::vector;
using std::string;
using std::map;
using std::tuple;
using std::endl;


// normal angle diffrence
inline double normalDiff(double angle1, double angle2) {

    double diff = angle1 - angle2;
    // normal to -pi,pi
    diff = atan2(sin(diff), cos(diff));

    return fabs(diff);
}

inline double normalAngle(double angle1, double angle2) {

    double diff = angle1 - angle2;
    // normal to -pi,pi
    diff = atan2(sin(diff), cos(diff));

    return diff;
}
inline bool angleCompare(const Position p1, const Position p2) {
    return atan2(p1.y, p1.x) < atan2(p2.y, p2.x);

}


class BoardFinder {

private:
    // ros
    ros::NodeHandle nh_;
    ros::NodeHandle nh_private_;
    tf::TransformBroadcaster *tfb_;

    // listener
    rosnode::Listener l;
    // publisher
    ros::Publisher boardPub, pointPub, markerPub;

    // shared data for topic
    std::shared_ptr<sensor_msgs::LaserScan> laser_data_;
    std::shared_ptr<geometry_msgs::PoseWithCovarianceStamped> mapOdom_data_;

    // internal state
    geometry_msgs::Pose laserPose_;
    valarray<float> bear_;
    tf::Transform baseLaserTf_;
    tf::Transform odomBaseTf_;
    tf::StampedTransform mapOdomTf_;

    // psrameter
    string scan_topic_;
    string odomtf_topic_;
    string odom_frame_id_;
    string base_frame_id_;
    string laser_frame_id_;
    string fixed_frame_id_;

    // read config file
    Yaml::Node param_;

    // pattern matcher
    PatternMatcher patternMatcher;

    // a thread for publishing tf
    threading_util::ThreadClass threadClass_;
    threading_util::Func_tfb tfThread_;
    std::shared_ptr<tf::StampedTransform> mapToodomtfPtr_;

    // ***** method
    // get laser pose
    // store to laserPose_
    bool getLaserPose();

    // get maptoodomtf from amcl or internal state
    // store to mapOdomTf_
    // if get first mapOdomTf_ ;sync with thread
    bool getFirstmapOdomTf_;

    // get tf from topic
    bool getMapOdomTf(int sleep = 0.1);

    void updateSharedData(tf::Transform mapTOodomTf);


    bool getBoardPosition(vector<Position> &pointsW, vector<Position> &points);

    bool xmlToPoints(vector<Position> &pointsW);

    bool findNN(vector<Position> &realPointsW, vector<Position> &realPoints, vector<Position> &detectPoints);

    void computeUpdatedPose(vector<Position> realPoints, vector<Position> detectPoints);

    void updateMapOdomTf(tf::Transform laserPose, ros::Time time);

    bool transformPoints(vector<Position> &realPoints);


    bool updateSensor();


public:

    BoardFinder(ros::NodeHandle nh, ros::NodeHandle nh_private);

    ~BoardFinder();



    vector<Position> detectBoard();

    void findLocation();


};


#endif //LOCATE_REFLECTION_BOARDFINDER_H