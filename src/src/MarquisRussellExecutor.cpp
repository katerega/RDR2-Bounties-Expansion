#include "Main.h";

using namespace std;

MarquisRussellExecutor::MarquisRussellExecutor(BountyMissionData missionData, MapAreasManager* areasMgr)
	: BaseMissionExecutor(missionData, areasMgr)
{
	setTargetAreaRadius(100);
	setRequiredDistanceToLocateTarget(75);
	setMustBeCloseToLocate(true);

	campfirePos = toVector3(-1963.304, 2158.636, 326.5899);
	enemiesGroup = new GuardsGroup(campfirePos, 25, true); // Create a new Guards Group. First parameter is the center of the defense area. The second one is the radius. The third is whether to tolerate the player when he gets close or not.

	campfire = NULL;
	horse = NULL;
}

void MarquisRussellExecutor::update()
{
	BaseMissionExecutor::update();
	releaseUnnecessaryEntities();
	Ped player = PLAYER::PLAYER_PED_ID();
	Vector3 lastImpactCoords;
	vector<Ped>::iterator pedItr;
	vector<Ped>* enemyPeds = enemiesGroup->peds();
	for (pedItr = enemyPeds->begin(); pedItr != enemyPeds->end(); ++pedItr)
	{
		if (!ENTITY::IS_ENTITY_DEAD(target) && !isPedHogtied(target))
		{
			if (!PED::IS_PED_ON_MOUNT(target) && !PED::_0xAAB0FE202E9FC9F0(horse, -1) && !PED::IS_PED_IN_COMBAT(target, player))
			{
				PED::_0x5337B721C51883A9(*pedItr, true, true);
			}
		}
		if ((ENTITY::HAS_ENTITY_BEEN_DAMAGED_BY_ENTITY(*pedItr, player, true, true) || WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(player, &lastImpactCoords) && distanceBetween(ENTITY::GET_ENTITY_COORDS(*pedItr, 1, 0), lastImpactCoords) <= GUARD_SUSPECT_RANGE) && getMissionStage() == BountyMissionStage::LocateTarget)
		{
			nextStage();
		}
	}

	enemiesGroup->update(); // Update the group to keep it working

	if (getMissionStage() == BountyMissionStage::CaptureTarget && !ENTITY::IS_ENTITY_DEAD(target))
	{
		if (distanceBetweenEntities(target, player) > 80)
		{
			showSubtitle("The target is getting too far!");
		}
		if (distanceBetweenEntities(target, player) > 120)
		{
			PED::DELETE_PED(&target);
			PED::DELETE_PED(&horse);
			fail("Bounty failed, target lost");
		}
	}
}

void MarquisRussellExecutor::prepareSet()
{
	campfire = createProp("P_CAMPFIRE02X", campfirePos);
	addHorse(horse);
	addHorse("A_C_Horse_KentuckySaddle_Black", toVector3(-1961.849, 2153.194, 327.0338));
	addHorse("A_C_Horse_KentuckySaddle_SilverBay", toVector3(-1959.186, 2153.141, 326.3642));

	// Now just add the enemies to the group to make them be controlled by it

	RoutineParams routine1;
	routine1.patrolName = "miss_hello15";
	routine1.patrolRoute.push_back(toVector3(-1967.23, 2161.94, 328.391));
	routine1.patrolHeading.push_back(toVector3(-1967.23, 2161.58, 328.399));
	routine1.patrolRoute.push_back(toVector3(-1956.91, 2166.75, 327.059));
	routine1.patrolHeading.push_back(toVector3(-1956.32, 2166.93, 326.893));
	routine1.patrolRoute.push_back(toVector3(-1969.07, 2170.07, 329.384));
	routine1.patrolHeading.push_back(toVector3(-1969.6, 2170.21, 329.447));

	enemiesGroup->add(createPed("G_M_M_UNIMICAHGOONS_01", toVector3(-1961.146, 2157.891, 326.5644), (rand() % 361)), IdlingModifier::Rest);
	enemiesGroup->add(createPed("G_M_M_UNIMICAHGOONS_01", toVector3(-1960.277, 2164.74, 326.5345), (rand() % 361)), IdlingModifier::Scout);//
	enemiesGroup->add(createPed("G_M_M_UNIMICAHGOONS_01", toVector3(-1963.44, 2165.132, 327.2058), (rand() % 361)), IdlingModifier::Scout);//
	enemiesGroup->add(createPed("G_M_M_UNIMICAHGOONS_01", toVector3(-1967.631, 2161.309, 327.4136), (rand() % 361)), IdlingModifier::Patrol, routine1);
	enemiesGroup->add(createPed("G_M_M_UNIMICAHGOONS_01", toVector3(-1958.29, 2158.37, 326.059), 269), IdlingModifier::Scout);
	enemiesGroup->add(createPed("G_M_M_UNIMICAHGOONS_01", toVector3(-1966.345, 2156.546, 327.421), (rand() % 361)), IdlingModifier::Scout);//
	enemiesGroup->add(createPed("G_M_M_UNIMICAHGOONS_01", toVector3(-1961.693, 2151.215, 327.0578), (rand() % 361)), IdlingModifier::Scout);//
	enemiesGroup->start();
}

Ped MarquisRussellExecutor::spawnTarget()
{
	RoutineParams routine3;
	this->horse = createPed("A_C_Horse_KentuckySaddle_Grey", toVector3(-1964.544, 2153.432, 327.3247));
	routine3.Horse = horse;
	routine3.isTarget = true;
	Vector3 targetPos = toVector3(-1960.786, 2161.016, 326.61);
	Ped target = createPed("G_M_M_UNIMICAHGOONS_01", targetPos, (rand() % 361));
	enemiesGroup->add(target, IdlingModifier::Rest, routine3);
	return target;
}

void MarquisRussellExecutor::onTargetLocated()
{
	BaseMissionExecutor::onTargetLocated();
	enemiesGroup->addBlips();
}

void MarquisRussellExecutor::addHorse(Ped horse)
{
	PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(horse, true);
	PED::_0xD3A7B003ED343FD9(horse, 0x8FFCF06B, true, false, false);
	horses.push_back(horse);
}

void MarquisRussellExecutor::addHorse(const char* model, Vector3 pos)
{
	Ped horse = createPed((char*)model, pos);
	addHorse(horse);
}

void MarquisRussellExecutor::releaseUnnecessaryEntities()
{
	Ped player = PLAYER::PLAYER_PED_ID();
	std::vector<Ped>::iterator it;

	if (getMissionStage() >= BountyMissionStage::ArriveToPoliceStation)
	{
		for (it = horses.begin(); it != horses.end(); it++)
		{
			releaseEntitySafe(&(*it));
		}
	}
}

void MarquisRussellExecutor::cleanup()
{
	BaseMissionExecutor::cleanup();

	enemiesGroup->stop();
	releaseEntitySafe(&campfire);

	vector<Ped>::iterator pedItr;
	for (pedItr = horses.begin(); pedItr != horses.end(); pedItr++)
	{
		releaseEntitySafe(&(*pedItr));
	}
	vector<Ped>* enemyPeds = enemiesGroup->peds();
	for (pedItr = enemyPeds->begin(); pedItr != enemyPeds->end(); ++pedItr)
	{
		releaseEntitySafe(&(*pedItr));
	}
}

/*#include "Main.h";

using namespace std;

const int IDLE_DIST = 100;
const int ALERT_DIST = 35;
const int WARN_DIST = 30;
const int HEARING_RANGE = 45;
const int COMBAT_RANGE = 20;

MarquisRussellExecutor::MarquisRussellExecutor(BountyMissionData missionData, MapAreasManager* areasMgr)
	: BaseMissionExecutor(missionData, areasMgr)
{
	setTargetAreaRadius(100);
	setRequiredDistanceToLocateTarget(50);
	setMustBeCloseToLocate(true);
	enemiesStatus = EnemiesMode::IDLE;
	campfirePos = toVector3(-1963.104, 2158.636, 326.5899);
	toleratePlayer = true;
	campfire = NULL;
	horse = NULL;
	weapon == false;
}

void MarquisRussellExecutor::update()
{
	BaseMissionExecutor::update();
	releaseUnnecessaryEntities();
	Ped player = PLAYER::PLAYER_PED_ID();
	vector<Ped>::iterator pedItr;
	for (pedItr = enemies.begin(); pedItr != enemies.end(); pedItr++)
	if (getMissionStage() == BountyMissionStage::LocateTarget || getMissionStage() == BountyMissionStage::CaptureTarget)
	{
		float distanceToTarget = distanceBetweenEntities(*pedItr, player);
		switch (enemiesStatus)
		{
		case EnemiesMode::IDLE:
			if (distanceToTarget <= ALERT_DIST)
			{
				if (toleratePlayer)
				{
					stopwatch.start();
					enterAlertMode();
				}
				else
				{
					enterCombatMode();
				}
			}
			break;
		case EnemiesMode::ALERTED:
			if (stopwatch.getElapsedSecondsRealTime() >= 5)
			{
				if (toleratePlayer)
				{
					stopwatch.start();
					enterWarningMode();
				}
				else
				{
					enterCombatMode();
				}
			}
			else if (distanceToTarget >= IDLE_DIST)
			{
				stopwatch.stop();
				enterIdleMode();
			}
			break;

		case EnemiesMode::WARNING:
			if (stopwatch.getElapsedSecondsRealTime() >= 4)
			{
				if (distanceToTarget <= WARN_DIST)
				{
					enterCombatMode();
				}
				else if (distanceToTarget >= ALERT_DIST)
				{
					toleratePlayer = false;
					stopwatch.stop();
					enterAlertMode();
				}
			}
			break;
		}

		if (enemiesStatus < EnemiesMode::COMBAT && distanceToTarget <= HEARING_RANGE)
		{
			if (distanceToTarget <= COMBAT_RANGE || PLAYER::IS_PLAYER_FREE_AIMING(PLAYER::PLAYER_ID()) || PED::IS_PED_SHOOTING(player))
			{
				enterCombatMode();
			}
		}
	}

	if (getMissionStage() == BountyMissionStage::LocateTarget && PED::IS_PED_SHOOTING(player) && enemiesStatus < EnemiesMode::COMBAT)
	{
		enterCombatMode();
	}
	else if (getMissionStage() == BountyMissionStage::CaptureTarget && PED::IS_PED_SHOOTING(player) && enemiesStatus < EnemiesMode::COMBAT)
	{
		enterCombatMode();
	}

	if (getMissionStage() == BountyMissionStage::CaptureTarget && enemiesStatus >= EnemiesMode::ALERTED && !ENTITY::IS_ENTITY_DEAD(target))
	{
		if (distanceBetweenEntities(target, player) > 80)
		{
			showSubtitle("The target is getting too far!");
		}
		if (distanceBetweenEntities(target, player) > 120)
		{
			PED::DELETE_PED(&target);
			fail("Bounty failed, target lost");
		}
	}

}

Ped MarquisRussellExecutor::spawnTarget()
{
	Vector3 targetPos = toVector3(-1960.786, 2161.016, 327.61);
	Ped target = createPed("G_M_M_UNIMICAHGOONS_01", targetPos);
	PED::SET_PED_RELATIONSHIP_GROUP_HASH(target, GAMEPLAY::GET_HASH_KEY("REL_CRIMINALS"));
	return target;
}

void MarquisRussellExecutor::enterIdleMode()
{
	char* scenarioName;
	Ped player = PLAYER::PLAYER_PED_ID();
	vector<Ped>::iterator pedItr;
	for (pedItr = enemies.begin(); pedItr != enemies.end(); pedItr++)
	{
		Object seq;
		AI::OPEN_SEQUENCE_TASK(&seq);
		AI::TASK_TURN_PED_TO_FACE_COORD(*pedItr, campfirePos.x, campfirePos.y, campfirePos.z, 0);
		AI::CLOSE_SEQUENCE_TASK(seq);
		AI::TASK_PERFORM_SEQUENCE(*pedItr, seq);
		AI::CLEAR_SEQUENCE_TASK(&seq);
	}

	enemiesStatus = EnemiesMode::IDLE;
}

void MarquisRussellExecutor::enterAlertMode()
{
	vector<Ped>::iterator pedItr;
	for (pedItr = enemies.begin(); pedItr != enemies.end(); pedItr++)
	{
		PED::_0xFE07FF6495D52E2A(*pedItr, 0, 0, 0);
		AI::TASK_TURN_PED_TO_FACE_ENTITY(*pedItr, PLAYER::PLAYER_PED_ID(), -1, 0, 0, 0);
	}

	if (enemiesStatus == EnemiesMode::IDLE)
	{
		playAmbientSpeech(target, "GET_LOST");
	}
	else if (enemiesStatus == EnemiesMode::WARNING)
	{
		playAmbientSpeech(target, "WON_DISPUTE");
	}

	enemiesStatus = EnemiesMode::ALERTED;
}

void MarquisRussellExecutor::enterWarningMode()
{
	vector<Ped>::iterator pedItr;
	for (pedItr = enemies.begin(); pedItr != enemies.end(); pedItr++)
	{
		weapon == true;
		int iWeapon = rand() % 2 + 1;
		if (iWeapon == 1)
		{
			giveWeaponToPed(*pedItr, RepeaterWinchester, 0xDC8FB3E9, true);
		}
		if (iWeapon == 2)
		{
			giveWeaponToPed(*pedItr, RevolverSchofield, 0x64356159, true);
		}
	}

	playAmbientSpeech(target, "FINAL_WARNING");
	enemiesStatus = EnemiesMode::WARNING;
}

void MarquisRussellExecutor::enterCombatMode()
{
	enemiesStatus = EnemiesMode::COMBAT;

	Ped player = PLAYER::PLAYER_PED_ID();
	vector<Ped>::iterator pedItr;
	for (pedItr = enemies.begin(); pedItr != enemies.end(); pedItr++)
	{
		PED::_0xFE07FF6495D52E2A(*pedItr, 0, 0, 0);
		if (weapon == false)
		{
			int iWeapon = rand() % 2 + 1;
			if (iWeapon == 1)
			{
				giveWeaponToPed(*pedItr, RepeaterWinchester, 0xDC8FB3E9, true);
			}
			if (iWeapon == 2)
			{
				giveWeaponToPed(*pedItr, RevolverSchofield, 0x64356159, true);
			}
		}
		if (*pedItr == target)
		{
			int iSecret = rand() % 2 + 1;
			if (iSecret == 1)
			{

				Object seq;
				AI::OPEN_SEQUENCE_TASK(&seq);
				AI::_0x92DB0739813C5186(0, horse, -1, -1, 2.0f, 1, 0, 0); // Mount the horse
				AI::_0xFD45175A6DFD7CE9(0, player, 3, 0, -999.0f, -1, 0); // FLEE
				AI::CLOSE_SEQUENCE_TASK(seq);

				AI::CLEAR_PED_TASKS(target, 1, 1);
				AI::TASK_PERFORM_SEQUENCE(target, seq);
				AI::CLEAR_SEQUENCE_TASK(&seq);
				playAmbientSpeech(target, "ITS_MALE_EXTREME");
			}
			else if (iSecret == 2)
			{
				Object seq;
				AI::OPEN_SEQUENCE_TASK(&seq);
				AI::TASK_COMBAT_PED(target, player, 0, 16);
				AI::CLOSE_SEQUENCE_TASK(seq);

				AI::CLEAR_PED_TASKS(target, 1, 1);
				AI::TASK_PERFORM_SEQUENCE(target, seq);
				AI::CLEAR_SEQUENCE_TASK(&seq);
				playAmbientSpeech(target, "ITS_MALE_EXTREME");
			}
		}
		else
		{
			Object seq;
			AI::OPEN_SEQUENCE_TASK(&seq);
			AI::TASK_COMBAT_PED(0, player, 0, 16);
			AI::CLOSE_SEQUENCE_TASK(seq);
			PED::SET_PED_RELATIONSHIP_GROUP_HASH(*pedItr, GAMEPLAY::GET_HASH_KEY("REL_CRIMINALS"));
			AI::CLEAR_PED_TASKS(*pedItr, 1, 1);
			AI::TASK_PERFORM_SEQUENCE(*pedItr, seq);
			AI::CLEAR_SEQUENCE_TASK(&seq);
		}
	}
}

void MarquisRussellExecutor::prepareSet()
{
	campfire = createProp("P_CAMPFIRE02X", campfirePos);

	this->horse = createPed("A_C_Horse_Turkoman_Gold", toVector3(-1964.544, 2153.432, 328.3247));
	addHorse(horse);
	addHorse("A_C_Horse_KentuckySaddle_Black", toVector3(-1961.849, 2153.194, 328.0338));
	addHorse("A_C_Horse_KentuckySaddle_SilverBay", toVector3(-1959.186, 2153.141, 327.3642));

	addEnemy(target);
	addEnemy(toVector3(-1961.146, 2157.891, 327.5644));
	addEnemy(toVector3(-1960.277, 2164.74, 327.5345));
	addEnemy(toVector3(-1963.44, 2165.132, 328.2058));
	addEnemy(toVector3(-1967.631, 2161.309, 328.4136));
	addEnemy(toVector3(-1967.93, 2158.524, 328.3761));
	addEnemy(toVector3(-1966.345, 2156.546, 328.421));
	addEnemy(toVector3(-1961.693, 2151.215, 328.0578));

	enterIdleMode();
}

void MarquisRussellExecutor::onTargetLocated()
{
	BaseMissionExecutor::onTargetLocated();

	vector<Ped>::iterator pedItr;
	for (pedItr = enemies.begin(); pedItr != enemies.end(); pedItr++)
	{
		if (*pedItr != target)
		{
			createBlip(*pedItr, BLIP_STYLE_ENEMY);
		}
	}
}

void MarquisRussellExecutor::createEnemyBlips()
{
	std::vector<Ped>::iterator it;
	for (it = enemies.begin(); it != enemies.end(); ++it)
	{
		if (*it != target)
		{
			createBlip(*it, BLIP_STYLE_ENEMY);
		}
	}
}

void MarquisRussellExecutor::releaseUnnecessaryEntities()
{
	Ped player = PLAYER::PLAYER_PED_ID();
	std::vector<Ped>::iterator it;

	if (getMissionStage() >= BountyMissionStage::ArriveToPoliceStation)
	{
		releaseEntitySafe(&horse);

		for (it = enemies.begin(); it != enemies.end(); ++it)
		{
			if (distanceBetweenEntities(*it, player) > 250 ||  ENTITY::IS_ENTITY_DEAD(*it))
			{
				releaseEntitySafe(&(*it));
			}
		}
	}
}

void MarquisRussellExecutor::addEnemy(Vector3 pos)
{
	Ped enemyPed = createPed("G_M_M_UNIMICAHGOONS_01", pos);
	addEnemy(enemyPed);
}

void MarquisRussellExecutor::addEnemy(Ped ped)
{
	enemies.push_back(ped);

	PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, 1);
	AI::CLEAR_PED_TASKS(ped, true, true);
}

void MarquisRussellExecutor::addHorse(const char* model, Vector3 pos)
{
	Ped horse = createPed((char*)model, pos);
	addHorse(horse);
}

void MarquisRussellExecutor::addHorse(Ped horse)
{
	PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(horse, true);
	PED::_0xD3A7B003ED343FD9(horse, 0x8FFCF06B, true, false, false); // give saddle
	horses.push_back(horse);
}

void MarquisRussellExecutor::cleanup()
{
	BaseMissionExecutor::cleanup();
	releaseEntitySafe(&campfire);

	vector<Ped>::iterator pedItr;
	for (pedItr = horses.begin(); pedItr != horses.end(); pedItr++)
	{
		releaseEntitySafe(&(*pedItr));
	}

	for (pedItr = enemies.begin(); pedItr != enemies.end(); ++pedItr)
	{
		releaseEntitySafe(&(*pedItr));
	}
}*/