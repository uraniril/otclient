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
    for(int_fast8_t i = -1; ++i < m_drawingData.size();) {
        m_drawingData[i].frame = g_framebuffers.createFrameBuffer();
    }

    m_drawingData[DRAWTYPE_LIGHT].frame->setCompositeMode(Painter::CompositionMode_Light);

    m_coordsBuffer.enableHardwareCaching();
}

void DrawPool::terminate()
{
    for(auto& data : m_drawingData) {
        data.frame = nullptr;
    }
}

void DrawPool::add(const Rect& dest, const TexturePtr& texture, const Rect& src)
{
    if(m_currentDrawType == -1) return;

    const size_t hash = generateHash(dest, texture, src);
    auto& drawingData = m_drawingData[m_currentDrawType];
    auto& list = drawingData.objects;

    if(!drawingData.redraw && !m_hashs.count(hash)) {
        drawingData.redraw = true;
    }

    auto drawObject = DrawObject{ texture, g_painter->getCurrentState() };
    if(!list.empty()) {
        auto& prevDrawObject = list[list.size() - 1];
        if(prevDrawObject == drawObject) {
            prevDrawObject.rects.emplace_back(dest, src);
            prevDrawObject.hashs.push_back(hash);
            return;
        }
    }

    drawObject.rects.emplace_back(dest, src);
    drawObject.hashs.push_back(hash);
    list.push_back(drawObject);
}

bool DrawPool::drawUp(DrawType type, Size size, const Rect& dest, const Rect& src)
{
    auto& drawingData = m_drawingData[type];
    drawingData.objects.clear();
    m_currentDrawType = -1;

    bool canUpdate = drawingData.frame->canUpdate();
    if(canUpdate) {
        drawingData.frame->resize(size);
        drawingData.dest = dest;
        drawingData.src = src;
        m_currentDrawType = type;
    }

    return canUpdate;
}

void DrawPool::update()
{
    for(const auto& buffer : m_drawingData) {
        if(buffer.frame->isValid()) buffer.frame->update();
    }
}

void DrawPool::draw()
{
    g_painter->saveAndResetState();
    for(uint8 type = DRAWTYPE_MAP; type < DRAWTYPE_LAST; ++type) {
        auto& drawingData = m_drawingData[type];
        if(!drawingData.frame->isValid()) continue;

        if(drawingData.redraw) {
            drawingData.redraw = false;

            auto& objects = drawingData.objects;
            if(!objects.empty()) {
                m_hashs.clear();

                drawingData.frame->bind();

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

                    for(const auto& hash : obj.hashs) {
                        m_hashs.insert(hash);
                    }
                }

                drawingData.frame->release();

                objects.clear();
            }
        }

        if(drawingData.dest.isNull())
            drawingData.frame->draw();
        else {
            glDisable(GL_BLEND);
            drawingData.frame->draw(drawingData.dest, drawingData.src);
            glEnable(GL_BLEND);
        }
    }
    g_painter->restoreSavedState();
}

size_t DrawPool::generateHash(const Rect& dest, const TexturePtr& texture, const Rect& src)
{
    const auto& currentState = g_painter->getCurrentState();
    const auto hashInt = std::hash<size_t>();

    size_t hash = 0;
    boost::hash_combine(hash, hashInt(dest.x()));
    boost::hash_combine(hash, hashInt(dest.y()));
    if(!src.isNull()) {
        boost::hash_combine(hash, hashInt(src.x()));
        boost::hash_combine(hash, hashInt(src.y()));
    }
    boost::hash_combine(hash, hashInt(texture->getId()));

    boost::hash_combine(hash, hashInt(currentState.alphaWriting));
    boost::hash_combine(hash, hashInt(currentState.blendEquation));
    boost::hash_combine(hash, hashInt(currentState.color.rgba()));
    boost::hash_combine(hash, hashInt(currentState.compositionMode));
    boost::hash_combine(hash, hashInt(currentState.opacity));
    boost::hash_combine(hash, hashInt(currentState.resolution.area()));
    if(currentState.shaderProgram)
        boost::hash_combine(hash, hashInt(currentState.shaderProgram->getProgramId()));

    if(!currentState.clipRect.isNull()) {
        boost::hash_combine(hash, hashInt(currentState.clipRect.x()));
        boost::hash_combine(hash, hashInt(currentState.clipRect.y()));
    }

    return hash;
}
