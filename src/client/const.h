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

#ifndef CLIENT_CONST_H
#define CLIENT_CONST_H

namespace Otc
{
    enum FrameUpdate_t : uint32 {
        FUpdateThing = 1 << 0,
        FUpdateLight = 1 << 1,
        FUpdateAll = FUpdateThing | FUpdateLight
    };

    enum DrawFlags_t : uint32 {
        DrawGround = 1 << 0,
        DrawGroundBorders = 1 << 1,
        DrawOnBottom = 1 << 2,
        DrawOnTop = 1 << 3,
        DrawItems = 1 << 4,
        DrawCreatures = 1 << 5,
        DrawEffects = 1 << 6,
        DrawMissiles = 1 << 7,
        DrawCreaturesInformation = 1 << 8,
        DrawStaticTexts = 1 << 9,
        DrawAnimatedTexts = 1 << 10,
        DrawAnimations = 1 << 11,
        DrawBars = 1 << 12,
        DrawNames = 1 << 13,
        DrawLights = 1 << 14,
        DrawManaBar = 1 << 15,
        DrawWalls = DrawOnBottom | DrawOnTop,
        DrawEverything = DrawGround | DrawGroundBorders | DrawWalls | DrawItems |
        DrawCreatures | DrawEffects | DrawMissiles | DrawCreaturesInformation |
        DrawStaticTexts | DrawAnimatedTexts | DrawAnimations | DrawBars | DrawNames |
        DrawLights | DrawManaBar
    };

    enum InventorySlot_t : uint8 {
        InventorySlotHead = 1,
        InventorySlotNecklace,
        InventorySlotBackpack,
        InventorySlotArmor,
        InventorySlotRight,
        InventorySlotLeft,
        InventorySlotLegs,
        InventorySlotFeet,
        InventorySlotRing,
        InventorySlotAmmo,
        InventorySlotPurse,
        InventorySlotExt1,
        InventorySlotExt2,
        InventorySlotExt3,
        InventorySlotExt4,
        LastInventorySlot
    };

    enum skills_t : uint8_t {
        SKILL_FIST = 0,
        SKILL_CLUB = 1,
        SKILL_SWORD = 2,
        SKILL_AXE = 3,
        SKILL_DISTANCE = 4,
        SKILL_SHIELD = 5,
        SKILL_FISHING = 6,
        SKILL_CRITICAL_HIT_CHANCE = 7,
        SKILL_CRITICAL_HIT_DAMAGE = 8,
        SKILL_LIFE_LEECH_CHANCE = 9,
        SKILL_LIFE_LEECH_AMOUNT = 10,
        SKILL_MANA_LEECH_CHANCE = 11,
        SKILL_MANA_LEECH_AMOUNT = 12,

        SKILL_MAGLEVEL = 13,
        SKILL_LEVEL = 14,

        SKILL_FIRST = SKILL_FIST,
        SKILL_LAST = SKILL_MANA_LEECH_AMOUNT
    };

    enum Direction_t : uint8 {
        North = 0,
        East,
        South,
        West,
        NorthEast,
        SouthEast,
        SouthWest,
        NorthWest,
        InvalidDirection
    };

    enum FluidsColor_t : uint8 {
        FluidTransparent = 0,
        FluidBlue,
        FluidRed,
        FluidBrown,
        FluidGreen,
        FluidYellow,
        FluidWhite,
        FluidPurple
    };

    enum FluidsType_t : uint8 {
        FluidNone = 0,
        FluidWater,
        FluidMana,
        FluidBeer,
        FluidOil,
        FluidBlood,
        FluidSlime,
        FluidMud,
        FluidLemonade,
        FluidMilk,
        FluidWine,
        FluidHealth,
        FluidUrine,
        FluidRum,
        FluidFruidJuice,
        FluidCoconutMilk,
        FluidTea,
        FluidMead
    };

    enum FightModes_t : uint8 {
        FightOffensive = 1,
        FightBalanced = 2,
        FightDefensive = 3
    };

    enum ChaseModes_t : uint8 {
        DontChase = 0,
        ChaseOpponent = 1
    };

    enum PlayerSkulls_t : uint8 {
        SkullNone = 0,
        SkullYellow,
        SkullGreen,
        SkullWhite,
        SkullRed,
        SkullBlack,
        SkullOrange
    };

    enum PlayerShields_t : uint8 {
        ShieldNone = 0,
        ShieldWhiteYellow, // 1 party leader
        ShieldWhiteBlue, // 2 party member
        ShieldBlue, // 3 party member sexp off
        ShieldYellow, // 4 party leader sexp off
        ShieldBlueSharedExp, // 5 party member sexp on
        ShieldYellowSharedExp, // 6 // party leader sexp on
        ShieldBlueNoSharedExpBlink, // 7 party member sexp inactive guilty
        ShieldYellowNoSharedExpBlink, // 8 // party leader sexp inactive guilty
        ShieldBlueNoSharedExp, // 9 party member sexp inactive innocent
        ShieldYellowNoSharedExp, // 10 party leader sexp inactive innocent
        ShieldGray // 11 member of another party
    };

    enum PlayerEmblems_t : uint8 {
        EmblemNone = 0,
        EmblemGreen,
        EmblemRed,
        EmblemBlue,
        EmblemMember,
        EmblemOther
    };

    enum CreatureIcons_t : uint8 {
        NpcIconNone = 0,
        NpcIconChat,
        NpcIconTrade,
        NpcIconQuest,
        NpcIconTradeQuest
    };

    enum PlayerIcons_t : uint32 {
        IconNone = 0,
        IconPoison = 1 << 0,
        IconBurn = 1 << 1,
        IconEnergy = 1 << 2,
        IconDrunk = 1 << 3,
        IconManaShield = 1 << 4,
        IconParalyze = 1 << 5,
        IconHaste = 1 << 6,
        IconSwords = 1 << 7,
        IconDrowning = 1 << 8,
        IconFreezing = 1 << 9,
        IconDazzled = 1 << 10,
        IconCursed = 1 << 11,
        IconPartyBuff = 1 << 12,
        IconPzBlock = 1 << 13,
        IconPz = 1 << 14,
        IconBleeding = 1 << 15,
        IconHungry = 1 << 16
    };

    enum MessageMode_t : uint32 {
        MESSAGE_NONE = 0, /* None */

        MESSAGE_SAY = 1,
        MESSAGE_WHISPER = 2,
        MESSAGE_YELL = 3,
        MESSAGE_PRIVATE_FROM = 4,
        MESSAGE_PRIVATE_TO = 5,
        MESSAGE_CHANNEL_MANAGER = 6,
        MESSAGE_CHANNEL_Y = 7,
        MESSAGE_CHANNEL_O = 8,
        MESSAGE_SPELL_USE = 9,
        MESSAGE_PRIVATE_NP = 10,
        MESSAGE_NPC_UNKOWN = 11, /* no effect (?)*/
        MESSAGE_PRIVATE_PN = 12,
        MESSAGE_BROADCAST = 13,

        MESSAGE_GAMEMASTER_CONSOLE = 13, /* Red message in the console*/ /* MESSAGE_BROADCAST */

        MESSAGE_CHANNEL_R1 = 14, //red - #c text
        MESSAGE_PRIVATE_RED_FROM = 15, //@name@text
        MESSAGE_PRIVATE_RED_TO = 16, //@name@text

        MESSAGE_LOGIN = 17, /* White message at the bottom of the game window and in the console*/
        MESSAGE_ADMINISTRADOR = 18, /* Red message in game window and in the console*/
        MESSAGE_EVENT_ADVANCE = 19, /* White message in game window and in the console*/
        MESSAGE_GAME_HIGHLIGHT = 20, /* Red message in game window and in the console*/
        MESSAGE_FAILURE = 21, /* White message at the bottom of the game window"*/
        MESSAGE_LOOK = 22, /* Green message in game window and in the console*/
        MESSAGE_DAMAGE_DEALT = 23, /* White message on the console*/
        MESSAGE_DAMAGE_RECEIVED = 24, /* White message on the console*/
        MESSAGE_HEALED = 25, /* White message on the console*/
        MESSAGE_EXPERIENCE = 26, /* White message on the console*/
        MESSAGE_DAMAGE_OTHERS = 27, /* White message on the console*/
        MESSAGE_HEALED_OTHERS = 28, /* White message on the console*/
        MESSAGE_EXPERIENCE_OTHERS = 29, /* White message on the console*/
        MESSAGE_STATUS = 30, /* White message at the bottom of the game window and in the console*/
        MESSAGE_LOOT = 31, /* White message on the game window and in the console*/
        MESSAGE_TRADE = 32, /* Green message in game window and in the console*/
        MESSAGE_GUILD = 33, /* White message in channel (+ channelId)*/
        MESSAGE_PARTY_MANAGEMENT = 34, /* Green message in game window and in the console*/
        MESSAGE_PARTY = 35, /* White message on the console*/

        MESSAGE_MONSTER_SAY = 36,
        MESSAGE_MONSTER_YELL = 37,

        MESSAGE_REPORT = 38, /* White message on the game window and in the console*/
        MESSAGE_HOTKEY_PRESSED = 39, /* Green message in game window and in the console*/
        MESSAGE_TUTORIAL_HINT = 40, /* no effect (?)*/
        MESSAGE_THANK_YOU = 41, /* no effect (?)*/
        MESSAGE_MARKET = 42, /* Popout a modal window with the message and a 'ok' button*/
        MESSAGE_MANA = 43, /* no effect (?)*/
        MESSAGE_BEYOND_LAST = 44, /* White message on the game window and in the console*/
        MESSAGE_ATTENTION = 48, /* White message on the console*/
        MESSAGE_BOOSTED_CREATURE = 49, /* White message on the game window and in the console*/
        MESSAGE_OFFLINE_TRAINING = 50, /* White message on the game window and in the console*/
        MESSAGE_TRANSACTION = 51, /* White message on the game window and in the console*/
        MESSAGE_POTION = 52, /* Orange creature say*/
        MESSAGE_CHANNEL_R2 = 0xFF, //#d
    };

    enum ImpactAnalyzerAndTracker_t : uint8_t {
        ANALYZER_HEAL = 0,
        ANALYZER_DAMAGE_DEALT = 1,
        ANALYZER_DAMAGE_RECEIVED = 2
    };

    enum PVPModes_t : uint8 {
        WhiteDove = 0,
        WhiteHand = 1,
        YellowHand = 2,
        RedFist = 3
    };

    enum PartyState_t : uint8_t {
        PARTY_STATE_CREATURE_UPDATE = 0,
        PARTY_STATE_PLAYER_MANA = 11,
        PARTY_STATE_CREATURE_SHOW_STATUS = 12,
        PARTY_STATE_PLAYER_VOCATION = 13,
        PARTY_STATE_CREATURE_ICON = 14,
    };

    enum PreyState_t : uint8_t
    {
        PREY_STATE_LOCKED = 0,
        PREY_STATE_INACTIVE = 1,
        PREY_STATE_ACTIVE = 2,
        PREY_STATE_SELECTION = 3,
        PREY_STATE_SELECTION_CHANGE_MONSTER = 4,
    };

    enum CreatureIcon_t {
        CREATUREICON_NONE = 0,
        CREATUREICON_HIGHERRECEIVEDDAMAGE = 1,
        CREATUREICON_LOWERDEALTDAMAGE = 2,
        CREATUREICON_TURNEDMELEE = 3,
    };

    enum GameFeature_t : uint8 {
        GameDiagonalAnimatedText = 1,
        GameFormatCreatureName = 2,
        GameChangeMapAwareRange = 3,
        GameMapMovePosition = 4,
        GameBlueNpcNameColor = 5,
        GameDoubleShopSellAmount = 6,
        GameSpritesAlphaChannel = 7,
        GameKeepUnawareTiles = 8,
        LastGameFeature = 101
    };

    enum PathFindResult_t : uint8 {
        PathFindResultOk = 0,
        PathFindResultSamePosition,
        PathFindResultImpossible,
        PathFindResultTooFar,
        PathFindResultNoWay
    };

    enum PathFindFlags_t : uint8 {
        PathFindAllowNotSeenTiles = 1 << 0,
        PathFindAllowCreatures = 1 << 1,
        PathFindAllowNonPathable = 1 << 2,
        PathFindAllowNonWalkable = 1 << 3
    };

    enum Blessings_t : uint32 {
        BlessingNone = 0,
        BlessingAdventurer = 1 << 0,
        BlessingSpiritualShielding = 1 << 1,
        BlessingEmbraceOfTibia = 1 << 2,
        BlessingFireOfSuns = 1 << 3,
        BlessingWisdomOfSolitude = 1 << 4,
        BlessingSparkOfPhoenix = 1 << 5
    };

    enum StoreProductTypes_t : uint8 {
        ProductTypeOther = 0,
        ProductTypeNameChange = 1
    };

    enum MagicEffectsType_t : uint8_t {
        MAGIC_EFFECTS_END_LOOP = 0,
        MAGIC_EFFECTS_DELTA = 1,
        MAGIC_EFFECTS_DELAY = 2,
        MAGIC_EFFECTS_CREATE_EFFECT = 3,
        MAGIC_EFFECTS_CREATE_DISTANCEEFFECT = 4,
        MAGIC_EFFECTS_CREATE_DISTANCEEFFECT_REVERSED = 5,
    };
}

#endif
