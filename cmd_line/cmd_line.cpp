#include "cmd_line.h"

namespace po = boost::program_options;
CmdLine:: CmdLine(int argc,char**argv) : should_exit_(false) {
  po::options_description prog_args("Program options");
  prog_args.add_options()
      ("trackklt,k", "print version string")
      ("trackdm,d", "use datamatrix global detection to recover the tracker")
      ("video-source,s", po::value<int>(&video_channel_)->default_value(1),"video source. For example 1 for /video/1")
      ("keypoints-limit,l", po::value<int>(&keypoints_limit_)->default_value(30),"least number of keypoints before considering the tracker is lost")
      ("reg-pad,r", po::value<int>(&reg_padding_)->default_value(20),"detected region padding in pixels")
      ("showfps,f", "show framerate")
      ("verbose,v", "show states of the tracker")
      ("visp-track,t", "track individual features with visp's klt tracker")
      ("use-klt-recovery,R", "when the datamatrix cannot be detected, try to recover using only klt")
      ("dmx-detector-timeout,T", po::value<int>(&dmx_timeout_)->default_value(1000), "timeout for datamatrix detection in ms")

      ("help", "produce help message")
      ;
  
  po::store(po::parse_command_line(argc, argv, prog_args), vm_);
  po::notify(vm_);

  if (vm_.count("help")) {
      std::cout << prog_args << std::endl;
      should_exit_ = true;
  }  
}

bool CmdLine:: using_klt(){
  return vm_.count("trackklt")>0;
}

bool CmdLine:: using_dmx(){
  return vm_.count("trackdm")>0;
}

bool CmdLine:: should_exit(){
  return should_exit_;
}

int CmdLine:: get_video_channel(){
  return video_channel_;
}

int CmdLine:: show_fps(){
  return vm_.count("showfps")>0;
}

int CmdLine:: get_reg_padding(){
  return reg_padding_;
}

int CmdLine:: get_keypoints_limit(){
  return keypoints_limit_;
}

int CmdLine:: get_verbose(){
  return vm_.count("verbose")>0;
}

int CmdLine:: get_dmx_timeout(){
  return dmx_timeout_;
}

bool CmdLine:: track_visp_features(){
  return vm_.count("visp-track")>0;
}

bool CmdLine:: use_klt_recovery(){
  return vm_.count("use-klt-recovery")>0;
}
