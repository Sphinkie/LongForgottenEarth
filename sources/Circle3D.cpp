/* ------------------------------------------------------------------------- */
// File    : Circle3D.cpp
// Project : Long Forgotten Earth
// Author  : David de Lorenzo
/* ------------------------------------------------------------------------- */

#include "Circle3D.h"
#include "DynamicLines.h"
#include "macros.h"

/* ------------------------------------------------------------------------- */
/// Construction
/* ------------------------------------------------------------------------- */
Circle3D::Circle3D(SceneManager* SceneMgr, SceneNode* ParentNode, Real Radius)
{
	mSceneMgr   = SceneMgr;
	mCircleNode = ParentNode->createChildSceneNode();

	for (int i=0; i<NB_PTS; i++)
	{
		Radian angle = Radian(i*2*Math::PI/NB_PTS);
		mCirclePoints[i] = Radius * Vector3(Math::Sin(angle), 0, Math::Cos(angle));
	}
}

/* ------------------------------------------------------------------------- */
/// Destruction
/* ------------------------------------------------------------------------- */
Circle3D::~Circle3D()
{
	/// TODO
}



/* ------------------------------------------------------------------------- */
///
/* ------------------------------------------------------------------------- */
void Circle3D::draw()
{
	DynamicLines* lines = new DynamicLines(RenderOperation::OT_LINE_STRIP); // cercles continus
//	DynamicLines* lines = new DynamicLines(RenderOperation::OT_LINE_LIST);  // pointillés
	for (int i=0; i<NB_PTS; i++) // mCirclePoints.size()
	{
		lines->addPoint(mCirclePoints[i]);
	}
	lines->update();
	mCircleNode->attachObject(lines);
}

/* ------------------------------------------------------------------------- */
/// Mise à jour des points du cercle
/** Il faut avoir modifié mCirclePoints avant d'appeler cette fonction */
/* ------------------------------------------------------------------------- */
void Circle3D::update()
{
/*
    DynamicLines *lines = dynamic_cast<DynamicLines*>(mCircleNode->getAttachedObject(0));

    if (lines->getNumPoints()!= mCirclePoints.size()) 
	{
      // Oh no!  Size changed, just recreate the list from scratch
      lines->clear();
      for (int i=0; mCirclePoints.size(); ++i) 
	  {
        lines->addPoint(mCirclePoints[i]);
      }
    }
    else 
	{
      // Just values have changed, use 'setPoint' instead of 'addPoint'
      for (int i=0; i<mCirclePoints.size(); ++i) 
	  {
        lines->setPoint(i,cvt(ppos[c->m_p1])); 
      }
    }
    lines->update();
*/
}


/* ------------------------------------------------------------------------- */
/// Change la couleur du cercle.
/* ------------------------------------------------------------------------- */
void Circle3D::setColour(ColourValue CircleColour)
{
    DynamicLines *lines = static_cast<DynamicLines*>(mCircleNode->getAttachedObject(0));
	lines->createMaterial(CircleColour);
}
