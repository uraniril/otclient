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

#ifndef DRAWPOOL_H
#define DRAWPOOL_H

#include <framework/graphics/declarations.h>
#include <framework/graphics/graphics.h>
#include <framework/graphics/framebuffer.h>

enum DrawType {
    DRAWTYPE_MAP,
    DRAWTYPE_LIGHT,
    DRAWTYPE_LAST
};

class DrawPool
{
public:
    DrawPool();
    void terminate();

    void addTexturedRect(const Rect& dest, const TexturePtr& texture) { add(dest, texture, Rect(Point(), texture->getSize())); }
    void addTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src) { add(dest, texture, src); }
    void addFilledRect(const Rect& dest) { add(dest, nullptr, Rect()); }
    void add(const Rect& dest, const TexturePtr& texture, const Rect& src);

    void draw();
    void setColorClear(const DrawType type, const Color color) { m_framebuffers[type].frame->setColorClear(color); }
    bool drawUp(DrawType type, Size size) { drawUp(type, size, Rect(), Rect()); }
    bool drawUp(DrawType type, Size size, const Rect& dest, const Rect& src);
    void update();
    FrameBufferPtr getFrameBuffer(const DrawType type) { return m_framebuffers[type].frame; }

private:
    struct DrawObject {
        TexturePtr texture;
        Painter::PainterState state;
        std::vector<std::pair<Rect, Rect>> rects;

        bool operator==(const DrawObject& o)
        {
            return o.state.isEqual(state) && o.texture == texture;
        }
    };

    struct FrameBufferData {
        Rect dest;
        Rect src;
        FrameBufferPtr frame;
    };

    void saveState(DrawObject obj);

    CoordsBuffer m_coordsBuffer;

    std::array<FrameBufferData, DRAWTYPE_LAST> m_framebuffers;
    std::array<std::vector<DrawObject>, DRAWTYPE_LAST> m_drawObjects;

    int8_t m_currentDrawType{ -1 };

    const DrawObject m_nullDrawObject;
};

extern DrawPool g_drawPool;

#endif
