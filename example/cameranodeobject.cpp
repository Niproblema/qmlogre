/*!
 * \copyright (c) Nokia Corporation and/or its subsidiary(-ies) (qt-info@nokia.com) and/or contributors
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 *
 * \license{This source file is part of QmlOgre abd subject to the BSD license that is bundled
 * with this source code in the file LICENSE.}
 */

#include "cameranodeobject.h"

#include <OgreRoot.h>
#include <OgreSceneNode.h>
#include <OgreCamera.h>

#include <QDebug>

static const Ogre::Vector3 initialPosition(0, 0, 300);

CameraNodeObject::CameraNodeObject(QObject *parent) :
    QObject(parent),
    OgreCameraWrapper(),
    m_camera(0),
    m_yaw(0),
    m_pitch(0),
    m_zoom(1)
{
    Ogre::SceneManagerInstanceMap sceneManagerSelection = Ogre::Root::getSingleton().getSceneManagers();
    Ogre::SceneManager *sceneManager = sceneManagerSelection.begin()->second;

    // let's use the current memory address to create a unique name
    QString instanceName;
    instanceName.sprintf("camera_%08p", this);

    m_cameraNode = sceneManager->getRootSceneNode()->createChildSceneNode();
    m_camera = sceneManager->createCamera(instanceName.toLatin1().data());
    m_camera->setNearClipDistance(1);
    m_camera->setFarClipDistance(99999);
    m_camera->setAspectRatio(1);
    //m_cameraNode->setAutoTracking(true, sceneManager->getRootSceneNode());

    m_cameraNode->attachObject(m_camera);
    m_cameraNode->setPosition(initialPosition);
}

void CameraNodeObject::updateRotation()
{
    m_cameraNode->resetOrientation();
    m_cameraNode->yaw(Ogre::Radian(Ogre::Degree(m_yaw)));
    m_cameraNode->pitch(Ogre::Radian(Ogre::Degree(m_pitch)));
}

void CameraNodeObject::setZoom(qreal z)
{
    m_zoom = z;
    // m_cameraNode->resetOrientation();
    m_cameraNode->setPosition(initialPosition * (1 / m_zoom));
    updateRotation();
}
