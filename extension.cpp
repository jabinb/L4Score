/**
 * vim: set ts=4 :
 * =============================================================================
 * SourceMod Sample Extension
 * Copyright (C) 2004-2008 AlliedModders LLC.  All rights reserved.
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

#define GAMECONFIG_FILE "l4score.l4d"

#include <string.h>

#include "extension.h"
#include <ISDKTools.h>
#include <util.h>
#include "vglobals.h"

#include "codepatch/patchmanager.h"
#include "codepatch/autopatch.h"

#include "detours/recalculate_versus_score.h"
#include "detours/recompute_team_scores.h"
#include "detours/gamerules_networkchanged.h"

/**
 * @file extension.cpp
 * @brief Implement extension code here.
 */
L4Score g_L4Score;		/**< Global singleton for extension's main interface */
ICvar *icvar = NULL;	/** Cvar interface to sourcemod, used for accessing server side console variables (eg. mp_gamemode or sv_maxcmdrate etc.) **/
IServerGameEnts *gameents = NULL; /** Game Entity interface to sourcemod, used for accessing Entities in the game (players, bots, weapons etc.) **/
CGlobalVars *gpGlobals = NULL; /** CGlobalVars pointer from the HL2 Engine. **/
IGameHelpers *g_pGameHelpers = NULL; /** Sourcemod Interface, provides misc helper functionss **/
IGameConfig *g_pGameConf = NULL; /** Game data config (l4score.l4d.txt) provides keyvalues for memory signatures or virtual offsets **/
IGameConfig *g_pGameConfSDKTools = NULL; /** SDK Tools game data config **/
IBinTools *g_pBinTools = NULL; /** BinTools Interface, used to call functions via memory offsets **/
PatchManager g_PatchManager; /** Detour patch manager, manages & applies all of the memory patches used to create the various detours **/

/** Sourcepawn scripting forwards **/
IForward *g_pFwdOnRecalculateVersusScore = NULL; /** Raised whenever the L4D engine calls CTerrorGameRules::RecalculateVersusScore, typically when anything should change a survivors HealthBonus**/
IForward *g_pFwdOnRecomputeTeamScores = NULL; /** Raised whenever the L4D Engine calls Director::RecomputeTeamScores, usually after each survivors healthbonus has been calculated**/
IForward *g_pFwdOnGameRulesNetworkChanged = NULL; /** Raised whenever the L4D engine needs to network a changed value on the GameRulesProxy. **/

/** Extension sourcepawn natives **/
extern sp_nativeinfo_t g_L4SNatives[];

bool L4Score::SDK_OnLoad(char *error, size_t maxlength, bool late)
{

	/** Sourcemod Macro to get an instance of the Game helper interface. **/
	SM_GET_IFACE(GAMEHELPERS, g_pGameHelpers);

	/** Load the extension gamedata file **/
	char conf_error[255] = "";
	if (!gameconfs->LoadGameConfigFile(GAMECONFIG_FILE, &g_pGameConf, conf_error, sizeof(conf_error)))
	{
		if (conf_error[0])
		{
			UTIL_Format(error, maxlength, "Could not read " GAMECONFIG_FILE ".txt: %s", conf_error);
		}
		return false;
	}

	/** Add a dependency upon the bintools extension, will fail to load without bintools being present. **/
	sharesys->AddDependency(myself, "bintools.ext", true, true);
	/** Expose the extension natives to sourcemod for use in sourcepawn scripts **/
	sharesys->AddNatives(myself, g_L4SNatives);

	/** load sigscans and offsets from the sdktools gamedata file **/
	if (!gameconfs->LoadGameConfigFile("sdktools.games", &g_pGameConfSDKTools, conf_error, sizeof(conf_error)))
	{
		return false;
	}

	/** Expose forwards to sourcemod for various useful script functions **/
	g_pFwdOnRecalculateVersusScore = forwards->CreateForward("L4D_OnRecalculateVersusScore", ET_Event, 2, /*types*/NULL, Param_Cell, Param_CellByRef);
	g_pFwdOnRecomputeTeamScores = forwards->CreateForward("L4D_OnRecomputeTeamScores", ET_Event, 0, /*types*/NULL);
	g_pFwdOnGameRulesNetworkChanged = forwards->CreateForward("L4D_OnGameRulesNetworkChanged", ET_Event, 0, /*types*/NULL);

	/** Detour all of our registered detours with signatures provided by the gameconf **/
	Detour::Init(g_pSM->GetScriptingEngine(), g_pGameConf);

	return true;
}

void L4Score::SDK_OnUnload()
{

}

void L4Score::SDK_OnAllLoaded()
{
	SM_GET_LATE_IFACE(BINTOOLS, g_pBinTools);

	if (!g_pBinTools)
	{
		L4D_DEBUG_LOG("Failed to loan bintools or failed to load sdktools");
		return;
	}

	InitializeValveGlobals();

	g_PatchManager.Register(new AutoPatch<Detours::RecalculateVersusScore>());
	g_PatchManager.Register(new AutoPatch<Detours::RecomputeTeamScores>());
	g_PatchManager.Register(new AutoPatch<Detours::GameRulesNetworkChanged>());

}

bool L4Score::SDK_OnMetamodLoad(SourceMM::ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
	GET_V_IFACE_CURRENT(GetEngineFactory, icvar, ICvar, CVAR_INTERFACE_VERSION);

	g_pCVar = icvar;

	GET_V_IFACE_ANY(GetServerFactory, gameents, IServerGameEnts, INTERFACEVERSION_SERVERGAMEENTS);
	gpGlobals = ismm->GetCGlobals();


	return true;
}

SMEXT_LINK(&g_L4Score);
