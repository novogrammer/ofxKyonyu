#pragma once
/*
 *  ofxKyonyuMesh.h
 *  HelloGlut
 *
 *  Created by のぼ on 11/05/01.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#include <vector>
#include "ofxVec3f.h"
#include "ofxVec2f.h"

class ofxKyonyuMesh
{
protected:
	std::vector<int> m_IndexList;
	std::vector<ofxVec3f> m_VertexList;
	std::vector<ofxVec3f> m_NormalList;
    std::vector<ofxVec2f> m_TexCoordList;
public:
	ofxKyonyuMesh():m_IndexList(),m_VertexList()
	{
	}
	void draw()
	{
        m_NormalList.resize(m_VertexList.size(),ofxVec3f());
        m_TexCoordList.resize(m_VertexList.size(),ofxVec2f());//念のため
        
		for(size_t i=0;i<m_IndexList.size()/3;++i)
		{
            ofxVec3f a=m_VertexList[m_IndexList[i*3+1]]-m_VertexList[m_IndexList[i*3+0]];
            ofxVec3f b=m_VertexList[m_IndexList[i*3+2]]-m_VertexList[m_IndexList[i*3+0]];
            ofxVec3f n=(a.crossed(b)).normalized();
            for(size_t j=0;j<3;++j)
            {
                m_NormalList[m_IndexList[i*3+j]]+=n;
            }
        }
        
		for(size_t i=0;i<m_NormalList.size();++i)
        {
            m_NormalList[i].normalize();
        }
        
		glPolygonMode(GL_FRONT, GL_FILL);
		glBegin(GL_TRIANGLES);
		for(size_t i=0;i<m_IndexList.size()/3;++i)
		{
            glTexCoord2fv(&m_TexCoordList[m_IndexList[i*3+0]][0]);
            glNormal3fv(&m_NormalList[m_IndexList[i*3+0]][0]);
			glVertex3fv(&m_VertexList[m_IndexList[i*3+0]][0]);
            glTexCoord2fv(&m_TexCoordList[m_IndexList[i*3+1]][0]);
            glNormal3fv(&m_NormalList[m_IndexList[i*3+1]][0]);
			glVertex3fv(&m_VertexList[m_IndexList[i*3+1]][0]);
            glTexCoord2fv(&m_TexCoordList[m_IndexList[i*3+2]][0]);
            glNormal3fv(&m_NormalList[m_IndexList[i*3+2]][0]);
			glVertex3fv(&m_VertexList[m_IndexList[i*3+2]][0]);
		}
		glEnd();
	}
};
