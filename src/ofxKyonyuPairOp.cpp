//
//  ofxKyonyuPairOp.cpp
//  HelloOpenNI
//
//  Created by のぼ on 11/06/12.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "ofxKyonyuPairOp.h"
void ofxKyonyuPairOp::update(float inDt)
{
    m_OpList[0].update(inDt);
    m_OpList[1].update(inDt);
}

ofxVec3f ofxKyonyuPairOp::getOffset(size_t inIndex)const
{
    if(inIndex==0)
    {
        return m_OpOffsetFront+m_OpOffsetSide*-1;
    }
    else
    {
        return m_OpOffsetFront+m_OpOffsetSide*+1;
    }
}

std::pair<ofxVec3f,ofxVec3f> ofxKyonyuPairOp::getBound(size_t inIndex)const
{
    std::pair<ofxVec3f,ofxVec3f> bound=m_OpList[inIndex].getBound();
    return std::make_pair(bound.first+getOffset(inIndex),bound.second+getOffset(inIndex));
}


void ofxKyonyuPairOp::setPinnedMatrix(ofxMatrix4x4 inPinnedMatrix)
{
    m_OpList[0].setPinnedMatrix(ofxMatrix4x4::newTranslationMatrix(getOffset(0))*m_OpRot*inPinnedMatrix);
    m_OpList[1].setPinnedMatrix(ofxMatrix4x4::newTranslationMatrix(getOffset(1))*m_OpRot*inPinnedMatrix);
}
void ofxKyonyuPairOp::draw()
{
    glBindTexture(GL_TEXTURE_2D, m_Texture.first);
    m_OpList[0].draw();
    glBindTexture(GL_TEXTURE_2D, m_Texture.second);
    m_OpList[1].draw();
    
}

void ofxKyonyuPairOp::addTouching(const ofxVec3f& inPosition,float inRadius)
{
    m_OpList[0].addTouching(inPosition,inRadius);
    m_OpList[1].addTouching(inPosition,inRadius);
}
void ofxKyonyuPairOp::clearTouching()
{
    m_OpList[0].clearTouching();
    m_OpList[1].clearTouching();
}
bool ofxKyonyuPairOp::isTouched()const
{
    return m_OpList[0].isTouched() || m_OpList[1].isTouched();
}



