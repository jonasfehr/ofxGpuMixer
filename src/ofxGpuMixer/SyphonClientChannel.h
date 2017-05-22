#pragma once

#include "BasicChannel.h"
#include "ofxAutoReloadedShader.h"

OFX_GPUMIXER_BEGIN_NAMESPACE
#include "ofxSyphonClient.h"

class SyphonClientChannel: public BasicChannel{
public:
    

    ofxSyphonClient *syphonClient;
    
    
    SyphonClientChannel(){}
    
    
    void setup(ofxSyphonClient * syphonClient, string channelName){
        this->name = channelName;
        this->syphonClient = syphonClient;
        
        
        fbo.allocate(syphonClient->getWidth(), syphonClient->getHeight(), GL_RGBA32F_ARB);
        fbo.begin();
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        fbo.end();
        
        setupParameterGroup(name);
    }
    
    
    void update(){
        fbo.begin();
        {
            syphonClient->getTexture().bind();
            {
                ofSetColor(255,255,255);
                ofFill();
                ofDrawRectangle(0, 0, fbo.getWidth(), fbo.getHeight());
            }
            syphonClient->getTexture().unbind();
        }
        fbo.end();
    }
    
    void setupParameterGroup(string name){
        parameterGroup.setName(name);
        
    }
};


OFX_GPUMIXER_END_NAMESPACE
