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

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "declarations.h"
#include "texture.h"
#include "painter.h"
#include <framework/core/scheduledevent.h>
#include <framework/core/timer.h>
#include <client/const.h>

enum class DrawMethodType {
	DRAW_FILL_COORDS,
	DRAW_TEXTURE_COORDS,
	DRAW_TEXTURED_RECT,
	DRAW_UPSIDEDOWN_TEXTURED_RECT,
	DRAW_REPEATED_TEXTURED_RECT,
	DRAW_FILLED_RECT,
	DRAW_FILLED_TRIANGLE,
	DRAW_BOUNDING_RECT,
	CLEAR_AREA,
	GL_DISABLE,
	GL_ENABLE
};

class FrameBuffer : public stdext::shared_object
{
public:
	enum class ScheduledMethodType {
		DRAW_FILL_COORDS,
		DRAW_TEXTURE_COORDS,
		DRAW_TEXTURED_RECT,
		DRAW_UPSIDEDOWN_TEXTURED_RECT,
		DRAW_REPEATED_TEXTURED_RECT,
		DRAW_FILLED_RECT,
		DRAW_FILLED_TRIANGLE,
		DRAW_BOUNDING_RECT,
		CLEAR_AREA,
		GL_DISABLE,
		GL_ENABLE
	};

	struct ScheduledMethod {
		DrawMethodType type;
		std::pair<Rect, Rect> rects;
		std::tuple<Point, Point, Point> points;
		uint intValue;
	};

	struct ActionObject {
		~ActionObject() { drawMethods.clear(); coordsBuffer = nullptr; state.texture = nullptr; }

		Painter::PainterState state;
		std::shared_ptr<CoordsBuffer> coordsBuffer;
		Painter::DrawMode drawMode{ Painter::DrawMode::Triangles };
		std::vector<ScheduledMethod> drawMethods;
	};

	~FrameBuffer() override;

	void resize(const Size& size);
	void bind(bool autoClear = true);
	void release();
	void clear(Color color = Color::black);
	void draw(const Rect& dest, const Rect& src);

	void setBackuping(bool enabled) { m_backuping = enabled; }
	void setSmooth(bool enabled) { m_smooth = enabled; }

	TexturePtr getTexture() { return m_texture; }
	Size getSize();
	bool isBackuping() { return m_backuping; }
	bool isSmooth() { return m_smooth; }

	bool canUpdate();
	void update() { m_forceUpdate = true; }
	void cleanTexture() { m_texture = nullptr; }
	bool isValid() const { return m_texture != nullptr; }
	void setColorClear(const Color color) { m_colorClear = color; }
	void setCompositeMode(const Painter::CompositionMode mode) { m_compositeMode = mode; }
	void disableBlend() { m_disableBlend = true; }

	void setDrawable(const bool v) { m_drawable = v; }
	bool isDrawable() const { return m_drawable; }

	void scheduleMethod(const ScheduledMethod& method);
	void scheduleDrawing(const std::shared_ptr<CoordsBuffer>& coordsBuffer, const TexturePtr& texture, const ScheduledMethod& method, const Painter::DrawMode drawMode = Painter::DrawMode::Triangles);

	bool hasModification() const { return m_statusHashCode != m_currentStatusHashcode; }
	void updateStatus() { m_statusHashCode = m_currentStatusHashcode; }
	void resetStatus() { m_currentStatusHashcode = 0; }

	std::vector<ActionObject>& getScheduledDrawings() { return m_actionObjects; }

protected:
	FrameBuffer(bool useAlphaWriting, uint16_t minTimeUpdate);

	friend class FrameBufferManager;

private:
	void internalCreate();
	void internalBind();
	void internalRelease();
	void updateHash(const TexturePtr& texture, const ScheduledMethod& method);

	static uint boundFbo;

	TexturePtr m_texture, m_screenBackup;

	Size m_oldViewportSize;

	uint32 m_fbo, m_prevBoundFbo;
	uint16_t m_minTimeUpdate;

	Timer m_lastRenderedTime;
	Color m_colorClear = { Color::black };
	Painter::CompositionMode m_compositeMode{ Painter::CompositionMode_Normal };

	std::vector<ActionObject> m_actionObjects;
	size_t m_statusHashCode{ 0 }, m_currentStatusHashcode{ 0 };

	bool m_forceUpdate{ true },
		m_backuping{ true },
		m_smooth{ true },
		m_useAlphaWriting{ false },
		m_disableBlend{ false },
		m_drawable{ true };

	std::hash<size_t> HASH_INT;
};

#endif
