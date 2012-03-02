/**
 * vim: set ts=4 :
 * =============================================================================
 * Left 4 Downtown SourceMod Extension
 * Copyright (C) 2009 Igor "Downtown1" Smirnov.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 * Version: $Id$
 */

#include "vglobals.h"
#include "extension.h"
#include "util.h"

#define ENTINDEX_TO_CBASEENTITY(ref, buffer) \
	buffer = g_pGameHelpers->ReferenceToEntity(ref); \
	if (!buffer) \
	{ \
		return pContext->ThrowNativeError("Entity %d (%d) is not a CBaseEntity", g_pGameHelpers->ReferenceToIndex(ref), ref); \
	}

cell_t L4D_GetSurvivorHealthBonus(IPluginContext *pContext, const cell_t *params)
{
	CBaseEntity *pPlayer;
	ENTINDEX_TO_CBASEENTITY(params[1], pPlayer);
	
	static int offset = 0;
	
	if(offset == 0)
	{
		if (!g_pGameConf->GetOffset("SurvivorHealthBonus", &offset) || !offset)
		{
			return pContext->ThrowNativeError("Could not read 'SurvivorHealthBonus' offset from GameConf");
		}
	}
	
	int healthbonus = *reinterpret_cast<int*>(reinterpret_cast<uint8_t*>(pPlayer) + offset);
	L4D_DEBUG_LOG("GetSurvivorHealthBonus for %d -> %d", params[1], healthbonus);
	
	return healthbonus;
}

cell_t L4D_SetSurvivorHealthBonus(IPluginContext *pContext, const cell_t *params)
{
	CBaseEntity *pPlayer;
	ENTINDEX_TO_CBASEENTITY(params[1], pPlayer);
	
	static int offset = 0;
	
	if(offset == 0)
	{
		if (!g_pGameConf->GetOffset("SurvivorHealthBonus", &offset) || !offset)
		{
			return pContext->ThrowNativeError("Could not read 'SurvivorHealthBonus' offset from GameConf");
		}
	}

	int healthbonus = params[2];
	*reinterpret_cast<int*>(reinterpret_cast<uint8_t*>(pPlayer) + offset) = healthbonus;

	L4D_DEBUG_LOG("SetSurvivorHealthBonus for %d -> %d", params[1], healthbonus);
	
	if (params[3] == 1)
	{
	
		static ICallWrapper *pWrapper = NULL;

		// Director::RestartScenario()
		if (!pWrapper)
		{
			REGISTER_NATIVE_ADDR("RecomputeTeamScores", 
				pWrapper = g_pBinTools->CreateCall(addr, CallConv_ThisCall, NULL, NULL, 0));
		}
		
		void *gamerules = *g_pGameRules;
		unsigned char vstk[sizeof(void *)];
		unsigned char *vptr = vstk;

		*(void **)vptr = gamerules;
		
		pWrapper->Execute(vstk, NULL);
		L4D_DEBUG_LOG("SetSurvivorHealthBonus Called RecomputeTeamScores");
	}
	
	
	
	return 1;
}

cell_t L4D_GetMapScoreMultiplier(IPluginContext *pContext, const cell_t *params)
{

	float MapMulti = *reinterpret_cast<float*>(reinterpret_cast<uint8_t*>(g_pDirector) + 0x3cc);
	
	return MapMulti;
}

cell_t L4D_SetMapScoreMultiplier(IPluginContext *pContext, const cell_t *params)
{

	*reinterpret_cast<float*>(reinterpret_cast<uint8_t*>(g_pDirector) + 0x3cc) = params[1];
	
	return 1;
}

sp_nativeinfo_t g_L4SNatives[] = 
{
	{"L4D_GetSurvivorHealthBonus",	L4D_GetSurvivorHealthBonus},
	{"L4D_SetSurvivorHealthBonus",	L4D_SetSurvivorHealthBonus},
	{"L4D_GetMapScoreMultiplier",	L4D_GetMapScoreMultiplier},
	{"L4D_SetMapScoreMultiplier",	L4D_SetMapScoreMultiplier},
	{NULL,							NULL}
};
