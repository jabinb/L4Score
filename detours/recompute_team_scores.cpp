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

#include "recompute_team_scores.h"
#include "extension.h"
#include "compat_wrappers.h"

namespace Detours
{
	void RecomputeTeamScores::OnRecomputeTeamScores()
	{		
		L4D_DEBUG_LOG("CTerrorGameRules::RecomputeTeamScores has been called");
		/*
		
		//unsigned char *calloffset = *(unsigned char **)writeaddr;
		float maxflow;
		
		//-0x208694(%ebx),%ecx
		
		__asm__("mov -0xf0(%%ebx),%%eax\n\t"
				"mov (%%eax),%%eax\n\t"
				"mov 0x5a8(%%eax),%0" :"=r"(maxflow));
		//__asm__("mov -0x227040(%%ebx), %0;" :"=r"(maxflow));
		L4D_DEBUG_LOG("L4D_OnRecomputeTeamScores MaxFlow -> %.2f", maxflow);
		*/
		
		
		cell_t result = Pl_Continue;
		if(g_pFwdOnRecomputeTeamScores)
		{
			L4D_DEBUG_LOG("L4D_OnRecomputeTeamScores forward has been sent out");
			g_pFwdOnRecomputeTeamScores->Execute(&result);
		}

		if(result == Pl_Handled)
		{
			L4D_DEBUG_LOG("Director::RecomputeTeamScores will be skipped");
			return;
		}
		else
		{		
			L4D_DEBUG_LOG("CTerrorGameRules::RecomputeTeamScores will be invoked");

			return (this->*(GetTrampoline()))();
		}
	}
};
