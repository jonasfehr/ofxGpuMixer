#pragma once

#include "BasicChannel.h"
#include "Constants.h"
#include "ofxAutoReloadedShader.h"

OFX_GPUMIXER_BEGIN_NAMESPACE


class ShaderChannel: public BasicChannel{
public:
    
    
    ofxAutoReloadedShader shader;
    
    vector<ofParameter<float>> floatParams;
    vector<ofParameter<int>> intParams;
    ShaderTypes shaderType = SHADER_SHADERTOY;
    vector<string> textureNames;
    ofTexture mappingTex;
    
    ofParameter<float> increment;
    double timer = 0.0;
    
    ShaderChannel(){}
    
    
    void setup(string channelName, string shaderNameNPath, int width, int height, ShaderTypes shaderType){
        this->name = channelName;
        this->shaderType = shaderType;
        
        switch(shaderType){
            case SHADER_SHADERTOY:
                this->shader.load(shaderNameNPath);
                
                break;
            case SHADER_ISF:
                this->shader.setupShaderFromSource(GL_FRAGMENT_SHADER, readIsfFormattedFile(shaderNameNPath));
                this->shader.linkProgram();
                
                break;
            case SHADER_MADMATERIAL:
                this->shader.setupShaderFromSource(GL_FRAGMENT_SHADER, readIsfFormattedFile(shaderNameNPath));
                this->shader.linkProgram();
                break;
        }
        
        fbo.allocate(width, height, GL_RGBA32F_ARB);
        fbo.begin();
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        fbo.end();
        
        
        timer = 0;
    }
    

    void setup(string channelName, string shaderNameNPath, int width, int height, ofTexture &mappingTex, ShaderTypes shaderType){
        this->name = channelName;
        this->shaderType = shaderType;
        this->mappingTex = mappingTex;
        
        switch(shaderType){
            case SHADER_SHADERTOY:
                this->shader.load(shaderNameNPath);
                
                break;
            case SHADER_ISF:
                this->shader.setupShaderFromSource(GL_FRAGMENT_SHADER, readIsfFormattedFile(shaderNameNPath));
                this->shader.linkProgram();
                
                break;
            case SHADER_MADMATERIAL:
                this->shader.setupShaderFromSource(GL_FRAGMENT_SHADER, readIsfFormattedFile(shaderNameNPath));
                this->shader.linkProgram();
                break;
        }
        
        
        fbo.allocate(width, height, GL_RGBA32F_ARB);
        fbo.begin();
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        fbo.end();
        
        //        setupParameterGroup(name, shaderNameNPath);
        timer = 0;
    }
    
    
    
    void update(){
        timer += increment/60; // /ofGetFrameRate();
       
        fbo.begin();
        {
            glClearColor(0.0, 0.0, 0.0, 0.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shader.begin();
            {
                switch(this->shaderType){
                    case SHADER_SHADERTOY:
                        shader.setUniform2f("iResolution", fbo.getWidth(), fbo.getHeight());
                        shader.setUniform1f("iGlobalTime", ofGetElapsedTimef() );
                        if(mappingTex.isAllocated())shader.setUniformTexture("tex0", mappingTex, 0);
                        break;
                        
                        
                    case SHADER_ISF:
                        shader.setUniform1i("PASSINDEX", 0 );
                        shader.setUniform2f("RENDERSIZE", fbo.getWidth(), fbo.getHeight());
                        shader.setUniform1f("TIME", timer ); //ofGetElapsedTimef()); //
                        shader.setUniform1f("TIMEDELTA", 1./ofGetFrameRate()); //  NEED DELTA TIME
                        shader.setUniform4f("DATE", ofGetYear(), ofGetMonth(), ofGetDay(), ofGetHours()*3600+ofGetMinutes()*60+ofGetSeconds());
                        shader.setUniform1i("FRAMEINDEX", ofGetFrameNum() );
                        for(auto tn : textureNames){
                            shader.setUniformTexture(tn, mappingTex, 0);
                        }
                        break;
                        
                    case SHADER_MADMATERIAL:
                        shader.setUniform1i("PASSINDEX", 0 );
                        shader.setUniform2f("RENDERSIZE", fbo.getWidth(), fbo.getHeight());
                        shader.setUniform1f("TIME", ofGetElapsedTimef() );
                        shader.setUniform1f("TIMEDELTA", ofGetElapsedTimef()); //  NEED DELTA TIME
                        shader.setUniform4f("DATE", ofGetYear(), ofGetMonth(), ofGetDay(), ofGetHours()*3600+ofGetMinutes()*60+ofGetSeconds());
                        shader.setUniform1i("FRAMEINDEX", ofGetFrameNum() );
                        for(auto tn : textureNames){
                            shader.setUniformTexture(tn, mappingTex, 0);
                        }
                        break;

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
    
    
private:
    
//    void setupParameterGroup(string name, ofJson controls){
//        
//        ofBuffer buffer = ofBufferFromFile(shaderNameNPath+".frag");
//        vector<string> justJson = ofSplitString(buffer.getText(), "*");
//        ofJson controls = nlohmann::json::parse(justJson[1]);
//        
//        for(auto & c : controls["INPUTS"]){
//            if(c["TYPE"]=="int"){
//                ofParameter<int> newParam;
//                newParam.set(c["NAME"], c["DEFAULT"], c["MIN"], c["MAX"]);
//                intParams.push_back(newParam);
//            }
//            if(c["TYPE"]=="float"){
//                ofParameter<float> newParam;
//                newParam.set(c["NAME"], c["DEFAULT"], c["MIN"], c["MAX"]);
//                floatParams.push_back(newParam);
//            }
//            
//        }
//        
//        parameterGroup.setName(name);
//        for(auto & p : intParams){
//            parameterGroup.add(p);
//        }
//        for(auto & p : floatParams){
//            parameterGroup.add(p);
//        }
//        
//    }
    
    
    std::string readIsfFormattedFile(string isfNameNPath){
        
        ofBuffer buffer = ofBufferFromFile(isfNameNPath+".fs");
        
        string content = buffer.getText();
        std::pair <std::string,std::string> jsonNShader = splitJsonShader(content);
        
        // extract and create parameters
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
        
        parameterGroup.setName(this->name);
        increment.set("increment", 1., 0., 2.);
        parameterGroup.add(increment);

        
        for(auto & p : intParams){
            parameterGroup.add(p);
        }
        for(auto & p : floatParams){
            parameterGroup.add(p);
        }
        
        
        // GENERATE THE SHADER
        stringstream shaderScript;
        
        // add uniforms form ISF header
        for(auto & c : controls["INPUTS"]){
            string cName = c["NAME"];
            
            if(c["TYPE"]=="int"){
                shaderScript << "uniform int "<< cName << ";" << endl;
            }
            else if(c["TYPE"]=="float"){
                shaderScript << "uniform float "<< cName << ";" << endl;
            }
            else if(c["TYPE"]=="image"){
                shaderScript << "uniform sampler2DRect "<< cName << ";" << endl;
                textureNames.push_back(c["NAME"]);
            }
            
        }
        
        if(this->shaderType == SHADER_ISF || this->shaderType == SHADER_MADMATERIAL){
            // add standart ISF uniforms
            shaderScript << "uniform int PASSINDEX;"<< endl;
            shaderScript << "uniform vec2 RENDERSIZE;"<< endl;
            shaderScript << "uniform float TIME;"<< endl;
            shaderScript << "uniform float TIMEDELTA;"<< endl;
            shaderScript << "uniform vec4 DATE;"<< endl;
            shaderScript << "uniform int FRAMEINDEX;"<< endl;
            shaderScript << "vec2 isf_FragNormCoord;"<< endl;
            
            if(this->shaderType == SHADER_ISF){
                // replace the ISF specific function names
                ofStringReplace(jsonNShader.second, "IMG_PIXEL(", "texture2DRect(");
                ofStringReplace(jsonNShader.second, "IMG_NORM_PIXEL(", "texture2D(");
                
                // add ISF functions to shader
                ofStringReplace(jsonNShader.second, "main(){", "main(){  vec2 isf_FragNormCoord = gl_FragCoord.xy/RENDERSIZE;");
                
                shaderScript << jsonNShader.second;
                
                
            }else if(this->shaderType == SHADER_MADMATERIAL ){
                // TODO: Create something like madmappers timing generators
                
                shaderScript << "float animation_time;"<< endl;
                shaderScript << jsonNShader.second;
                shaderScript << "void main(){" << endl;
                shaderScript << "vec2 isf_FragNormCoord = gl_FragCoord.xy/RENDERSIZE;" << endl;
                shaderScript << "animation_time = TIME*speed;" << endl;
                shaderScript << "gl_FragColor = materialColorForPixel(isf_FragNormCoord);" << endl;
                shaderScript << "}" << endl;
                
                ofStringReplace(jsonNShader.second, "main(){", "main(){  vec2 isf_FragNormCoord = gl_FragCoord.xy/RENDERSIZE;");
            }
        }
        
        switch(this->shaderType){
            case SHADER_SHADERTOY:
                
                break;
            case SHADER_ISF:
                
                break;
            case SHADER_MADMATERIAL:
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
                
                break;
        }

        return shaderScript.str();
    }
    
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

