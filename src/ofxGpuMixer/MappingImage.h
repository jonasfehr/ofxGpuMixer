//
//  MappingImage.h
//  texturedlight
//
//  Created by Jonas on 20/07/2017.
//
//

#ifndef MappingImage_h
#define MappingImage_h

struct StripLinear{
    ofVec3f startPosWorld;
    ofVec3f endPosWorld;
    ofVec2f startPosOut;
    int numLeds;
};

class MappingImage : public ofImage {
public:
    
    void setup( vector<StripLinear> linearStrips, int width, int height ){
        
        this->linearStripsNorm = linearStrips;
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
    
private:
    vector<StripLinear> linearStripsNorm;

    
};

#endif /* MappingImage_h */
