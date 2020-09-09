@echo off
echo usage: Drag and drop the MESH.XML file on MESH_ANALYSE.BAT
echo         The adjust.xml file will be created here

E:

perl mesh_analyze.pl %1
rem E:\sphinx\vrac\hibernation\hibernation.mesh.xml

echo NOW YOU CAN EDIT THE ADJUST.XML FILE
echo THEN RUN THE MESH_ADJUST SCRIPT
pause
