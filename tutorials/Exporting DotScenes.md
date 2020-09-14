
# Exporting Blender DotScenes

## Python

Install Python 2.3 (the 2.4 may not work)

## Scripts setup

Open the `ogredotscene.py` and `ogreexport.py` script in blender text editor `shift+F11`.

Setup the `ogreexport.py` script: 

you can set path variables in script where find the `OgreXMLConverter.exe`. 
i.e: `OGRE_XML_CONVERTER = 'D:\yourPath\ogrexmlconverter -t`
(the -t make the tangent vector for normal maps)

- select the objects that you want to export and then run the script width (`left Alt+P`)
- if you dont use dotScene `.xml`, but only `.mesh` you can set the `x=-90` for good orientation.

## Setup

How set own user data:

- press `F4` (this is the **blender game engine** window)
- select an object that youu can setup an userdata
- push the big `ADD property` button
- select the types (float, bool etc) and add name and value.

## Export dotScene xml format

change to `ogredotscene.py` in text editor
run script `left Alt+p` push all button and `Logic Properties` button (this is the user data).


# others:

## How can you named the object:

press `F9`: this is the editing window in buttons windows types
`OB`: this is will the **name of entity**
`ME`: this is will the **name of .mesh**

## The name of scene:

You can rename the scene at top of header `SCE:yourSceneName`
this is will same name for the `.material`  and `dotScene.xml` files.

## render/and camera moving:

You can move the camera by select with **right button** and press `g` or **drag** instantly
and for **ok** press `left mouse`.

**rotate** : press `r` and `left mouse` ok.

## Views

- look across from camera: `numpad 0`
- top: `numpad 7`
- front: `numpad 1`
- left: `numpad 3`
- ortho/pespective look: `numpad 5`
- pan view: `shift+MMB`
- zoom in/out: `ctrl+MMB`

Press `F12` for **render**








