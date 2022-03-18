#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
    ofSetVerticalSync(true);    //synchronizes the redraw of the screen to the vertical refresh rate of the screen
    cam.setup(640, 480);
    
    // set two elements with the same size and type
    //void imitate(M& mirror, const O& original)
    imitate(previous, cam);
    imitate(diff, cam);
    
    //advanced, set the size of contourfinder circle
    contourFinder.setMinAreaRadius(10);
    contourFinder.setMaxAreaRadius(100);
    
    gui.setup();
    gui.add(threshold.set("Threshold", 128, 0, 255));
    gui.add(trackHs.set("Track Hue/Saturation", false));
    gui.add(holes.set("Holes", false));
    
    //shader
    motionAmplifier.setup(cam.getWidth(), cam.getHeight(), 2, .25);
}

void ofApp::update() {
    
    //shader
    motionAmplifier.setStrength(ofRandom(-10, 10));
    //motionAmplifier.setStrength(ofMap(mouseX, 0, ofGetWidth(), -10, 10));
    motionAmplifier.setLearningRate(ofRandom(0.1, 3));
    //motionAmplifier.setLearningRate(ofMap(mouseY, 0, ofGetHeight(), 0, 1, true));
    motionAmplifier.setBlurAmount(0);
    motionAmplifier.setWindowSize(8);
    
    cam.update();
    
    if(cam.isFrameNew()) {
        // take the absolute difference of prev and cam and save it inside diff
        absdiff(cam, previous, diff);
        diff.update();
        copy(cam, previous);    // like ofSetPixels, but more concise and cross-toolkit
        columnMean = meanCols(diff);    // this is the key line: get the average of each column

        //advanced
        contourFinder.setTargetColor(targetColor, trackHs ? TRACK_COLOR_HS : TRACK_COLOR_RGB);  //the color based on the pixel color clicked by the mouse
        contourFinder.setThreshold(threshold);
        contourFinder.setFindHoles(holes);
        contourFinder.findContours(cam);
        
        //shader
        motionAmplifier.update(cam);

    }
}

void ofApp::draw() {
    ofSetColor(255);
    cam.draw(0, 0);
    
    ofTranslate(640, 0);
    diff.draw(0, 0);
    
    //shader
    ofSetupScreenOrtho(ofGetWidth(), ofGetHeight(), -100, +100);
    ofEnableDepthTest();
    motionAmplifier.draw(cam);
//    motionAmplifier.drawMesh();
    ofDisableDepthTest();
    
    //advanced
    ofSetLineWidth(4);
    contourFinder.draw();
    int n = contourFinder.size();
    
    //advanced
    for(int i = 0; i < n; i++) {
        // smallest rectangle that fits the contour
        ofSetColor(12,30,62);
        //ofSetColor(cyanPrint);
        ofPolyline minAreaRect = toOf(contourFinder.getMinAreaRect(i));
        minAreaRect.draw();
        
        // ellipse that best fits the contour
        ofSetColor(142,82,245);
        //ofSetColor(magentaPrint);
        cv::RotatedRect ellipse = contourFinder.getFitEllipse(i);
        ofPushMatrix();
        ofVec2f ellipseCenter = toOf(ellipse.center);
        ofVec2f ellipseSize = toOf(ellipse.size);
        ofTranslate(ellipseCenter.x, ellipseCenter.y);
        ofRotate(ellipse.angle);
        ofDrawEllipse(0, 0, ellipseSize.x, ellipseSize.y);
        ofPopMatrix();
        
        // minimum area circle that encloses the contour
        ofSetColor(56,251,219);
        //ofSetColor(cyanPrint);
        float circleRadius;
        ofVec2f circleCenter = toOf(contourFinder.getMinEnclosingCircle(i, circleRadius));
        ofDrawCircle(circleCenter, circleRadius);
        
        // convex hull of the contour
        ofSetColor(252,15,245);
        //ofSetColor(yellowPrint);
        ofPolyline convexHull = toOf(contourFinder.getConvexHull(i));
        convexHull.draw();
        
        // defects of the convex hull
        vector<cv::Vec4i> defects = contourFinder.getConvexityDefects(i);
        for(int j = 0; j < defects.size(); j++) {
            ofDrawLine(defects[j][0], defects[j][1], defects[j][2], defects[j][3]);
        }
        
        // some different styles of contour centers
        ofVec2f centroid = toOf(contourFinder.getCentroid(i));
        ofVec2f average = toOf(contourFinder.getAverage(i));
        ofVec2f center = toOf(contourFinder.getCenter(i));
        ofSetColor(56,251,219);
        //ofSetColor(cyanPrint);
        ofDrawCircle(centroid, 8);
        ofSetColor(142,82,245);
        //ofSetColor(magentaPrint);
        ofDrawCircle(average, 4);
        ofSetColor(252,15,245);
        //ofSetColor(yellowPrint);
        ofDrawCircle(center, 1);
        
        // you can also get the area and perimeter using ofPolyline:
        // ofPolyline::getArea() and ofPolyline::getPerimeter()
        double area = contourFinder.getContourArea(i);
        double length = contourFinder.getArcLength(i);
        
        // balance is useful for detecting when a shape has an "arm" sticking out
        // if balance.length() is small, the shape is more symmetric: like I, O, X...
        // if balance.length() is large, the shape is less symmetric: like L, P, F...
        ofVec2f balance = toOf(contourFinder.getBalance(i));
        ofPushMatrix();
        ofTranslate(centroid.x, centroid.y);
        ofScale(5, 5);
        ofDrawLine(0, 0, balance.x, balance.y);
        ofPopMatrix();
        
        if(contourFinder.getHole(i)) {
            ofDrawBitmapStringHighlight("hole", center.x, center.y);
        }
    }
    ofTranslate(8, 90);
    ofFill();
    ofSetColor(0);
    ofDrawRectangle(-3, -3, 64+6, 64+6);
    ofSetColor(targetColor);
    ofDrawRectangle(0, 0, 64, 64);
    
    
    // draw the mean for each channel
    for(int k = 0; k < 3; k++) {
        // use the correct color for each channel
        switch(k) {
            case 0: ofSetColor(ofColor::red); break;
            case 1: ofSetColor(ofColor::blue); break;
            case 2: ofSetColor(ofColor::green); break;
        }
        
    ofNoFill();
    ofBeginShape();
    for(int i = 0; i < columnMean.rows; i++) {
        // Vec3b is one way of storing 24-bit (3 byte) colors
        Vec3b cur = columnMean.at<Vec3b>(i);
        ofVertex(i, cur[k]);
    }
    ofEndShape();
    }
    
    //gui.draw();

}

void ofApp::mousePressed(int x, int y, int button) {
    targetColor = cam.getPixels().getColor(x, y);
}
