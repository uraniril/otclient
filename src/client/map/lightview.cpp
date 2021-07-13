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

#include <framework/graphics/framebuffer.h>
#include <framework/graphics/framebuffermanager.h>
#include <client/map/lightview.h>
#include <client/map/mapview.h>
#include <client/map/map.h>

LightView::LightView(const MapViewPtr& mapView) :
	m_lightbuffer(g_framebuffers.createFrameBuffer()), m_mapView(mapView)
{
	m_lightbuffer->setCompositionMode(Painter::CompositionMode_Light);
	resize();
}

void LightView::addLightSource(const Point& pos, const Light& light)
{
	if(!isDark()) return;

	const uint16 radius = light.intensity * SPRITE_SIZE * m_mapView->m_scaleFactor;

	auto& lights = m_lights[m_currentFloor];
	if(!lights.empty()) {
		auto& prevLight = lights.back();
		if(prevLight.pos == pos && prevLight.color == light.color) {
			prevLight.radius = std::max<uint16>(prevLight.radius, radius);
			return;
		}
	}

	lights.push_back(LightSource{ pos , light.color, radius, light.brightness });
}

void LightView::setShade(const Point& point)
{
	const size_t index = (m_mapView->m_drawDimension.width() * (point.y / m_mapView->m_tileSize)) + (point.x / m_mapView->m_tileSize);
	if(index >= m_shades.size()) return;
	m_shades[index] = ShadeBlock{ m_currentFloor, point };
}

void LightView::resize()
{
	m_lightbuffer->resize(m_mapView->m_framebuffer->getSize());
	m_shades.resize(m_mapView->m_drawDimension.area());
}
