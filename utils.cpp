#include "utils.h"
#include <algorithm>

cv::RotatedRect& KLTRegion:: get_rect(){
  return rect_;
}

cv::RotatedRect& KLTRegion:: get_augmented_rect(){
  return augmented_rect_;
}

std::vector<cv::Point>& KLTRegion:: get_selected_keypoints(){
  return sel_pts_;
}

KLTRegion:: KLTRegion() : is_initialized_(false){
}

bool KLTRegion:: is_initialized(){
  return is_initialized_;
}

void KLTRegion:: init(std::vector<cv::KeyPoint>& keypoints, int padding){
  sel_pts_.clear();
  for(std::vector<cv::KeyPoint>::iterator i = keypoints.begin();i!=keypoints.end();i++)
    sel_pts_.push_back(i->pt);

  if(sel_pts_.size()<2)
    throw KLTRegionException();

  rect_ = cv::minAreaRect(sel_pts_);
  augmented_rect_ = rect_;
  augmented_rect_.size.width+=padding;
  augmented_rect_.size.height+=padding;
  is_initialized_ = true;
}

void KLTRegion:: init(std::vector<cv::KeyPoint>& keypoints, std::vector<cv::Point>& initial_region, int padding){
  sel_pts_.clear();
  for(std::vector<cv::KeyPoint>::iterator i = keypoints.begin();i!=keypoints.end();i++)
    if(cv::pointPolygonTest ( initial_region,i->pt , false ) > 0)
      sel_pts_.push_back(i->pt);

  if(sel_pts_.size()<2)
    throw KLTRegionException();
  rect_ = cv::minAreaRect(sel_pts_);
  augmented_rect_ = rect_;
  augmented_rect_.size.width+=padding;
  augmented_rect_.size.height+=padding;
  is_initialized_ = true;
}

void KLTRegion:: compute_new_region(std::vector<cv::KeyPoint>& keypoints, int padding){
  if(keypoints.size()<10)
    throw KLTRegionException();
  cv::Point2f   pts[4];
  std::vector<cv::Point> pts_vec(4);
  augmented_rect_.points(&(pts[0]));
  std::copy(pts,pts+4,pts_vec.begin());

  init(keypoints,pts_vec,padding);
}

boost::shared_ptr<cv::Point> KLTRegion:: get_points(cv::RotatedRect& rect){
  cv::Point2f   pts[4];
  boost::shared_ptr<cv::Point> pts2(new cv::Point[4]);

  rect.points(pts);
  std::copy(pts,pts+4,pts2.get());

  return pts2;
}

