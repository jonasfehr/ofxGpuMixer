#pragma once

#define STRINGIFY(x) #x
#include "MixingShaderSnipets.h"
#include "SimpleColorChannel.h"
#include "Constants.h"

OFX_GPUMIXER_BEGIN_NAMESPACE


class TextureGroup{
public:
    
    ofParameter<float> hue{"Hue",0.5, 0, 1.};
    ofParameter<float> saturation{"Saturation",1, 0, 1.};
    ofParameter<float> brightness{"Brightness",1, 0, 1.};
    ofParameter<float> tintAmt{"TintAmt", 0, 0., 1};
    ofParameter<float> contrast{"Contrast", 1., 0., 2.};
    ofParameter<float> gain{"Gain", 1.0, 1., 5.};
    ofParameter<float> opacity{"Opacity", 1., 0., 1.};
    ofParameter<int> blendMode{"blendMode", 1, 1, 12};
    
    ofParameterGroup parametersTint{"Tint", hue, saturation, brightness, tintAmt};
    ofParameterGroup parameters{"Channel", parametersTint, contrast, gain, opacity, blendMode};
    
    string name;
    
    ofTexture texture;
    
    TextureGroup(string name, int blendMode, ofTexture texture){
        addParameters(name, blendMode);
        this->name = name;
        this->texture = texture;
    }
    
    void addParameters(string name, int initialBlendMode){
        parameters.setName(name);
        blendMode = initialBlendMode;
    }
    
};

class Mixer{
public:
    
    vector<BasicChannel*> channels;
    ofParameter<bool> doPreview{"doPreview", false};
    ofParameter<int> channelSelect{"channelSelect", 0, 0, 0};
    
    ofParameterGroup parameterPreview{"Preview", doPreview, channelSelect};
    
    
    void setup(){
        
        //ParameterGroup
        parameterGroup.clear();
        parameterGroup.setName("Mixer");
        
        channelSelect.setMax(texGroups.size()-1);
        parameterGroup.add(parameterPreview);
        
        for(int i = 0; i < texGroups.size(); i++){
            parameterGroup.add(texGroups[i].parameters);
        }
        
        generateShader();
        generateShaderSingleChannel();
    }
    
    void update(){
        for(auto t : texGroups){
            for( auto c : channels){
                if(t.name == c->name && t.opacity > 0) c->update();
            }
        }
    }
    
    void draw(int x, int y, int w, int h){
        ofPushMatrix();
        
        if(doPreview){
             
            shaderSingleChannel.begin();
            {
                
                shaderSingleChannel.setUniform2f("iResolution", w, h);
                shaderSingleChannel.setUniform1f("iGlobalTime", ofGetElapsedTimef()); //tempo p nr 1
                
                int i = channelSelect;
                shaderSingleChannel.setUniformTexture("tex0", texGroups[i].texture, i);
                shaderSingleChannel.setUniform1f("u_H_0", texGroups[i].hue);
                shaderSingleChannel.setUniform1f("u_S_0", texGroups[i].saturation);
                shaderSingleChannel.setUniform1f("u_B_0", texGroups[i].brightness);
                shaderSingleChannel.setUniform1f("u_tintAmt_0", texGroups[i].tintAmt);
                shaderSingleChannel.setUniform1f("u_contrast_0", texGroups[i].contrast);
                shaderSingleChannel.setUniform1f("u_gain_0", texGroups[i].gain);
                shaderSingleChannel.setUniform1f("u_opacity_0", texGroups[i].opacity);
                shaderSingleChannel.setUniform1i("u_blendMode_0", 1);
                shaderSingleChannel.setUniform2f("resolution_0", texGroups[i].texture.getWidth(), texGroups[i].texture.getHeight());
                
                
                ofSetColor(255,255,255);
                ofFill();
                ofDrawRectangle(0, 0, w, h);
            }
            shaderSingleChannel.end();
            
        }
        else
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
                    shader.setUniform1f("u_tintAmt_"+ofToString(i), texGroups[i].tintAmt);
                    shader.setUniform1f("u_contrast_"+ofToString(i), texGroups[i].contrast);
                    shader.setUniform1f("u_gain_"+ofToString(i), texGroups[i].gain);
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
    }
    
    
    void addChannel(ofTexture texture, string name, int blendMode){
        TextureGroup texGroup = TextureGroup(name, blendMode, texture);
        texGroups.push_back( texGroup );
    }
    
    /*    void addChannel(ShaderChannel & channel, int blendMode){
     addChannel(channel.getFbo(), channel.getName(), blendMode);
     channels.push_back(&channel);
     }
     */
    void addChannel(SimpleColorChannel &  channel, int blendMode){
        addChannel(channel.getFbo(), channel.getName(), blendMode);
        channels.push_back(&channel);
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
    ofShader shaderSingleChannel;
    
    ofParameterGroup parameterGroup;
    
    void generateShader(){
        // GENERATE THE SHADER
        stringstream shaderScript;
        shaderScript << "#version 120" << endl;
        shaderScript << uniformsHeader;
        for(int i = 0; i < texGroups.size(); i++){
            string snipet = uniforms;
            ofStringReplace(snipet, "$0", ofToString(i));
            shaderScript << snipet;
        }
        shaderScript << functions;
        shaderScript << mainHeader;
        for(int i = 0; i < texGroups.size(); i++){
            string snipet = channel;
            ofStringReplace(snipet, "$0", ofToString(i));
            shaderScript << snipet;
        }
        shaderScript << output;
        
        shader.setupShaderFromSource(GL_FRAGMENT_SHADER, shaderScript.str());
        shader.linkProgram();
    }
    
    void generateShaderSingleChannel(){
        // GENERATE THE SHADER
        stringstream shaderScript;
        shaderScript << "#version 120" << endl;
        shaderScript << uniformsHeader;
        string snipet = uniforms;
        ofStringReplace(snipet, "$0", "0");
        shaderScript << snipet;
        shaderScript << functions;
        shaderScript << mainHeader;
        snipet = channel;
        ofStringReplace(snipet, "$0", "0");
        shaderScript << snipet;
        shaderScript << output;
        
        shaderSingleChannel.setupShaderFromSource(GL_FRAGMENT_SHADER, shaderScript.str());
        shaderSingleChannel.linkProgram();
    }
};


OFX_GPUMIXER_END_NAMESPACE
