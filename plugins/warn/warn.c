/* This file has been generated by the Hex-Rays decompiler.
   Copyright (c) 2007-2014 Hex-Rays <info@hex-rays.com>

   Detected compiler: GNU C++
*/

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "../pinc.h"


//-------------------------------------------------------------------------
// Function declarations

signed int __cdecl Q_stricmpn(const char *a1, const char *a2, int a3); // idb
signed int __cdecl Q_stricmp(int a1, int a2);
void OpenWarnFile(void);
int SV_DisplayWarnCmd(); // weak
int SV_ClearWarnCmd(); // weak
char *__cdecl Q_strncpyz(char *dest, char *src, int a3);
char *__cdecl Q_strcat(char *, int a2, const char *src);
void __cdecl SV_IssueWarnCmd();

cvar_t *c_wlimit;
cvar_t *c_wbantime;
FILE *w_filehandle; // idb
int savedwarns[MAX_CLIENTS];

int Q_stricmpn(const char *s1, const char *s2, int n) {
	int		c1, c2;

	if (s1 == NULL) {
		if (s2 == NULL)
			return 0;
		else
			return -1;
	}
	else if (s2 == NULL)
		return 1;



	do {
		c1 = *s1++;
		c2 = *s2++;

		if (!n--) {
			return 0;		// strings are equal until end point
		}

		if (c1 != c2) {
			if (c1 >= 'a' && c1 <= 'z') {
				c1 -= ('a' - 'A');
			}
			if (c2 >= 'a' && c2 <= 'z') {
				c2 -= ('a' - 'A');
			}
			if (c1 != c2) {
				return c1 < c2 ? -1 : 1;
			}
		}
	} while (c1);

	return 0;		// strings are equal
}

//----- (00000AD3) --------------------------------------------------------
signed int __cdecl Q_stricmp(int a1, int a2)
{
  signed int result; // eax@3

  if ( a1 && a2 )
    result = Q_stricmpn((const char *)a1, (const char *)a2, 99999);
  else
    result = -1;
  return result;
}

//----- (00000B04) --------------------------------------------------------
void __cdecl OnPlayerDC(client_t *client, const char* reason)
{
  savedwarns[NUMFORCLIENT(client)] = 0;
}

//----- (00000B25) --------------------------------------------------------
void OpenWarnFile(void)
{
  char filename[512]; // [sp+10h] [bp-208h]@1
  char stringbuf[512];
  Plugin_Cvar_VariableStringBuffer("fs_homepath", stringbuf, sizeof(stringbuf));
  snprintf(filename, 0x200u, "%s/warn.dat", stringbuf);
  w_filehandle = fopen(filename, "a+");
  Plugin_Printf("Warn_System: File has been opened!\n");
}

//----- (00000B87) --------------------------------------------------------
void OnExitLevel()
{
  fclose(w_filehandle);
  OpenWarnFile();
}

//----- (00000BA1) --------------------------------------------------------
void SV_DisplayWarnCmd_f()
{
  client_t *wcl; // eax@1
  int clientnum;

  if(Plugin_Cmd_Argc() > 2)
  {
    Plugin_Printf("Plugin_WarnCount_Usage: %s (<clientid>)\n", Plugin_Cmd_Argv(0));
    return;
  }
  wcl = NULL;
  int invokernum = Plugin_Cmd_GetInvokerSlot();

  if(Plugin_Cmd_Argc() == 2)
  {
      wcl = Plugin_SV_Cmd_GetPlayerClByHandle(Plugin_Cmd_Argv(1));//Prints message if bad in most cases
      if(wcl == NULL)
      {
        if(invokernum < 0 || invokernum >= MAX_CLIENTS)
        {
          Plugin_Printf("Can't display warnings for such a player\n");
        }else{
          Plugin_ChatPrintf(invokernum, "Can't display warnings for such a player\n");
        }
        return;
      }
      clientnum = NUMFORCLIENT(wcl);
      if(invokernum < 0 || invokernum >= MAX_CLIENTS)
      {
        Plugin_Printf( "Player %s^7 has %i warnings out of %i\n", wcl->name, savedwarns[clientnum], c_wlimit->integer);
      }else{
        Plugin_ChatPrintf(invokernum, "^1Server_Warning^7: %s^7 has %i warnings out of %i\n", wcl->name, savedwarns[clientnum], c_wlimit->integer);
      }
  }else{
      //One argument only
      if(invokernum < 0 || invokernum >= MAX_CLIENTS)
      {
        //One argument but not from a player
        Plugin_Printf("Plugin_WarnCount_Usage: %s <clientid>\n", Plugin_Cmd_Argv(0));
        return;
      }
      //Print to the invoking client the own status
      Plugin_ChatPrintf(invokernum, "^1Server_Warning: You have %i warnings out of %i\n", savedwarns[invokernum], c_wlimit->integer);
  }

}

// BA1: using guessed type int SV_DisplayWarnCmd();
// 27B0: using guessed type int Plugin_Cmd_Argc(void);
// 27B4: using guessed type int Plugin_ChatPrintf(_DWORD, const char *, ...);
// 27C8: using guessed type int Plugin_Cmd_GetInvokerSlot(void);

//----- (00000C66) --------------------------------------------------------
void SV_ClearWarnCmd_f()
{
  int clientnum; // esi@1
  client_t *wcl; // eax@1
  int v7; // [sp+1Ch] [bp-1Ch]@1

  if(Plugin_Cmd_Argc() != 2)
  {
    Plugin_Printf("Plugin_RemoveWarnings_Usage: %s <clientnumber>\n", Plugin_Cmd_Argv(0));
    return;
  }

  v7 = Plugin_Cmd_GetInvokerSlot();

  wcl = Plugin_SV_Cmd_GetPlayerClByHandle(Plugin_Cmd_Argv(1));//Prints message if bad in most cases
  if(wcl == NULL)
  {
      if(v7 < 0 || v7 >= MAX_CLIENTS)
      {
          Plugin_Printf("Can't clear warn for such a player\n");
      }else{
          Plugin_ChatPrintf(v7, "Can't clear warn for such a player\n");
      }
      return;
  }

  clientnum = NUMFORCLIENT(wcl);

  if ( wcl->state < CS_PRIMED )
  {
    Plugin_PrintError("^7This player is not in world\n");
    return;
  }
  savedwarns[clientnum] = 0;
  Plugin_ChatPrintf(clientnum, "^1Server_Warning: ^7Warnings removed\n");
  if(v7 < 0 || v7 >= MAX_CLIENTS)
  {
      Plugin_Printf("^1Server_Warning: ^7Removed Warnings for %s at slot %i\n", wcl->name, savedwarns[clientnum]);
  }else{
      Plugin_ChatPrintf(v7, "^1Server_Warning: ^7Removed Warnings for %s at slot %i\n", wcl->name, savedwarns[clientnum]);
  }

}
// C66: using guessed type int SV_ClearWarnCmd();
// 27B0: using guessed type int Plugin_Cmd_Argc(void);
// 27B4: using guessed type int Plugin_ChatPrintf(_DWORD, const char *, ...);
// 27C8: using guessed type int Plugin_Cmd_GetInvokerSlot(void);

//----- (00000D40) --------------------------------------------------------

//----- (00000DBF) --------------------------------------------------------
char *__cdecl Q_strncpyz(char *dest, char *src, int a3)
{
  char *result; // eax@1

  result = strncpy(dest, src, a3 - 1);
  dest[a3 - 1] = 0;
  return result;
}

//----- (00000DF4) --------------------------------------------------------
PCL int OnInit()
{
  Plugin_AddCommand("warn", SV_IssueWarnCmd, 60);
  c_wlimit = Plugin_Cvar_RegisterInt("wlimit", 3, 1, 10, 0, "Maximum number of warnings to be banned after them.");
  c_wbantime = Plugin_Cvar_RegisterInt("wbantime", 32*60, -1, 43200, 0, "Warning system ban duration.");
  Plugin_AddCommand("warnr", SV_ClearWarnCmd_f, 80);
  Plugin_AddCommand("warnc", SV_DisplayWarnCmd_f, 1);
  OpenWarnFile();
  return 0;
}
// BA1: using guessed type int SV_DisplayWarnCmd();
// C66: using guessed type int SV_ClearWarnCmd();
// 27BC: using guessed type int __cdecl Plugin_AddCommand(_DWORD, _DWORD, _DWORD);

//----- (00000ECC) --------------------------------------------------------
char *__cdecl Q_strcat(char *a1, int a2, const char *src)
{
  unsigned int v3; // kr04_4@1

  v3 = strlen(a1) + 1;
  return Q_strncpyz(&a1[v3 - 1], (char *)src, a2 - (v3 - 1));
}

//----- (00000F08) --------------------------------------------------------
void __cdecl SV_IssueWarnCmd()
{
  int numargs; // ebx@1
  int clientnum; // edi@1
  int i; // ebx@4
  const char *cmdname; // eax@10
  client_t *wcl; // [sp+2Ch] [bp-11Ch]@1
  char v12[256]; // [sp+30h] [bp-118h]@1
  char adrbuf[128];
  char steamidstr[128];
  char playeridstr[128];
  char invname[128];

  numargs = Plugin_Cmd_Argc();

  v12[0] = 0;
  if ( numargs <= 2 )
  {
    cmdname = Plugin_Cmd_Argv(0);
    Plugin_Printf("Plugin_Warn_Usage: %s <player> <reason>\n", cmdname);
    Plugin_Printf("Plugin_Warn: The limit is %i", c_wlimit->integer);
    return;
  }

  wcl = Plugin_SV_Cmd_GetPlayerClByHandle(Plugin_Cmd_Argv(1));//Prints message if bad in most cases
  if(wcl == NULL)
  {
      Plugin_Printf("Can't warn such a player\n");
      return;
  }

  clientnum = NUMFORCLIENT(wcl);

  if ( wcl->state < CS_ACTIVE)
  {
    Plugin_PrintError("^7This player is not in world\n");
    return;
  }

  for ( i = 2; i < Plugin_Cmd_Argc(); ++i )
  {
        Q_strcat(v12, sizeof(v12), Plugin_Cmd_Argv(i));
        Q_strcat(v12, sizeof(v12), " ");
  }
  ++savedwarns[clientnum];
  Plugin_ChatPrintf(clientnum, "^1Server_Warning: You have been warned with reason: %s\n", v12);
  if(savedwarns[clientnum] == (c_wlimit->integer - 1))
  {
    Plugin_ChatPrintf(clientnum, "^1Attention^7: If you get one more warning you face a %d hours cooldown\n", c_wbantime->integer / 60);
  }

  Plugin_SteamIDToString(Plugin_Cmd_GetInvokerSteamID(), steamidstr, sizeof(steamidstr));
  Plugin_SteamIDToString(wcl->playerid, playeridstr, sizeof(playeridstr));

  fprintf(w_filehandle, "AdminName: %s Admin: %s PlayerName: %s PlayerGUID\\UID: %s PLayer IP: %s\n", Plugin_Cmd_GetInvokerName(invname, sizeof(invname)), steamidstr, wcl->name, playeridstr, Plugin_NET_AdrToStringMT(&wcl->netchan.remoteAddress, adrbuf, sizeof(adrbuf)));
  if ( savedwarns[clientnum] == c_wlimit->integer )
  {
	char banreason[1024];
	snprintf(banreason, sizeof(banreason), "Too many warnings received in one session. Last warning is: %s", v12);
      banreason[1023] = 0;
      Plugin_BanClient(clientnum, c_wbantime->integer, 0, banreason);
      savedwarns[clientnum] = 0;
  }
}

// ALL OK, 18 function(s) have been successfully decompiled





PCL void OnInfoRequest(pluginInfo_t *info){	// Function used to obtain information about the plugin
    // Memory pointed by info is allocated by the server binary, just fill in the fields

    // =====  MANDATORY FIELDS  =====
    info->handlerVersion.major = PLUGIN_HANDLER_VERSION_MAJOR;
    info->handlerVersion.minor = PLUGIN_HANDLER_VERSION_MINOR;	// Requested handler version

    // =====  OPTIONAL  FIELDS  =====
    info->pluginVersion.major = 1;
    info->pluginVersion.minor = 0;	// Plugin version
    strncpy(info->fullName,"Warn system plugin by DeltaMX", sizeof(info->fullName)); //Full plugin name
    strncpy(info->shortDescription,"This plugin will enable admins to warn players before being banned.",sizeof(info->shortDescription)); // Short plugin description
    strncpy(info->longDescription,"This plugin will enable admins to issue warnings to players at which after a certain limit will ban these players",sizeof(info->longDescription));
}
