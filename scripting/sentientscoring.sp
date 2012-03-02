#include <sourcemod>
#include <sdktools>
#include <sdkhooks>
#include <regex>
#include "l4score.inc"

static bool:g_bFirstRoundEnd = true;

public Plugin:myinfo = 
{
	name = "L4D Sentient Scoring",
	author = "sfr | sentient",
	description = "Modifies l4d1 scoring system (hb and surv multi)",
	version = "1.0.0",
	url = "http://sendforroar.org/"
}

public OnPluginStart()
{
	HookEvent("round_start", Event_Round_Start, EventHookMode_PostNoCopy);
	HookEvent("round_end", Event_Round_End, EventHookMode_PostNoCopy);
}

public Action:L4D_OnRecalculateVersusScore(clientc)
{

	//LogMessage("FORWARD: OnRecalculateVersusScore - healthbonus -> %i", healthbonus);
	for (new client = 1; client <= MaxClients; client++)
	{
		if (!IsClientInGame(client) || GetClientTeam(client) != 2) continue;
		
		//LogMessage("Client %i healthbonus -> %i", client, L4D_GetSurvivorHealthBonus(client));
		L4D_SetSurvivorHealthBonus(client, SurvivorHealthBonus(client), 1);
		//LogMessage("Client %i new healthbonus -> %i", client, L4D_GetSurvivorHealthBonus(client));
	}
	GameRules_SetProp("m_iVersusSurvivalMultiplier", 0, 4, 0, true);
	return Plugin_Handled;

}

public Action:Event_Round_Start(Handle:event, const String:name[], bool:dontBroadcast)
{
	
	if (!g_bFirstRoundEnd)
	{
		g_bFirstRoundEnd = true;
	}
	
}

public Action:Event_Round_End(Handle:event, const String:name[], bool:dontBroadcast)
{
	if (g_bFirstRoundEnd)
	{
		GameRules_SetProp("m_iVersusSurvivalMultiplier", 0, 4, 0, true);
		g_bFirstRoundEnd = false;
	}
}

SurvivorHealthBonus(client)
{

	if (!IsPlayerAlive(client) || GetEntProp(client, Prop_Send, "m_isIncapacitated") == 1)
	{
		return 0;
	}
	
	new HealthBonus = 0;
	
	new PermHealth = GetEntProp(client, Prop_Send, "m_iHealth");
	
	if (PermHealth > 0)
	{
		PermHealth = RoundFloat((PermHealth / 2.0));
		HealthBonus += PermHealth;
	}
	//PrintToChatAll("PermHealth -> %d", PermHealth);

	new TempHealth = RoundFloat(GetEntPropFloat(client, Prop_Send, "m_healthBuffer") - ((GetGameTime() - GetEntPropFloat(client, Prop_Send, "m_healthBufferTime")) * GetConVarFloat(FindConVar("pain_pills_decay_rate"))) - 1.0);
	new IncapCount = GetEntProp(client, Prop_Send, "m_currentReviveCount");
	
	if (TempHealth > 0)
	{
		TempHealth = RoundFloat(TempHealth / ((IncapCount + 1) * 4.0));
	}
	else
		TempHealth = 0;
	
	//PrintToChatAll("TempHealth -> %d", TempHealth);
	HealthBonus += TempHealth;	
	
	new weapon = GetPlayerWeaponSlot(client, 4);
	new bool:hasPills = !(weapon == -1 || !IsValidEntity(weapon));
	
	//PrintToChatAll("hasPills -> %i", hasPills);
	
	if (hasPills)
	{
		HealthBonus += 12;
	}
	
	//PrintToChatAll("HealthBonus -> %i (p%d + t%d + %i)", HealthBonus, PermHealth, TempHealth, (hasPills) ? 12 : 0);
	
	return HealthBonus;
	
}