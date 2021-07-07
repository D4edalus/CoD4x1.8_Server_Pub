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



//typedef char[32] plugin_event_t;

enum PluginEvents{
    // plugin handling events

    //PLUGINS_ONINIT,
    PLUGINS_ONINFOREQUEST,

    // runtime events:

    PLUGINS_ONPLAYERDC,
    PLUGINS_ONPLAYERCONNECT,
    PLUGINS_ONEXITLEVEL,
    PLUGINS_ONMESSAGESENT,
    PLUGINS_ONFRAME,		//new
    PLUGINS_ONONESECOND,	// new
    PLUGINS_ONTENSECONDS,
    PLUGINS_ONCLIENTAUTHORIZED,	// new
    PLUGINS_ONCLIENTSPAWN,
    PLUGINS_ONCLIENTENTERWORLD,
    PLUGINS_ONTCPSERVERPACKET,	// new
    PLUGINS_ONUDPNETEVENT,
    PLUGINS_ONUDPNETSEND,
    PLUGINS_ONSPAWNSERVER,
    PLUGINS_ONPREFASTRESTART,
    PLUGINS_ONPOSTFASTRESTART,
    PLUGINS_ONCLIENTUSERINFOCHANGED,
    PLUGINS_ONCLIENTMOVECOMMAND,
    PLUGINS_ONPLAYERWANTRESERVEDSLOT,
    PLUGINS_ONFSSTARTED,
    PLUGINS_ONPLAYERGOTAUTHINFO,
    PLUGINS_ONPLAYERADDBAN,
    PLUGINS_ONPLAYERGETBANSTATUS,
    PLUGINS_ONPLAYERREMOVEBAN,
	  PLUGINS_ONMODULELOADED,
    PLUGINS_ONSCREENSHOTARRIVED,
    PLUGINS_ONDEMOARRIVED,
    PLUGINS_ONTERMINATE,
    PLUGINS_ITEMCOUNT

};

extern char PHandler_Events[PLUGINS_ITEMCOUNT][32]; // defined in plugin_handler.c
