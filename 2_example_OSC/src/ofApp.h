#pragma once

#include "ofMain.h"

#include "ofxBeatClock.h"
#include "ofxOscPublisher.h"

class ofApp: public ofBaseApp{
	public:
    
    void setup();
    void update();
    void draw();
    void exit();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    ofxBeatClock CLOCKER;

//    void Changed_OSC_beats(int & beatsInBar);
//    ofParameter<bool> beatTrig;

    //-

    // OSC OUT
    string out_ip;
    int out_port;
};