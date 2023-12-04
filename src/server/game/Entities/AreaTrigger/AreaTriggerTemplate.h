#ifndef AREATRIGGER_TEMPLATE_H
#define AREATRIGGER_TEMPLATE_H

#include "Define.h"
#include "ObjectGuid.h"
#include "Optional.h"
#include "Position.h"
#include <vector>
#include <variant>

#define MAX_AREATRIGGER_ENTITY_DATA 6
#define MAX_AREATRIGGER_SCALE 7

enum class CurveInterpolationMode : uint8
{
    Linear = 0,
    Cosine = 1,
    CatmullRom = 2,
    Bezier3 = 3,
    Bezier4 = 4,
    Bezier = 5,
    Constant = 6,
};

enum AreaTriggerFlags
{
    AREATRIGGER_FLAG_HAS_ABSOLUTE_ORIENTATION   = 0x00001, // NYI
    AREATRIGGER_FLAG_HAS_DYNAMIC_SHAPE          = 0x00002, // Implemented for Spheres
    AREATRIGGER_FLAG_HAS_ATTACHED               = 0x00004,
    AREATRIGGER_FLAG_HAS_FACE_MOVEMENT_DIR      = 0x00008,
    AREATRIGGER_FLAG_HAS_FOLLOWS_TERRAIN        = 0x00010, // NYI
    AREATRIGGER_FLAG_UNK1                       = 0x00020,
    AREATRIGGER_FLAG_HAS_TARGET_ROLL_PITCH_YAW  = 0x00040,
    AREATRIGGER_FLAG_HAS_ANIM_ID                = 0x00080,
    AREATRIGGER_FLAG_UNK3                       = 0x00100,
    AREATRIGGER_FLAG_HAS_ANIM_KIT_ID            = 0x00200,
    AREATRIGGER_FLAG_HAS_CIRCULAR_MOVEMENT      = 0x00400,
    AREATRIGGER_FLAG_UNK5                       = 0x00800,
};

enum AreaTriggerTypes
{
    AREATRIGGER_TYPE_SPHERE = 0,
    AREATRIGGER_TYPE_BOX = 1,
    AREATRIGGER_TYPE_UNK = 2,
    AREATRIGGER_TYPE_POLYGON = 3,
    AREATRIGGER_TYPE_CYLINDER = 4,
    AREATRIGGER_TYPE_DISK = 5,
    AREATRIGGER_TYPE_BOUNDED_PLANE = 6,
    AREATRIGGER_TYPE_MAX
};

enum AreaTriggerActionTypes
{
    AREATRIGGER_ACTION_CAST = 0,
    AREATRIGGER_ACTION_ADDAURA = 1,
    AREATRIGGER_ACTION_TELEPORT = 2,
    AREATRIGGER_ACTION_MAX = 3
};

enum AreaTriggerActionUserTypes
{
    AREATRIGGER_ACTION_USER_ANY = 0,
    AREATRIGGER_ACTION_USER_FRIEND = 1,
    AREATRIGGER_ACTION_USER_ENEMY = 2,
    AREATRIGGER_ACTION_USER_RAID = 3,
    AREATRIGGER_ACTION_USER_PARTY = 4,
    AREATRIGGER_ACTION_USER_CASTER = 5,
    AREATRIGGER_ACTION_USER_MAX = 6
};

struct AreaTriggerId
{
    uint32 Id = 0;
    bool IsServerSide = false;

    friend bool operator==(AreaTriggerId const& left, AreaTriggerId const& right) = default;
};

struct AreaTriggerAction
{
    uint32 Param;
    AreaTriggerActionTypes ActionType;
    AreaTriggerActionUserTypes TargetType;
};

struct AreaTriggerScaleCurvePointsTemplate
{
    AreaTriggerScaleCurvePointsTemplate();

    CurveInterpolationMode Mode;
    std::array<DBCPosition2D, 2> Points;
};

struct AreaTriggerScaleCurveTemplate
{
    AreaTriggerScaleCurveTemplate();

    uint32 StartTimeOffset;
    std::variant<float, AreaTriggerScaleCurvePointsTemplate> Curve;
};

struct AreaTriggerShapeInfo
{
    AreaTriggerShapeInfo();

    bool IsSphere()         const { return Type == AREATRIGGER_TYPE_SPHERE; }
    bool IsBox()            const { return Type == AREATRIGGER_TYPE_BOX; }
    bool IsPolygon()        const { return Type == AREATRIGGER_TYPE_POLYGON; }
    bool IsCylinder()       const { return Type == AREATRIGGER_TYPE_CYLINDER; }
    bool IsDisk()           const { return Type == AREATRIGGER_TYPE_DISK; }
    bool IsBoudedPlane()    const { return Type == AREATRIGGER_TYPE_BOUNDED_PLANE; }
    float GetMaxSearchRadius() const;

    AreaTriggerTypes Type;

    union
    {
        struct
        {
            float Data[MAX_AREATRIGGER_ENTITY_DATA];
        } DefaultDatas;

        // AREATRIGGER_TYPE_SPHERE
        struct
        {
            float Radius;
            float RadiusTarget;
        } SphereDatas;

        // AREATRIGGER_TYPE_BOX
        struct
        {
            float Extents[3];
            float ExtentsTarget[3];
        } BoxDatas;

        // AREATRIGGER_TYPE_POLYGON
        struct
        {
            float Height;
            float HeightTarget;
        } PolygonDatas;

        // AREATRIGGER_TYPE_CYLINDER
        struct
        {
            float Radius;
            float RadiusTarget;
            float Height;
            float HeightTarget;
            float LocationZOffset;
            float LocationZOffsetTarget;
        } CylinderDatas;

        // AREATRIGGER_TYPE_DISK
        struct
        {
            float InnerRadius;
            float InnerRadiusTarget;
            float OuterRadius;
            float OuterRadiusTarget;
            float Height;
            float HeightTarget;
            float LocationZOffset;
            float LocationZOffsetTarget;
        } DiskDatas;

        // AREATRIGGER_TYPE_BOUNDED_PLANE
        struct
        {
            float Extents[2];
            float ExtentsTarget[2];
        } BoundedPlaneDatas;
    };
};

struct AreaTriggerOrbitInfo
{
    Optional<ObjectGuid> PathTarget;
    Optional<Position> Center;
    bool CounterClockwise = false;
    bool CanLoop = false;
    uint32 TimeToTarget = 0;
    int32 ElapsedTimeForMovement = 0;
    uint32 StartDelay = 0;
    float Radius = 0.0f;
    float BlendFromRadius = 0.0f;
    float InitialAngle = 0.0f;
    float ZOffset = 0.0f;
};

class AreaTriggerTemplate
{
public:
    AreaTriggerTemplate();
    ~AreaTriggerTemplate();

    bool HasFlag(AreaTriggerFlags flag) const { return (Flags & flag) != 0; }

    AreaTriggerId Id;
    uint32 Flags;
    std::vector<AreaTriggerAction> Actions;
};

class AreaTriggerCreateProperties
{
public:
    AreaTriggerCreateProperties();
    ~AreaTriggerCreateProperties();

    bool HasSplines() const;
    float GetMaxSearchRadius() const;

    uint32 Id;
    AreaTriggerTemplate const* Template;

    uint32 MoveCurveId;
    uint32 ScaleCurveId;
    uint32 MorphCurveId;
    uint32 FacingCurveId;

    int32 AnimId;
    int32 AnimKitId;

    uint32 DecalPropertiesId;

    uint32 TimeToTarget;
    uint32 TimeToTargetScale;

    Optional<AreaTriggerScaleCurveTemplate> OverrideScale;
    Optional<AreaTriggerScaleCurveTemplate> ExtraScale;

    AreaTriggerShapeInfo Shape;
    std::vector<Position> PolygonVertices;
    std::vector<Position> PolygonVerticesTarget;

    std::vector<Position> SplinePoints;
    Optional<AreaTriggerOrbitInfo> OrbitInfo;

    uint32 ScriptId;
};

#endif
