#ifndef __UTILS_H__
#define __UTILS_H__
#include "cv.h"
#include <vector>
#include <boost/shared_ptr.hpp>
#include <exception>

class KLTRegionException : public std::exception{

};

class KLTRegion{
  private:
    bool is_initialized_;
    cv::RotatedRect rect_;
    cv::RotatedRect augmented_rect_;
    std::vector<cv::Point> sel_pts_;
  public:
    KLTRegion();
    void init(std::vector<cv::KeyPoint>& keypoints, std::vector<cv::Point>& initial_region,int padding=0);
    void init(std::vector<cv::KeyPoint>& keypoints, int padding=0.);
    cv::RotatedRect& get_rect();
    cv::RotatedRect& get_augmented_rect();
    bool is_initialized();
    std::vector<cv::Point>& get_selected_keypoints();

    void compute_new_region(std::vector<cv::KeyPoint>& keypoints,int padding=0);
    static boost::shared_ptr<cv::Point> get_points(cv::RotatedRect& rect);
};
#endif
