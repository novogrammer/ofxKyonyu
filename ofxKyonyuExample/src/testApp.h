#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxOpenNI.h"
#include "ofxKyonyuPairOp.h"
#include "ofSoundPlayer.h"

#include<map>
//#define ENABLE_DEBUG_RING

//#define FOVY 10
class testApp : public ofBaseApp{
	
public:
	void setup();
	void update();
	void draw();
	
	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	
private:
	ofxOpenNIContext	recordContext;
	ofxDepthGenerator	recordDepth;
	
#ifdef USE_IR
	ofxIRGenerator		recordImage;
#else
	ofxImageGenerator	recordImage; 
#endif
	
	ofxHandGenerator	recordHandTracker;
	
	ofxUserGenerator	recordUser;
#ifdef TARGET_OSX // only working on Mac at the moment
	ofxHardwareDriver	hardware;
#endif
    
	float m_PixelToXY;
	
	float m_FilterFactor;
	float m_Fovy;
	float m_RotY;
    ofxKyonyuPairOp m_OpList[MAX_NUMBER_USERS];
    
    std::map<XnUserID,std::pair<GLuint,GLuint> > m_OpTextureMap;
    ofSoundPlayer m_SoundPlayerTada;
    ofSoundPlayer m_SoundPlayerTouch;
    
	void initializeOpenNI();
	void finalizeOpenNI();
	void setupPerspective();
	void setupModelView();
	void drawPointCloud(ofxUserGenerator * user_generator, int userID);
	void drawUsers();
    bool getSkeltonMatrix(XnUserID inUser, XnSkeletonJoint inJoint,ofxMatrix4x4& inoutMatrix,float inConfidence);
    bool getSkeltonRotation(XnUserID inUser, XnSkeletonJoint inJoint,ofxMatrix4x4& inoutMatrix,float inConfidence);
    bool getSkeltonPosition(XnUserID inUser, XnSkeletonJoint inJoint,ofxVec3f& inoutPosition,float inConfidence);
    
    static void XN_CALLBACK_TYPE onCalibrationEnd(
                                                  xn::SkeletonCapability& rCapability
                                                  ,XnUserID nID
                                                  ,XnBool bSuccess
                                                  ,void* pCookie
                                                  );
    static void XN_CALLBACK_TYPE onLostUser(
                                            xn::UserGenerator& rGenerator
                                            ,XnUserID nID
                                            ,void* pCookie
                                            );
};
#endif
