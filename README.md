# QmlOgre
QmlOgre is a library that allows integrating Ogre3D into Qt QML scenes.

## Features

* Renders Ogre viewports to FBO (Frame Buffer Objects) which are then applied to QML items as textures
* Window states, the application event loop and user input are managed by Qt
* Creates an OpenGL context for Ogre which is shared with Qt's QML OpenGL context
* Allows using the full range of QML features
* Places no restriction on the depth sorting of Ogre items versus other QML elements
* Allows multiple independent cameras, each assigned to an Ogre QML item, viewing the same scene

## Requirements (tested with)

* Ogre 14.2.0
* Qt 5.12.8

## Notes:

 - Ogre must be built with `OGRE_GLSUPPORT_USE_EGL : OFF`. Otherwise EGL is forced over GLX.
 -  My Ogre build did not resolve glad dependency. Provided is my Glad1 with OpenGL 4.6 compatibiliy in the included `glad` folder. It should be installed to `/usr/local/include/glad/glad.h`. If using your own, make sure to replace glad.c.
 - Set `$OGRE_HOME` either as environment variable or in the example.pro

License
-------
QmlOgre is licensed under the BSD license. See ```LICENSE``` for details.
