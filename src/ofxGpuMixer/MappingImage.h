//
//  MappingImage.h
//  texturedlight
//
//  Created by Jonas on 20/07/2017.
//
//

#ifndef MappingImage_h
#define MappingImage_h

#include "ofMain.h"
#include <math.h>

struct StripLinear{
    ofVec3f startPosWorld;
    ofVec3f endPosWorld;
    ofVec2f startPosOut;
    int numLeds;
};

struct Globe{
    ofVec3f center;
    float diameter;
    ofVec2f centerOnScreen;
    float diaOnScreen;
};

class MappingImage : public ofShortImage {
public:
    
    void setup( vector<StripLinear> linearStrips, int width, int height ){
        vector<StripLinear> linearStripsNorm;
        
        linearStripsNorm = linearStrips;
        // normalize positions
        ofVec3f minPos = ofVec3f(1.);
        ofVec3f maxPos = ofVec3f(0);
        for( auto & s : linearStripsNorm){
            if(s.startPosWorld.x < minPos.x) minPos.x = s.startPosWorld.x;
            if(s.startPosWorld.y < minPos.y) minPos.y = s.startPosWorld.y;
            if(s.startPosWorld.z < minPos.z) minPos.z = s.startPosWorld.z;
            if(s.endPosWorld.x   < minPos.x) minPos.x = s.endPosWorld.x;
            if(s.endPosWorld.y   < minPos.y) minPos.y = s.endPosWorld.y;
            if(s.endPosWorld.z   < minPos.z) minPos.z = s.endPosWorld.z;
            
            if(s.startPosWorld.x > maxPos.x) maxPos.x = s.startPosWorld.x;
            if(s.startPosWorld.y > maxPos.y) maxPos.y = s.startPosWorld.y;
            if(s.startPosWorld.z > maxPos.z) maxPos.z = s.startPosWorld.z;
            if(s.endPosWorld.x   > maxPos.x) maxPos.x = s.endPosWorld.x;
            if(s.endPosWorld.y   > maxPos.y) maxPos.y = s.endPosWorld.y;
            if(s.endPosWorld.z   > maxPos.z) maxPos.z = s.endPosWorld.z;
        }
        
        for( auto & s : linearStripsNorm){
            s.startPosWorld.x = ofMap(s.startPosWorld.x, minPos.x, maxPos.x, 0., 1.);
            s.startPosWorld.y = ofMap(s.startPosWorld.y, minPos.y, maxPos.y, 0., 1.);
            s.startPosWorld.z = ofMap(s.startPosWorld.z, minPos.z, maxPos.z, 0., 1.);
            s.endPosWorld.x   = ofMap(s.endPosWorld.x,   minPos.x, maxPos.x, 0., 1.);
            s.endPosWorld.y   = ofMap(s.endPosWorld.y,   minPos.y, maxPos.y, 0., 1.);
            s.endPosWorld.z   = ofMap(s.endPosWorld.z,   minPos.z, maxPos.z, 0., 1.);
        }
        
        ofFloatPixels mappingPixels;
        mappingPixels.allocate(width, height, 4);
        mappingPixels.setColor(ofFloatColor(0.,0.,0., 0.)); // Set all to zero
        
        for( auto & s : linearStripsNorm){
            for(int i = 0; i < s.numLeds; i++){
                float pctOnStrip = (float)i/s.numLeds;
                ofVec3f posWorld = s.endPosWorld.getInterpolated(s.startPosWorld, pctOnStrip);
                
                mappingPixels.setColor(s.startPosOut.x,s.startPosOut.y-i,ofFloatColor(posWorld.x, posWorld.y, posWorld.z, pctOnStrip));
            }

        }
        
        this->setFromPixels(mappingPixels);
    }
    
    void setup( vector<Globe> spheres, int width, int height ){
        
        ofFloatPixels mappingPixels;
        mappingPixels.allocate(width, height, 4);
        mappingPixels.setColor(ofFloatColor(0.,0.,0., 0.)); // Set all to zero
        
//        ofVec3f minPos = ofVec3f(1.);
//        ofVec3f maxPos = ofVec3f(0);
//        for( auto & s : spheres){
//            if(s.center.x-s.diameter/2 < minPos.x) minPos.x = s.center.x-s.diameter/2;
//            if(s.center.y-s.diameter/2 < minPos.y) minPos.y = s.center.y-s.diameter/2;
//            if(s.center.z-s.diameter/2 < minPos.z) minPos.z = s.center.z-s.diameter/2;
//            
//            if(s.center.x+s.diameter/2 > maxPos.x) maxPos.x = s.center.x+s.diameter/2;
//            if(s.center.y+s.diameter/2 > maxPos.y) maxPos.y = s.center.y+s.diameter/2;
//            if(s.center.z+s.diameter/2 > maxPos.z) maxPos.z = s.center.z+s.diameter/2;
//        }
        float minPos = 1.;
        float maxPos = 0.;
        for( auto & s : spheres){
            if(s.center.x-s.diameter/2 < minPos) minPos = s.center.x-s.diameter/2;
            if(s.center.y-s.diameter/2 < minPos) minPos = s.center.y-s.diameter/2;
            if(s.center.z-s.diameter/2 < minPos) minPos = s.center.z-s.diameter/2;
            
            if(s.center.x+s.diameter/2 > maxPos) maxPos = s.center.x+s.diameter/2;
            if(s.center.y+s.diameter/2 > maxPos) maxPos = s.center.y+s.diameter/2;
            if(s.center.z+s.diameter/2 > maxPos) maxPos = s.center.z+s.diameter/2;
        }
        

        
        for( auto & s : spheres){
            for(int x =  0; x < s.diaOnScreen; x++){
                for(int y =  0; y < s.diaOnScreen; y++){
                    
                    ofVec2f pixelCoords = ofVec2f(x,y)-ofVec2f(s.diaOnScreen/2)+s.centerOnScreen;
                    ofVec3f sphereUV = sphereCoords(ofVec2f(x,y).operator/(s.diaOnScreen), s.diameter);
                    
                    float r = ofMap(sphereUV.x+s.center.x, minPos, maxPos, 0., 1.);
                    float g = ofMap(sphereUV.y+s.center.y, minPos, maxPos, 0., 1.);
                    float b = ofMap(sphereUV.z+s.center.z, minPos, maxPos, 0., 1.);
                    
                    mappingPixels.setColor(pixelCoords.x,pixelCoords.y,ofFloatColor(r, g, b, 1.0));
                }
            }
            
        }

        
        
        
        
        
        this->setFromPixels(mappingPixels);
    }
    
private:
    glm::vec3 sphereCoords(ofVec2f _st, float _scale){

        float maxFactor = sin(1.570796327);
        glm::vec3 uv = glm::vec3(0.0);
        glm::vec2 xy = 2.0 * _st - 1.0;
        float d = length(xy);
        if (d < (2.0-maxFactor)){
            d = length(xy * maxFactor);
            float z = sqrt(1.0 - d * d);
            float r = atan2(d, z) / 3.1415926535 * _scale;
            float phi = atan2(xy.y, xy.x);
            
            uv.z = r * z;
            uv.x = r * cos(phi) + 0.5;
            uv.y = r * sin(phi) + 0.5;
        } else {
            uv = glm::vec3(0.0);//_st.xy;
        }
        return uv;
    }
    
};

#endif /* MappingImage_h */
