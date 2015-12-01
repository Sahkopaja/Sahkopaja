CXX=g++

LIBS=-lopencv_core -opencv_highgui -lopencv_imgproc -lopencv_photo -lopencv_video -ljsoncpp
MISC=-pthread

STD=-std=c++11

FILES=main.cpp camera.cpp motiontrack.cpp hardware.cpp

build:	
	$(CXX) $(LIBS) $(STD) $(MISC) $(FILES) -o Tracking

verbose:
	$(CXX) $(LIBS) $(STD) $(MISC) $(FILES) -v -o Tracking

