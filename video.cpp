#include "logitech.h"

LogitechVideoCapture::LogitechVideoCapture(int device) : VideoCapture(device){
  this->set(CV_CAP_PROP_FRAME_WIDTH,640);
  this->set(CV_CAP_PROP_FRAME_HEIGHT,480);
}
