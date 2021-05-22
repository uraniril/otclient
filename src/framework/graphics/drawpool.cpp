/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "drawpool.h"
#include "declarations.h"
#include <framework/core/declarations.h>
#include <framework/graphics/framebuffermanager.h>
#include <framework/graphics/graphics.h>
#include "painter.h"

DrawPool g_drawPool;

DrawPool::DrawPool()
{
    for(int_fast8_t i = -1; ++i < m_framebuffers.size();) {
        m_framebuffers[i].frame = g_framebuffers.createFrameBuffer();
    }

    m_framebuffers[DRAWTYPE_LIGHT].frame->setCompositeMode(Painter::CompositionMode_Light);
}

void DrawPool::addTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src)
{
    if(m_currentDrawType == -1) return;
    m_drawObjects[m_currentDrawType].push_back(DrawObject{ dest, src, texture, g_painter->getCurrentState() });
}

void DrawPool::addFilledRect(const Rect& dest)
{
    if(m_currentDrawType == -1) return;
    m_drawObjects[m_currentDrawType].push_back(DrawObject{ dest, Rect(), nullptr, g_painter->getCurrentState() });
}

bool DrawPool::drawUp(DrawType type, Size size, const Rect& dest, const Rect& src)
{
    auto& dataBuffer = m_framebuffers[type];
    bool canUpdate = dataBuffer.frame->canUpdate();
    if(canUpdate) {
        dataBuffer.frame->resize(size);
        dataBuffer.dest = dest;
        dataBuffer.src = src;
        m_currentDrawType = type;
        m_coordsBuffer.clear();
    }

    return canUpdate;
}

void DrawPool::draw()
{
    DrawObject lastObject;
    for(uint8 type = DRAWTYPE_MAP; type < DRAWTYPE_LAST; ++type) {
        const auto& buffer = m_framebuffers[type];
        if(!buffer.frame->isValid()) continue;

        if(buffer.frame->canUpdate()) {
            auto& objects = m_drawObjects[type];
            const size_t objSize = objects.size();

            if(objSize > 0) {
                buffer.frame->bind();

                for(uint_fast32_t i = 0;; ++i) {
                    const bool last = i == objSize;
                    const auto& obj = last ? m_nullDrawObject : objects[i];
                    if(last || i > 0 && !lastObject.isEqual(obj)) {
                        g_painter->saveAndResetState();
                        g_painter->executeState(lastObject.state);

                        if(lastObject.texture == nullptr && lastObject.src.isNull()) {
                            g_painter->drawFillCoords(m_coordsBuffer);
                        } else {
                            g_painter->drawTextureCoords(m_coordsBuffer, lastObject.texture);
                        }

                        g_painter->restoreSavedState();
                        m_coordsBuffer.clear();

                        if(last) break;
                    }

                    lastObject = obj;

                    if(obj.src.isNull()) {
                        m_coordsBuffer.addRect(obj.dest);
                    } else {
                        m_coordsBuffer.addRect(obj.dest, obj.src);
                    }
                }
                buffer.frame->release();
                objects.clear();
            }
        }

        if(buffer.dest.isNull())
            buffer.frame->draw();
        else {
            //glDisable(GL_BLEND);
            buffer.frame->draw(buffer.dest, buffer.src);
            //glEnable(GL_BLEND);
        }
    }
}
