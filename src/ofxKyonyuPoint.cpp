//
//  ofxKyonyuPoint.cpp
//  HelloOpenNI
//
//  Created by のぼ on 11/06/12.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "ofxKyonyuPoint.h"

void ofxKyonyuPoint::updatePosition(float inDt)
{
    if (m_IsDragging || m_IsPinned)
    {
        m_Velocity=ofxVec3f();
    }
    else
    {
        ofxVec3f a = m_Force/m_Mass;
        a.y -= GRAVITY_A;
        if (0)
        {
            m_Velocity+=a*inDt;
            m_Position+=m_Velocity*inDt;
        }
        else
        {
            ofxVec3f k;
            ofxVec3f l;
            rungeKutta(a, m_Velocity, m_Position,inDt, k, l);
            m_Position+=k;
            m_Velocity+=l;
        }
    }
    m_Force=ofxVec3f();
}
