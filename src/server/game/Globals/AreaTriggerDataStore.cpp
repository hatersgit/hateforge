
#include "AreaTriggerDataStore.h"
#include "AreaTriggerTemplate.h"
#include "DatabaseEnv.h"
#include "DBCStores.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "Timer.h"
#include <cmath>

namespace
{
    std::unordered_map<uint32, AreaTriggerTemplate> _areaTriggerTemplateStore;
    std::unordered_map<uint32, AreaTriggerMiscTemplate> _areaTriggerTemplateSpellMisc;
}

void AreaTriggerDataStore::LoadAreaTriggerTemplates()
{
    uint32 oldMSTime = getMSTime();
    std::unordered_map<uint32, std::vector<Position>> verticesByAreaTrigger;
    std::unordered_map<uint32, std::vector<Position>> verticesTargetByAreaTrigger;
    std::unordered_map<uint32, std::vector<Position>> splinesBySpellMisc;
    std::unordered_map<uint32, std::vector<Position>> rollpitchyawBySpellMisc;
    std::unordered_map<uint32, std::vector<AreaTriggerAction>> actionsByAreaTrigger;

    //                                                            0              1           2            3
    if (QueryResult templateActions = WorldDatabase.Query("SELECT AreaTriggerId, ActionType, ActionParam, TargetType FROM `areatrigger_template_actions`"))
    {
        do
        {
            Field* templateActionFields = templateActions->Fetch();
            uint32 areaTriggerId = templateActionFields[0].Get<uint32>();

            AreaTriggerAction action;
            action.Param = templateActionFields[2].Get<uint32>();
            uint32 actionType = templateActionFields[1].Get<uint32>();
            uint32 targetType = templateActionFields[3].Get<uint32>();

            if (actionType >= AREATRIGGER_ACTION_MAX)
            {
                LOG_ERROR("sql.sql", "Table `areatrigger_template_actions` has invalid ActionType (%u) for AreaTriggerId %u and Param %u", actionType, areaTriggerId, action.Param);
                continue;
            }

            if (targetType >= AREATRIGGER_ACTION_USER_MAX)
            {
                LOG_ERROR("sql.sql", "Table `areatrigger_template_actions` has invalid TargetType (%u) for AreaTriggerId %u and Param %u", targetType, areaTriggerId, action.Param);
                continue;
            }

            action.TargetType = AreaTriggerActionUserTypes(targetType);
            action.ActionType = AreaTriggerActionTypes(actionType);

            actionsByAreaTrigger[areaTriggerId].push_back(action);
        } while (templateActions->NextRow());
    }
    else
    {
        LOG_INFO("server.loading", ">> Loaded 0 AreaTrigger templates actions. DB table `areatrigger_template_actions` is empty.");
    }

    //                                                     0              1    2         3         4               5
    if (QueryResult vertices = WorldDatabase.Query("SELECT AreaTriggerId, Idx, VerticeX, VerticeY, VerticeTargetX, VerticeTargetY FROM `areatrigger_template_polygon_vertices` ORDER BY `AreaTriggerId`, `Idx`"))
    {
        do
        {
            Field* verticeFields = vertices->Fetch();
            uint32 areaTriggerId = verticeFields[0].Get<uint32>();

            verticesByAreaTrigger[areaTriggerId].emplace_back(verticeFields[2].Get<float>(), verticeFields[3].Get<float>());

            if (!verticeFields[4].IsNull() && !verticeFields[5].IsNull())
                verticesTargetByAreaTrigger[areaTriggerId].emplace_back(verticeFields[4].Get<float>(), verticeFields[5].Get<float>());
            else if (verticeFields[4].IsNull() != verticeFields[5].IsNull())
                LOG_ERROR("sql.sql", "Table `areatrigger_template_polygon_vertices` has listed invalid target vertices (AreaTrigger: %u, Index: %u).", areaTriggerId, verticeFields[1].Get<uint32>());
        } while (vertices->NextRow());
    }
    else
    {
        LOG_INFO("server.loading", ">> Loaded 0 AreaTrigger templates polygon vertices. DB table `areatrigger_template_polygon_vertices` is empty.");
    }

    //                                                    0            1  2  3
    if (QueryResult splines = WorldDatabase.Query("SELECT SpellMiscId, X, Y, Z FROM `spell_areatrigger_splines` ORDER BY `SpellMiscId`, `Idx`"))
    {
        do
        {
            Field* splineFields = splines->Fetch();
            uint32 spellMiscId = splineFields[0].Get<uint32>();
            splinesBySpellMisc[spellMiscId].emplace_back(splineFields[1].Get<float>(), splineFields[2].Get<float>(), splineFields[3].Get<float>());
        } while (splines->NextRow());
    }
    else
    {
        LOG_INFO("server.loading", ">> Loaded 0 AreaTrigger templates splines. DB table `spell_areatrigger_splines` is empty.");
    }

    //                                                          0            1  2  3  4        5        6
    if (QueryResult rollpitchyaws = WorldDatabase.Query("SELECT SpellMiscId, X, Y, Z, TargetX, TargetY, TargetZ FROM `spell_areatrigger_rollpitchyaw` ORDER BY `SpellMiscId`"))
    {
        do
        {
            Field* rollpitchyawFields = rollpitchyaws->Fetch();
            uint32 spellMiscId = rollpitchyawFields[0].Get<uint32>();
            rollpitchyawBySpellMisc[spellMiscId].emplace_back(rollpitchyawFields[1].Get<float>(), rollpitchyawFields[2].Get<float>(), rollpitchyawFields[3].Get<float>());
            rollpitchyawBySpellMisc[spellMiscId].emplace_back(rollpitchyawFields[4].Get<float>(), rollpitchyawFields[5].Get<float>(), rollpitchyawFields[6].Get<float>());
        } while (rollpitchyaws->NextRow());
    }
    else
    {
        LOG_INFO("server.loading", ">> Loaded 0 AreaTrigger templates Roll Pitch Yaw. DB table `spell_areatrigger_rollpitchyaw` is empty.");
    }

    //                                                      0   1     2      3      4      5      6      7      8      9
    if (QueryResult templates = WorldDatabase.Query("SELECT Id, Type, Flags, Data0, Data1, Data2, Data3, Data4, Data5, ScriptName FROM `areatrigger_template`"))
    {
        do
        {
            Field* fields = templates->Fetch();

            AreaTriggerTemplate areaTriggerTemplate;
            areaTriggerTemplate.Id = fields[0].Get<uint32>();
            uint8 type = fields[1].Get<uint8>();

            if (type >= AREATRIGGER_TYPE_MAX)
            {
                LOG_ERROR("sql.sql", "Table `areatrigger_template` has listed areatrigger (Id: %u) with invalid type %u.", areaTriggerTemplate.Id, type);
                continue;
            }

            areaTriggerTemplate.Type = static_cast<AreaTriggerTypes>(type);
            areaTriggerTemplate.Flags = fields[2].Get<uint32>();

            for (uint8 i = 0; i < MAX_AREATRIGGER_ENTITY_DATA; ++i)
                areaTriggerTemplate.DefaultDatas.Data[i] = fields[3 + i].Get<float>();

            areaTriggerTemplate.ScriptId = sObjectMgr->GetScriptId(fields[9].Get<std::string>());
            areaTriggerTemplate.PolygonVertices = std::move(verticesByAreaTrigger[areaTriggerTemplate.Id]);
            areaTriggerTemplate.PolygonVerticesTarget = std::move(verticesTargetByAreaTrigger[areaTriggerTemplate.Id]);
            areaTriggerTemplate.Actions = std::move(actionsByAreaTrigger[areaTriggerTemplate.Id]);

            areaTriggerTemplate.InitMaxSearchRadius();
            _areaTriggerTemplateStore[areaTriggerTemplate.Id] = areaTriggerTemplate;
        } while (templates->NextRow());
    }

    //                                                                  0            1              2            3             4             5              6       7          8                  9             10
    if (QueryResult areatriggerSpellMiscs = WorldDatabase.Query("SELECT SpellMiscId, AreaTriggerId, MoveCurveId, ScaleCurveId, MorphCurveId, FacingCurveId, AnimId, AnimKitId, DecalPropertiesId, TimeToTarget, TimeToTargetScale FROM `spell_areatrigger`"))
    {
        do
        {
            AreaTriggerMiscTemplate miscTemplate;

            Field* areatriggerSpellMiscFields = areatriggerSpellMiscs->Fetch();
            miscTemplate.MiscId = areatriggerSpellMiscFields[0].Get<uint32>();

            uint32 areatriggerId = areatriggerSpellMiscFields[1].Get<uint32>();
            miscTemplate.Template = GetAreaTriggerTemplate(areatriggerId);

            if (!miscTemplate.Template)
            {
                LOG_ERROR("sql.sql", "Table `spell_areatrigger` reference invalid AreaTriggerId %u for miscId %u", areatriggerId, miscTemplate.MiscId);
                continue;
            }
            /*
#define VALIDATE_AND_SET_CURVE(Curve, Value) \
            miscTemplate.Curve = Value; \
            if (miscTemplate.Curve && !sCurveStore.LookupEntry(miscTemplate.Curve)) \
            { \
                LOG_ERROR("sql.sql", "Table `spell_areatrigger` has listed areatrigger (MiscId: %u, Id: %u) with invalid " #Curve " (%u), set to 0!", \
                    miscTemplate.MiscId, areatriggerId, miscTemplate.Curve); \
                miscTemplate.Curve = 0; \
            }

            VALIDATE_AND_SET_CURVE(MoveCurveId, areatriggerSpellMiscFields[2].Get<uint32>());
            VALIDATE_AND_SET_CURVE(ScaleCurveId, areatriggerSpellMiscFields[3].Get<uint32>());
            VALIDATE_AND_SET_CURVE(MorphCurveId, areatriggerSpellMiscFields[4].Get<uint32>());
            VALIDATE_AND_SET_CURVE(FacingCurveId, areatriggerSpellMiscFields[5].Get<uint32>()); 

#undef VALIDATE_AND_SET_CURVE
            */
            miscTemplate.AnimId = areatriggerSpellMiscFields[6].Get<uint32>();
            miscTemplate.AnimKitId = areatriggerSpellMiscFields[7].Get<uint32>();

            miscTemplate.DecalPropertiesId = areatriggerSpellMiscFields[8].Get<uint32>();

            miscTemplate.TimeToTarget = areatriggerSpellMiscFields[9].Get<uint32>();
            miscTemplate.TimeToTargetScale = areatriggerSpellMiscFields[10].Get<uint32>();

            miscTemplate.SplinePoints = std::move(splinesBySpellMisc[miscTemplate.MiscId]);

            if (rollpitchyawBySpellMisc.find(miscTemplate.MiscId) != rollpitchyawBySpellMisc.end())
            {
                miscTemplate.RollPitchYaw = std::move(rollpitchyawBySpellMisc[miscTemplate.MiscId][0]);
                miscTemplate.TargetRollPitchYaw = std::move(rollpitchyawBySpellMisc[miscTemplate.MiscId][1]);
            }

            _areaTriggerTemplateSpellMisc[miscTemplate.MiscId] = miscTemplate;
        } while (areatriggerSpellMiscs->NextRow());
    }
    else
    {
        LOG_INFO("server.loading", ">> Loaded 0 Spell AreaTrigger templates. DB table `spell_areatrigger` is empty.");
    }

    //                                                                  0            1           2             3                4             5        6                 7
    if (QueryResult circularMovementInfos = WorldDatabase.Query("SELECT SpellMiscId, StartDelay, CircleRadius, BlendFromRadius, InitialAngle, ZOffset, CounterClockwise, CanLoop FROM `spell_areatrigger_circular` ORDER BY `SpellMiscId`"))
    {
        do
        {
            Field* circularMovementInfoFields = circularMovementInfos->Fetch();
            uint32 spellMiscId = circularMovementInfoFields[0].Get<uint32>();

            auto atSpellMiscItr = _areaTriggerTemplateSpellMisc.find(spellMiscId);
            if (atSpellMiscItr == _areaTriggerTemplateSpellMisc.end())
            {
                LOG_ERROR("sql.sql", "Table `spell_areatrigger_circular` reference invalid SpellMiscId %u", spellMiscId);
                continue;
            }

            AreaTriggerOrbitInfo& orbitInfo = atSpellMiscItr->second.OrbitInfo;

            orbitInfo.StartDelay = circularMovementInfoFields[1].Get<uint32>();

#define VALIDATE_AND_SET_FLOAT(Float, Value) \
            orbitInfo.Float = Value; \
            if (!std::isfinite(orbitInfo.Float)) \
            { \
                LOG_ERROR("sql.sql", "Table `spell_areatrigger_circular` has listed areatrigger (MiscId: %u) with invalid " #Float " (%f), set to 0!", \
                    spellMiscId, orbitInfo.Float); \
                orbitInfo.Float = 0.0f; \
            }

            VALIDATE_AND_SET_FLOAT(Radius, circularMovementInfoFields[2].Get<float>());
            VALIDATE_AND_SET_FLOAT(BlendFromRadius, circularMovementInfoFields[3].Get<float>());
            VALIDATE_AND_SET_FLOAT(InitialAngle, circularMovementInfoFields[4].Get<float>());
            VALIDATE_AND_SET_FLOAT(ZOffset, circularMovementInfoFields[5].Get<float>());

#undef VALIDATE_AND_SET_FLOAT

            orbitInfo.CounterClockwise = circularMovementInfoFields[6].Get<bool>();
            orbitInfo.CanLoop = circularMovementInfoFields[7].Get<bool>();
        } while (circularMovementInfos->NextRow());
    }
    else
    {
        LOG_INFO("server.loading", ">> Loaded 0 AreaTrigger templates circular movement infos. DB table `spell_areatrigger_circular` is empty.");
    }

    LOG_INFO("server.loading", ">> Loaded {} spell areatrigger templates in %u ms.", _areaTriggerTemplateStore.size(), GetMSTimeDiffToNow(oldMSTime));
}

AreaTriggerTemplate const* AreaTriggerDataStore::GetAreaTriggerTemplate(uint32 areaTriggerId) const
{
    auto itr = _areaTriggerTemplateStore.find(areaTriggerId);
    if (itr != _areaTriggerTemplateStore.end())
        return &itr->second;

    return nullptr;
}

AreaTriggerMiscTemplate const* AreaTriggerDataStore::GetAreaTriggerMiscTemplate(uint32 spellMiscValue) const
{
    auto itr = _areaTriggerTemplateSpellMisc.find(spellMiscValue);
    if (itr != _areaTriggerTemplateSpellMisc.end())
        return &itr->second;

    return nullptr;
}

AreaTriggerDataStore::AreaTriggerDataList const* AreaTriggerDataStore::GetStaticAreaTriggersByMap(uint32 map_id) const
{
    auto itr = _areaTriggerData.find(map_id);
    if (itr != _areaTriggerData.end())
        return &itr->second;

    return nullptr;
}

AreaTriggerDataStore* AreaTriggerDataStore::Instance()
{
    static AreaTriggerDataStore instance;
    return &instance;
}
