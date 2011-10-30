#include "testApp.h"
#include "ofxMatrix4x4.h"
#include "ofxMatrix3x3.h"

#define KANSETSU (150)

//--------------------------------------------------------------
void testApp::setup(){
	ofSetFrameRate(30);
	initializeOpenNI();
    m_SoundPlayerTada.loadSound("sounds/tada.wav");
    m_SoundPlayerTada.setMultiPlay(true);
    m_SoundPlayerTouch.loadSound("sounds/tada.wav");
    m_SoundPlayerTouch.setMultiPlay(true);
}

//--------------------------------------------------------------
void testApp::update(){
	// update all nodes
	recordContext.update();
	recordDepth.update();
	recordImage.update();
	
	// update tracking/recording nodes
	recordUser.update();
	
    for(size_t j=0;j<MAX_NUMBER_USERS;++j)
    {
        m_OpList[j].clearTouching();
    }
	xn::UserGenerator& ug=recordUser.getXnUserGenerator();
	for (int i=0; i<recordUser.getNumberOfTrackedUsers(); ++i)
    {
		if(ofxTrackedUser* user=recordUser.getTrackedUser(i+1))
		{
            for(size_t s=0;s<4;++s)
            {
                XnSkeletonJoint skel=(XnSkeletonJoint)0;
                switch(s)
                {
                    case 0:
                        skel=XN_SKEL_RIGHT_HAND;
                        break;
                    case 1:
                        skel=XN_SKEL_LEFT_HAND;
                        break;
                    case 2:
                        skel=XN_SKEL_LEFT_ELBOW;
                        break;
                    case 3:
                        skel=XN_SKEL_RIGHT_ELBOW;
                        break;
                }
                ofxVec3f pos;
                if(getSkeltonPosition(user->id,skel,pos,0.3f))
                {
                    for(size_t j=0;j<MAX_NUMBER_USERS;++j)
                    {
                        //他人のもタッチ
                        m_OpList[j].addTouching(pos,KANSETSU);
                    }
                }
            }
        }
    }
	for (int i=0; i<recordUser.getNumberOfTrackedUsers(); ++i)
    {
		if(ofxTrackedUser* user=recordUser.getTrackedUser(i+1))
		{
            ofxMatrix4x4 m;
            if(getSkeltonMatrix(user->id,XN_SKEL_TORSO,m,0.3f))
            {
                m_OpList[i].setPinnedMatrix(m);
                m_OpList[i].update(1/30.0f);
                if(m_OpList[i].isTouched())
                {
                    //m_SoundPlayerTouch.play();
                }
            }
            
        }
    }
    
    
	
}

//--------------------------------------------------------------
void testApp::draw(){
	const GLfloat lightPos[] = { 0 , -3 , 3 , 0 };
    
	glLightfv(GL_LIGHT0 , GL_POSITION , lightPos);
	glEnable(GL_LIGHT0);
	
	setupPerspective();
	setupModelView();
	
	glEnable(GL_DEPTH_TEST);
	
	glClearColor(0x00,0x00,0x00,0x00);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ofSetColor(0xff,0xff,0xff);
    
	glPushMatrix();
	glRotatef(180,0,0,1);
	float t=tan(m_Fovy*.5*DEG_TO_RAD);
	int w=ofGetWidth();
	int h=ofGetHeight();
	glTranslatef(w*-.5, h*-.5, h*.5/t);
	
	
	glPopMatrix();
    {
        glPushMatrix();
        glScalef(-1, 1, 1);//mirror
        drawPointCloud(&recordUser, 0);	// 0 gives you all point clouds; use userID to see point clouds for specific users
        drawUsers();
        glPushMatrix();
        glTranslatef(100, 100, 500);
        //glutSolidSphere(100,12,6);
        glPopMatrix();
        
        glPushMatrix();
        glTranslatef(500, 500, 1000);
        //	glutSolidSphere(100,12,6);
        glPopMatrix();
        glPopMatrix();
    }
	
	glDisable(GL_DEPTH_TEST);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	switch (key) {
#ifdef TARGET_OSX // only working on Mac at the moment
		case 357: // up key
			hardware.setTiltAngle(hardware.tilt_angle++);
			break;
		case 359: // down key
			hardware.setTiltAngle(hardware.tilt_angle--);
			break;
#endif
        case 'r':
            m_RotY=0;
            break;
	}
	
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    
    m_RotY=((x/(float)ofGetWidth())-0.5)*2*PI*-1;
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
    
}
void testApp::initializeOpenNI()
{
	m_FilterFactor=0.3;
	m_RotY=0;
#ifdef TARGET_OSX // only working on Mac at the moment
	hardware.setup();				// libusb direct control of motor, LED and accelerometers
	hardware.setLedOption(LED_OFF); // turn off the led just for yacks (or for live installation/performances ;-)
#endif
	
	recordContext.setup();	// all nodes created by code -> NOT using the xml config file at all
	recordDepth.setup(&recordContext);
	recordImage.setup(&recordContext);
	
	recordUser.setup(&recordContext);
	recordUser.setSmoothing(m_FilterFactor);				// built in openni skeleton smoothing...
	recordUser.setUseMaskPixels(false);
	recordUser.setUseCloudPoints(true);
	recordUser.setMaxNumberOfUsers(MAX_NUMBER_USERS);					// use this to set dynamic max number of users (NB: that a hard upper limit is defined by MAX_NUMBER_USERS in ofxUserGenerator)
	
	recordHandTracker.setup(&recordContext, 4);
	recordHandTracker.setSmoothing(m_FilterFactor);		// built in openni hand track smoothing...
	recordHandTracker.setFilterFactors(m_FilterFactor);	// custom smoothing/filtering (can also set per hand with setFilterFactor)...set them all to 0.1f to begin with
	
	recordContext.toggleRegisterViewport();
	//recordContext.toggleMirror();
    
    
	XnUInt64 foc_len=0;
	XnDouble pixel_size=0;
	recordDepth.getXnDepthGenerator().GetIntProperty("ZPD", foc_len);
	recordDepth.getXnDepthGenerator().GetRealProperty("ZPPS", pixel_size);
	m_Fovy=atan((pixel_size*recordDepth.getHeight()/2*2)/foc_len)/DEG_TO_RAD*2;
	
	m_PixelToXY=pixel_size/foc_len*2;//二倍する必要あり？
	
    
	xn::UserGenerator& ug=recordUser.getXnUserGenerator();
    
	XnCallbackHandle calibration_cb_handle;
	ug.GetSkeletonCap().RegisterCalibrationCallbacks(
                                                     NULL,&testApp::onCalibrationEnd,this,calibration_cb_handle);
    
    
	// register user callbacks
	XnCallbackHandle user_cb_handle;
	ug.RegisterUserCallbacks(NULL,&testApp::onLostUser,this,user_cb_handle);
    
    
}
void testApp::finalizeOpenNI()
{
}

void testApp::setupPerspective()
{
	GLint matrixMode=0;
	glGetIntegerv(GL_MATRIX_MODE, &matrixMode);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//TODO:定数化
	gluPerspective(m_Fovy, ofGetWidth()/(float)ofGetHeight(),100, 10000);//cameraFovy[deg] 100[mm]to10000[mm]
	
	glMatrixMode(matrixMode);
}

void testApp::setupModelView()
{
	GLint matrixMode=0;
	glGetIntegerv(GL_MATRIX_MODE, &matrixMode);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//TODO:定数化
	ofxVec3f eye(0,0,0);
	ofxVec3f center(0,0,1500);//1.5[m]
	ofxVec3f up(0,1,0);
	ofxMatrix4x4 rot=ofxMatrix4x4::newRotationMatrix(m_RotY,0,1,0);
	eye=(rot*(eye-center))+center;
	gluLookAt(
			  eye.x, eye.y, eye.z,
			  center.x,center.y, center.z,
			  up.x, up.y, up.z);
	//	glTranslatef(0, 0, ofGetHeight()*0.5/t);
	
	glMatrixMode(matrixMode);
}


void testApp::drawPointCloud(ofxUserGenerator * user_generator, int userID) {
	
	glPushMatrix();
	
	int w = user_generator->getWidth();
	int h = user_generator->getHeight();
	
	static const int step = 2;
	std::vector<ofPoint> points;
    points.reserve(h*w);
	std::vector<ofColor> colors;
	colors.reserve(h*w);
	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
			ofPoint pos = user_generator->getWorldCoordinateAt(x, y, userID);
			colors.push_back(user_generator->getWorldColorAt(x,y, userID));
            if(pos.z>9000 || pos.z==0)
            {
                pos.z=9000;//9[m]まで
            }
			points.push_back(ofPoint(-(pos.x-w/2)*m_PixelToXY*pos.z,-(pos.y-h/2)*m_PixelToXY*pos.z,pos.z));
		}
	}
	glBegin(GL_TRIANGLES);
	static const float s=0.5*step;
	for(size_t i=0;i<points.size();++i)
	{
		ofPoint &point=points[i];
		float sz=1*m_PixelToXY*point.z*s;
		ofPoint p1=point+ofPoint(-sz,-sz,0);
		ofPoint p2=point+ofPoint(+sz,-sz,0);
		ofPoint p3=point+ofPoint(+sz,+sz,0);
		ofPoint p4=point+ofPoint(-sz,+sz,0);
		ofColor &color=colors[i];
        glColor4ub((unsigned char)color.r, (unsigned char)color.g, (unsigned char)color.b, (unsigned char)color.a);
        
		glVertex3fv(&p1[0]);
		glVertex3fv(&p2[0]);
		glVertex3fv(&p3[0]);
		
		glVertex3fv(&p3[0]);
		glVertex3fv(&p4[0]);
		glVertex3fv(&p1[0]);
	}
	glEnd();
	
	glEnable(GL_LIGHTING);
    {
        float color[]={0.75,0.75,0.75,1.0};
        glMaterialfv(GL_FRONT, GL_DIFFUSE,color);
    }
    {
        float color[]={0.5,0.5,0.5,1.0};
        glMaterialfv(GL_FRONT, GL_AMBIENT,color);
    }
#ifdef ENABLE_DEBUG_RING
	for(int i=0;i<2;++i)
	{
		glPushMatrix();
		glTranslatef(0, -100+200*i, 1500);
		glRotatef(90, 1, 0, 0);
		glutSolidTorus(10, 200, 20, 10);
		glPopMatrix();
	}
#endif
	
	glDisable(GL_LIGHTING);
	
	glColor3f(1.0f, 1.0f, 1.0f);
	
	glPopMatrix();
}
bool testApp::getSkeltonPosition(XnUserID inUser, XnSkeletonJoint inJoint,ofxVec3f& inoutPosition,float inConfidence)
{
    bool isOk=false;
	xn::UserGenerator& ug=recordUser.getXnUserGenerator();
    XnSkeletonJointPosition sjp;
    ug.GetSkeletonCap().GetSkeletonJointPosition(inUser, inJoint,sjp);
    if(sjp.fConfidence >= inConfidence)
    {
        XnVector3D& p=sjp.position;
        inoutPosition=ofxVec3f(-p.X,p.Y,p.Z);//DirectX to OpenGL?
        isOk=true;
    }
    return isOk;
}
bool testApp::getSkeltonRotation(XnUserID inUser, XnSkeletonJoint inJoint,ofxMatrix4x4& inoutMatrix,float inConfidence)
{
    bool isOk=false;
	xn::UserGenerator& ug=recordUser.getXnUserGenerator();
    XnSkeletonJointOrientation sjo;
    ug.GetSkeletonCap().GetSkeletonJointOrientation(inUser, inJoint, sjo);
    if(sjo.fConfidence>=inConfidence)
    {
        
        XnMatrix3X3& r=sjo.orientation;
        ofxMatrix4x4 mr;
        for(int x=0;x<3;++x)
        {
            for(int y=0;y<3;++y)
            {
                mr._mat[y][x]=r.elements[x*3+y];
            }
        }
        mr.scale(-1, 1, 1);//DirectX to OpenGL?
        inoutMatrix=mr;
        
        isOk=true;
    }
    return isOk;
}

bool testApp::getSkeltonMatrix(XnUserID inUser, XnSkeletonJoint inJoint,ofxMatrix4x4& inoutMatrix,float inConfidence)
{
    bool isOk=false;
    ofxVec3f position;
    ofxMatrix4x4 rot;
    if(getSkeltonPosition(inUser, inJoint, position, inConfidence) && getSkeltonRotation(inUser, inJoint, rot, inConfidence))
    {
        
        ofxMatrix4x4 mt=ofxMatrix4x4::newTranslationMatrix(position);
        ofxMatrix4x4 mr=rot;
        inoutMatrix=mr*mt;
        
        isOk=true;
    }
    return isOk;
}

void testApp::drawUsers()
{
	glEnable(GL_LIGHTING);
    {
        float color[]={1.0,1.0,1.0,1.0};
        glMaterialfv(GL_FRONT, GL_DIFFUSE,color);
    }
    {
        float color[]={0.90,0.90,0.90,1.0};
        glMaterialfv(GL_FRONT, GL_AMBIENT,color);
    }
	
	xn::UserGenerator& ug=recordUser.getXnUserGenerator();
	for (int i=0; i<recordUser.getNumberOfTrackedUsers(); ++i) {
		if(ofxTrackedUser* user=recordUser.getTrackedUser(i+1))
		{
#if 0
			for(int j=1;j<=24;++j)
			{
                ofxVec3f v;
                if(getSkeltonPosition(user->id,(XnSkeletonJoint) j,v,0.3f))
				{
                    
					
					glPushMatrix();
					glTranslatef(v.x,v.y,v.z);
					glutSolidSphere(KANSETSU/2, 12, 6);
					glPopMatrix();
				}
			}
#endif
            ofxMatrix4x4 m;
            std::map<XnUserID,std::pair<GLuint,GLuint> >::iterator it=m_OpTextureMap.find(user->id);
            if(getSkeltonMatrix(user->id,XN_SKEL_TORSO,m,0.3f) && m.getTranslation().z>100.0f)
            {
                
                //テクスチャがない。撮影する。
                if(it==m_OpTextureMap.end())
                {
                    std::pair<GLuint,GLuint> tp;
                    for(size_t j=0;j<2;++j)
                    {
                        GLuint t=0;
                        glGenTextures(1,&t);
                        glBindTexture(GL_TEXTURE_2D,t);
                        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
                        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                        
                        
#define W 256
#define H 256
                        std::vector<unsigned char> imageData(W*H*3,0);
                        
                        //        xn::UserGenerator& ug=This->recordUser.getXnUserGenerator();
                        
                        int w = recordUser.getWidth();
                        int h = recordUser.getHeight();
                        ofxVec2f pos1;
                        ofxVec2f pos2;
                        ofxVec3f to=m.getTranslation();
                        std::pair<ofxVec3f,ofxVec3f> bound=m_OpList[i].getBound(j);
                        {
                            ofxVec3f p=to+bound.first;
                            pos1=ofxVec2f(p.x/(m_PixelToXY*p.z)+w/2,p.y/(m_PixelToXY*p.z)+h/2);
                        }
                        {
                            ofxVec3f p=to+bound.second;
                            pos2=ofxVec2f(p.x/(m_PixelToXY*p.z)+w/2,p.y/(m_PixelToXY*p.z)+h/2);
                        }
                        
                        for(size_t x=0;x<W;++x)
                        {
                            for(size_t y=0;y<H;++y)
                            {
                                ofxVec2f pos=pos1;
                                pos.x+=(pos2.x-pos1.x)*x/(float)W;
                                pos.y+=(pos2.y-pos1.y)*y/(float)H;
                                pos.x=w-pos.x;//反転？
                                pos.y=h-pos.y;
                                ofColor col;
                                if(
                                   0<=pos.x&& pos.x<w &&
                                   0<=pos.y&& pos.y<h
                                   )
                                {
                                    col= recordUser.getWorldColorAt(pos.x,pos.y, 0);   
                                }
                                else
                                {
                                    col.r=0xff;
                                    col.g=0xff;
                                    col.b=0xff;
                                }
                                imageData[(x+y*W)*3+0]=col.r;
                                imageData[(x+y*W)*3+1]=col.g;
                                imageData[(x+y*W)*3+2]=col.b;
                            }
                        }
                        
                        glTexImage2D(GL_TEXTURE_2D, // テキスチャの種類。
                                     0, // level: 解像度が単一の場合は0
                                     GL_RGB, // どの成分が画像のテクセルに使用されているか
                                     W, H, // サイズ
                                     0, // border: 境界の幅
                                     GL_RGB, // 描画フォーマット
                                     GL_UNSIGNED_BYTE, // データの型
                                     &imageData[0] // 実際のデータ
                                     );
                        if(j==0)
                        {
                            tp.first=t;
                        }
                        else
                        {
                            tp.second=t;
                        }
                    }
                    
                    std::pair<std::map<XnUserID,std::pair<GLuint,GLuint> >::iterator,bool> result=m_OpTextureMap.insert(std::make_pair(user->id,tp));
                    if(result.second)
                    {
                        it=result.first;
                    }
                    m_SoundPlayerTada.play();//tada
                }
                
                
                
                glEnable(GL_TEXTURE_2D);
                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                if(it!=m_OpTextureMap.end())
                {
                    m_OpList[i].setTexture(it->second);
                }
                else
                {
                    m_OpList[i].setTexture(std::make_pair<GLuint,GLuint>(0,0));
                }
                m_OpList[i].draw();
                
                glBindTexture(GL_TEXTURE_2D, 0);
                glDisable(GL_TEXTURE_2D);
                
                glPushMatrix();
				glMultMatrixf(&m._mat[0][0]);
                
#ifdef ENABLE_DEBUG_RING
				glRotatef(90, 1, 0, 0);
				glutSolidTorus(10, 300, 20, 10);
#endif
                glPopMatrix();
            }
		}
	}
	glDisable(GL_LIGHTING);
}


void XN_CALLBACK_TYPE testApp::onCalibrationEnd(
                                                xn::SkeletonCapability& rCapability
                                                ,XnUserID nID
                                                ,XnBool bSuccess
                                                ,void* pCookie
                                                )
{
    
    
}

void XN_CALLBACK_TYPE testApp::onLostUser(
                                          xn::UserGenerator& rGenerator
                                          ,XnUserID nID
                                          ,void* pCookie
                                          )
{
    if(testApp* This=static_cast<testApp*>(pCookie))
    {
        std::map<XnUserID,std::pair<GLuint,GLuint> >::iterator it=This->m_OpTextureMap.find(nID);
        if(it!=This->m_OpTextureMap.end())
        {
            glDeleteTextures(1,&(it->second.first));
            glDeleteTextures(1,&(it->second.second));
            This->m_OpTextureMap.erase(it);
        }
    }
}


