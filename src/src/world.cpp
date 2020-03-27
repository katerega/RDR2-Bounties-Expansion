#include "Main.h"

void getGroundPos(Vector3 originalPos, Vector3* outPos)
{
	float groundZ;
	GAMEPLAY::GET_GROUND_Z_FOR_3D_COORD(originalPos.x, originalPos.y, originalPos.z, &groundZ, false);

	outPos->x = originalPos.x;
	outPos->y = originalPos.y;
	outPos->z = groundZ;
}

float getGroundPos(Vector3 originalPos)
{
	float groundZ;
	GAMEPLAY::GET_GROUND_Z_FOR_3D_COORD(originalPos.x, originalPos.y, originalPos.z, &groundZ, false);
	return groundZ;
}

float distanceBetweenEntities(Entity entity1, Entity entity2)
{
	Vector3 pos1 = ENTITY::GET_ENTITY_COORDS(entity1, 1, 0);
	Vector3 pos2 = ENTITY::GET_ENTITY_COORDS(entity2, 1, 0);

	return GAMEPLAY::GET_DISTANCE_BETWEEN_COORDS(pos1.x, pos1.y, pos1.z, pos2.x, pos2.y, pos2.z, 1);
}

Ped createPed(char* modelName, Vector3 pos, float heading)
{
	Hash model = GAMEPLAY::GET_HASH_KEY(modelName);
	STREAMING::REQUEST_MODEL(model, false);
	while (!STREAMING::HAS_MODEL_LOADED(model))
	{
		WAIT(0);
	}

	Ped ped = PED::CREATE_PED(model, pos.x, pos.y, pos.z, heading, false, false, false, false);
	PED::SET_PED_VISIBLE(ped, true);

	return ped;
}

Ped createPedOnHorse(char* modelName, Ped horse, int seatIndex)
{
	Hash model = GAMEPLAY::GET_HASH_KEY(modelName);
	STREAMING::REQUEST_MODEL(model, false);
	while (!STREAMING::HAS_MODEL_LOADED(model))
	{
		WAIT(0);
	}

	Ped ped = PED::_0xF89AA2BD01FC06B7(horse, model, seatIndex, 0, 0, 0, 0);

	PED::SET_PED_VISIBLE(ped, true);
	return ped;
}

Vehicle createVehicle(char* modelName, Vector3 pos, float heading)
{
	Hash model = GAMEPLAY::GET_HASH_KEY(modelName);
	return createVehicle(model, pos, heading);;
}

Vehicle createVehicle(Hash model, Vector3 pos, float heading)
{
	Vehicle veh;

	if (!STREAMING::HAS_MODEL_LOADED(model))
	{
		STREAMING::REQUEST_MODEL(model, false);
	}

	while (!STREAMING::HAS_MODEL_LOADED(model))
	{
		WAIT(0);
	}

	veh = VEHICLE::CREATE_VEHICLE(model, pos.x, pos.y, pos.z, heading, true, true, false, false);
	VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(veh, 0);

	return veh;
}

Object createProp(char* model, Vector3 position, bool isStatic, bool isVisible)
{
	Object prop = OBJECT::CREATE_OBJECT(GAMEPLAY::GET_HASH_KEY(model), position.x, position.y, position.z, false, false, !isStatic, 0, 0);

	ENTITY::FREEZE_ENTITY_POSITION(prop, isStatic);
	ENTITY::SET_ENTITY_VISIBLE(prop, isVisible);

	return prop;
}

RaycastResult raycast(Vector3 source, Vector3 direction, float maxDist, RaycastIntersectionOptions intersectionOptions)
{
	RaycastResult result;
	Vector3 target = add(&source, &multiply(&direction, maxDist));
	int rayHandle = SHAPETEST::_START_SHAPE_TEST_RAY(source.x, source.y, source.z, target.x, target.y, target.z, intersectionOptions, 0, 7);
	SHAPETEST::GET_SHAPE_TEST_RESULT(rayHandle, (BOOL*)&result.didHit, &result.hitPos, &result.normal, &result.hitEntity);
	return result;
}

bool isPedHogtied(Ped ped)
{
	return AI::GET_IS_TASK_ACTIVE(ped, 399);
}

void releaseEntitySafe(Entity* entity)
{
	if ((*entity) && ENTITY::DOES_ENTITY_EXIST(*entity))
	{
		ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(entity);
	}
}

tm getGameTime()
{
	tm gameTime;
	gameTime.tm_year = 70; // make problems only with years aroud 1970, not 1900.
	gameTime.tm_mon = TIME::GET_CLOCK_MONTH();
	gameTime.tm_mday = TIME::GET_CLOCK_DAY_OF_MONTH();
	gameTime.tm_wday = TIME::GET_CLOCK_DAY_OF_WEEK();
	gameTime.tm_hour = TIME::GET_CLOCK_HOURS();
	gameTime.tm_min = TIME::GET_CLOCK_MINUTES();
	gameTime.tm_sec = TIME::GET_CLOCK_SECONDS();

	return gameTime;
}