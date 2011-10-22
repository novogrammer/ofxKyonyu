#pragma once
/*
 *  ofxKyonyuOp.h
 *  HelloGlut
 *
 *  Created by のぼ on 11/05/01.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#include"ofxKyonyuMesh.h"
#include"ofxKyonyuJoint.h"
#include"ofxMatrix4x4.h"
#include<cmath>//TODO: MoveTo ofxKyonyuOp.cpp

class ofxKyonyuOp:public ofxKyonyuMesh
{
public:
	static const int COLS=12;
	static const int ROWS=6;
    static const float RADIUS;// = 90;//[mm]
    static const float WARP_LENGTH = 100;//[mm]
private:
	std::vector<ofxKyonyuJoint> m_JointList;
	std::vector<ofxKyonyuPointPointer> m_PointList;
	ofxMatrix4x4 m_PinnedMatrix;
    std::vector<std::pair<ofxVec3f,float> > m_TouchingList;
    bool m_IsTouched;
    
	inline size_t getOpCount()
	{
        return COLS * (ROWS - 1) + 1+1;//周囲＋極＋中心
	}
    inline size_t getOpIndex(int inY,int inX)
    {
        if (inY >= ROWS-1)
        {
            return inY * COLS;
        }
        else
        {
            return inY * COLS + (COLS + inX) % COLS;
        }
    }
    void putPointAndJoint();
	inline void transfer()
	{
		m_VertexList.resize(m_PointList.size());
        m_TexCoordList.resize(m_PointList.size());
		for(size_t i=0;i<m_PointList.size();++i)
		{
			m_VertexList[i]=m_PointList[i]->m_Position;
			m_TexCoordList[i]=m_PointList[i]->m_TexCoord;
		}
	}
public:
	ofxKyonyuOp()
	:ofxKyonyuMesh()
	,m_JointList()
	,m_PointList()
	,m_PinnedMatrix()
    ,m_IsTouched(false)
	{
		putPointAndJoint();
//		for(size_t i=0;i<m_PointList.size();++i)
//		{
//			m_PointList[i]+=inTranslate;
//		}
	}
    void update(float inDt);
	void setPinnedMatrix(ofxMatrix4x4 inPinnedMatrix);
    std::pair<ofxVec3f,ofxVec3f> getBound()const;
    void touch(const ofxVec3f& inPosition,float inRadius);
    void addTouching(const ofxVec3f& inPosition,float inRadius);
    void clearTouching()
    {
        m_TouchingList.clear();
        m_IsTouched=false;
    }
    //valid after update
    bool isTouched()const
    {
        return m_IsTouched;
    }
	
};



