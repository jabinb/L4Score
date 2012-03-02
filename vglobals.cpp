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

void **g_pGameRules = NULL;
void **g_pEngine = NULL;
void **g_pDirector = NULL;
void **g_pZombieManager = NULL;
void **g_pNavMesh = NULL;

#ifdef PLATFORM_WINDOWS
void InitializeValveGlobals()
{
	char *addr = NULL;
	int offset;

	/* g_pGameRules */
	if (!g_pGameConfSDKTools->GetMemSig("CreateGameRulesObject", (void **)&addr) || !addr)
	{
		return;
	}
	if (!g_pGameConfSDKTools->GetOffset("g_pGameRules", &offset) || !offset)
	{
		return;
	}
	g_pGameRules = *reinterpret_cast<void ***>(addr + offset);

	/* g_pDirector */
	const char *directorConfKey = "SelectModelByPopulation";
	if (!g_pGameConf->GetMemSig(directorConfKey, (void **)&addr) || !addr)
	{
		return;
	}
	if (!g_pGameConf->GetOffset("TheDirector", &offset) || !offset)
	{
		return;
	}
	g_pDirector = *reinterpret_cast<void ***>(addr + offset);

	/*note that IEngine != IVEngineServer
	also MPGameModeChangedConVar is looks like gone from L4D2
	but we never use it for anything but IVEngineServer::IsReserved on Linux
	so we probably won't need it ever
	since IVEngineServer::IsReserved got removed from the Linux binary
	*/
	/* g_pEngine */
	if (!g_pGameConf->GetMemSig("MPGameModeChangedConVar", (void **)&addr) || !addr)
	{
		return;
	}
	if (!g_pGameConf->GetOffset("IVEngineServer", &offset) || !offset)
	{
		return;
	}
	g_pEngine = *reinterpret_cast<void ***>(addr + offset);

	/* g_pZombieManager */
	//TODO
}
#elif defined PLATFORM_LINUX
void InitializeValveGlobals()
{
	char *addr = NULL;

	/* g_pGameRules */
	if (!g_pGameConfSDKTools->GetMemSig("g_pGameRules", (void **)&addr) || !addr)
	{
		return;
	}
	g_pGameRules = reinterpret_cast<void **>(addr);

	/* g_pDirector */
	if (!g_pGameConf->GetMemSig("TheDirector", (void **)&addr) || !addr)
	{
		return;
	}
	g_pDirector = reinterpret_cast<void **>(addr);

	if (!g_pGameConf->GetMemSig("TheNavMesh", (void **)&addr) || !addr)
	{
		return;
	}
	g_pNavMesh = reinterpret_cast<void **>(addr);

	L4D_DEBUG_LOG("NavMesh pointer calculated to be 0x%x", g_pNavMesh);

	/* g_pZombieManager */
	if (!g_pGameConf->GetMemSig("TheZombieManager", (void **)&addr) || !addr)
	{
		return;
	}
	g_pZombieManager = reinterpret_cast<void **>(addr);

	/* g_pEngine */
	if (!g_pGameConf->GetMemSig("IVEngineServer", (void **)&addr) || !addr)
	{
		return;
	}
	g_pEngine = reinterpret_cast<void **>(addr);
}
#endif
