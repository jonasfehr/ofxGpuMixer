#pragma once

OFX_GPUMIXER_BEGIN_NAMESPACE



class BasicChannel{
public:
    ofFbo fbo;
    ofParameterGroup parameterGroup;
    string name;
    
    BasicChannel(){};
    
    virtual void setup(){};
    
    virtual void update(){};
    
    void draw(){
        fbo.draw(0,0);
    }
    void draw(int x, int y){
        fbo.draw(x, y);
    }
    void draw(int x, int y, int w, int h){
        fbo.draw(x,y,w,h);
    }
    
    ofTexture* getTexturePtr(){ return &fbo.getTexture(); }
    ofTexture getTexture(){ return fbo.getTexture(); }

    
    ofFbo* getFboPtr(){ return &fbo; }
    
    virtual void setupParameterGroup(string name){}
    
    ofParameterGroup* getPointerToParameterGroup(){ return &parameterGroup; }
    
    string getName(){ return name; }
    
    
private:
    
};



OFX_GPUMIXER_END_NAMESPACE
