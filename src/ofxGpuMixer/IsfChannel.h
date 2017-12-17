#pragma once

#include "BasicChannel.h"

OFX_GPUMIXER_BEGIN_NAMESPACE


class IsfChannel: public BasicChannel{
public:
    
    ofShader shader;
    
    vector<ofParameter<float>> floatParams;
    vector<ofParameter<int>> intParams;
    vector<string> textureNames;
    
    IsfChannel(){}
    
    
    void setup(string channelName, string IsfNameNPath, int width, int height){
        this->name = channelName;
        this->shader.setupShaderFromSource(GL_FRAGMENT_SHADER, readIsfFormattedFile(IsfNameNPath));
        this->shader.linkProgram();
        
        
        fbo.allocate(width, height, GL_RGBA32F_ARB);
        fbo.begin();
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        fbo.end();
        
        
        
        setupParameterGroup(name);
    }
    
    void setup(string channelName, string IsfNameNPath, int width, int height, MappingImage &mappingImg){
        this->name = channelName;
        this->shader.setupShaderFromSource(GL_FRAGMENT_SHADER, readIsfFormattedFile(IsfNameNPath));
        this->shader.linkProgram();
        
        this->mappingImg = mappingImg;
        hasMappingImg = true;

        
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
                    shader.setUniform1i("PASSINDEX", 0 );
                    shader.setUniform2f("RENDERSIZE", fbo.getWidth(), fbo.getHeight());
                    shader.setUniform1f("TIME", ofGetElapsedTimef() );
                    shader.setUniform1f("TIMEDELTA", ofGetElapsedTimef()); //  NEED DELTA TIME
                    shader.setUniform4f("DATE", ofGetYear(), ofGetMonth(), ofGetDay(), ofGetHours()*3600+ofGetMinutes()*60+ofGetSeconds());
                    shader.setUniform1i("FRAMEINDEX", ofGetFrameNum() );

                    
                    
                    if(hasMappingImg)
                        for(auto tn : textureNames){
                            shader.setUniformTexture(tn, mappingImg.getTexture(), 0);
                        }
                            
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
    
    void setupParameterGroup(string name){
        parameterGroup.setName(name);

    }
    
    std::string readIsfFormattedFile(string isfNameNPath){
    
        ofBuffer buffer = ofBufferFromFile(isfNameNPath+".fs");
        
        string content = buffer.getText();
        std::pair <std::string,std::string> jsonNShader = splitJsonShader(content);
        
        // extract parameters
        ofJson controls = nlohmann::json::parse(jsonNShader.first);

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
        for(auto & p : intParams){
            parameterGroup.add(p);
        }
        for(auto & p : floatParams){
            parameterGroup.add(p);
        }


        
        ofShader shader;
        
        // GENERATE THE SHADER
        stringstream shaderScript;
        //shaderScript << "#version 120" << endl;
        // create uniforms;
        
        for(auto & c : controls["INPUTS"]){
            string name = c["NAME"];
            name = name.substr(0, name.length()); // remove " in front and back

            if(c["TYPE"]=="int"){
                shaderScript << "uniform int "<< name << ";" << endl;
            }
            else if(c["TYPE"]=="float"){
                shaderScript << "uniform float "<< name << ";" << endl;
            }
            else if(c["TYPE"]=="image"){
                shaderScript << "uniform sampler2DRect "<< name << ";" << endl;
                textureNames.push_back(c["NAME"]);
            }
            
        }

        
        for(auto & p : intParams){
        }
        for(auto & p : floatParams){
        }
        
        shaderScript << "uniform int PASSINDEX;"<< endl;
        shaderScript << "uniform vec2 RENDERSIZE;"<< endl;
        shaderScript << "uniform float TIME;"<< endl;
        shaderScript << "uniform float TIMEDELTA;"<< endl;
        shaderScript << "uniform vec4 DATE;"<< endl;
        shaderScript << "uniform int FRAMEINDEX;"<< endl;
        shaderScript << "vec2 isf_FragNormCoord;"<< endl;

        // replace the ISF specific function names
        ofStringReplace(jsonNShader.second, "IMG_PIXEL(", "texture2DRect(");
        ofStringReplace(jsonNShader.second, "IMG_NORM_PIXEL(", "texture2D(");
        
        // add ISF functions to shader
        ofStringReplace(jsonNShader.second, "main(){", "main(){  vec2 isf_FragNormCoord = gl_FragCoord.xy/RENDERSIZE;");

        
        shaderScript << jsonNShader.second;
        
        
        shader.setupShaderFromSource(GL_FRAGMENT_SHADER, shaderScript.str());

        
        return shaderScript.str();
        
    }

    
private:
    bool hasMappingImg = false;
    
    pair<string, string> splitJsonShader(string & val) {
        string sJson;
        string sShader;
        
        
        string::size_type pos = val.find("*/");
        if(val.npos != pos) {
            sShader = val.substr(pos+2);
            sJson = val.substr(2, pos-2);
        }
        return make_pair(sJson, sShader);
    }
    

};


OFX_GPUMIXER_END_NAMESPACE
