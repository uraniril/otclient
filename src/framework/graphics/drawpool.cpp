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

void DrawPool::init()
{
	for(int_fast8_t i = -1; ++i < DRAWTYPE_LAST;) {
		const bool useAlphaWriting = i != DRAWTYPE_LIGHT && i != DRAWTYPE_MAP;
		m_drawingData[i].frame = g_framebuffers.createFrameBuffer(useAlphaWriting);
	}

	m_drawingData[DRAWTYPE_MAP].frame->disableBlend();
	m_drawingData[DRAWTYPE_LIGHT].frame->setCompositeMode(Painter::CompositionMode_Light);
}

void DrawPool::terminate()
{
	for(auto& data : m_drawingData) {
		data.destroy();
	}
}

void DrawPool::add(const std::shared_ptr<CoordsBuffer>& coordsBuffer, const TexturePtr& texture, const DrawMethod& method, const Painter::DrawMode drawMode)
{
	if(m_currentDrawType == -1) return;

	updateHash(texture, method);

	auto& drawingData = m_drawingData[m_currentDrawType];
	auto& list = drawingData.objects;

	auto currentState = g_painter->getCurrentState();
	currentState.texture = texture;

	if(!list.empty() && !(method.type == DrawMethodType::DRAW_TEXTURE_COORDS || method.type == DrawMethodType::DRAW_FILL_COORDS)) {
		auto& prevDrawObject = drawingData.objects.back();

		if(prevDrawObject.state.isEqual(currentState)) {
			prevDrawObject.drawMode = Painter::Triangles;
			prevDrawObject.drawMethods.push_back(method);
			return;
		}
	}

	drawingData.objects.push_back(DrawObject{ currentState, coordsBuffer, drawMode, {method} });
}

bool DrawPool::drawUp(DrawType type, Size size, const Rect& dest, const Rect& src)
{
	auto& drawingData = m_drawingData[type];
	m_currentDrawType = -1;

	bool canUpdate = drawingData.frame->canUpdate();
	if(canUpdate) {
		drawingData.frame->resize(size);
		drawingData.dest = dest;
		drawingData.src = src;
		drawingData.currentHashcode = 0;
		m_currentDrawType = type;
	}

	return canUpdate;
}

void DrawPool::update()
{
	for(auto& buffer : m_drawingData) {
		if(buffer.frame->isValid())
			buffer.frame->update();
	}
}

void DrawPool::draw(const bool updateForeground, const TexturePtr& foregroundTexture)
{
	g_painter->saveAndResetState();

	Rect viewportRect(0, 0, g_painter->getResolution());
	int type = -1;
	for(auto& drawingData : m_drawingData) {
		++type;

		if(!drawingData.frame->isValid() || !drawingData.frame->isDrawable()) continue;

		auto& objects = drawingData.objects;
		if(objects.empty()) continue;

		if(drawingData.currentHashcode != drawingData.lastHashcode) {
			drawingData.lastHashcode = drawingData.currentHashcode;
			drawingData.frame->bind();
			for(auto& obj : objects)
				drawObject(obj);

			if(updateForeground && type == DrawType::DRAWTYPE_FOREGROUND) {
				foregroundTexture->copyFromScreen(viewportRect);
				g_painter->clear(Color::black);
				g_painter->setAlphaWriting(false);
			}

			drawingData.frame->release();
		}
		drawingData.objects.clear();

		if(drawingData.dest.isNull()) {
			drawingData.frame->draw();
		} else {
			drawingData.frame->draw(drawingData.dest, drawingData.src);
		}
	}

	if(foregroundTexture) {
		g_painter->resetColor();
		g_painter->resetOpacity();
		g_painter->drawTexturedRect(viewportRect, foregroundTexture, viewportRect);
	}

	g_painter->restoreSavedState();
}

void DrawPool::drawObject(const DrawObject& obj)
{
	g_painter->executeState(obj.state);
	if(obj.coordsBuffer != nullptr) {
		g_painter->drawCoords(*obj.coordsBuffer, obj.drawMode);
	} else {
		for(const auto& method : obj.drawMethods) {
			if(method.type == DrawMethodType::DRAW_BOUNDING_RECT) {
				m_coordsBuffer.addBoudingRect(method.rects.first, method.innerLineWidth);
			} else if(method.type == DrawMethodType::DRAW_FILLED_RECT) {
				m_coordsBuffer.addRect(method.rects.first);
			} else if(method.type == DrawMethodType::DRAW_FILLED_TRIANGLE) {
				m_coordsBuffer.addTriangle(std::get<0>(method.points), std::get<1>(method.points), std::get<2>(method.points));
			} else if(method.type == DrawMethodType::DRAW_REPEATED_TEXTURED_RECT) {
				m_coordsBuffer.addRepeatedRects(method.rects.first, method.rects.second);
			} else if(method.type == DrawMethodType::DRAW_TEXTURED_RECT) {
				if(obj.drawMode == Painter::Triangles)
					m_coordsBuffer.addRect(method.rects.first, method.rects.second);
				else
					m_coordsBuffer.addQuad(method.rects.first, method.rects.second);
			} else if(method.type == DrawMethodType::DRAW_UPSIDEDOWN_TEXTURED_RECT) {
				if(obj.drawMode == Painter::Triangles)
					m_coordsBuffer.addUpsideDownRect(method.rects.first, method.rects.second);
				else
					m_coordsBuffer.addUpsideDownQuad(method.rects.first, method.rects.second);
			}
		}
	}

	g_painter->drawCoords(m_coordsBuffer, obj.drawMode);
	m_coordsBuffer.clear();
}

void DrawPool::updateHash(const TexturePtr& texture, const DrawMethod& method)
{
	const auto& currentState = g_painter->getCurrentState();
	auto& drawingData = m_drawingData[m_currentDrawType];

	if(texture)
		boost::hash_combine(drawingData.currentHashcode, HASH_INT(texture->getId()));

	if(currentState.opacity < 1.f)
		boost::hash_combine(drawingData.currentHashcode, HASH_INT(currentState.opacity));

	if(currentState.color != Color::white)
		boost::hash_combine(drawingData.currentHashcode, HASH_INT(currentState.color.rgba()));

	if(currentState.compositionMode != Painter::CompositionMode_Normal)
		boost::hash_combine(drawingData.currentHashcode, HASH_INT(currentState.compositionMode));

	if(currentState.shaderProgram)
		boost::hash_combine(drawingData.currentHashcode, HASH_INT(currentState.shaderProgram->getProgramId()));

	if(currentState.clipRect.isValid()) {
		boost::hash_combine(drawingData.currentHashcode, HASH_INT(currentState.clipRect.x()));
		boost::hash_combine(drawingData.currentHashcode, HASH_INT(currentState.clipRect.y()));
	}

	if(method.rects.first.isValid()) {
		boost::hash_combine(drawingData.currentHashcode, HASH_INT(method.rects.first.x()));
		boost::hash_combine(drawingData.currentHashcode, HASH_INT(method.rects.first.y()));
	}

	if(method.rects.second.isValid()) {
		boost::hash_combine(drawingData.currentHashcode, HASH_INT(method.rects.second.x()));
		boost::hash_combine(drawingData.currentHashcode, HASH_INT(method.rects.second.y()));
	}

	const auto& a = std::get<0>(method.points),
		b = std::get<1>(method.points),
		c = std::get<2>(method.points);

	if(!a.isNull()) {
		boost::hash_combine(drawingData.currentHashcode, HASH_INT(a.x));
		boost::hash_combine(drawingData.currentHashcode, HASH_INT(a.y));
	}
	if(!b.isNull()) {
		boost::hash_combine(drawingData.currentHashcode, HASH_INT(b.x));
		boost::hash_combine(drawingData.currentHashcode, HASH_INT(b.y));
	}
	if(!c.isNull()) {
		boost::hash_combine(drawingData.currentHashcode, HASH_INT(c.x));
		boost::hash_combine(drawingData.currentHashcode, HASH_INT(c.y));
	}
}

void DrawPool::addFillCoords(CoordsBuffer& coordsBuffer)
{
	DrawMethod method;
	method.type = DrawMethodType::DRAW_FILL_COORDS;
	add(std::shared_ptr<CoordsBuffer>(&coordsBuffer, [](CoordsBuffer*) {}), nullptr, method);
}

void DrawPool::addTextureCoords(CoordsBuffer& coordsBuffer, const TexturePtr& texture, const Painter::DrawMode drawMode)
{
	if(texture && texture->isEmpty())
		return;

	DrawMethod method;
	method.type = DrawMethodType::DRAW_TEXTURE_COORDS;
	add(std::shared_ptr<CoordsBuffer>(&coordsBuffer, [](CoordsBuffer*) {}), texture, method, drawMode);
}

void DrawPool::addTexturedRect(const Rect& dest, const TexturePtr& texture)
{
	addTexturedRect(dest, texture, Rect(Point(), texture->getSize()));
}

void DrawPool::addTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src)
{
	if(dest.isEmpty() || src.isEmpty() || texture->isEmpty())
		return;

	DrawMethod method;
	method.type = DrawMethodType::DRAW_TEXTURED_RECT;
	method.rects = std::make_pair(dest, src);

	add(nullptr, texture, method, Painter::TriangleStrip);
}

void DrawPool::addUpsideDownTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src)
{
	if(dest.isEmpty() || src.isEmpty() || texture->isEmpty())
		return;

	DrawMethod method;
	method.type = DrawMethodType::DRAW_UPSIDEDOWN_TEXTURED_RECT;
	method.rects = std::make_pair(dest, Rect(Point(), texture->getSize()));

	add(nullptr, texture, method, Painter::TriangleStrip);
}

void DrawPool::addRepeatedTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src)
{
	if(dest.isEmpty() || src.isEmpty() || texture->isEmpty())
		return;

	DrawMethod method;
	method.type = DrawMethodType::DRAW_REPEATED_TEXTURED_RECT;
	method.rects = std::make_pair(dest, Rect(Point(), texture->getSize()));

	add(nullptr, texture, method);
}

void DrawPool::addFilledRect(const Rect& dest)
{
	if(dest.isEmpty())
		return;

	DrawMethod method;
	method.type = DrawMethodType::DRAW_FILLED_RECT;
	method.rects = std::make_pair(dest, Rect());

	add(nullptr, nullptr, method);
}

void DrawPool::addFilledTriangle(const Point& a, const Point& b, const Point& c)
{
	if(a == b || a == c || b == c)
		return;

	DrawMethod method;
	method.type = DrawMethodType::DRAW_FILLED_TRIANGLE;
	method.points = std::make_tuple(a, b, c);

	add(nullptr, nullptr, method);
}

void DrawPool::addBoundingRect(const Rect& dest, int innerLineWidth)
{
	if(dest.isEmpty() || innerLineWidth == 0)
		return;

	DrawMethod method;
	method.type = DrawMethodType::DRAW_BOUNDING_RECT;
	method.rects = std::make_pair(dest, Rect());
	method.innerLineWidth = innerLineWidth;

	add(nullptr, nullptr, method);
}
