#pragma once

#define STRINGIFY(x) #x
#include "MixingShaderSnipets.h"
#include "ShaderChannel.h"
#include "Constants.h"

OFX_GPUMIXER_BEGIN_NAMESPACE


class TextureGroup{
public:
    ofParameterGroup parameters;
    ofParameter<float> hue;
    ofParameter<float> saturation;
    ofParameter<float> brightness;
    ofParameter<float> contrast;
    ofParameter<float> opacity;
    ofParameter<int> blendMode;
    
    ofTexture texture;
    
    TextureGroup(string name, int blendMode, ofTexture texture){
        addParameters(name, blendMode);
        this->texture = texture;
    }
    
    void addParameters(string name, int initialBlendMode){
        parameters.setName(name);
        parameters.add(hue.set("hue", ofRandom(1.), 0., 1.));
        parameters.add(saturation.set("saturation", 1., 0., 1.));
        parameters.add(brightness.set("brightness", 1., 0., 1.));
        parameters.add(contrast.set("contrast", .5, 0., 1.));
        parameters.add(opacity.set("opacity", 1., 0., 1.));
        parameters.add(blendMode.set("blendMode", initialBlendMode, 1, 10));
    }
    
};

class Mixer{
public:
    
    void draw(int x, int y, int w, int h){
        ofPushMatrix();
        {
           // ofTranslate(x,y);
            shader.begin();
            {
                
                shader.setUniform2f("iResolution", w, h);
                shader.setUniform1f("iGlobalTime", ofGetElapsedTimef()); //tempo p nr 1
                
                for(int i = 0; i < texGroups.size(); i++){
                    shader.setUniformTexture("tex"+ofToString(i), texGroups[i].texture, i);
                    shader.setUniform1f("u_H_"+ofToString(i), texGroups[i].hue);
                    shader.setUniform1f("u_S_"+ofToString(i), texGroups[i].saturation);
                    shader.setUniform1f("u_B_"+ofToString(i), texGroups[i].brightness);
                    shader.setUniform1f("u_contrast_"+ofToString(i), texGroups[i].contrast);
                    shader.setUniform1f("u_opacity_"+ofToString(i), texGroups[i].opacity);
                    shader.setUniform1i("u_blendMode_"+ofToString(i), texGroups[i].blendMode);
                    shader.setUniform2f("resolution_"+ofToString(i), texGroups[i].texture.getWidth(), texGroups[i].texture.getHeight());
                    
                }
                
                ofSetColor(255,255,255);
                ofFill();
                ofDrawRectangle(0, 0, w, h);
            }
            shader.end();
        }
        ofPopMatrix();
        
    }
    
    void addChannel(ofFbo& fbo, string name, int blendMode){
        TextureGroup texGroup = TextureGroup(name, blendMode, fbo.getTexture());
        texGroups.push_back( texGroup );
        setup();
    }
    
    void addChannel(ShaderChannel channel, int blendMode){
        addChannel(channel.getFbo(), channel.getName(), blendMode);
    }
    
    void addChannel(ofTexture texture, string name, int blendMode){
        TextureGroup texGroup = TextureGroup(name, blendMode, texture);
        texGroups.push_back( texGroup );
        setup();
    }
    
    
    ofParameterGroup& getParameterGroup(){ return parameterGroup; }
    
    vector<ofParameterGroup*> getVectorOfParameterSubgroups(){
        
        vector<ofParameterGroup*> paramSubGroups;
        
        for(int i = 0; i < texGroups.size(); i++){
            paramSubGroups.push_back(&texGroups[i].parameters);
        }
        return paramSubGroups;
    }
    
private:
    vector <TextureGroup> texGroups;
    ofShader shader;
    
    ofParameterGroup parameterGroup;
    
    void setup(){
        
        //ParameterGroup
        parameterGroup.clear();
        parameterGroup.setName("Mixer");
        for(int i = 0; i < texGroups.size(); i++){
            parameterGroup.add(texGroups[i].parameters);
        }
        
        // GENERATE THE SHADER
        stringstream shaderScript;
        shaderScript << "#version 120" << endl;
        shaderScript << uniformsHeader;
        for(int i = 0; i < texGroups.size(); i++){
            shaderScript << replaceAll(uniforms, "$0", ofToString(i));
        }
        shaderScript << functions;
        shaderScript << mainHeader;
        for(int i = 0; i < texGroups.size(); i++){
            shaderScript << replaceAll(channel, "$0", ofToString(i));
        }
        shaderScript << output;
        
        shader.setupShaderFromSource(GL_FRAGMENT_SHADER, shaderScript.str());
        shader.linkProgram();
    }
    
    // function from http://stackoverflow.com/questions/3418231/replace-part-of-a-string-with-another-string
    string replaceAll(std::string str, const std::string& from, const std::string& to) {
        if(from.empty())
            return "";
        size_t start_pos = 0;
        while((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
        }
        return str;
    }
};


OFX_GPUMIXER_END_NAMESPACE
