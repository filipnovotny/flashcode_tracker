#ifndef __CMD_LINE_H__
#define __CMD_LINE_H__
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <exception>

class CmdLine{
 private:
  boost::program_options::variables_map vm_;
  bool should_exit_;
  int video_channel_;
  int reg_padding_;
  int keypoints_limit_;
  int dmx_timeout_;
 public:
  CmdLine(int argc,char**argv);
  bool using_klt();
  bool using_dmx();
  bool should_exit();
  bool track_visp_features();
  bool use_klt_recovery();
  int get_video_channel();
  int show_fps();
  int get_reg_padding();
  int get_keypoints_limit();
  int get_dmx_timeout();
  int get_verbose();
};
#endif
