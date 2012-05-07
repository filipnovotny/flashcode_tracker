#include "cv.h"
#include "highgui.h"

#include <iostream>
#include "logitech.hpp"
#include <boost/format.hpp>
#include "cmd_line/cmd_line.h"
#include "datamatrix/detector.h"
#include <ctime>
#include "utils.h"
#include <visp/vpKltOpencv.h>

#define INIT_DMX 0
#define INIT_KLT 1
#define TRACK_KLT 2
#define RECOVER_KLT 3

void fillKeyPoints(vpKltOpencv& visp_klt_tracker, std::vector<cv::KeyPoint>& keypoints){
  for(int i=0;i<visp_klt_tracker.getNbFeatures();i++){
      int id; float x; float y;
      visp_klt_tracker.getFeature(i,id,x,y);

      keypoints.push_back(cv::KeyPoint(x,y,-1,-1,-1,-1,id));
    }
}

int main(int argc, char** argv)
{
  CmdLine cmd(argc,argv);
  KLTRegion klt_reg;
  const int n = 4;
  datamatrix::Detector dmx_detector;

  if(cmd.should_exit()) return 1;
  LogitechVideoCapture cap(cmd.get_video_channel()); // open the default camera

  if(!cap.isOpened())  // check if we succeeded
    return -1;

  cv::Mat disp_mat;
  cv::GoodFeaturesToTrackDetector gftt(1000, 0.5,1.);
  vpKltOpencv visp_klt_tracker;

  cv::namedWindow("video",1);

  int nbframes = 0;
  time_t start = clock();

  int status = INIT_DMX;

  for(;;nbframes++){
    if(cmd.show_fps()){
      double sec = ((double)(clock()-start))/CLOCKS_PER_SEC;
      if( sec > 2.){
        double fps = (double)nbframes/sec;
        start = clock();
        nbframes = 0;
        std::cout << "framerate: " << fps << " fps"<< std::endl;
      }
    }

    cv::Mat frame;
    cv::Mat frame_gray;
    cap >> frame; // get a new frame from camera
    cv::cvtColor(frame,frame_gray,CV_RGB2GRAY);

    disp_mat = frame;
    if((cmd.using_klt() && status == INIT_DMX) || !cmd.using_klt()){
      if(cmd.get_verbose())
        std::cout << "Detecting datamatrix region" << std::endl;
      if((klt_reg.is_initialized() && !cmd.using_dmx()) ||  dmx_detector.detect(frame,cmd.get_dmx_timeout())){
        const cv::Point *pts = &(dmx_detector.get_polygon()[0]);

        cv::polylines(disp_mat, &pts,&n,1,true, cv::Scalar(0,255,0), 2, CV_AA);
        status = INIT_KLT;
      }else if(klt_reg.is_initialized() && cmd.use_klt_recovery()){
        if(cmd.get_verbose())
          std::cout << "Datamatrix lost. Trying to recover from region ..." << std::endl;
        status = RECOVER_KLT;
      }else{
        if(cmd.get_verbose())
          std::cout << "Failed to detect datamatrix" << std::endl;
      }
    }

    if( cmd.using_klt()){
      std::vector<cv::KeyPoint> keypoints;
      if(!cmd.track_visp_features())
        gftt.detect(frame, keypoints);
      try{
        if(status == INIT_KLT){
          if(cmd.get_verbose())
            std::cout << "KLT initialisation..." << std::endl;
          if(cmd.track_visp_features()){
            IplImage iplframe =  (IplImage)frame_gray;

            visp_klt_tracker.initTracking(&iplframe, NULL);
            fillKeyPoints(visp_klt_tracker,keypoints);
          }

          klt_reg.init(keypoints,dmx_detector.get_polygon(),cmd.get_reg_padding());

          for(std::vector<cv::Point>::iterator i = klt_reg.get_selected_keypoints().begin();
              i!=klt_reg.get_selected_keypoints().end();
              i++)
              cv::circle(disp_mat,*i,5,CV_RGB(0,0,255),2);

          if(cmd.get_verbose())
            std::cout << klt_reg.get_selected_keypoints().size() << " points initialized." << std::endl;
          cv::RotatedRect& rect = klt_reg.get_augmented_rect();
          boost::shared_ptr<cv::Point> ptr = KLTRegion::get_points(rect);
          const cv::Point *pts = ptr.get();

          cv::polylines(disp_mat, &pts,&n,1,true, cv::Scalar(255,0,0), 2, CV_AA);
          status = TRACK_KLT;
          if(cmd.get_verbose())
            std::cout << "tracking..." << std::endl;
        }else if (status == TRACK_KLT || status == RECOVER_KLT){
          cv::Scalar color = CV_RGB(0,0,255);
          if(cmd.track_visp_features()){
            IplImage iplframe =  (IplImage)frame_gray;
            if(status == TRACK_KLT){
              visp_klt_tracker.track(&iplframe);
            }else{
              visp_klt_tracker.initTracking(&iplframe, NULL);
              status = TRACK_KLT;
              color = CV_RGB(255,0,255);
            }
            fillKeyPoints(visp_klt_tracker,keypoints);
          }

          klt_reg.compute_new_region(keypoints,cmd.get_reg_padding());
          for(std::vector<cv::Point>::iterator i = klt_reg.get_selected_keypoints().begin();
                      i!=klt_reg.get_selected_keypoints().end();
                      i++)
                      cv::circle(disp_mat,*i,5,color,2);

          if(klt_reg.get_selected_keypoints().size() < cmd.get_keypoints_limit()){
            status = INIT_DMX;
            continue;
          }
          cv::RotatedRect& rect = klt_reg.get_augmented_rect();
          boost::shared_ptr<cv::Point> ptr = KLTRegion::get_points(rect);
          const cv::Point *pts = ptr.get();

          cv::polylines(disp_mat, &pts,&n,1,true, cv::Scalar(0,255,0), 2, CV_AA);
        }
      }catch(KLTRegionException&){
        status = INIT_DMX;
      }
    }

    cv::imshow("video", disp_mat);
    if(cv::waitKey(27) >= 0)
      break;
  }

  return 0;
}
