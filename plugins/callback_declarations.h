/*
===========================================================================
    Copyright (C) 2010-2013  Ninja and TheKelm

    This file is part of CoD4X Plugin Handler source code.

    CoD4X Plugin Handler source code is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    CoD4X Plugin Handler source code is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
===========================================================================
*/


#ifndef PLUGIN_INCLUDES

    #error Please include pinc.h instead!

#endif /*PLUGIN_INCLUDES*/

    // ----------------------------------------------------------------------------//
    // Callbacks available for use in plugin API, sorted alphabetically by module. //
    // ----------------------------------------------------------------------------//

	PCL void OnInfoRequest(pluginInfo_t *info);
	PCL int OnInit();
	PCL void OnMessageSent(char* message, int slot, qboolean *show, int mode);
	PCL void OnPreFastRestart();
	PCL void OnExitLevel();
	PCL void OnPostFastRestart();
	PCL void OnSpawnServer();
	PCL void OnFrame();
	PCL void OnOneSecond();
	PCL void OnTenSeconds();
	PCL void OnUdpNetEvent(netadr_t* from, void* data, int size, qboolean* returnNow);
	PCL void OnUdpNetSend(netadr_t* to, void* data, int len, qboolean* returnNow);
	PCL void OnPlayerConnect(int clientnum, netadr_t* netaddress, char* pbguid, char* userinfo, int authstatus, char* deniedmsg,  int deniedmsgbufmaxlen);
	PCL void OnPlayerDC(client_t* client, const char* reason);
	PCL void OnClientSpawn(gentity_t* ent);
	PCL void OnClientEnterWorld(client_t* client);
	PCL void OnClientUserinfoChanged(client_t* client);
	PCL void OnClientMoveCommand(client_t* client, usercmd_t* ucmd);
	PCL void OnPlayerWantReservedSlot(netadr_t* from, char* pbguid, char* userinfo, int authstate, qboolean *isallowed);
	PCL void OnModuleLoaded(client_t* client, char* fullpath, long checksum);
	PCL void OnScreenshotArrived(client_t* client, const char* path);
	PCL void OnDemoArrived(client_t* client,char *demoname);
	//Following function gets called when the player has basic authentication completed. Here we got a pbguid, the uid which is a short steamid, rejectmsg can be written a text to but never exceed 1023 characters, returnNow will keep the client idle when set to true, client_t*
	PCL void OnPlayerGotAuthInfo(netadr_t* from, uint64_t* playerid, uint64_t *steamid, char *rejectmsg /*never exceed a limit of 1023 chars*/, qboolean *returnNow, client_t* cl); //an unverified playerid, SteamID "steamIdPending" and "clanIdPending" is not getting verified yet at all - you can get this info from client_t* structure
  PCL void OnPlayerAddBan(baninfo_t* baninfo);
  PCL void OnPlayerGetBanStatus(baninfo_t* baninfo, char* message, int len);
  PCL void OnPlayerRemoveBan(baninfo_t* baninfo);
  PCL void OnTerminate(); //Plugins using threads have to listen to this and terminate all threads without keeping critical sections locked. Plugins are susposed not to return from this functions until all child threads are terminated
