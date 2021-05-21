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

#ifndef OUTFIT_H
#define OUTFIT_H

#include <framework/util/color.h>
#include <client/manager/thingtypemanager.h>

class Outfit
{
    enum {
        HSI_SI_VALUES = 7,
        HSI_H_STEPS = 19
    };

public:
    struct Clothes {
        uint16 id{ 0 };

        void setHead(uint8_t color) { m_head = color; m_headColor = std::make_pair(Outfit::getColor(color), SpriteMaskYellow); }
        void setBody(uint8_t color) { m_body = color; m_bodyColor = std::make_pair(Outfit::getColor(color), SpriteMaskRed); }
        void setLegs(uint8_t color) { m_legs = color; m_legsColor = std::make_pair(Outfit::getColor(color), SpriteMaskGreen); }
        void setFeet(uint8_t color) { m_feet = color; m_feetColor = std::make_pair(Outfit::getColor(color), SpriteMaskBlue); }

        uint8_t getHead() const { return m_head; }
        uint8_t getBody() const { return m_body; }
        uint8_t getLegs() const { return m_legs; }
        uint8_t getFeet() const { return m_feet; }

        std::pair<Color, SpriteMask> getHeadColor() const { return m_headColor; }
        std::pair<Color, SpriteMask> getBodyColor() const { return m_bodyColor; }
        std::pair<Color, SpriteMask> getLegsColor() const { return m_legsColor; }
        std::pair<Color, SpriteMask> getFeetColor() const { return m_feetColor; }

    private:
        uint8_t m_head, m_body, m_legs, m_feet;
        std::pair<Color, SpriteMask> m_headColor, m_bodyColor, m_legsColor, m_feetColor;
    };

    static Color getColor(int color);

    void setAuxId(uint16_t id) { m_auxId = id; }
    void setFamiliarId(uint16_t id) { m_familiarId = id; }

    void setAddons(uint8_t addons) { m_addons = addons; }
    void setCategory(ThingCategory category) { m_category = category; }

    void setClothes(const Clothes clothes) { m_clothes = clothes; }

    uint16_t getAuxId() const { return m_auxId; }
    uint8_t getAddons() const { return m_addons; }
    uint16_t getFamiliarId() const { return m_familiarId; }

    Clothes getClothes() const { return m_clothes; }
    Clothes getMountClothes() const { return m_mountClothes; }

    Clothes& getClothes() { return m_clothes; }
    Clothes& getMountClothes() { return m_mountClothes; }

    ThingCategory getCategory() const { return m_category; }

    bool hasMount() const { return m_mountClothes.id > 0; }

private:
    ThingCategory m_category{ ThingCategoryCreature };

    uint8_t m_addons;
    uint16 m_auxId{ 0 }, m_familiarId{ 0 };

    Clothes m_clothes, m_mountClothes;
};

#endif
