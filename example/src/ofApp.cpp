#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    fboA.allocate(ofGetWidth(),ofGetHeight());
    fboB.allocate(ofGetWidth(),ofGetHeight());
    texB = fboB.getTexture(); // adding a texture insted of a fbo
    colorChannel.setup("Red", ofColor(255,0,0),ofGetWidth(),ofGetHeight());

    
    mixer.addChannel(fboA,"A", ofxGpuMixer::BLEND_ADD);
    mixer.addChannel(texB,"B", ofxGpuMixer::BLEND_ADD);
    mixer.addChannel(colorChannel, ofxGpuMixer::BLEND_ADD);

    mixer.setup(); // Creates the shader in order to mix.
    
    
    // GUI
    gui.setup( mixer.getParameterGroup() );

}

//--------------------------------------------------------------
void ofApp::update(){
    int s = ofGetHeight()/3;
    fboA.begin();
    {
        ofBackground(0,0,255);
        ofSetColor(255,255,0);
        ofDrawCircle(ofGetWidth()/2, ofGetHeight()/2, s);
    }
    fboA.end();
    
    fboB.begin();
    {
        ofBackground(255, 0, 0);
        ofSetColor(0, 255, 255);
        ofDrawRectangle(ofGetWidth()/2-s, ofGetHeight()/2-s, s*2, s*2);
    }
    fboB.end();
    
    mixer.update();

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(20);
    ofSetColor(255);
    mixer.draw(0,0,ofGetWidth(), ofGetHeight());
    
    gui.draw();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
