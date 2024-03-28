/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "AreaScript.h"

class Player;
class GameObject;
class Creature;
class ZoneScript;
struct GossipMenuItems;
class AreaScript;

// class to handle player enter / leave / areatrigger / GO use events
class AreaScriptMgr
{
public:
    AreaScriptMgr();
    ~AreaScriptMgr();

    static AreaScriptMgr* instance();
    void AddZone(uint32 zoneid, AreaScript* handle);

    ZoneScript* GetZoneScript(uint32 zoneId);
    AreaScript* GetAreaScript(uint32 areaId);

    void HandlePlayerEnterZone(Player* player, uint32 areaflag);

    void HandlePlayerLeaveZone(Player* player, uint32 areaflag);

    void Update(uint32 diff);

    bool IsHotSpot(uint32 zone, uint32 area) {
        return _hotSpots[zone] == area;
    }

    uint32 GetHotspotForZone(uint32 zone) {
        return _hotSpots[zone];
    }

    uint32 GetParentZone(uint32 area) {
        for (auto zone : _areaByZone) {
            if (std::find(zone.second.begin(), zone.second.end(), area) != zone.second.end())
                return zone.first;
        }
        return 0;
    }

    void FillNewHotSpots() {
        std::random_device rd; // obtain a random number from hardware
        std::mt19937 gen(rd()); // seed the generator
        for (auto zone : _areaByZone) {
            auto zoneCount = zone.second.size();
            std::uniform_int_distribution<> hotspotRng(0, zoneCount-1);
            _hotSpots[zone.first] = _areaByZone[zone.first][hotspotRng(gen)];
        }
    }

    uint32 GetRandomMobAffix() {
        std::random_device rd; // obtain a random number from hardware
        std::mt19937 gen(rd()); // seed the generator
        std::uniform_int_distribution<> hotspotRng(0, _mobAffixes.size() - 1);
        return _mobAffixes[hotspotRng(gen)];
    }

private:
    std::unordered_map<uint32/*zoneId*/, AreaScript*> _scriptMap;
    uint32 m_UpdateTimer;
    uint32 m_hotSpotTimer;

    std::unordered_map<uint32 /*zone*/, uint32/*area*/> _hotSpots;

    std::unordered_map<uint32 /*zone*/, std::vector<uint32>/*areas*/> _areaByZone = {
        {12, {9, 18, 34, 54, 56, 57, 59, 60, 61, 62, 63, 64, 86, 87, 88, 89, 92, 120, 797, 798}},
        {1, {131, 132, 133, 134, 135, 136, 137, 138, 189, 211, 212, 716, 800, 801, 802, 803, 804, 805, 806, 807, 808}},
        {14, {362, 363, 364, 365, 366, 367, 368, 369, 370, 371, 372, 373, 374, 375, 393, 410, 638, 639, 640, 814, 816, 817, 1297, 2979}},
        {215, {220, 221, 222, 223, 224, 225, 358, 360, 397, 398, 404, 637, 818, 819, 820, 821}},
        {141, {186, 188, 256, 257, 258, 259, 260, 261, 262, 263, 264, 265, 266, 478, 702, 736, 2322}},
        {85, {152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 459, 810, 811, 812, 2117}},
        {148, {442, 443, 444, 445, 446, 447, 448, 449, 450, 452, 453, 454, 455, 456, 2077, 2078, 2326}},
        {38, {142, 143, 144, 146, 149, 556, 837, 838, 839, 923, 924, 925, 936}},
        {130, {172, 204, 213, 226, 227, 228, 229, 231, 232, 233, 235, 236, 237, 238, 239, 240, 305, 927, 928}},
        {40, {2, 20, 107, 108, 109, 111, 113, 115, 219, 916, 917, 918, 919, 920, 921, 922}},
        {17, {359, 378, 379, 380, 381, 382, 383, 384, 385, 386, 387, 388, 390, 391, 392, 458, 815, 1156, 1157, 1316, 1599, 1697, 1698, 1699, 1700, 1701, 1702, 1703, 1704, 1717, 1718, 2138, 2157}},
        {44, {68, 69, 70, 71, 95, 96, 97, 98, 996, 997, 998, 999, 1000, 1001, 1002, 2099}},
        {406, {460, 461, 463, 464, 465, 467, 468, 469, 636, 1076, 1277, 2160, 2537, 2538, 2539, 2540, 2541, 3157}},
        {331, {411, 412, 413, 414, 415, 416, 417, 418, 419, 420, 421, 422, 424, 426, 427, 428, 429, 430, 431, 432, 433, 434, 435, 436, 437, 438, 441, 879, 1276, 2301, 2325, 2357, 2358, 2359, 2360, 2457, 2637, 2797, 2897, 3177, 3319}},
        {10, {32, 42, 93, 94, 121, 241, 242, 245, 492, 536, 576, 799, 856, 1097, 1098, 2098, 2161}},
        {11, {118, 150, 205, 298, 299, 309, 836, 881, 1016, 1017, 1018, 1020, 1021, 1022, 1023, 1024, 1025, 1036, 1037, 1038, 1039}},
        {267, {271, 272, 275, 285, 286, 288, 289, 290, 294, 295, 896, 1056, 1057}},
        {400, {439, 479, 480, 481, 482, 483, 484, 485, 487, 488, 1557, 2097, 2237, 2240, 2303, 3037, 3038, 3039}},
        {36, {278, 279, 280, 281, 282, 283, 284, 1357, 1677, 1678, 1679, 1680, 1681, 1682, 1683, 1684, 2839, 3486}},
        {45, {313, 314, 315, 316, 317, 318, 320, 321, 324, 326, 327, 328, 333, 334, 335, 336, 880, 1857, 1858}},
        {405, {596, 597, 598, 599, 600, 602, 603, 604, 606, 607, 608, 609, 2198, 2324, 2404, 2405, 2406, 2407, 2408, 2617, 2657}},
        {33, {19, 35, 37, 43, 99, 100, 101, 102, 103, 104, 105, 106, 117, 122, 123, 125, 126, 127, 128, 129, 297, 301, 302, 310, 311, 312, 477, 1577, 1578, 1737, 1738, 1739, 1740, 1741, 1742, 2177, 3357}},
        {15, {403, 496, 497, 498, 499, 501, 502, 503, 504, 506, 507, 508, 509, 510, 511, 512, 513, 516, 517, 518, 2079, 2158, 2302, 4010, 4047}},
        {3, {337, 338, 339, 340, 341, 342, 343, 344, 345, 346, 347, 1517, 1877, 1878, 1879, 1897, 1898}},
        {8, {74, 75, 76, 116, 300, 657, 1777, 1778, 1780, 1798, 1817, 1978}},
        {357, {489, 1099, 1100, 1101, 1103, 1105, 1106, 1108, 1111, 1112, 1113, 1114, 1115, 1116, 1117, 1119, 1120, 1121, 1136, 1137, 2323, 2518, 2519, 2520, 2521, 2522, 3117}},
        {47, {307, 348, 349, 350, 351, 352, 353, 354, 355, 356, 1880, 1881, 1882, 1883, 1884, 1885, 1886, 1887, 1917, 3317}},
        {440, {976, 977, 978, 979, 980, 981, 982, 983, 984, 985, 986, 987, 988, 989, 990, 992, 1336, 1937, 1938, 1939, 1940, 2300}},
        {51, {246, 247, 1442, 1443, 1444, 1445, 1446, 1957, 1958, 1959}},
        {16, {878, 1216, 1219, 1220, 1221, 1222, 1224, 1225, 1226, 1227, 1228, 1229, 1230, 1231, 1232, 1233, 1234, 1235, 1236, 1237, 1256, 2497, 3137, 3138, 3140}},
        {4, {72, 73, 1437, 1438, 1439, 1440, 1441, 1457, 2517}},
        {490, {537, 538, 539, 540, 541, 542, 543, 1942, 1943}},
        {361, {1761, 1762, 1763, 1764, 1765, 1766, 1767, 1768, 1769, 1770, 1771, 1997, 1998, 2478, 2479, 2480, 2481, 2618}},
        {46, {249, 250, 251, 252, 253, 254, 255, 2417, 2418, 2419, 2420, 2421}},
        {28, {190, 192, 193, 196, 197, 198, 199, 200, 201, 202, 203, 813, 2297, 2298, 2620, 3197}},
        {139, {1019, 2258, 2260, 2261, 2262, 2263, 2264, 2265, 2266, 2267, 2268, 2269, 2270, 2271, 2272, 2273, 2274, 2275, 2276, 2277, 2279, 2299, 2619, 2621, 2622, 2623, 2624, 2625, 2626, 2627, 4067}},
        {618, {2241, 2242, 2243, 2244, 2245, 2246, 2247, 2248, 2249, 2250, 2251, 2252, 2253, 2255, 2256}},
        {41, {2558, 2560, 2561, 2562, 2697, 2837, 2937, 2938}},
        {1377, {2477, 2737, 2738, 2739, 2740, 2741, 2742, 2743, 2744, 3077, 3097, 3098, 3099, 3100, 3425, 3426, 3427, 3446, 3447}}
    };

    std::vector<uint32> _mobAffixes = { EMPOWERED, GHOSTLY, FANATIC, BERSERKER, POSSESSED, FIERY };
};

#define sAreaScriptMgr AreaScriptMgr::instance()
