/*
-----------------------------------------------------------
Copyright © 2003 by cTh
Use as you see fit.
Questions : gavocanov@rambler.ru
-----------------------------------------------------------
*/
/***************************************************************************************************/
#ifndef INC_MovableText_H
#define INC_MovableText_H
/***************************************************************************************************/
namespace Ogre
{
	/***************************************************************************************************/
	class MovableText : public MovableObject, public Renderable
	{
	public:
		MovableText(const String &name, const String &caption, const String &fontName = "TrebuchetMSBold", int charHeight = 16, const ColourValue &color = ColourValue::White);
		virtual ~MovableText();
	
		void setFontName(const String &fontName);
		void setCaption(const String &caption);
		void setColor(const ColourValue &color);
		void setCharacterHeight(uint height);
		void setSpaceWidth(uint width);
		void showOnTop(bool show);

		const String &getCaption() const {return mCaption;}
		const String &getFontName() const {return mFontName;}
		uint getCharacterHeight() const {return mCharHeight;}
		uint getSpaceWidth() const {return mSpaceWidth;}
		const ColourValue &getColor() const {return mColor;}
		bool getShowOnTop() const {return mOnTop;}

	private:
		String			mFontName;
		String			mType;
		String			mName;
		String			mCaption;

		ColourValue		mColor;
		RenderOperation	mRenderOp;
		AxisAlignedBox	mAABB;
		LightList		mLList;

		uint			mCharHeight;
		uint			mSpaceWidth;

		bool			mNeedUpdate;
		bool			mUpdateColors;
		bool			mOnTop;

		Real			mTimeUntilNextToggle;
		Real			mRadius;

		Camera			*mpCam;
		RenderWindow	*mpWin;
		Font			*mpFont;
		MaterialPtr		mpMaterial;

		Vector3 _translate3Dto2D(Camera *pCam, const Vector3 vertex);
		void	_translate3Dto2DPixels(Camera *pCam, const Vector3 vertex, int *x, int *y);
		void	_setupGeometry();
		void	_updateColors();

		// from MovableObject
		void getWorldTransforms(Matrix4 *xform) const;
        const Quaternion& getWorldOrientation(void) const;
        const Vector3& getWorldPosition(void) const;
		void _notifyCurrentCamera(Camera *cam);
		void _updateRenderQueue(RenderQueue* queue);

		const AxisAlignedBox &getBoundingBox(void) const {return mAABB;};
		const String &getName(void) const {return mName;};
		const String &getMovableType(void) const {static Ogre::String movType = "MovableText"; return movType;};
		Real getBoundingRadius(void) const {return mRadius;};
		Real getSquaredViewDepth(const Camera *cam) const {return 0;};

		// from renderable
		void getRenderOperation(RenderOperation &op);
		const MaterialPtr& getMaterial(void) const {assert(!mpMaterial.isNull());return mpMaterial;};
		const LightList& getLights(void) const {return mLList;};
	};
	/***************************************************************************************************/
}
/***************************************************************************************************/
#endif
/***************************************************************************************************/
