/*!
 * \copyright (c) Nokia Corporation and/or its subsidiary(-ies) (qt-info@nokia.com) and/or contributors
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 *
 * \license{This source file is part of QmlOgre abd subject to the BSD license that is bundled
 * with this source code in the file LICENSE.}
 */

#include "ogreengine.h"
#include "ogreitem.h"

#include <QOpenGLFunctions>

#include <OgreFileSystemLayer.h>
#include <OgreFileSystem.h>

OgreEngine::OgreEngine(QQuickWindow *window)
    : QObject(),
      m_resources_cfg(Ogre::StringUtil::BLANK)
{
    qmlRegisterType<OgreItem>("Ogre", 1, 0, "OgreItem");
    qmlRegisterType<OgreEngine>("OgreEngine", 1, 0, "OgreEngine");

    setQuickWindow(window);
}

OgreEngine::~OgreEngine()
{
    delete m_ogreContext;
}

Ogre::Root* OgreEngine::startEngine()
{
    m_resources_cfg = "resources.cfg";

    activateOgreContext();

    Ogre::Root *ogreRoot = new Ogre::Root("plugins.cfg");
    Ogre::RenderSystemList list = ogreRoot->getAvailableRenderers();
    ogreRoot->setRenderSystem(list[0]);
    ogreRoot->initialise(false);

    Ogre::NameValuePairList params;

    params["externalGLControl"] = "true";
    params["currentGLContext"] = "true";

    //Finally create our window.
    m_ogreWindow = ogreRoot->createRenderWindow("OgreWindow", 1, 1, false, &params);
    m_ogreWindow->setVisible(false);
    m_ogreWindow->update(false);

    doneOgreContext();

    return ogreRoot;
}

void OgreEngine::stopEngine(Ogre::Root *ogreRoot)
{
    if (ogreRoot) {
        //        m_root->detachRenderTarget(m_renderTexture);
        // TODO tell node(s) to detach

    }

    delete ogreRoot;
}

void OgreEngine::setQuickWindow(QQuickWindow *window)
{
    Q_ASSERT(window);

    m_quickWindow = window;
    m_qtContext = QOpenGLContext::currentContext();
    QSurfaceFormat format = m_quickWindow->requestedFormat();
    qDebug() << "red: " << format.redBufferSize();
    qDebug() << "green: " << format.greenBufferSize();
    qDebug() << "blue: " << format.blueBufferSize();
    qDebug() << "alpha: " << format.alphaBufferSize();
    // create a new shared OpenGL context to be used exclusively by Ogre
    m_ogreContext = new QOpenGLContext();
    m_ogreContext->setFormat(m_quickWindow->requestedFormat());
    m_ogreContext->setShareContext(m_qtContext);
    m_ogreContext->create();
}

void OgreEngine::activateOgreContext()
{
    //    glPopAttrib();
    //    glPopClientAttrib();

    m_qtContext->functions()->glUseProgram(0);
    m_qtContext->doneCurrent();

    m_ogreContext->makeCurrent(m_quickWindow);
}

void OgreEngine::doneOgreContext()
{
    m_ogreContext->functions()->glBindBuffer(GL_ARRAY_BUFFER, 0);
    m_ogreContext->functions()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    m_ogreContext->functions()->glBindRenderbuffer(GL_RENDERBUFFER, 0);
    m_ogreContext->functions()->glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // unbind all possible remaining buffers; just to be on safe side
    //    m_ogreContext->functions()->glBindBuffer(GL_ARRAY_BUFFER, 0);
    //    m_ogreContext->functions()->glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
    //    m_ogreContext->functions()->glBindBuffer(GL_COPY_READ_BUFFER, 0);
    //    m_ogreContext->functions()->glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
    //    m_ogreContext->functions()->glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
    //    m_ogreContext->functions()->glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, 0);
    //    m_ogreContext->functions()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    //    m_ogreContext->functions()->glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    //    m_ogreContext->functions()->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    //    m_ogreContext->functions()->glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    //    m_ogreContext->functions()->glBindBuffer(GL_TEXTURE_BUFFER, 0);
    //    m_ogreContext->functions()->glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
    //    m_ogreContext->functions()->glBindBuffer(GL_UNIFORM_BUFFER, 0);

    m_ogreContext->doneCurrent();

    m_qtContext->makeCurrent(m_quickWindow);
    //    glPushAttrib(GL_ALL_ATTRIB_BITS);
    //    glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
}

QOpenGLContext* OgreEngine::ogreContext() const
{
    return m_ogreContext;
}

QSGTexture* OgreEngine::createTextureFromId(uint id, const QSize &size, QQuickWindow::CreateTextureOptions options) const
{
    return m_quickWindow->createTextureFromId(id, size, options);
}

void OgreEngine::setupResources(void)
{
    Ogre::ConfigFile cf;
    cf.load(m_resources_cfg);
    auto& rgm = Ogre::ResourceGroupManager::getSingleton();

    Ogre::String sec, type, arch;
    // go through all specified resource groups
    for(auto& s : cf.getSettingsBySection()) {
        sec = s.first;
        // go through all resource paths
        for (auto& t : s.second)
        {
            type = t.first;
            arch = t.second;

            Ogre::StringUtil::trim(arch);
            if (arch.empty() || arch[0] == '.')
            {
                // resolve relative path with regards to configfile
                Ogre::String baseDir, filename;
                Ogre::StringUtil::splitFilename(m_resources_cfg, filename, baseDir);
                arch = baseDir + arch;
            }

            arch = Ogre::FileSystemLayer::resolveBundlePath(arch);

            if((type == "Zip" || type == "FileSystem") && !Ogre::FileSystemLayer::fileExists(arch))
            {
                Ogre::LogManager::getSingleton().logWarning("resource location '"+arch+"' does not exist - skipping");
                continue;
            }

            rgm.addResourceLocation(arch, type, sec);
        }
    }

    if(rgm.getResourceLocationList(Ogre::RGN_INTERNAL).empty())
    {
        const auto& mediaDir = Ogre::FileSystemLayer::resolveBundlePath("/usr/local/share/OGRE-14.2/Media");
        // add default locations
        rgm.addResourceLocation(mediaDir + "/Main", "FileSystem", Ogre::RGN_INTERNAL);
        rgm.addResourceLocation(mediaDir + "/Terrain", "FileSystem", Ogre::RGN_INTERNAL);
        rgm.addResourceLocation(mediaDir + "/RTShaderLib", "FileSystem", Ogre::RGN_INTERNAL);
    }
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}
