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
#include <unordered_set>

using CoordsBufferPtr = std::shared_ptr<CoordsBuffer>;

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

	void addFillCoords(const CoordsBuffer& coordsBuffer);
	void addTextureCoords(const CoordsBuffer& coordsBuffer, const TexturePtr& texture, Painter::DrawMode drawMode = Painter::Triangles);

	void addTexturedRect(const Rect& dest, const TexturePtr& texture);
	void addTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src);
	void addUpsideDownTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src);
	void addRepeatedTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src);
	void addFilledRect(const Rect& dest);
	void addFilledTriangle(const Point& a, const Point& b, const Point& c);
	void addBoundingRect(const Rect& dest, int innerLineWidth = 1);

	void draw();
	void setColorClear(const DrawType type, const Color color) { m_drawingData[type].frame->setColorClear(color); }
	bool drawUp(DrawType type, Size size) { drawUp(type, size, Rect(), Rect()); }
	bool drawUp(DrawType type, Size size, const Rect& dest, const Rect& src);
	void update();

	FrameBufferPtr getFrameBuffer(const DrawType type) { return m_drawingData[type].frame; }

private:
	enum DrawMethodType {
		DRAW_FILL_COORDS,
		DRAW_TEXTURE_COORDS,
		DRAW_TEXTURED_RECT,
		DRAW_UPSIDEDOWN_TEXTURED_RECT,
		DRAW_REPEATED_TEXTURED_RECT,
		DRAW_FILLED_RECT,
		DRAW_FILLED_TRIANGLE,
		DRAW_BOUNDING_RECT
	};

	struct DrawMethod {
		DrawMethodType type;
		std::pair<Rect, Rect> rects;
		std::tuple<Point, Point, Point> points;
		uint8 innerLineWidth;
	};

	struct DrawObject {
		/*~DrawObject() { coordsBuffer.clear(); }*/
		Painter::PainterState state;
		std::vector<DrawMethod> drawMethods;
		CoordsBufferPtr coordsBuffer;
		Painter::DrawMode drawMode{ Painter::Triangles };
	};

	struct DrawingData {
		Rect dest, src;
		FrameBufferPtr frame;
		std::vector<std::shared_ptr<DrawObject>> objects;
		size_t lastHashcode{ 0 }, currentHashcode{ 0 };

		void destroy() { frame = nullptr; objects.clear(); }
	};

	void updateHash(const TexturePtr& texture, const DrawMethod& method);

	void add(const CoordsBufferPtr& coordsBuffer, const TexturePtr& texture, const DrawMethod& method, const Painter::DrawMode drawMode = Painter::Triangles);

	std::array<DrawingData, DRAWTYPE_LAST> m_drawingData;

	int8_t m_currentDrawType{ -1 };

	CoordsBuffer m_coordsBuffer;

	const DrawObject m_nullDrawObject;
};

extern DrawPool g_drawPool;

#endif
