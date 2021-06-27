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

	m_drawingData[DRAWTYPE_MAP].frame->disableBlend();
	m_drawingData[DRAWTYPE_LIGHT].frame->setCompositeMode(Painter::CompositionMode_Light);
}

void DrawPool::terminate()
{
	for(auto& data : m_drawingData) {
		data.destroy();
	}
}

void DrawPool::add(const CoordsBufferPtr& coordsBuffer, const TexturePtr& texture, const DrawMethod& method, const Painter::DrawMode drawMode)
{
	if(m_currentDrawType == -1) return;

	updateHash(texture, method);

	auto& drawingData = m_drawingData[m_currentDrawType];
	auto& list = drawingData.objects;

	auto currentState = g_painter->getCurrentState();

	currentState.texture = texture;
	std::shared_ptr<DrawObject> drawObject;

	if(!method.type == DrawMethodType::DRAW_TEXTURE_COORDS && !method.type == DrawMethodType::DRAW_FILL_COORDS) {
		if(!list.empty()) {
			auto& prevDrawObject = drawingData.objects.back();

			if(prevDrawObject->state.isEqual(currentState))
				drawObject = prevDrawObject;
		}
	}

	if(!drawObject) {
		drawObject = std::make_shared<DrawObject>();
		drawObject->state = currentState;
		drawObject->coordsBuffer = coordsBuffer;
		drawObject->drawMode = drawMode;
		if(texture)
			drawObject->drawMode = Painter::TriangleStrip;

		drawingData.objects.push_back(drawObject);
	} else {
		drawObject->drawMode = Painter::Triangles;
	}

	drawObject->drawMethods.push_back(method);
}

bool DrawPool::drawUp(DrawType type, Size size, const Rect& dest, const Rect& src)
{
	auto& drawingData = m_drawingData[type];
	m_currentDrawType = -1;

	bool canUpdate = drawingData.frame->canUpdate();
	if(canUpdate) {
		drawingData.frame->resize(size);
		drawingData.objects.clear();
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

void DrawPool::draw()
{
	g_painter->saveAndResetState();
	for(uint8 type = DRAWTYPE_MAP; type < DRAWTYPE_LAST; ++type) {
		auto& drawingData = m_drawingData[type];
		if(!drawingData.frame->isValid() || !drawingData.frame->isDrawable()) continue;

		if(drawingData.currentHashcode != drawingData.lastHashcode) {
			drawingData.lastHashcode = drawingData.currentHashcode;
			auto& objects = drawingData.objects;
			if(!objects.empty()) {
				drawingData.frame->bind();
				for(auto& obj : objects) {
					g_painter->executeState(obj->state);

					CoordsBuffer& coords = obj->coordsBuffer ? *obj->coordsBuffer.get() : m_coordsBuffer;

					if(!obj->coordsBuffer) {
						m_coordsBuffer.clear();
						for(const auto& method : obj->drawMethods) {
							if(method.type == DrawMethodType::DRAW_BOUNDING_RECT) {
								coords.addBoudingRect(method.rects.first, method.innerLineWidth);
							} else if(method.type == DrawMethodType::DRAW_FILLED_RECT) {
								coords.addRect(method.rects.first);
							} else if(method.type == DrawMethodType::DRAW_FILLED_TRIANGLE) {
								coords.addTriangle(std::get<0>(method.points), std::get<1>(method.points), std::get<2>(method.points));
							} else if(method.type == DrawMethodType::DRAW_REPEATED_TEXTURED_RECT) {
								coords.addRepeatedRects(method.rects.first, method.rects.second);
							} else if(method.type == DrawMethodType::DRAW_TEXTURED_RECT) {
								if(obj->drawMode == Painter::Triangles)
									coords.addRect(method.rects.first, method.rects.second);
								else
									coords.addQuad(method.rects.first, method.rects.second);
							} else if(method.type == DrawMethodType::DRAW_UPSIDEDOWN_TEXTURED_RECT) {
								if(obj->drawMode == Painter::Triangles)
									coords.addUpsideDownRect(method.rects.first, method.rects.second);
								else
									coords.addUpsideDownQuad(method.rects.first, method.rects.second);
							}
						}
					}

					g_painter->drawCoords(coords, obj->drawMode);
				}
				drawingData.frame->release();
			}
		}

		if(drawingData.dest.isNull())
			drawingData.frame->draw();
		else {
			drawingData.frame->draw(drawingData.dest, drawingData.src);
		}
	}

	g_painter->restoreSavedState();
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

	if(method.rects.first.isValid()) {
		boost::hash_combine(drawingData.currentHashcode, HASH_INT(method.rects.first.x()));
		boost::hash_combine(drawingData.currentHashcode, HASH_INT(method.rects.first.y()));
	}

	if(method.rects.second.isValid()) {
		boost::hash_combine(drawingData.currentHashcode, HASH_INT(method.rects.second.x()));
		boost::hash_combine(drawingData.currentHashcode, HASH_INT(method.rects.second.y()));
	}

	if(currentState.clipRect.isValid()) {
		boost::hash_combine(drawingData.currentHashcode, HASH_INT(currentState.clipRect.x()));
		boost::hash_combine(drawingData.currentHashcode, HASH_INT(currentState.clipRect.y()));
	}
}

void DrawPool::addFillCoords(const CoordsBuffer& coordsBuffer)
{
	DrawMethod method;
	method.type = DRAW_FILL_COORDS;
	add(std::make_shared<CoordsBuffer>(coordsBuffer), nullptr, method);
}

void DrawPool::addTextureCoords(const CoordsBuffer& coordsBuffer, const TexturePtr& texture, const Painter::DrawMode drawMode)
{
	if(texture && texture->isEmpty())
		return;

	DrawMethod method;
	method.type = DRAW_TEXTURE_COORDS;
	add(std::make_shared<CoordsBuffer>(coordsBuffer), texture, method, drawMode);
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
	method.type = DRAW_TEXTURED_RECT;
	method.rects = std::make_pair(dest, src);

	add(nullptr, texture, method);
}

void DrawPool::addUpsideDownTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src)
{
	if(dest.isEmpty() || src.isEmpty() || texture->isEmpty())
		return;

	DrawMethod method;
	method.type = DRAW_UPSIDEDOWN_TEXTURED_RECT;
	method.rects = std::make_pair(dest, Rect(Point(), texture->getSize()));

	add(nullptr, texture, method);
}

void DrawPool::addRepeatedTexturedRect(const Rect& dest, const TexturePtr& texture, const Rect& src)
{
	if(dest.isEmpty() || src.isEmpty() || texture->isEmpty())
		return;

	DrawMethod method;
	method.type = DRAW_REPEATED_TEXTURED_RECT;
	method.rects = std::make_pair(dest, Rect(Point(), texture->getSize()));

	add(nullptr, texture, method);
}

void DrawPool::addFilledRect(const Rect& dest)
{
	if(dest.isEmpty())
		return;

	DrawMethod method;
	method.type = DRAW_FILLED_RECT;
	method.rects = std::make_pair(dest, Rect());

	add(nullptr, nullptr, method);
}

void DrawPool::addFilledTriangle(const Point& a, const Point& b, const Point& c)
{
	if(a == b || a == c || b == c)
		return;

	DrawMethod method;
	method.type = DRAW_FILLED_TRIANGLE;
	method.points = std::make_tuple(a, b, c);

	add(nullptr, nullptr, method);
}

void DrawPool::addBoundingRect(const Rect& dest, int innerLineWidth)
{
	if(dest.isEmpty() || innerLineWidth == 0)
		return;

	DrawMethod method;
	method.type = DRAW_BOUNDING_RECT;
	method.rects = std::make_pair(dest, Rect());
	method.innerLineWidth = innerLineWidth;

	add(nullptr, nullptr, method);
}
