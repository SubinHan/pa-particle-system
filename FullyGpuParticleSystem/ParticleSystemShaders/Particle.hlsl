struct Particle
{
	float3 Position;
	float InitialSize;

	float3 Velocity;
	float InitialLifetime;

	float3 Acceleration;
	float InitialOpacity;

	float3 InitialColor;
	float RemainLifetime;

	float3 EndColor;
	float EndSize;

	float EndOpacity;
	float SpawnTime;
	uint SpawnOrderInFrame;
	float DistanceFromPrevious;

	float DistanceFromStart;
};

static const uint PARTICLECOUNTER_OFFSET_NUMDEADS = 0;
static const uint PARTICLECOUNTER_OFFSET_NUMALIVES = 4;
static const uint PARTICLECOUNTER_OFFSET_NUMALIVES_POST_UPDATE = 8;