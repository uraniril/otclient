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
#include <client/config.h>

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

void DrawPool::add(const TexturePtr& texture, const DrawMethod& method)
{
    if(m_currentDrawType == -1) return;

    auto& drawingData = m_drawingData[m_currentDrawType];
    auto& list = drawingData.objects;

    auto drawObject = DrawObject{ generateHash(texture, method), texture, g_painter->getCurrentState() };

    if(!drawingData.redraw && !m_hashs.count(drawObject.id)) {
        drawingData.redraw = true;
    } else {
        if(!list.empty()) {
            auto& prevDrawObject = list.back();
            if(prevDrawObject.id == drawObject.id) {
                prevDrawObject.methods.push_back(method);
                return;
            }
        }
    }

    drawObject.methods.push_back(method);
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
            m_hashs.clear();

            auto& objects = drawingData.objects;
            if(!objects.empty()) {
                drawingData.frame->bind();
                for(const auto& obj : objects) {
                    g_painter->executeState(obj.state);
                    for(const auto& method : obj.methods) {
                        if(method.type == DrawMethodType::DRAW_BOUNDING_RECT) {
                            m_coordsBuffer.addBoudingRect(method.rects.first, method.innerLineWidth);
                        } else if(method.type == DrawMethodType::DRAW_FILLED_RECT) {
                            m_coordsBuffer.addRect(method.rects.first);
                        } else if(method.type == DrawMethodType::DRAW_FILLED_TRIANGLE) {
                            m_coordsBuffer.addTriangle(std::get<0>(method.points), std::get<1>(method.points), std::get<2>(method.points));
                        } else if(method.type == DrawMethodType::DRAW_FILL_COORDS) {
                            //m_coordsBuffer.addRect(method.rects.first);
                        } else if(method.type == DrawMethodType::DRAW_REPEATED_TEXTURED_RECT) {
                            m_coordsBuffer.addRepeatedRects(method.rects.first, method.rects.second);
                        } else if(method.type == DrawMethodType::DRAW_TEXTURED_RECT) {
                            m_coordsBuffer.addRect(method.rects.first, method.rects.second);
                        } else if(method.type == DrawMethodType::DRAW_TEXTURE_COORDS) {
                            //m_coordsBuffer.addRect(method.rects.first);
                        } else if(method.type == DrawMethodType::DRAW_UPSIDEDOWN_TEXTURED_RECT) {
                            m_coordsBuffer.addUpsideDownQuad(method.rects.first, method.rects.second);
                        }
                    }

                    if(obj.texture == nullptr) {
                        g_painter->drawFillCoords(m_coordsBuffer);
                    } else {
                        g_painter->drawTextureCoords(m_coordsBuffer, obj.texture);
                    }

                    m_coordsBuffer.clear();
                    m_hashs.insert(obj.id);
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

size_t DrawPool::generateHash(const TexturePtr& texture, const DrawMethod& method)
{
    const auto& currentState = g_painter->getCurrentState();
    const auto hashInt = std::hash<size_t>();

    size_t hash = 0;

    boost::hash_combine(hash, hashInt(currentState.opacity));
    boost::hash_combine(hash, hashInt(currentState.color.rgba()));
    boost::hash_combine(hash, hashInt(currentState.compositionMode));

    if(method.rects.first.isValid()) {
        boost::hash_combine(hash, hashInt(method.rects.first.x()));
        boost::hash_combine(hash, hashInt(method.rects.first.y()));
    }
    if(method.rects.second.isValid()) {
        boost::hash_combine(hash, hashInt(method.rects.second.x()));
        boost::hash_combine(hash, hashInt(method.rects.second.y()));
    }

    if(currentState.clipRect.isValid()) {
        boost::hash_combine(hash, hashInt(currentState.clipRect.x()));
        boost::hash_combine(hash, hashInt(currentState.clipRect.y()));
    }

    if(texture) {
        boost::hash_combine(hash, hashInt(texture->getId()));
    }

    if(currentState.shaderProgram) {
        boost::hash_combine(hash, hashInt(currentState.shaderProgram->getProgramId()));
    }

    return hash;
}

void DrawPool::addFillCoords(const CoordsBuffer& coordsBuffer)
{
}
void DrawPool::addTextureCoords(const CoordsBuffer& coordsBuffer, const TexturePtr& texture, Painter::DrawMode drawMode)
{
}

void DrawPool::addTexturedRect(const Rect& dest, const TexturePtr& texture)
{
    DrawMethod method;
    method.type = DRAW_TEXTURED_RECT;
    method.rects = std::make_pair(dest, Rect(Point(), texture->getSize()));

    add(texture, method);
}
void DrawPool::addTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src)
{
    DrawMethod method;
    method.type = DRAW_TEXTURED_RECT;
    method.rects = std::make_pair(dest, src);

    add(texture, method);
}
void DrawPool::addUpsideDownTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src)
{
    DrawMethod method;
    method.type = DRAW_UPSIDEDOWN_TEXTURED_RECT;
    method.rects = std::make_pair(dest, Rect(Point(), texture->getSize()));

    add(texture, method);
}
void DrawPool::addRepeatedTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src)
{
    DrawMethod method;
    method.type = DRAW_REPEATED_TEXTURED_RECT;
    method.rects = std::make_pair(dest, Rect(Point(), texture->getSize()));

    add(texture, method);
}
void DrawPool::addFilledRect(const Rect& dest)
{
    DrawMethod method;
    method.type = DRAW_FILLED_RECT;
    method.rects = std::make_pair(dest, Rect());

    add(nullptr, method);
};
void DrawPool::addFilledTriangle(const Point& a, const Point& b, const Point& c)
{
    DrawMethod method;
    method.type = DRAW_FILLED_TRIANGLE;
    method.points = std::make_tuple(a, b, c);

    add(nullptr, method);
};
void DrawPool::addBoundingRect(const Rect& dest, int innerLineWidth)
{
}
