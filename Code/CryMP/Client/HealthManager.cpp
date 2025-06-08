#include "HealthManager.h"
#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CrySystem/IConsole.h"

#include "CryCommon/CryAction/IActorSystem.h"
#include "CryCommon/CryAction/IVehicleSystem.h"
#include "CryCommon/CryAction/IMovementController.h"
#include "CryCommon/CrySystem/ICryPak.h"

#include "CrySystem/RandomGenerator.h"

#include "CryGame/Actors/Actor.h"
#include "CryGame/Game.h"
#include "CryGame/GameCVars.h"
#include "CryGame/Actors/Player/Player.h"
#include "CryGame/GameRules.h"

#define HEALTH_KEY			901

CHealthManager::CHealthManager() {

}

CHealthManager::~CHealthManager() {

}

bool CHealthManager::IsActive() {
	CGameRules* pGR = g_pGame->GetGameRules();
	CActor* pLocalActor = static_cast<CActor*>(g_pGame->GetIGameFramework()->GetClientActor());
	return pGR && pLocalActor;
}

int CHealthManager::GetHealth(IActor* iActor) {
	int health = 0;
	CSynchedStorage* pSSS = g_pGame->GetSynchedStorage();
	CActor* pActor = static_cast<CActor*>(iActor);
	IActor* pLocalActor = g_pGame->GetIGameFramework()->GetClientActor();

	if (pLocalActor == pActor) {
		CryLogAlways("Local");
		return pActor->GetHealth();
	} else if (pActor->GetHealth() <= 0) {
		CryLogAlways("Dead");
		return 0;
	} else if (pSSS && pSSS->GetEntityValue(iActor->GetEntityId(), HEALTH_KEY, health)) {
		CryLogAlways("Health: %d", health);
		return health;
	} else {
		CryLogAlways("Bad fallback");
		return 100;
	}
}

void CHealthManager::Reset() {

}
