/*
-----------------------------------------------------------
Copyright � 2003 by cTh
Use as you see fit.
Questions : gavocanov@rambler.ru
-----------------------------------------------------------
*/
/***************************************************************************************************/
#include "Ogre.h"
#include "OgreFontManager.h"
#include "movableText.h"

namespace Ogre
{
	/***************************************************************************************************/
    #define POS_TEX_BINDING		0
	#define COLOUR_BINDING		1
	/***************************************************************************************************/
	MovableText::MovableText(const String &name, const String &caption, const String &fontName, int charHeight, const ColourValue &color)
	{
		if (name == "")
			OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Trying to create MovableText without name", "MovableText::MovableText");

		if (caption == "")
			OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Trying to create MovableText without name", "MovableText::MovableText");

		mpCam = NULL;
		mpWin = NULL;
		mpFont = NULL;
		//mpMaterial = NULL;
		mRenderOp.vertexData = NULL;
		mName = name;
		mCaption = caption;
		mFontName = fontName;
		mCharHeight = charHeight;
		mColor = color;
		mType = "MovableText";
		mTimeUntilNextToggle = 0;
		mSpaceWidth = 0;
		mUpdateColors = true;
		mOnTop = true;

		this->setFontName(mFontName);
		this->_setupGeometry();
	}
	/***************************************************************************************************/
	MovableText::~MovableText()
	{
		if (mRenderOp.vertexData)
			delete mRenderOp.vertexData;
	}
	/***************************************************************************************************/
	void MovableText::setFontName(const String &fontName)
	{
		if (mFontName != fontName || mpMaterial.isNull() || !mpFont)
		{
			mFontName = fontName;
			mpFont = (Font *)FontManager::getSingleton().getByName(mFontName).getPointer();
			if (!mpFont)
				OGRE_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Could not find font " + fontName, "MovableText::setFontName" );
			mpFont->load();
			if (!mpMaterial.isNull())
			{
				MaterialManager::getSingletonPtr()->remove(mpMaterial->getName());
				mpMaterial.setNull();
			}
			mpMaterial = mpFont->getMaterial()->clone(mName + "Material");
			if (!mpMaterial->isLoaded())
				mpMaterial->load();
			mpMaterial->setDepthCheckEnabled(!mOnTop);
			mpMaterial->setLightingEnabled(false);
			mNeedUpdate = true;
		}
	}
	/***************************************************************************************************/
	void MovableText::showOnTop(bool show)
	{
		mOnTop = show;
		if (!mpMaterial.isNull())
			mpMaterial->setDepthBias(!mOnTop);
	}
	/***************************************************************************************************/
	void MovableText::setSpaceWidth(uint width)
	{
		if (width != mSpaceWidth)
		{
			mSpaceWidth = width;
			mNeedUpdate = true;
		}
	}
	/***************************************************************************************************/
	void MovableText::setCaption(const String &caption)
	{
		if (caption != mCaption)
		{
			mCaption = caption;
			mNeedUpdate = true;
		}
	}
	/***************************************************************************************************/
	void MovableText::setColor(const ColourValue &color)
	{
		if (color != mColor)
		{
			mColor = color;
			mUpdateColors = true;
		}
	}
	/***************************************************************************************************/
	void MovableText::setCharacterHeight(uint height)
	{
		if (height != mCharHeight)
		{
			mCharHeight = height;
			mNeedUpdate = true;
		}
	}
	/***************************************************************************************************/
	Vector3 MovableText::_translate3Dto2D(Camera *pCam, const Vector3 vertex)
	{
		return pCam->getProjectionMatrix() * pCam->getViewMatrix() * vertex;
	}
	/***************************************************************************************************/
	void MovableText::_translate3Dto2DPixels(Camera *pCam, const Vector3 vertex, int *x, int *y)
	{
		// calculate hsc screen coordinates
		Vector3 hsc = _translate3Dto2D(pCam, vertex);
		// convert to window position in pixels
		RenderTarget *rt = Root::getSingleton().getRenderTarget(mpWin->getName());
		if (!rt) 
			OGRE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Can't find '" + mpWin->getName() + "' render target", "MovableText::translate3Dto2DPixels");
		*x = (int)((hsc.x + 1.0f) / 2.0f * rt->getWidth());
		*y = (int)((-hsc.y + 1.0f) / 2.0f * rt->getHeight());
	}
	/***************************************************************************************************/
	void MovableText::_setupGeometry()
	{
        assert(mpFont);
		assert(!mpMaterial.isNull());

		uint vertexCount = mCaption.size() * 6;
		if (mRenderOp.vertexData)
		{
			if (mRenderOp.vertexData->vertexCount != vertexCount)
			{
				delete mRenderOp.vertexData;
				mRenderOp.vertexData = NULL;
				mUpdateColors = true;
			}
		}

		if (!mRenderOp.vertexData)
			mRenderOp.vertexData = new VertexData();

        mRenderOp.indexData = 0;
		mRenderOp.vertexData->vertexStart = 0;
		mRenderOp.vertexData->vertexCount = vertexCount;
		mRenderOp.operationType = RenderOperation::OT_TRIANGLE_LIST; 
		mRenderOp.useIndexes = false; 

        VertexDeclaration	*decl = mRenderOp.vertexData->vertexDeclaration;
        VertexBufferBinding	*bind = mRenderOp.vertexData->vertexBufferBinding;
        size_t offset = 0;

		// create/bind positions/tex.ccord. buffer
		if (!decl->findElementBySemantic(VES_POSITION))
			decl->addElement(POS_TEX_BINDING, offset, VET_FLOAT3, VES_POSITION);
        offset += VertexElement::getTypeSize(VET_FLOAT3);
		if (!decl->findElementBySemantic(VES_TEXTURE_COORDINATES))
			decl->addElement(POS_TEX_BINDING, offset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES, 0);
        HardwareVertexBufferSharedPtr ptbuf = 
            HardwareBufferManager::getSingleton().createVertexBuffer(
                decl->getVertexSize(POS_TEX_BINDING),
                mRenderOp.vertexData->vertexCount,
				HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);
        bind->setBinding(POS_TEX_BINDING, ptbuf);

        // Colours - store these in a separate buffer because they change less often
		if (!decl->findElementBySemantic(VES_DIFFUSE))
			decl->addElement(COLOUR_BINDING, 0, VET_COLOUR, VES_DIFFUSE);
        HardwareVertexBufferSharedPtr cbuf = 
            HardwareBufferManager::getSingleton().createVertexBuffer(
                decl->getVertexSize(COLOUR_BINDING),
                mRenderOp.vertexData->vertexCount,
				HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);
        bind->setBinding(COLOUR_BINDING, cbuf);

		size_t charlen = mCaption.size();
        Real *pPCBuff = static_cast<Real*>(ptbuf->lock(HardwareBuffer::HBL_DISCARD));

		float largestWidth = 0;
        float left = 0 * 2.0 - 1.0;
        float top = -( (0 * 2.0 ) - 1.0 );

        // Derive space with from a capital A
		if (mSpaceWidth == 0)
			mSpaceWidth = mpFont->getGlyphAspectRatio( 'A' ) * mCharHeight * 2.0;

		// for calculation of AABB
		Ogre::Vector3 min, max, currPos;
		Ogre::Real maxSquaredRadius;
		bool first = true;

		// Use iterator
        String::iterator i, iend;
        iend = mCaption.end();
        bool newLine = true;
        for( i = mCaption.begin(); i != iend; ++i )
        {
            if( newLine )
            {
                Real len = 0.0f;
                for( String::iterator j = i; j != iend && *j != '\n'; j++ )
                {
                    if (*j == ' ')
                        len += mSpaceWidth;
                    else 
                        len += mpFont->getGlyphAspectRatio( *j ) * mCharHeight * 2.0;
                }
                newLine = false;
            }

            if( *i == '\n' )
            {
                left = 0 * 2.0 - 1.0;
                top -= mCharHeight * 2.0;
                newLine = true;
                continue;
            }

            if ( *i == ' ')
            {
                // Just leave a gap, no tris
                left += mSpaceWidth;
                // Also reduce tri count
                mRenderOp.vertexData->vertexCount -= 6;
                continue;
            }

            Real horiz_height = mpFont->getGlyphAspectRatio( *i );
            Real u1, u2, v1, v2; 
            mpFont->getGlyphTexCoords( *i, u1, v1, u2, v2 );

            // each vert is (x, y, z, u, v)
            //-------------------------------------------------------------------------------------
            // First tri
            //
            // Upper left
            *pPCBuff++ = left;
            *pPCBuff++ = top;
            *pPCBuff++ = -1.0;
            *pPCBuff++ = u1;
            *pPCBuff++ = v1;

			// Deal with bounds
			currPos = Ogre::Vector3(left, top, -1.0);
			if (first)
			{
				min = max = currPos;
				maxSquaredRadius = currPos.squaredLength();
				first = false;
			}
			else
			{
				min.makeFloor(currPos);
				max.makeCeil(currPos);
				maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());
			}

			top -= mCharHeight * 2.0;

            // Bottom left
            *pPCBuff++ = left;
            *pPCBuff++ = top;
            *pPCBuff++ = -1.0;
            *pPCBuff++ = u1;
            *pPCBuff++ = v2;

			// Deal with bounds
			currPos = Ogre::Vector3(left, top, -1.0);
			min.makeFloor(currPos);
			max.makeCeil(currPos);
			maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());

			top += mCharHeight * 2.0;
            left += horiz_height * mCharHeight * 2.0;

            // Top right
            *pPCBuff++ = left;
            *pPCBuff++ = top;
            *pPCBuff++ = -1.0;
            *pPCBuff++ = u2;
            *pPCBuff++ = v1;
            //-------------------------------------------------------------------------------------

			// Deal with bounds
			currPos = Ogre::Vector3(left, top, -1.0);
			min.makeFloor(currPos);
			max.makeCeil(currPos);
			maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());

			//-------------------------------------------------------------------------------------
            // Second tri
            //
            // Top right (again)
            *pPCBuff++ = left;
            *pPCBuff++ = top;
            *pPCBuff++ = -1.0;
            *pPCBuff++ = u2;
            *pPCBuff++ = v1;

			currPos = Ogre::Vector3(left, top, -1.0);
			min.makeFloor(currPos);
			max.makeCeil(currPos);
			maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());

			top -= mCharHeight * 2.0;
            left -= horiz_height  * mCharHeight * 2.0;

            // Bottom left (again)
            *pPCBuff++ = left;
            *pPCBuff++ = top;
            *pPCBuff++ = -1.0;
            *pPCBuff++ = u1;
            *pPCBuff++ = v2;

			currPos = Ogre::Vector3(left, top, -1.0);
			min.makeFloor(currPos);
			max.makeCeil(currPos);
			maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());

			left += horiz_height  * mCharHeight * 2.0;

            // Bottom right
            *pPCBuff++ = left;
            *pPCBuff++ = top;
            *pPCBuff++ = -1.0;
            *pPCBuff++ = u2;
            *pPCBuff++ = v2;
            //-------------------------------------------------------------------------------------

			currPos = Ogre::Vector3(left, top, -1.0);
			min.makeFloor(currPos);
			max.makeCeil(currPos);
			maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());

			// Go back up with top
            top += mCharHeight * 2.0;

			float currentWidth = (left + 1)/2 - 0;
			if (currentWidth > largestWidth)
				largestWidth = currentWidth;
        }
        // Unlock vertex buffer
        ptbuf->unlock();

		// update AABB/Sphere radius
		mAABB = Ogre::AxisAlignedBox(min, max);
		mRadius = Ogre::Math::Sqrt(maxSquaredRadius);

		if (mUpdateColors)
			this->_updateColors();

		mNeedUpdate = false;
	}
	/***************************************************************************************************/
    void MovableText::_updateColors(void)
    {
        assert(mpFont);
		assert(!mpMaterial.isNull());

		// Convert to system-specific
        RGBA color;
        Root::getSingleton().convertColourValue(mColor, &color);
        HardwareVertexBufferSharedPtr vbuf = mRenderOp.vertexData->vertexBufferBinding->getBuffer(COLOUR_BINDING);
        RGBA *pDest = static_cast<RGBA*>(vbuf->lock(HardwareBuffer::HBL_DISCARD) );
        for (uint i = 0; i < mRenderOp.vertexData->vertexCount; ++i)
            *pDest++ = color;
        vbuf->unlock();
		mUpdateColors = false;
    }
	/***************************************************************************************************/
	const Quaternion& MovableText::getWorldOrientation(void) const
	{
        assert(mpCam);
		return const_cast<Quaternion&>(mpCam->getDerivedOrientation());
	}
	/***************************************************************************************************/
	const Vector3& MovableText::getWorldPosition(void) const
	{
        assert(mParentNode);
		return mParentNode->_getDerivedPosition();
	}
	/***************************************************************************************************/
    void MovableText::getWorldTransforms(Matrix4 *xform) const 
    {
		if (this->isVisible() && mpCam)
		{
			Matrix3 rot3x3, scale3x3 = Matrix3::IDENTITY;
			// store rotation in a matrix
			mpCam->getDerivedOrientation().ToRotationMatrix(rot3x3);
			// parent node position
			Vector3 ppos = mParentNode->_getDerivedPosition();
			// compute scale
			//Vector3 p2D_1 = _translate3Dto2D(mpCam, ppos);
			//Vector3 p2D_2 = _translate3Dto2D(mpCam, ppos + mAABB.getMaximum());
			//Vector3 p2D_diff = p2D_1 - p2D_2;
			//Real scaleFactor = (float)1.0f / mCharHeight;//Math::Abs(mCharHeight / (p2D_diff.y * 8));
			// apply scale
			scale3x3[0][0] = mParentNode->_getDerivedScale().x / 2;
			scale3x3[1][1] = mParentNode->_getDerivedScale().y / 2;
			scale3x3[2][2] = mParentNode->_getDerivedScale().z / 2;
			// apply all transforms to xform			
			*xform = (rot3x3 * scale3x3);
			xform->setTrans(ppos);
		}
    }
	/***************************************************************************************************/
	void MovableText::getRenderOperation(RenderOperation &op)
	{
		if (this->isVisible())
		{
			if (mNeedUpdate)
				this->_setupGeometry();
			if (mUpdateColors)
				this->_updateColors();
			op = mRenderOp;
		}
	}
	/***************************************************************************************************/
	void MovableText::_notifyCurrentCamera(Camera *cam)
	{
		mpCam = cam;
	}
	/***************************************************************************************************/
    void MovableText::_updateRenderQueue(RenderQueue* queue)
    {
		if (this->isVisible())
		{
			if (mNeedUpdate)
				this->_setupGeometry();
			if (mUpdateColors)
				this->_updateColors();
			queue->addRenderable(this, mRenderQueueID, OGRE_RENDERABLE_DEFAULT_PRIORITY);
		}
    }
}
/***************************************************************************************************/
