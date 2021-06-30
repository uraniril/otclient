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

void DrawPool::init() {}
void DrawPool::terminate() { m_currentFrameBuffer = nullptr; }

void DrawPool::add(const std::shared_ptr<CoordsBuffer>& coordsBuffer, const TexturePtr& texture, const FrameBuffer::ScheduledMethod& method, const Painter::DrawMode drawMode)
{
	if(!m_currentFrameBuffer || !m_currentFrameBuffer->isValid() ||
		 !m_currentFrameBuffer->isDrawable()) return;

	m_currentFrameBuffer->scheduleDrawing(coordsBuffer, texture, method, drawMode);
}

void DrawPool::setFrameBuffer(const FrameBufferPtr& frameBuffer)
{
	m_currentFrameBuffer = frameBuffer;
	if(frameBuffer)	frameBuffer->resetStatus();
}

void DrawPool::draw(const FrameBufferPtr& frameBuffer, const Rect& dest, const Rect& src)
{
	if(!frameBuffer->isValid() || !frameBuffer->isDrawable()) return;

	g_painter->saveAndResetState();
	if(frameBuffer->hasModification()) {
		frameBuffer->updateStatus();
		frameBuffer->bind();

		for(auto& obj : frameBuffer->getScheduledDrawings())
			drawObject(obj);

		if(m_onBind) {
			m_onBind();
		}

		frameBuffer->release();
	}

	m_onBind = []() {};

	frameBuffer->getScheduledDrawings().clear();
	frameBuffer->draw(dest, src);

	g_painter->restoreSavedState();
}

void DrawPool::drawObject(const FrameBuffer::ActionObject& obj)
{
	if(obj.drawMode != Painter::DrawMode::None)
		g_painter->executeState(obj.state);

	if(obj.coordsBuffer != nullptr) {
		g_painter->drawCoords(*obj.coordsBuffer, obj.drawMode);
	} else {
		for(const auto& method : obj.drawMethods) {
			if(method.type == DrawMethodType::GL_ENABLE) {
				glEnable(GL_BLEND);
				return;
			} else if(method.type == DrawMethodType::GL_DISABLE) {
				glDisable(GL_BLEND);
				return;
			} else if(method.type == DrawMethodType::DRAW_BOUNDING_RECT) {
				m_coordsBuffer.addBoudingRect(method.rects.first, method.intValue);
			} else if(method.type == DrawMethodType::DRAW_FILLED_RECT) {
				m_coordsBuffer.addRect(method.rects.first);
			} else if(method.type == DrawMethodType::DRAW_FILLED_TRIANGLE) {
				m_coordsBuffer.addTriangle(std::get<0>(method.points), std::get<1>(method.points), std::get<2>(method.points));
			} else if(method.type == DrawMethodType::DRAW_REPEATED_TEXTURED_RECT) {
				m_coordsBuffer.addRepeatedRects(method.rects.first, method.rects.second);
			} else if(method.type == DrawMethodType::DRAW_TEXTURED_RECT) {
				if(obj.drawMode == Painter::DrawMode::Triangles)
					m_coordsBuffer.addRect(method.rects.first, method.rects.second);
				else
					m_coordsBuffer.addQuad(method.rects.first, method.rects.second);
			} else if(method.type == DrawMethodType::DRAW_UPSIDEDOWN_TEXTURED_RECT) {
				if(obj.drawMode == Painter::DrawMode::Triangles)
					m_coordsBuffer.addUpsideDownRect(method.rects.first, method.rects.second);
				else
					m_coordsBuffer.addUpsideDownQuad(method.rects.first, method.rects.second);
			}
		}
	}

	g_painter->drawCoords(m_coordsBuffer, obj.drawMode);
	m_coordsBuffer.clear();
}

void DrawPool::addFillCoords(CoordsBuffer& coordsBuffer)
{
	FrameBuffer::ScheduledMethod method;
	method.type = DrawMethodType::DRAW_FILL_COORDS;
	add(std::shared_ptr<CoordsBuffer>(&coordsBuffer, [](CoordsBuffer*) {}), nullptr, method);
}

void DrawPool::addTextureCoords(CoordsBuffer& coordsBuffer, const TexturePtr& texture, const Painter::DrawMode drawMode)
{
	if(texture && texture->isEmpty())
		return;

	FrameBuffer::ScheduledMethod method;
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

	FrameBuffer::ScheduledMethod method;
	method.type = DrawMethodType::DRAW_TEXTURED_RECT;
	method.rects = std::make_pair(dest, src);

	add(nullptr, texture, method, Painter::DrawMode::TriangleStrip);
}

void DrawPool::addUpsideDownTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src)
{
	if(dest.isEmpty() || src.isEmpty() || texture->isEmpty())
		return;

	FrameBuffer::ScheduledMethod method;
	method.type = DrawMethodType::DRAW_UPSIDEDOWN_TEXTURED_RECT;
	method.rects = std::make_pair(dest, Rect(Point(), texture->getSize()));

	add(nullptr, texture, method, Painter::DrawMode::TriangleStrip);
}

void DrawPool::addRepeatedTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src)
{
	if(dest.isEmpty() || src.isEmpty() || texture->isEmpty())
		return;

	FrameBuffer::ScheduledMethod method;
	method.type = DrawMethodType::DRAW_REPEATED_TEXTURED_RECT;
	method.rects = std::make_pair(dest, Rect(Point(), texture->getSize()));

	add(nullptr, texture, method);
}

void DrawPool::addFilledRect(const Rect& dest)
{
	if(dest.isEmpty())
		return;

	FrameBuffer::ScheduledMethod method;
	method.type = DrawMethodType::DRAW_FILLED_RECT;
	method.rects = std::make_pair(dest, Rect());

	add(nullptr, nullptr, method);
}

void DrawPool::addFilledTriangle(const Point& a, const Point& b, const Point& c)
{
	if(a == b || a == c || b == c)
		return;

	FrameBuffer::ScheduledMethod method;
	method.type = DrawMethodType::DRAW_FILLED_TRIANGLE;
	method.points = std::make_tuple(a, b, c);

	add(nullptr, nullptr, method);
}

void DrawPool::addBoundingRect(const Rect& dest, int innerLineWidth)
{
	if(dest.isEmpty() || innerLineWidth == 0)
		return;

	FrameBuffer::ScheduledMethod method;
	method.type = DrawMethodType::DRAW_BOUNDING_RECT;
	method.rects = std::make_pair(dest, Rect());
	method.intValue = innerLineWidth;

	add(nullptr, nullptr, method);
}

void DrawPool::disableGL(GLenum cap)
{
	if(!m_currentFrameBuffer) return;

	FrameBuffer::ScheduledMethod method;
	method.type = DrawMethodType::GL_DISABLE;
	method.intValue = cap;

	m_currentFrameBuffer->scheduleMethod(method);
}
void DrawPool::enableGL(GLenum cap)
{
	if(!m_currentFrameBuffer) return;

	FrameBuffer::ScheduledMethod method;
	method.type = DrawMethodType::GL_ENABLE;
	method.intValue = cap;

	m_currentFrameBuffer->scheduleMethod(method);
}
