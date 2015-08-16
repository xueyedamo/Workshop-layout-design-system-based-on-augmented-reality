/*******************************************************
  Function : // 立体显示类，用于Oculus Rift头盔显示器（HMD）显示立体场景图像
  CopyRight: // 版权公开(Public)
  Author   : // 张帆 Eli zhang
  ================= Modified list ====================  
  Version    : // v1.2.0
  Modify Date: // 2015.02.20
  Mender     : // zhangxuanyu0904@163.com
  Why        : // 立体显示类，生成具有立体偏差的两个图像
  How        : // 进行分屏两次渲染
  Attention  : // 注意事项
*******************************************************/
#include "StdAfx.h"
#include "OHMDStereoRender.h"

#include <iostream>
#include <cassert>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace std;

static const char* vertexShader =
"void main() {"
"  gl_Position = vec4(gl_Vertex.xy, 0.0, 1.0);"
"  gl_TexCoord[0].st = gl_MultiTexCoord0;"
"}";

static const char *fragShader =
"uniform vec2 scale;"
"uniform vec2 scaleIn;"
"uniform vec2 lensCenter;"
"uniform vec4 hmdWarpParam;"
"uniform sampler2D texid;"
"void main()"
"{"
"  vec2 uv = (gl_TexCoord[0].st*2.0)-1.0;" // range from [0,1] to [-1,1]
"  vec2 theta = (uv-lensCenter)*scaleIn;"
"  float rSq = theta.x*theta.x + theta.y*theta.y;"
"  vec2 rvector = theta*(hmdWarpParam.x + hmdWarpParam.y*rSq + hmdWarpParam.z*rSq*rSq + hmdWarpParam.w*rSq*rSq*rSq);"
"  vec2 tc = (lensCenter + scale * rvector);"
"  tc = (tc+1.0)/2.0;" // range from [-1,1] to [0,1]
"  if (any(bvec2(clamp(tc, vec2(0.0,0.0), vec2(1.0,1.0))-tc)))"
"    gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);"
"  else"
"    gl_FragColor = texture2D(texid, tc);"
"}";

	/*******************************************************
	Function : OHMDStereoRender类构造函数
	Parameter: 
             1   In    irr::IrrlichtDevice *   device     引擎的设备指针
			 2   In    HMDDescriptor           HMD        头盔显示器的参数结构体  
			 3   In    irr::f32                worldScale 场景比率
	Return   : 无
	Used Var : 无
	Author   : 张帆 Eli zhang
	*******************************************************/
OHMDStereoRender::OHMDStereoRender(irr::IrrlichtDevice *device, HMDDescriptor HMD, f32 worldScale)
  : _worldScale(worldScale),
    _renderTexture(NULL)
{
  _smgr = device->getSceneManager();
  _driver = device->getVideoDriver();
  _timer = device->getTimer();

  // Create perspectiva camera used for rendering
  _pCamera = _smgr->addCameraSceneNode();

  // Init shader parameters
  _distortionCB.scale[0] = 1.0f; _distortionCB.scale[1] = 1.0f;
  _distortionCB.scaleIn[0] = 1.0f; _distortionCB.scaleIn[1] = 1.0f;
  _distortionCB.lensCenter[0] = 0.0f;_distortionCB.lensCenter[1] = 0.0f;
  _distortionCB.hmdWarpParam[0] = 1.0f;_distortionCB.hmdWarpParam[1] = 0.0f;_distortionCB.hmdWarpParam[2] = 0.0f;_distortionCB.hmdWarpParam[3] = 0.0f;
  
  // Plane
  _planeVertices[0] = video::S3DVertex(-1.0f, -1.0f, 1.0f,1,1,0, video::SColor(255,0,255,255), 0.0f, 0.0f);
  _planeVertices[1] = video::S3DVertex(-1.0f,  1.0f, 1.0f,1,1,0, video::SColor(255,0,255,255), 0.0f, 1.0f);
  _planeVertices[2] = video::S3DVertex( 1.0f,  1.0f, 1.0f,1,1,0, video::SColor(255,0,255,255), 1.0f, 1.0f);
  _planeVertices[3] = video::S3DVertex( 1.0f, -1.0f, 1.0f,1,1,0, video::SColor(255,0,255,255), 1.0f, 0.0f);
  _planeIndices[0] = 0; _planeIndices[1] = 1; _planeIndices[2] = 2; _planeIndices[3] = 0; _planeIndices[4] = 2; _planeIndices[5] = 3;

  // Create shader material
  _renderMaterial.Wireframe = false;
  _renderMaterial.Lighting = false;
  _renderMaterial.TextureLayer[0].TextureWrapU = ETC_CLAMP;
  _renderMaterial.TextureLayer[0].TextureWrapV = ETC_CLAMP;

  IGPUProgrammingServices* gpu = _driver->getGPUProgrammingServices(); 
  _renderMaterial.MaterialType=(E_MATERIAL_TYPE)gpu->addHighLevelShaderMaterial(vertexShader, "main", EVST_VS_3_0, fragShader, "main", EPST_PS_3_0, &_distortionCB);

  setHMD(HMD);
}

OHMDStereoRender::~OHMDStereoRender() {
}

  /*******************************************************
	Function : 设定头盔显示器的参数结构体
	Parameter:
			 1   In    HMDDescriptor   HMD   头盔显示器的参数结构体  
	Return   : 无
	Used Var : 无
	Author   : 张帆 Eli zhang
	*******************************************************/
void OHMDStereoRender::setHMD(HMDDescriptor HMD) {
  _HMD = HMD;

  // Compute aspect ratio and FOV
  float aspect = HMD.hResolution / (2.0f*HMD.vResolution);

  // Fov is normally computed with:
  //   2*atan2(HMD.vScreenSize,2*HMD.eyeToScreenDistance)
  // But with lens distortion it is increased (see Oculus SDK Documentation)
  float r = -1.0f - (4.0f * (HMD.hScreenSize/4.0f - HMD.lensSeparationDistance/2.0f) / HMD.hScreenSize);
  float distScale = (HMD.distortionK[0] + HMD.distortionK[1] * pow(r,2) + HMD.distortionK[2] * pow(r,4) + HMD.distortionK[3] * pow(r,6));
  float fov = 2.0f*atan2(HMD.vScreenSize*distScale, 2.0f*HMD.eyeToScreenDistance);

  // Compute camera projection matrices
  matrix4 centerProjection = matrix4().buildProjectionMatrixPerspectiveFovLH (fov, aspect, 1, 10000);
  float h = 4 * (HMD.hScreenSize/4 - HMD.interpupillaryDistance/2) / HMD.hScreenSize;
  _projectionLeft = matrix4().setTranslation(vector3df(h, 0.0, 0.0)) * centerProjection;
  _projectionRight = matrix4().setTranslation(vector3df(-h, 0.0, 0.0)) * centerProjection;

  // Compute camera offset
  _eyeSeparation = _worldScale * HMD.interpupillaryDistance/2.0f;

  // Compute Viewport
  _viewportLeft = rect<s32>(0, 0, HMD.hResolution/2, HMD.vResolution);
  _viewportRight = rect<s32>(HMD.hResolution/2, 0, HMD.hResolution, HMD.vResolution);

  // Distortion shader parameters
  _lensShift = 4.0f * (HMD.hScreenSize/4.0f - HMD.lensSeparationDistance/2.0f) / HMD.hScreenSize;

  _distortionCB.scale[0] = 1.0f/distScale;
  _distortionCB.scale[1] = 1.0f*aspect/distScale;
  
  _distortionCB.scaleIn[0] = 1.0f;
  _distortionCB.scaleIn[1] = 1.0f/aspect;
  
  _distortionCB.hmdWarpParam[0] = HMD.distortionK[0];
  _distortionCB.hmdWarpParam[1] = HMD.distortionK[1];
  _distortionCB.hmdWarpParam[2] = HMD.distortionK[2];
  _distortionCB.hmdWarpParam[3] = HMD.distortionK[3];

  // Create render target
  if (_driver->queryFeature(video::EVDF_RENDER_TO_TARGET))
  {
    if (_renderTexture != NULL) _renderTexture->drop();
    _renderTexture = _driver->addRenderTargetTexture(dimension2d<u32>(HMD.hResolution*distScale/2.0f, HMD.vResolution*distScale));
    _renderMaterial.setTexture(0, _renderTexture);
  }
  else {
    // Render to target not supported
    assert(0);
  }
}

  /*******************************************************
	Function : 渲染绘制所有场景模型及背景
	Parameter: 
             1   In    scene::ISceneManager*   smgr       引擎场景管理器指针
			 2   In    video::ITexture*        images     左右背景图片数组 
	Return   : 无
	Used Var : 无
	Author   : 张帆 Eli zhang
	*******************************************************/
void OHMDStereoRender::drawAll(ISceneManager* smgr,video::ITexture* images[]) {

  ICameraSceneNode* camera = smgr->getActiveCamera();
  camera->OnAnimate(_timer->getTime());

  // Render Left
  _driver->setRenderTarget(_renderTexture, true, true, video::SColor(0,0,0,0));
  
  _pCamera->setProjectionMatrix(_projectionLeft);

  vector3df r = camera->getRotation();
  vector3df tx(-_eyeSeparation, 0.0,0.0);
  tx.rotateXZBy(-r.Y);
  tx.rotateYZBy(-r.X);
  tx.rotateXYBy(-r.Z);

  _pCamera->setPosition(camera->getPosition() + tx);
  _pCamera->setTarget(camera->getTarget() + tx);  

  smgr->setActiveCamera(_pCamera);
  //绘制背景图片和模型
  //******************注意*************************
  //高度上的位置是图片像素的一半
  _driver->draw2DImage(images[0], core::position2d<s32>(0,200));
  smgr->drawAll(); 

  _driver->setRenderTarget(0, false, false, video::SColor(0,100,100,100));
  _driver->setViewPort(_viewportLeft);

  _distortionCB.lensCenter[0] = _lensShift;

  _driver->setMaterial(_renderMaterial); 
  _driver->drawIndexedTriangleList(_planeVertices, 4, _planeIndices, 2);
 
  // Render Right
  _driver->setRenderTarget(_renderTexture, true, true, video::SColor(0,0,0,0));
  _pCamera->setProjectionMatrix(_projectionRight);

  vector3df r2 = camera->getRotation();
  vector3df tx2(-_eyeSeparation, 0.0,0.0);
  tx.rotateXZBy(-r2.Y);
  tx.rotateYZBy(-r2.X);
  tx.rotateXYBy(-r2.Z);

  _pCamera->setPosition(camera->getPosition() + tx2);
  _pCamera->setTarget(camera->getTarget() + tx2);  
  //绘制背景图片和模型
  //******************注意*************************
  //高度上的位置是图片像素的一半
  _driver->draw2DImage(images[1], core::position2d<s32>(0,200));
  smgr->drawAll();

  _driver->setRenderTarget(0, false, false, video::SColor(0,100,100,100));  
  _driver->setViewPort(_viewportRight);

  _distortionCB.lensCenter[0] = -_lensShift;

  _driver->setMaterial(_renderMaterial); 
  _driver->drawIndexedTriangleList(_planeVertices, 4, _planeIndices, 2);

  smgr->setActiveCamera(camera);
}