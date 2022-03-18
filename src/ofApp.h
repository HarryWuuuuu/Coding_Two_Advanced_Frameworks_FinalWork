#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include "MotionAmplifier.h"

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void mousePressed(int x, int y, int button);
    
    ofVideoGrabber cam;
    ofPixels previous;
    ofImage diff;
    cv::Mat columnMean;
    
    //advanced
    ofxCv::ContourFinder contourFinder;
    ofColor targetColor;
    
    ofxPanel gui;
    ofParameter<float> threshold;
    ofParameter<bool> trackHs;
    ofParameter<bool> holes;
    
    //shader
    MotionAmplifier motionAmplifier;
};
