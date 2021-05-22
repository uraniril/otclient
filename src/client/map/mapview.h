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

#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <framework/core/inputevent.h>
#include <framework/core/declarations.h>
#include <framework/graphics/declarations.h>
#include <framework/graphics/texturemanager.h>
#include <framework/graphics/paintershaderprogram.h>
#include <framework/luaengine/luaobject.h>
#include <client/map/lightview.h>
#include <client/painter/mapviewpainter.h>

struct AwareRange
{
    uint8 top, right, bottom, left;
    uint8 horizontal() { return left + right + 1; }
    uint8 vertical() { return top + bottom + 1; }
};

// @bindclass
class MapView : public LuaObject
{
public:
    enum ViewMode : uint8 {
        NEAR_VIEW,
        MID_VIEW,
        FAR_VIEW,
        HUGE_VIEW
    };

    enum AntialiasingMode :uint8 {
        ANTIALIASING_DISABLED,
        ANTIALIASING_ENABLED,
        ANTIALIASING_SMOOTH_RETRO
    };

    MapView();
    ~MapView() override;

public:
    // floor visibility related
    uint8 getLockedFirstVisibleFloor() { return m_lockedFirstVisibleFloor; }
    uint8 getTileSize() { return m_tileSize; }
    uint8 getFloorMin() { return m_floorMin; }
    uint8 getFloorMax() { return m_floorMax; }

    void lockFirstVisibleFloor(uint8 firstVisibleFloor);
    void unlockFirstVisibleFloor();

    bool isMultifloor() { return m_multifloor; }
    void setMultifloor(bool enable) { m_multifloor = enable; requestVisibleTilesCacheUpdate(); }

    // map dimension related
    Point getVisibleCenterOffset() { return m_visibleCenterOffset; }
    Size getVisibleDimension() { return m_visibleDimension; }
    void setVisibleDimension(const Size& visibleDimension);

    // view mode related
    ViewMode getViewMode() { return m_viewMode; }
    void setViewMode(ViewMode viewMode);
    void optimizeForSize(const Size& visibleSize);

    void setAutoViewMode(bool enable);
    bool isAutoViewModeEnabled() { return m_autoViewMode; }

    // camera related
    CreaturePtr getFollowingCreature() { return m_followingCreature; }
    void followCreature(const CreaturePtr& creature);
    bool isFollowingCreature() { return m_followingCreature && m_follow; }

    Position getCameraPosition();
    void setCameraPosition(const Position& pos);

    void setMinimumAmbientLight(float intensity) { m_minimumAmbientLight = intensity; updateLight(); }
    float getMinimumAmbientLight() { return m_minimumAmbientLight; }

    // drawing related
    void setDrawTexts(bool enable) { m_drawTexts = enable; }
    bool isDrawingTexts() { return m_drawTexts; }

    void setDrawNames(bool enable) { m_drawNames = enable; }
    bool isDrawingNames() { return m_drawNames; }

    void setDrawHealthBars(bool enable) { m_drawHealthBars = enable; }
    bool isDrawingHealthBars() { return m_drawHealthBars; }

    void setDrawLights(bool enable);
    bool isDrawingLights() { return m_drawLights && m_lightView->isDark(); }

    void setDrawViewportEdge(bool enable) { m_drawViewportEdge = enable; }
    bool isDrawingViewportEdge() { return m_drawViewportEdge; }

    void setDrawManaBar(bool enable) { m_drawManaBar = enable; }
    bool isDrawingManaBar() { return m_drawManaBar; }

    void move(int32 x, int32 y);

    void setShader(const PainterShaderProgramPtr& shader, float fadein, float fadeout);
    PainterShaderProgramPtr getShader() { return m_shader; }

    Position getPosition(const Point& point, const Size& mapSize);

    MapViewPtr asMapView() { return static_self_cast<MapView>(); }

    std::vector<CreaturePtr>& getVisibleCreatures() { return m_visibleCreatures; }
    std::vector<CreaturePtr> getSpectators(const Position& centerPos, bool multiFloor);
    std::vector<CreaturePtr> getSightSpectators(const Position& centerPos, bool multiFloor);

    bool isInRange(const Position& pos, bool ignoreZ = false);

    void setMousePosition(const Position& mousePos) { m_mousePosition = mousePos; }
    const Position& getMousePosition() { return m_mousePosition; }

    TilePtr getTopTile(Position tilePos);

    void setCrosshairEffect(const uint32 id);
    void setCrosshairTexture(const std::string& texturePath);
    void setAntiAliasingMode(const AntialiasingMode mode);
    void setDrawHighlightTarget(const bool enable) { m_drawHighlightTarget = enable; }

    void onMouseMove(const Position& mousePos, bool isVirtualMove = false);
    void onKeyRelease(const InputEvent& inputEvent);

protected:
    void onCameraMove(const Point& offset);
    void onTileUpdate(const Position& pos);
    void onFloorDrawingEnd(uint8 floor);
    void onFloorDrawingStart(uint8 floor);
    void onMapCenterChange(const Position& pos);
    void onGlobalLightChange(const Light& light);
    void onFloorChange(uint8 floor, uint8 previousFloor);
    void onPositionChange(const Position& newPos, const Position& oldPos);

    friend class Map;
    friend class Tile;
    friend class LightView;
    friend class MapViewPainter;

private:
    struct FrameCache {
        FrameBufferPtr staticText, dynamicText, creatureInformation;

        uint32_t flags = Otc::FUpdateAll;
    };

    struct RectCache {
        Rect rect, srcRect;
        Point drawOffset;
        float horizontalStretchFactor, verticalStretchFactor;
    };

    void updateStaticTextFrame() { m_frameCache.staticText->update(); }
    void requestVisibleTilesCacheUpdate() { m_mustUpdateVisibleTilesCache = true; }
    void updateGeometry(const Size& visibleDimension, const Size& optimizedSize);
    void updateVisibleTilesCache();

    uint8 calcFirstVisibleFloor();
    uint8 calcLastVisibleFloor();

    void updateLight();
    void updateViewportDirectionCache();

    void addVisibleCreature(const CreaturePtr& creature);
    void removeVisibleCreature(const CreaturePtr& creature);

    Rect calcFramebufferSource(const Size& destSize);

    Point transformPositionTo2D(const Position& position, const Position& relativePosition)
    {
        return Point((m_virtualCenterOffset.x + (position.x - relativePosition.x) - (relativePosition.z - position.z)) * m_tileSize,
                     (m_virtualCenterOffset.y + (position.y - relativePosition.y) - (relativePosition.z - position.z)) * m_tileSize);
    }

    uint8 m_lockedFirstVisibleFloor{ UINT8_MAX },
        m_cachedFirstVisibleFloor{ SEA_FLOOR },
        m_cachedLastVisibleFloor{ SEA_FLOOR },
        m_renderScale{ 100 },
        m_tileSize,
        m_floorMin{ 0 },
        m_floorMax{ 0 },
        m_antiAliasingMode;

    float m_minimumAmbientLight{ 0 },
        m_fadeInTime{ 0 },
        m_fadeOutTime{ 0 },
        m_scaleFactor;

    Rect m_rectDimension;

    Size m_drawDimension,
        m_visibleDimension,
        m_optimizedSize;

    Point m_virtualCenterOffset,
        m_visibleCenterOffset,
        m_moveOffset;

    Position m_customCameraPosition,
        m_lastCameraPosition,
        m_mousePosition;

    std::array<AwareRange, Otc::InvalidDirection + 1> m_viewPortDirection;
    AwareRange m_viewport;

    bool m_drawLights{ false },
        m_autoViewMode{ false },
        m_drawViewportEdge{ false },
        m_drawHighlightTarget{ false },
        m_shiftPressed{ false },
        m_mustUpdateVisibleTilesCache{ true },
        m_mustUpdateVisibleCreaturesCache{ true },
        m_shaderSwitchDone{ true },
        m_drawHealthBars{ true },
        m_drawManaBar{ true },
        m_multifloor{ true },
        m_drawTexts{ true },
        m_drawNames{ true },
        m_smooth{ true },
        m_follow{ true },
        m_antiAliasing{ true };

    std::vector<CreaturePtr> m_visibleCreatures;

    std::array<std::vector<TilePtr>, MAX_Z + 1> m_cachedVisibleTiles;

    PainterShaderProgramPtr m_shader, m_nextShader;
    LightViewPtr m_lightView;
    CreaturePtr m_followingCreature;

    FrameCache m_frameCache;
    RectCache m_rectCache;
    ViewMode m_viewMode;

    Timer m_fadeTimer;

    AwareRange m_awareRange;

    TilePtr m_lastHighlightTile;
    TexturePtr m_crosshairTexture;
    EffectPtr m_crosshairEffect;
};

#endif
