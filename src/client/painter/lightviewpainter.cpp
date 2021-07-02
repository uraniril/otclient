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

#include <client/painter/lightviewpainter.h>
#include <client/map/mapview.h>

#include <framework/core/declarations.h>
#include <framework/graphics/image.h>
#include <framework/graphics/graphics.h>
#include <framework/graphics/declarations.h>

#include <framework/graphics/drawpool.h>

LightViewPainter g_lightViewPaint;

void LightViewPainter::init()
{
    generateLightTexture();
    generateShadeTexture();
}

void LightViewPainter::terminate()
{
    m_shadeTexture = nullptr;
    m_lightTexture = nullptr;
}

void LightViewPainter::draw(const LightViewPtr& lightView, const Rect& dest, const Rect& src)
{
    // draw light, only if there is darkness
    if(!lightView || !lightView->isDark()) return;

    if(g_drawPool.canFill(lightView->m_lightbuffer)) {
        lightView->m_lightbuffer->setColorClear(lightView->m_globalLightColor);

        const auto& mapView = lightView->m_mapView;
        const auto& shadeBase = std::make_pair<Point, Size>(Point(mapView->getTileSize() / 4.8), Size(mapView->getTileSize() * 1.4));
        for(int_fast8_t z = mapView->getFloorMax(); z >= mapView->getFloorMin(); --z) {
            if(z < mapView->getFloorMax()) {
                g_painter->setColor(lightView->m_globalLightColor);
                for(auto& shade : lightView->m_shades) {
                    if(shade.floor != z) continue;
                    shade.floor = -1;

                    g_drawPool.addTexturedRect(Rect(shade.pos - shadeBase.first, shadeBase.second), g_lightViewPaint.m_shadeTexture);
                }
            }

            auto& lights = lightView->m_lights[z];
            std::sort(lights.begin(), lights.end(), orderLightComparator);
            for(LightSource& light : lights) {
                g_painter->setColor(Color::from8bit(light.color, light.brightness));
                g_drawPool.addTexturedRect(Rect(light.pos - Point(light.radius), Size(light.radius * 2)), g_lightViewPaint.m_lightTexture);
            }
            lights.clear();
        }
    }

    g_drawPool.draw(lightView->m_lightbuffer, dest, src);
}

bool LightViewPainter::orderLightComparator(const LightSource& a, const LightSource& b)
{
    return (a.brightness == b.brightness && a.color < b.color) || (a.brightness < b.brightness);
}

void LightViewPainter::generateLightTexture()
{
    const float brightnessIntensity = 1.3f,
        centerFactor = .0f;

    const uint16 bubbleRadius = 256,
        centerRadius = bubbleRadius * centerFactor,
        bubbleDiameter = bubbleRadius * 2;

    auto lightImage = ImagePtr(new Image(Size(bubbleDiameter, bubbleDiameter)));
    for(int_fast16_t x = -1; ++x < bubbleDiameter;) {
        for(int_fast16_t y = -1; ++y < bubbleDiameter;) {
            const float radius = std::sqrt((bubbleRadius - x) * (bubbleRadius - x) + (bubbleRadius - y) * (bubbleRadius - y));
            const float intensity = stdext::clamp<float>((bubbleRadius - radius) / static_cast<float>(bubbleRadius - centerRadius), .0f, 1.0f);

            // light intensity varies inversely with the square of the distance
            const uint8_t colorByte = std::min<int16>((intensity * intensity * brightnessIntensity) * 0xff, 0xff);

            uint8_t pixel[4] = { 0xff, 0xff, 0xff, colorByte };
            lightImage->setPixel(x, y, pixel);
        }
    }

    m_lightTexture = new Texture(lightImage);
    m_lightTexture->setSmooth(true);
}

void LightViewPainter::generateShadeTexture()
{
    if(g_lightViewPaint.m_shadeTexture != nullptr) return;

    const uint16 diameter = 10;
    const auto image = ImagePtr(new Image(Size(diameter, diameter)));
    for(int_fast16_t x = -1; ++x < diameter;) {
        for(int_fast16_t y = -1; ++y < diameter;) {
            const uint8 alpha = x == 0 || y == 0 || x == diameter - 1 || y == diameter - 1 ? 0 : 0xff;
            uint8_t pixel[4] = { 0xff, 0xff, 0xff, alpha };
            image->setPixel(x, y, pixel);
        }
    }

    m_shadeTexture = new Texture(image);
    m_shadeTexture->setSmooth(true);
}
