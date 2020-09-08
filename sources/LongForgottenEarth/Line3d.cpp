/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */

#include "Line3D.h"


/* ------------------------------------------------------------------------- */
/// Trace une ligne 3D entre deux positions
/* ------------------------------------------------------------------------- */
Line3D::Line3D(Vector3 point1, Vector3 point2) 
{ 

	VertexDeclaration* decl; 

	this->setMaterial("BaseWhite"); // NoLighting");
	
	mRenderOp.vertexData = new VertexData(); 
	mRenderOp.indexData = 0; 
	mRenderOp.operationType = RenderOperation::OT_LINE_LIST; 
	mRenderOp.useIndexes = false; 
	decl = mRenderOp.vertexData->vertexDeclaration; 
	decl->addElement(0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION); 

	Ogre::VertexBufferBinding* bind; 
	Ogre::Real *pos; 

	mRenderOp.vertexData->vertexCount = 2; // 2 points 
	mRenderOp.vertexData->vertexStart = 0; 

	// create and bind the buffer 
	HardwareVertexBufferSharedPtr vbuf = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(mRenderOp.vertexData->vertexDeclaration->getVertexSize(0), mRenderOp.vertexData->vertexCount, Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY); 
	bind = mRenderOp.vertexData->vertexBufferBinding; 
	bind->setBinding(0, vbuf); 

	pos = (Real*)vbuf->lock(HardwareBuffer::HBL_DISCARD); 
	*pos++ = point1.x; // x coordinate of point 1 
	*pos++ = point1.y; // y coordinate of point 1 
	*pos++ = point1.z; // z coordinate of point 1 

	*pos++ = point2.x; // x coordinate of point 2 
	*pos++ = point2.y; // y coordinate of point 2 
	*pos++ = point2.z; // z coordinate of point 2 

	vbuf->unlock(); 

	AxisAlignedBox aabb(point1,point2); 
	setBoundingBox(aabb); 
} 

/* ------------------------------------------------------------------------- */
/// Efface la ligne
/* ------------------------------------------------------------------------- */
Line3D::~Line3D(void) 
{
	
	delete mRenderOp.vertexData; 
} 

/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
Real Line3D::getSquaredViewDepth(const Camera* cam) const 
{ 
	// just some value for now 
	return cam->getDerivedPosition().squaredLength(); 
} 

/* ------------------------------------------------------------------------- */
/// Non implémenté
/* ------------------------------------------------------------------------- */
Real Line3D::getBoundingRadius(void) const 
{ 
	// just some value for now 
	return 1000.0; 
} 


/* ------------------------------------------------------------------------- */
/// Change the colour of all the lines.
/* ------------------------------------------------------------------------- */
void Line3D::setColor(ColourValue LineColour) 
{
	if (!m_pMaterial.isNull()) 
	{
		m_pMaterial->setAmbient(LineColour); 
		m_pMaterial->setSelfIllumination(LineColour); 
	}
}