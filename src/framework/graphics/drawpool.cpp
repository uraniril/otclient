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

    m_coordsBuffer.enableHardwareCaching();
}

void DrawPool::terminate()
{
    for(int_fast8_t i = -1; ++i < m_framebuffers.size();) {
        m_framebuffers[i].frame = nullptr;
    }
}

void DrawPool::add(const Rect& dest, const TexturePtr& texture, const Rect& src)
{
    if(m_currentDrawType == -1) return;

    auto& list = m_drawObjects[m_currentDrawType];

    auto drawObject = DrawObject{ texture, g_painter->getCurrentState() };
    if(!list.empty()) {
        auto& prevDrawObject = list[list.size() - 1];
        if(prevDrawObject == drawObject) {
            prevDrawObject.rects.emplace_back(dest, src);
            return;
        }
    }

    drawObject.rects.emplace_back(dest, src);
    list.push_back(drawObject);
}

bool DrawPool::drawUp(DrawType type, Size size, const Rect& dest, const Rect& src)
{
    auto& dataBuffer = m_framebuffers[type];
    m_drawObjects[type].clear();
    m_currentDrawType = -1;

    bool canUpdate = dataBuffer.frame->canUpdate();
    if(canUpdate) {
        dataBuffer.frame->resize(size);
        dataBuffer.dest = dest;
        dataBuffer.src = src;
        m_currentDrawType = type;
    }

    return canUpdate;
}

void DrawPool::update()
{
    for(const auto& buffer : m_framebuffers) {
        if(buffer.frame->isValid()) buffer.frame->update();
    }
}

void DrawPool::draw()
{
    g_painter->saveAndResetState();
    for(uint8 type = DRAWTYPE_MAP; type < DRAWTYPE_LAST; ++type) {
        const auto& buffer = m_framebuffers[type];
        if(!buffer.frame->isValid()) continue;

        auto& objects = m_drawObjects[type];
        const size_t objSize = objects.size();
        if(objSize > 0) {
            buffer.frame->bind();

            for(const auto& obj : objects) {
                g_painter->executeState(obj.state);
                for(const auto& rect : obj.rects) {
                    if(rect.second.isNull()) {
                        m_coordsBuffer.addRect(rect.first);
                    } else {
                        m_coordsBuffer.addRect(rect.first, rect.second);
                    }
                }

                if(obj.texture == nullptr) {
                    g_painter->drawFillCoords(m_coordsBuffer);
                } else {
                    g_painter->drawTextureCoords(m_coordsBuffer, obj.texture);
                }

                m_coordsBuffer.clear();
            }

            buffer.frame->release();

            objects.clear();
        }

        if(buffer.dest.isNull())
            buffer.frame->draw();
        else {
            //glDisable(GL_BLEND);
            buffer.frame->draw(buffer.dest, buffer.src);
            //glEnable(GL_BLEND);
        }
    }
    g_painter->restoreSavedState();
}
