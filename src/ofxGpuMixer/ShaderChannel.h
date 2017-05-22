#pragma once

#include "BasicChannel.h"
#include "ofxAutoReloadedShader.h"

OFX_GPUMIXER_BEGIN_NAMESPACE


class ShaderChannel: public BasicChannel{
public:
    
    ofxAutoReloadedShader shader;

    
    
    ShaderChannel(){}
    
    
    void setup(string channelName, string shaderNameNPath, int width, int height){
        this->name = channelName;
        this->shader.load(shaderNameNPath);
        
        fbo.allocate(width, height, GL_RGBA32F_ARB);
        fbo.begin();
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        fbo.end();
        
        setupParameterGroup(name);
    }
    
    
    void update(){
        fbo.begin();
        {
                glClearColor(0.0, 0.0, 0.0, 0.0);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                shader.begin();
                {
                    shader.setUniform2f("iResolution", fbo.getWidth(), fbo.getHeight());
                    shader.setUniform1f("iGlobalTime", ofGetElapsedTimef() );//counter);
                    
                    ofSetColor(255,255,255);
                    ofFill();
                    ofDrawRectangle(0, 0, fbo.getWidth(), fbo.getHeight());
                }
                shader.end();
        }
        fbo.end();
    }
    
    void setupParameterGroup(string name){
        parameterGroup.setName(name);
        
    }
};


OFX_GPUMIXER_END_NAMESPACE
