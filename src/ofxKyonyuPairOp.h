#pragma once
//
//  ofxKyonyuPairOp.h
//  HelloOpenNI
//
//  Created by のぼ on 11/06/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#include<boost/array.hpp>
#include"ofxKyonyuOp.h"
class ofxKyonyuPairOp
{
    boost::array<ofxKyonyuOp,2> m_OpList;
    ofxVec3f m_OpOffsetSide;
    ofxVec3f m_OpOffsetFront;
    ofxMatrix4x4 m_OpRot;
    std::pair<GLuint,GLuint> m_Texture;//cache
public:
    ofxKyonyuPairOp()
    :m_OpOffsetSide(75,0,0)
    ,m_OpOffsetFront(0,100,50)
    ,m_OpRot(ofxMatrix4x4::newRotationMatrix(PI, 0, 1, 0))
    {
        
        
    }
    ~ofxKyonyuPairOp()
    {
        
    }
    std::pair<ofxVec3f,ofxVec3f> getBound(size_t inIndex)const;
    void update(float inDt);
	void setPinnedMatrix(ofxMatrix4x4 inPinnedMatrix);
    void draw();
    void setTexture(std::pair<GLuint,GLuint> inTexture)
    {
        m_Texture=inTexture;
    }
    void addTouching(const ofxVec3f& inPosition,float inRadius);
    void clearTouching();
    
    //valid after update
    bool isTouched()const;
    
private:
    ofxVec3f getOffset(size_t inIndex)const;
};