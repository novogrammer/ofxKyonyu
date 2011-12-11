#pragma once
/*
 *  ofxKyonyuPoint.h
 *  HelloGlut
 *
 *  Created by のぼ on 11/05/01.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#include<memory>
#include"ofxVec3f.h"
#include"ofxVec2f.h"

static const float GRAVITY_A = 9.8 * 1000;//[mm/(s*s)]
static const float AIR_FRICTION = 0.005;//[N/(mm/s)]

struct ofxKyonyuPoint
{
	ofxVec3f m_Position;//[mm]
	ofxVec3f m_Velocity;//[mm/s]
	ofxVec3f m_Force;//[mm/(s*s)]
    ofxVec2f m_TexCoord;//uv
	float m_Mass;//[kg];
    bool m_IsPinned;
    bool m_IsDragging;
	
	ofxKyonyuPoint()
	:m_Position()
	,m_Velocity()
	,m_Force()
	,m_Mass(6.0/1000)
	,m_IsPinned(false)
	,m_IsDragging(false)
	{
	}
    void updateForce()
    {
		m_Force+=m_Velocity*(AIR_FRICTION*-1);
    }
    void updatePosition(float inDt);
    
    //オイラー法で暴走したので、ルンゲクッタ法という積分法を使う
    //http://www6.ocn.ne.jp/~simuphys/runge-kutta.html
    inline void rungeKutta(ofxVec3f inA,ofxVec3f inV,ofxVec3f inX,float inDt,ofxVec3f& outK,ofxVec3f& outL)
    {
        ofxVec3f x1 = inV*inDt;
        ofxVec3f v1 = inA*inDt;
        ofxVec3f x2 = (inV+(v1*0.5) )*inDt;
        ofxVec3f v2 = inA*(inDt * 0.5);//あってる？　Aを求めなおす必要がある？
        ofxVec3f x3 = (inV+v2*0.5)*inDt;
        ofxVec3f v3 = inA*(inDt * 0.5);//あってる？　Aを求めなおす必要がある？
        ofxVec3f x4 = (inV+v3)*inDt;
        ofxVec3f v4 = inA*inDt;//あってる？　Aを求めなおす必要がある？
        outK=(x1+(x2*2)+(x3*2)+x4)/6;
        outL=(v1+(v2*2)+(v3*2)+v4)/6;
    }
	
};
