CXX=g++-4.8

LIBS=-lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_video
MISC=-pthread

STD=-std=c++11

build:	
	$(CXX) $(LIBS) $(STD) $(MISC) motiontrack.cpp -o MotionTrack
