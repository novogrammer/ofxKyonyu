//
//  ofxKyonyuOp.cpp
//  HelloOpenNI
//
//  Created by のぼ on 11/06/12.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "ofxKyonyuOp.h"

const float ofxKyonyuOp::RADIUS = 90;//[mm]
const float ofxKyonyuOp::WARP_LENGTH = 100;//[mm]


void ofxKyonyuOp::update(float inDt)
{
    static const int times = 20; 
    volatile size_t jointCount=m_JointList.size();
    volatile size_t pointCount=m_PointList.size();
    for (int j = 0; j < times;++j)
    {
        //touch対応
        for(size_t i=0;i<m_TouchingList.size();++i)
        {
            touch(m_TouchingList[i].first, m_TouchingList[i].second);
            
        }
        //フェーズを二つに分ける。力更新と位置更新
        // update force
        for(size_t i=0;i<jointCount;++i) {
            m_JointList[i].updateForce();
        }
        for(size_t i=0;i<pointCount;++i) {
            m_PointList[i].updateForce();
        }
        // update position
        for(size_t i=0;i<pointCount;++i) {
            m_PointList[i].updatePosition(inDt/times);
        }
        
    }
    
    //drawLine();
    transfer();
}


void ofxKyonyuOp::putPointAndJoint()
{
    //Xを経度
    //Yを緯度
    float diffRotX = DEG_TO_RAD*360.0 / COLS;
    float diffRotY = DEG_TO_RAD*90.0 / (ROWS-1);
    
    m_PointList.resize(0);
    m_JointList.resize(0);
    m_IndexList.resize(0);
    
    m_PointList.resize(getOpCount(),ofxKyonyuPoint());
    m_VertexList.resize(getOpCount());
    m_IndexList.reserve(getOpCount()*3);//だいたい予約
    
    
    //ポイントとジョイントを一気に配置したほうが楽なので
    for(int y=0;y<ROWS;++y)
    {
        for(int x=0;x<COLS;++x)
        {
            ofxKyonyuPoint& point=m_PointList[getOpIndex(y,x)];
            //point.name = String(y) + "-" + String(x) + " ";//デバッグ用
            //var noize:Number = Math.random()*10;//0から1
            //よこ
            //var tmp:MyVector3D = new MyVector3D(0,Math.cos(diffRotX*x),Math.sin(diffRotX*x));
            //point._position = new MyVector3D(Math.sin(diffRotY * y) * RADIUS, Math.cos(diffRotY*y)*tmp.y*RADIUS, Math.cos(diffRotY*y)*tmp.z*RADIUS);
            ofxVec3f tmp = ofxVec3f(sinf(diffRotX*x),cosf(diffRotX*x),0);
            point.m_Position = ofxVec3f(
                                         cos(diffRotY * y) * tmp.x * RADIUS,
                                         cos(diffRotY * y) * tmp.y * RADIUS,
                                         sin(diffRotY * y) * RADIUS
                                         );
            if (y == ROWS - 1)
            {
                point.m_Position.z *= 1.1;
            }
            
            
            //面
            if (y > 0)
            {
                m_IndexList.push_back(getOpIndex(y - 1,x - 1));
                m_IndexList.push_back(getOpIndex(y - 1,x + 0));
                m_IndexList.push_back(getOpIndex(y + 0,x - 1));
                
                if (y < ROWS - 1)
                {
                    m_IndexList.push_back(getOpIndex(y - 1, x + 0));
                    m_IndexList.push_back(getOpIndex(y + 0, x + 0));
                    m_IndexList.push_back(getOpIndex(y + 0, x - 1));
                }
                
            }
            
            if(y>0)
            {
                ofxKyonyuPoint& pointUp = m_PointList[getOpIndex(y - 1, x)];
                m_JointList.push_back(ofxKyonyuJoint(&point, &pointUp));//たて
                
                if (y < ROWS - 1)
                {
                    ofxKyonyuPoint& pointNaname1 = m_PointList[getOpIndex(y - 1, x - 1)];
                    ofxKyonyuJoint jointNaname1 = ofxKyonyuJoint(&point, &pointNaname1);
                    jointNaname1.m_Spring *= 0.75;
                    m_JointList.push_back(jointNaname1);//ななめ1（せん断抵抗）
                    
                    ofxKyonyuPoint& pointNaname2 = m_PointList[getOpIndex(y - 1, x + 1)];
                    ofxKyonyuJoint jointNaname2 = ofxKyonyuJoint(&point, &pointNaname2);
                    jointNaname2.m_Spring *= 0.75;
                    m_JointList.push_back(jointNaname2);//ななめ2（せん断抵抗）
                }
            }
            if (y < ROWS - 1)
            {
                ofxKyonyuPoint& pointLeft = m_PointList[getOpIndex(y, x - 1)];
                m_JointList.push_back(ofxKyonyuJoint(&point,&pointLeft));//よこ
            }
            if(y>1)
            {
                ofxKyonyuPoint& pointUp2 = m_PointList[getOpIndex(y - 2, x)];
                ofxKyonyuJoint jointUp2 = ofxKyonyuJoint(&point, &pointUp2);
                if (y < ROWS -1)
                {
                    jointUp2.m_Spring *= 1.0;
                }
                else
                {//極部分
                    jointUp2.m_Spring *= 2.0;
                    jointUp2.m_Damper *= 2.0;
                }
                m_JointList.push_back(jointUp2);//たて　ひとつ飛ばし（角度抵抗）
            }
            if (y < ROWS - 1)
            {
                ofxKyonyuPoint& pointLeft2 = m_PointList[getOpIndex(y,x - 2)];
                ofxKyonyuJoint jointLeft2 = ofxKyonyuJoint(&point, &pointLeft2);
                jointLeft2.m_Spring *= 1.0;
                m_JointList.push_back(jointLeft2);//よこ　ひとつ飛ばし（角度抵抗）
                
                //            var pointDiagonal:Point = _points[getOpIndex(y,x+_cols/2)];
                //            var jointDiagonal:Joint = new Joint(point, pointDiagonal);
                //            jointDiagonal.SPRING *= 0.5;
                //            _joints.push(jointDiagonal);//よこ　対角線
            }
            
        }
        //極部分はバネが集中するので重くする
        ofxKyonyuPoint& pointPole = m_PointList[m_PointList.size() - 2];
        pointPole.m_Mass *= 1.5;
        
        //中心部分を設定
        ofxKyonyuPoint& pointCenter = m_PointList[m_PointList.size() - 1];
        pointCenter.m_Position = ofxVec3f(0,0,RADIUS/4*-1);
        pointCenter.m_IsPinned = true;
        for (int volIndex = 0; volIndex < m_PointList.size() -1;++volIndex)
        {//圧力
            ofxKyonyuJoint jointVol = ofxKyonyuJoint(&pointCenter, &m_PointList[volIndex]);
            if (volIndex < m_PointList.size() - 2)
            {
                jointVol.m_Spring *= 0.5;
            }
            else
            {//極部分
                jointVol.m_Spring *= 2.0;
            }
            m_JointList.push_back(jointVol);
        }
        
        for(size_t i=0;i<m_JointList.size();++i)
        {
            ofxKyonyuJoint &joint=m_JointList[i];
            joint.resetNaturalLength();
        }
        
        
        //根元を固定します。
        for (int i = 0; i < COLS;++i)
        {
            m_PointList[i].m_IsPinned = true;
            m_PointList[i+COLS].m_IsPinned = true;
        }
    }
    
    //テクスチャ座標
    for(size_t i=0;i<m_PointList.size();++i)
    {
        m_PointList[i].m_TexCoord.x=m_PointList[i].m_Position.x/(RADIUS*2)+0.5;
        m_PointList[i].m_TexCoord.y=m_PointList[i].m_Position.y/(RADIUS*2)+0.5;
    }
    //trace("_poinits.length"+_points.length);
    //trace("_joints.length"+_joints.length);
    //_points.fixed = true;
    //_joints.fixed = true;
    //_faces.fixed = true;
}



void ofxKyonyuOp::setPinnedMatrix(ofxMatrix4x4 inPinnedMatrix)
{
    ofxMatrix4x4 diff= ofxMatrix4x4::getInverseOf(m_PinnedMatrix)*inPinnedMatrix;
    m_PinnedMatrix=inPinnedMatrix;
    
    if(diff.getTranslation().length()>WARP_LENGTH)
    {
        putPointAndJoint();//リセット
        for(size_t i=0;i<m_PointList.size();++i)
        {
            ofxKyonyuPoint& p=m_PointList[i];
            p.m_Position=p.m_Position*m_PinnedMatrix;//今の姿勢へ
        }
    }
    else
    {
		
        for(size_t i=0;i<m_PointList.size();++i)
        {
            ofxKyonyuPoint& p=m_PointList[i];
            if((p.m_IsPinned || p.m_IsDragging))
            {
                p.m_Position=p.m_Position*diff;
            }
        }
    }
}

std::pair<ofxVec3f,ofxVec3f> ofxKyonyuOp::getBound()const
{
    return std::make_pair(ofxVec3f(-RADIUS,-RADIUS,0),ofxVec3f(+RADIUS,+RADIUS,0));
}

void ofxKyonyuOp::touch(const ofxVec3f& inPosition,float inRadius)
{
    float closest=1000;
    for(size_t i=0;i<m_PointList.size();++i)
    {
        ofxKyonyuPoint& p=m_PointList[i];
        if(!(p.m_IsPinned || p.m_IsDragging))
        {
            closest=MAX(closest,(p.m_Position-inPosition).length());
            if((p.m_Position-inPosition).length()<inRadius)
            {
#if 0
                p.m_Position=(p.m_Position-inPosition).getNormalized()*inRadius+inPosition;
                p.m_Velocity=ofxVec3f();//タッチは速度なし
#else
                ofxVec3f f=(p.m_Position-inPosition).getNormalized()*(inRadius-(p.m_Position-inPosition).length())*10;
//                ofxVec3f f=(p->m_Position-inPosition).getNormalized()*300;
                static const float maxForce=250;
                if(f.length()>maxForce)
                {
                    f=f.getNormalized()*maxForce;
                }
                p.m_Force+=f;
#endif
                m_IsTouched=true;
            }
        }
    }
}

void ofxKyonyuOp::addTouching(const ofxVec3f& inPosition,float inRadius)
{
    m_TouchingList.push_back(std::make_pair(inPosition,inRadius));
}




