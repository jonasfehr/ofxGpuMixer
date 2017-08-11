#pragma once

#include "BasicChannel.h"
#include "ofxAutoReloadedShader.h"

OFX_GPUMIXER_BEGIN_NAMESPACE


class ShaderChannel: public BasicChannel{
public:
    
    ofxAutoReloadedShader shader;
    
    vector<ofParameter<float>> floatParams;
    vector<ofParameter<int>> intParams;
    
    ShaderChannel(){}
    
    
    void setup(string channelName, string shaderNameNPath, int width, int height){
        this->name = channelName;
        this->shader.load(shaderNameNPath);
        
        fbo.allocate(width, height, GL_RGBA32F_ARB);
        fbo.begin();
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        fbo.end();
        
        
        
        setupParameterGroup(name, shaderNameNPath);
    }
    
    void setup(string channelName, string shaderNameNPath, int width, int height, MappingImage &mappingImg){
        this->name = channelName;
        this->shader.load(shaderNameNPath);
        this->mappingImg = mappingImg;
        hasMappingImg = true;

        
        fbo.allocate(width, height, GL_RGBA32F_ARB);
        fbo.begin();
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        fbo.end();
        
        setupParameterGroup(name, shaderNameNPath);
    }
    
    
    void update(){
        fbo.begin();
        {
                glClearColor(0.0, 0.0, 0.0, 0.0);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                shader.begin();
                {
                    shader.setUniform2f("iResolution", fbo.getWidth(), fbo.getHeight());
                    shader.setUniform1f("iGlobalTime", ofGetElapsedTimef() );
                    if(hasMappingImg) shader.setUniformTexture("tex0", mappingImg.getTexture(), 0);
                    for(auto & p : intParams){
                        shader.setUniform1i(p.getName(), (float)p.get() );
                    }
                    for(auto & p : floatParams){
                        shader.setUniform1f(p.getName(), (float)p.get() );
                    }
                    
                    ofSetColor(255,255,255);
                    ofFill();
                    ofDrawRectangle(0, 0, fbo.getWidth(), fbo.getHeight());
                }
                shader.end();
        }
        fbo.end();
    }
    
    void setupParameterGroup(string name, string shaderNameNPath){

        ofBuffer buffer = ofBufferFromFile(shaderNameNPath+".frag");
        vector<string> justJson = ofSplitString(buffer.getText(), "*");
        ofJson controls = nlohmann::json::parse(justJson[1]);

        for(auto & c : controls["INPUTS"]){
            if(c["TYPE"]=="int"){
                ofParameter<int> newParam;
                newParam.set(c["NAME"], c["DEFAULT"], c["MIN"], c["MAX"]);
                intParams.push_back(newParam);
            }
            if(c["TYPE"]=="float"){
                ofParameter<float> newParam;
                newParam.set(c["NAME"], c["DEFAULT"], c["MIN"], c["MAX"]);
                floatParams.push_back(newParam);
            }
            
        }
        
        parameterGroup.setName(name);
        for(auto & p : intParams){
            parameterGroup.add(p);
        }
        for(auto & p : floatParams){
            parameterGroup.add(p);
        }
        
    }
    
private:
    bool hasMappingImg = false;
};


OFX_GPUMIXER_END_NAMESPACE
