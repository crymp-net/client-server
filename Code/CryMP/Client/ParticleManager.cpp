#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CrySystem/IConsole.h"
#include "CryCommon/CryEntitySystem/IEntitySystem.h"
#include "CryCommon/Cry3DEngine/I3DEngine.h"
#include "CryCommon/CryAnimation/ICryAnimation.h"
#include "ParticleManager.h"
#include "CryCommon/CryAction/IGameObject.h"

void ParticleManager::OnTick()
{

}

void ParticleManager::CmdListEmitters(IConsoleCmdArgs* pArgs)
{

	int count = 0;
	int acount = 0;
	int dcount = 0;
	IEntityItPtr pIIt = gEnv->pEntitySystem->GetEntityIterator();
	IEntity* pEntity = 0;

	pIIt->MoveFirst();

	while (pEntity = pIIt->Next())
	{
		for (int i = 0; i < pEntity->GetSlotCount(); i++)
		{
			IParticleEmitter* pEmitter = pEntity->GetParticleEmitter(i);
			if (pEmitter)
			{
				//pEmitter->Activate(false);
				bool bParentVisibile = false;
				if (IGameObject* pGameObject = gEnv->pGame->GetIGameFramework()->GetGameObject(pEntity->GetId()))
				{
					if (pGameObject->IsProbablyVisible())
					{ 
						bParentVisibile = true;
					}
				}

				++count;
				CryLogAlways("$9[$5%4d$9] %55s | %s$9| %10s $9| Parent: %s", i, pEmitter->GetName(), pEmitter->IsAlive() ? "alive" : "$4 dead", bParentVisibile ? "$3vis" : "invis", pEntity->GetName());
				if (pEmitter->IsAlive())
					++acount;
				else
					++dcount;
			}
		}
	}

	if (count > 0)
	{
		CryLogAlways("$9Result: Emitters count $1%i $9| Visible $3%i $9| Hidden %i", count, acount, dcount);
		return;
	}
}

ParticleManager::ParticleManager()
{
	IConsole* pConsole = gEnv->pConsole;

	pConsole->AddCommand("emitters", CmdListEmitters, 0, "List particle emitters");
}

ParticleManager::~ParticleManager()
{
}

