/*
===========================================================================
    Copyright (C) 2010-2013  Ninja and TheKelm

    This file is part of CoD4X18-Server source code.

    CoD4X18-Server source code is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    CoD4X18-Server source code is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
===========================================================================
*/



#ifndef __SCR_VM_FUNCTIONS_H__
#define __SCR_VM_FUNCTIONS_H__

#include "scr_vm.h"
#include "g_sv_shared.h"

#ifndef SCR_ENTREF_DEFINED
#define SCR_ENTREF_DEFINED
typedef int scr_entref_t;
#endif

void GSC_writeToConsole();
void PlayerCmd_GetUid(scr_entref_t arg);
void PlayerCmd_SetUid(scr_entref_t arg);
void PlayerCmd_GetGuid(scr_entref_t arg);
void PlayerCmd_GetSteamID(scr_entref_t arg);
void PlayerCmd_GetPlayerID(scr_entref_t arg);
void PlayerCmd_GetPower(scr_entref_t arg);
void PlayerCmd_SetPower(scr_entref_t arg);
void PlayerCmd_SetStat(scr_entref_t arg);
void PlayerCmd_GetStat(scr_entref_t arg);
void PlayerCmd_GetUserinfo(scr_entref_t arg);
void PlayerCmd_GetPing(scr_entref_t arg);
void PlayerCmd_SetGravity(scr_entref_t arg);
void PlayerCmd_SetGroundReferenceEnt(scr_entref_t arg);
void PlayerCmd_SetJumpHeight(scr_entref_t arg);
void PlayerCmd_SetMoveSpeed(scr_entref_t arg);
void PlayerCmd_GetGeoLocation(scr_entref_t arg);
void PlayerCmd_GetCountedFPS(scr_entref_t arg);
void PlayerCmd_GetSpectatorClient(scr_entref_t arg);

void PlayerCmd_ForwardButtonPressed(scr_entref_t object);
void PlayerCmd_BackButtonPressed(scr_entref_t object);
void PlayerCmd_MoveLeftButtonPressed(scr_entref_t object);
void PlayerCmd_MoveRightButtonPressed(scr_entref_t object);
void PlayerCmd_SprintButtonPressed(scr_entref_t object);
void PlayerCmd_ReloadButtonPressed(scr_entref_t object);
void PlayerCmd_LeanLeftButtonPressed(scr_entref_t object);
void PlayerCmd_LeanRightButtonPressed(scr_entref_t object);
void PlayerCmd_IsProning(scr_entref_t object);
void PlayerCmd_IsCrouching(scr_entref_t object);
void PlayerCmd_IsStanding(scr_entref_t object);
void PlayerCmd_JumpButtonPressed(scr_entref_t object);
void PlayerCmd_IsInADS(scr_entref_t object);
void PlayerCmd_HoldBreathButtonPressed(scr_entref_t object);
void PlayerCmd_ADSButtonPressed(scr_entref_t object);

void GScr_StrTokByPixLen();
void GScr_StrTokByLen();
void GScr_StrPixLen();
void GScr_StrColorStrip();
void GScr_StrRepl();
void GScr_CopyString();
void GScr_GetRealTime();
void GScr_TimeToString();
void GScr_SHA256();
void GScr_CbufAddText();
void GScr_CbufAddTextEx();
void GScr_FS_FOpen();
void GScr_FS_FClose();
void GScr_FS_FCloseAll();
void GScr_FS_TestFile();
void GScr_FS_ReadLine();
void GScr_FS_WriteLine();
void GScr_FS_Remove();
void GScr_SpawnBot();
void GScr_RemoveAllBots();
void GScr_RemoveBot();
void GScr_KickClient();
void GScr_BanClient();
void PlayerCmd_spawn(scr_entref_t arg);
void GScr_NewHudElem();
void GScr_NewClientHudElem();
void HECmd_SetText(scr_entref_t entnum);
void GScr_Spawn();
void GScr_SpawnVehicle();
void GScr_SpawnHelicopter();
void GScr_VectorAdd();
void GScr_TypeTest();
__cdecl void ClientScr_SetSessionTeam(gclient_t* gcl, client_fields_t* gfl);

void PlayerCmd_GetSteamGroupMembership(scr_entref_t arg);

void Scr_PrecacheString_f();
void Scr_Destroy_f(scr_entref_t hud_elem_num);

void GScr_IsCvarDefined();

void Scr_IsArray_f();

/* SQL */
void gsc_mysql_version();
void gsc_mysql_init();
void gsc_mysql_real_connect(int mysql, char *host, char *user, char *pass, char *db, int port);
void gsc_mysql_close(int mysql);
void gsc_mysql_query(int mysql, char *sql);
void gsc_mysql_errno(int mysql);
void gsc_mysql_error(int mysql);
void gsc_handle_row();

#endif
