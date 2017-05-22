#pragma once

#include "BasicChannel.h"
#include "ofxAutoReloadedShader.h"
#include "ofxSyphonClientDir.h"

OFX_GPUMIXER_BEGIN_NAMESPACE

class SyphonClientDirChannel: public BasicChannel{
public:
    

    ofxSyphonClientDir *syphonClient;
    
    
    SyphonClientDirChannel(){}
    
    
    void setup(ofxSyphonClientDir  *syphonClient, string channelName){
        this->name = channelName;
        this->syphonClient = syphonClient;
        
        
        if(syphonClient->isSetup()){
            fbo.allocate(syphonClient->getWidth(), syphonClient->getHeight(), GL_RGBA32F_ARB);
            fbo.begin();
            glClearColor(0.0, 0.0, 0.0, 0.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            fbo.end();
        }

        
        setupParameterGroup(name);
    }
    
    
    void update(){
        if(fbo.isAllocated()){
            fbo.begin();
            {
                ofSetColor(255);
                ofFill();
                syphonClient->draw();
            }
            fbo.end();
        } else {
            if(syphonClient->isSetup()){
                fbo.allocate(syphonClient->getWidth(), syphonClient->getHeight(), GL_RGBA32F_ARB);
                fbo.begin();
                glClearColor(0.0, 0.0, 0.0, 0.0);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                fbo.end();
            }
        }
  
    }
    
    void setupParameterGroup(string name){
        parameterGroup.setName(name);
        
    }
};


OFX_GPUMIXER_END_NAMESPACE
