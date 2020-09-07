rem ----------------------------------
rem Reduction de la taille des screenshots
rem ----------------------------------
nconvert  -out jpeg -n 1 40 1 -q 90 -o .\screenshots\screen_##.jpg  .\screenshots\screenshot_##.jpg




rem ----------------------------------
rem Generation des Thumbs
rem ----------------------------------

nconvert  -out jpeg -n 1 40 1 -o .\thumb\thumb_##.jpg -resize 160 120 .\screenshots\screenshot_##.jpg

pause



