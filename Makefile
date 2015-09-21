CXX=g++

LIBS=-lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_video
MISC=-pthread

STD=-std=c++11

FILES=motiontrack.cpp camera.cpp

build:	
	$(CXX) $(LIBS) $(STD) $(MISC) $(FILES) -o MotionTrack
