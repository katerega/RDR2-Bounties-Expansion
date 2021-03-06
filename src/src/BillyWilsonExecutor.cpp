#include "Main.h";

using namespace std;

BillyWilsonExecutor::BillyWilsonExecutor(BountyMissionData missionData, MapAreasManager* areasMgr)
	: BaseMissionExecutor(missionData, areasMgr)
{
	setTargetAreaRadius(100);
	setRequiredDistanceToLocateTarget(60);
	setMustBeCloseToLocate(true);

	campfirePos = toVector3(-4459.897, -3628.035, 56.84821);
	enemiesGroup = new GuardsGroup(campfirePos, 25, true); // Create a new Guards Group. First parameter is the center of the defense area. The second one is the radius. The third is whether to tolerate the player when he gets close or not.

	campfire = NULL;
	horse = NULL;
}

void BillyWilsonExecutor::update()
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

void BillyWilsonExecutor::prepareSet()
{
	campfire = createProp("P_CAMPFIRE02X", campfirePos);
	addHorse(horse);
	addHorse("A_C_Horse_KentuckySaddle_Black", toVector3(-4454.534, -3645.554, 56.88246));
	addHorse("A_C_Horse_KentuckySaddle_SilverBay", toVector3(-4452.479, -3649.343, 56.87785));

	// Now just add the enemies to the group to make them be controlled by it

	RoutineParams routine2;
	routine2.patrolName = "miss_hello22";
	routine2.patrolRoute.push_back(toVector3(-4474.35, -3624.58, 57.4038));
	routine2.patrolHeading.push_back(toVector3(-4475.48, -3624.03, 57.1959));
	routine2.patrolRoute.push_back(toVector3(-4466.76, -3615.98, 56.8411));
	routine2.patrolHeading.push_back(toVector3(-4466.5, -3614.74, 56.6528));
	routine2.patrolRoute.push_back(toVector3(-4455.91, -3624.1, 57.8472));
	routine2.patrolHeading.push_back(toVector3(-4456.55, -3624.73, 57.8523));
	routine2.patrolRoute.push_back(toVector3(-4445.88, -3636.21, 58.083));
	routine2.patrolHeading.push_back(toVector3(-4444.19, -3636.78, 58.0166));
	enemiesGroup->add(createPed("G_M_M_UniBanditos_01", toVector3(-4465.518, -3630.684, 56.82176), 302.607), IdlingModifier::Rest);
	enemiesGroup->add(createPed("G_M_M_UniBanditos_01", toVector3(-4466.473, -3624.259, 56.72544), (rand() % 361)), IdlingModifier::Rest);
	enemiesGroup->add(createPed("G_M_M_UniBanditos_01", toVector3(-4463.2, -3619.767, 56.39746), (rand() % 361)), IdlingModifier::Rest);
	enemiesGroup->add(createPed("G_M_M_UniBanditos_01", toVector3(-4471.412, -3618.398, 56.15459), (rand() % 361)), IdlingModifier::Rest);
	enemiesGroup->add(createPed("G_M_M_UniBanditos_01", toVector3(-4475.483, -3625.59, 56.46838), (rand() % 361)), IdlingModifier::Patrol, routine2);
	enemiesGroup->add(createPed("G_M_M_UniBanditos_01", toVector3(-4467.389, -3633.805, 56.97105), (rand() % 361)), IdlingModifier::Rest);
	enemiesGroup->add(createPed("G_M_M_UniBanditos_01", toVector3(-4459.62, -3636.262, 56.81227), 349.419), IdlingModifier::Rest);
	enemiesGroup->start();
}

Ped BillyWilsonExecutor::spawnTarget()
{
	RoutineParams routine3;
	this->horse = createPed("A_C_Horse_KentuckySaddle_Grey", toVector3(-4456.349, -3642.207, 56.85976));
	routine3.Horse = horse;
	routine3.isTarget = true;
	Vector3 targetPos = toVector3(-4461.601, -3633.93, 56.91957);
	Ped target = createPed(M_BOUNTY_TARGET, targetPos, 341.55);
	enemiesGroup->add(target, IdlingModifier::Rest, routine3);
	return target;
}

void BillyWilsonExecutor::onTargetLocated()
{
	BaseMissionExecutor::onTargetLocated();
	enemiesGroup->addBlips();
}

void BillyWilsonExecutor::addHorse(Ped horse)
{
	PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(horse, true);
	PED::_0xD3A7B003ED343FD9(horse, 0x8FFCF06B, true, false, false);
	horses.push_back(horse);
}

void BillyWilsonExecutor::addHorse(const char* model, Vector3 pos)
{
	Ped horse = createPed((char*)model, pos);
	addHorse(horse);
}

void BillyWilsonExecutor::releaseUnnecessaryEntities()
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

void BillyWilsonExecutor::cleanup()
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

BillyWilsonExecutor::BillyWilsonExecutor(BountyMissionData missionData, MapAreasManager* areasMgr)
	: BaseMissionExecutor(missionData, areasMgr)
{
	setTargetAreaRadius(100);
	setRequiredDistanceToLocateTarget(50);
	setMustBeCloseToLocate(true);
	enemiesStatus = EnemiesMode::IDLE;
	campfirePos = toVector3(-4459.897, -3628.035, 56.84821); //test deze bounty
	toleratePlayer = true;
	campfire = NULL;
	horse = NULL;
}

void BillyWilsonExecutor::update()
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

Ped BillyWilsonExecutor::spawnTarget()
{
	Vector3 targetPos = toVector3(-4461.601, -3633.93, 57.91957);
	Ped target = createPed(M_BOUNTY_MEXICAN, targetPos);
	return target;
}

void BillyWilsonExecutor::enterIdleMode()
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

void BillyWilsonExecutor::enterAlertMode()
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

void BillyWilsonExecutor::enterWarningMode()
{
	vector<Ped>::iterator pedItr;
	for (pedItr = enemies.begin(); pedItr != enemies.end(); pedItr++)
	{
			WEAPON::SET_CURRENT_PED_WEAPON(*pedItr, WEAPON::GET_BEST_PED_WEAPON(*pedItr, 0, 0), true, 0, false, false);
	}

	playAmbientSpeech(target, "FINAL_WARNING");
	enemiesStatus = EnemiesMode::WARNING;
}

void BillyWilsonExecutor::enterCombatMode()
{
	enemiesStatus = EnemiesMode::COMBAT;

	Ped player = PLAYER::PLAYER_PED_ID();
	vector<Ped>::iterator pedItr;
	for (pedItr = enemies.begin(); pedItr != enemies.end(); pedItr++)
	{
		PED::_0xFE07FF6495D52E2A(*pedItr, 0, 0, 0);
		WEAPON::SET_CURRENT_PED_WEAPON(*pedItr, WEAPON::GET_BEST_PED_WEAPON(*pedItr, 0, 0), true, 0, false, false);
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

void BillyWilsonExecutor::prepareSet()
{
	campfire = createProp("P_CAMPFIRE02X", campfirePos);

	this->horse = createPed("A_C_Horse_Turkoman_Gold", toVector3(-4456.349, -3642.207, 57.85976));
	addHorse(horse);
	addHorse("A_C_Horse_KentuckySaddle_Black", toVector3(-4454.534, -3645.554, 57.88246));
	addHorse("A_C_Horse_KentuckySaddle_SilverBay", toVector3(-4452.479, -3649.343, 57.87785));

	addEnemy(target);
	addEnemy(toVector3(-4465.518, -3630.684, 57.82176));
	addEnemy(toVector3(-4466.473, -3624.259, 57.72544));
	addEnemy(toVector3(-4463.2, -3619.767, 57.39746));
	addEnemy(toVector3(-4471.412, -3618.398, 57.15459));
	addEnemy(toVector3(-4475.483, -3625.59, 57.46838));
	addEnemy(toVector3(-4467.389, -3633.805, 57.97105));
	addEnemy(toVector3(-4459.62, -3636.262, 57.81227));

	enterIdleMode();
}

void BillyWilsonExecutor::onTargetLocated()
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

void BillyWilsonExecutor::createEnemyBlips()
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

void BillyWilsonExecutor::releaseUnnecessaryEntities()
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

void BillyWilsonExecutor::addEnemy(Vector3 pos)
{
	Ped enemyPed = createPed(M_BOUNTY_MEXICAN, pos);
	addEnemy(enemyPed);
}

void BillyWilsonExecutor::addEnemy(Ped ped)
{
	enemies.push_back(ped);

	PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, 1);
	AI::CLEAR_PED_TASKS(ped, true, true);
}

void BillyWilsonExecutor::addHorse(const char* model, Vector3 pos)
{
	Ped horse = createPed((char*)model, pos);
	addHorse(horse);
}

void BillyWilsonExecutor::addHorse(Ped horse)
{
	PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(horse, true);
	PED::_0xD3A7B003ED343FD9(horse, 0x8FFCF06B, true, false, false); // give saddle
	horses.push_back(horse);
}

void BillyWilsonExecutor::cleanup()
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