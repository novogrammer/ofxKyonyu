#pragma once
/*
 *  ofxKyonyuJoint.h
 *  HelloGlut
 *
 *  Created by のぼ on 11/05/01.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#include "ofxKyonyuPoint.h"
#include<cassert>
struct ofxKyonyuJoint
{
	ofxKyonyuPoint* m_Point;
	ofxKyonyuPoint* m_Target;
    float m_Spring;//[N/mm]
    float m_Damper;//[N/(mm/s)]
    float m_NaturalLength;
	ofxKyonyuJoint(ofxKyonyuPoint* inPoint=NULL,ofxKyonyuPoint* inTarget=NULL)
	:m_Point(inPoint)
	,m_Target(inTarget)
	,m_Spring(10.0f)
	,m_Damper(0.01f)
	,m_NaturalLength(0.0f)
	{
        assert(inPoint&&inTarget);//EXPECT NOT NULL
		resetNaturalLength();
	}
	
    void resetNaturalLength()
    {
        m_NaturalLength = m_Point->m_Position.distance(m_Target->m_Position);
        //trace( _point.name+ " " +_target.name +" length:"+ _naturalLength);
    }
    void updateForce()
    {
        //バネの力
		ofxVec3f dx=(m_Target->m_Position)-(m_Point->m_Position);
		ofxVec3f nx=dx.getNormalized();//単位ベクトル
		
        ofxVec3f springForce = nx*((dx.length() - m_NaturalLength) * m_Spring);
        
        //ダンパの力
        ofxVec3f dv = (m_Target->m_Velocity)-(m_Point->m_Velocity);
        ofxVec3f damperForce = dv*m_Damper;
        
        //合力
        ofxVec3f totalForce = springForce+damperForce;
		m_Point->m_Force+=totalForce;
        //逆の力をかける
		m_Target->m_Force+=totalForce*-1;
        
    }
	
};