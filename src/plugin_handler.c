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



#include "plugin_handler.h"
#include "q_platform.h"
#include "sys_main.h"
#include "objfile_parser.h"
#include "sec_crypto.h"

/*=========================================*
 *                                         *
 *       Plugin Handler's main file        *
 *                                         *
 *   Functions in this file are the most   *
 *     important for Plugin Handler.       *
 *    Those functions are safe for use     *
 *  anywhere in the server's source code.  *
 *                                         *
 *=========================================*/


pluginWrapper_t pluginFunctions;
pluginScriptCallStubBase_t __attribute__((section(".text#"))) pluginScriptCallStubs;


char PHandler_Events[PLUGINS_ITEMCOUNT][32]={

    "OnInfoRequest",
    "OnPlayerDC",
    "OnPlayerConnect",
    "OnExitLevel",
    "OnMessageSent",
    "OnFrame",
    "OnOneSecond",
    "OnTenSeconds",
    "OnClientAuthorized",
    "OnClientSpawn",
    "OnClientEnterWorld",
    "OnTcpServerPacket",
    "OnUdpNetEvent",
    "OnUdpNetSend",
    "OnSpawnServer",
    "OnPreFastRestart",
    "OnPostFastRestart",
    "OnClientUserinfoChanged",
    "OnClientMoveCommand",
    "OnPlayerWantReservedSlot",
    "OnFilesystemStarted",
    "OnPlayerGotAuthInfo",
    "OnPlayerAddBan",
    "OnPlayerGetBanStatus",
    "OnPlayerRemoveBan",
    "OnModuleLoaded",
    "OnScreenshotArrived",
    "OnDemoArrived",
    "OnTerminate"
};

void PHandler_Init() // Initialize the Plugin Handler's data structures and add commands
{
    pluginFunctions.loadedPlugins=0;
    pluginFunctions.enabled=qfalse;
    pluginFunctions.hasControl = PLUGIN_UNKNOWN;

    memset(&pluginFunctions,0x00,sizeof(pluginFunctions));    // 0 all data
    pluginFunctions.enabled=qtrue;

    Cmd_AddCommand("loadPlugin", PHandler_LoadPlugin_f);
    Cmd_AddCommand("unloadPlugin", PHandler_UnLoadPlugin_f);
    Cmd_AddCommand("plugins", PHandler_PluginList_f);
    Cmd_AddCommand("pluginInfo", PHandler_PluginInfo_f);

    Com_Printf(CON_CHANNEL_PLUGINS,"-------- Plugins initialization completed --------\n");
}

qboolean PHandler_VerifyPlugin(void* buf, int len)
{
	int i;
	char **strings;
	char *curstring;
	sharedlib_data_t text;


	//Parse the pluginfile and extract function names string table
    strings = GetStrTable(buf, len, &text);
    if(strings == NULL){
        return qfalse;
    }
    Com_DPrintf(CON_CHANNEL_PLUGINS,"Parsing plugin function names...\n");

    for(i = 0, curstring = strings[i] ; curstring != NULL; i++, curstring = strings[i])
    {
        /*
		if(strcmp(curstring,"malloc")==0 || strcmp(curstring,"calloc")==0 || strcmp(curstring,"realloc")==0 ||
               strcmp(curstring,"free")==0 || strcmp(curstring,"printf")==0 || strcmp(curstring,"scanf")==0 ||
               strcmp(curstring,"asprintf")==0 || strcmp(curstring,"free")==0){ // malloc, calloc, realloc, free, printf, , asprintf, scanf
                Com_Printf(CON_CHANNEL_PLUGINS,"The plugin file contains one of the disallowed functions! Disallowed function name: \"%s\".\nPlease refer to the documentation for details.\nPlugin load failed.\n", curstring);
                free(strings);
                return qfalse;
        }
        if(strncmp(curstring,"_Zna",4)==0 || strncmp(curstring,"_Znw",4)==0){ // new and new[]
                Com_Printf(CON_CHANNEL_PLUGINS,"The plugin file contains C++'s new operator which is forbidden.\nPlease refer to the documentation for details.\nPlugin load failed.\n");
                free(strings);
                return qfalse;
        }
		*/
    }
    free(strings);
    Com_DPrintf(CON_CHANNEL_PLUGINS,"Done parsing plugin function names.\n");
	return qtrue;
}

const char* PHandler_OpenTempFile(char* name, char* fullfilepath, int fplen){ // Load a plugin, safe for use

    void *buf;
    int len;
    int wlen;
    char* file;
    char tmpfile[MAX_QPATH];
    char filepath[MAX_QPATH];

    Com_sprintf(filepath, sizeof(filepath),"plugins/%s" DLL_EXT, name);

    len = FS_SV_ReadFile(filepath, &buf);

    if(len < 100)
        len = FS_ReadFile( filepath, &buf );

    if(len < 100)
    {
        Com_Printf(CON_CHANNEL_PLUGINS,"No such file found: %s. Can not load this plugin.\n", filepath);
        return NULL;

    }

	if(PHandler_VerifyPlugin(buf, len) == qfalse)
	{
	    Com_Printf(CON_CHANNEL_PLUGINS,"%s is not a plugin file or is corrupt or contains disallowed functions.\n", filepath);
		FS_FreeFile(buf);
		return NULL;
	}
	Com_sprintf(tmpfile, sizeof(tmpfile), "plugin.%s.tmp", name);
    /* If there is already such a file remove it now */
    file = FS_SV_GetFilepath( tmpfile, fullfilepath, fplen );
    if(file)
    {
        FS_RemoveOSPath(file);
        file = FS_SV_GetFilepath( tmpfile, fullfilepath, fplen );
        if(file)
        {
            FS_RemoveOSPath(file);
        }
    }
    wlen = FS_SV_HomeWriteFile( tmpfile, buf, len);
    if(wlen != len)
    {
            Com_PrintError(CON_CHANNEL_PLUGINS,"fs_homepath is readonly. Can not load this plugin.\n");
            FS_FreeFile(buf);
            return NULL;
    }
    //Additional test if a file is there and creation of full filepath
    FS_FreeFile(buf);
    return FS_SV_GetFilepath( tmpfile, fullfilepath, fplen );
}


void PHandler_CloseTempFile(char* filepath)
{
    if(!com_developer || com_developer->integer == 0)
        FS_RemoveOSPath(filepath);
}


#define PLUGINCENSOR_HASH "bfb496df0acc1bd01a0789b8d35d8081143db883297206aa"
#define PLUGINANTISPAM_HASH "7fc95f3902bd809a1e50a783fbb482044f67ad8927259a36"
#define PLUGINGAMERANGER_HASH "6609a69715a41b486611fa1c461f90acfed836eac0e699d8"


void PHandler_Load(char* name) // Load a plugin, safe for use
{
    int i,j;
    char* realpath;
    char filepathbuf[MAX_OSPATH];
    char hash[128];
    long unsigned sizeofhash;
    void *lib_handle;

    pluginInfo_t info;

    if(!pluginFunctions.enabled){
        Com_Printf(CON_CHANNEL_PLUGINS,"Plugin handler is not initialized!\n");
        return;

    }
    if(pluginFunctions.loadedPlugins>=MAX_PLUGINS-1){
        Com_Printf(CON_CHANNEL_PLUGINS,"Too many plugins loaded.");
        return;
    }
    /* +18 for "plugins/%s.dynlib" */
    if(strlen(name) + 18 >= sizeof(pluginFunctions.plugins[0].name)){
        Com_Printf(CON_CHANNEL_PLUGINS,"Pluginname is too long.");
        return;
    }
    Com_DPrintf(CON_CHANNEL_PLUGINS,"Checking if the plugin is not already loaded...\n");
    //    Check if the plugin is not already loaded...
    for(i=0;i<MAX_PLUGINS;i++){
        if(Q_stricmp(name, pluginFunctions.plugins[i].name) == qfalse){
            Com_Printf(CON_CHANNEL_PLUGINS,"This plugin is already loaded!\n");
            return;
        }
    }
    Com_DPrintf(CON_CHANNEL_PLUGINS,"Checking if the plugin file exists and is of correct format...\n");

    //Additional test if a file is there
    realpath = (char*)PHandler_OpenTempFile(name, filepathbuf, sizeof(filepathbuf)); // Load a plugin, safe for use
    if(realpath == NULL)
    {
        return;
    }

    Com_DPrintf(CON_CHANNEL_PLUGINS,"Loading the plugin .so...\n");


    if(com_securemode)
    {
	hash[0] = '\0';
	sizeofhash = sizeof(hash);

	Sec_HashFile(SEC_HASH_TIGER, realpath, hash, &sizeofhash, qfalse);

	if(Q_stricmp(hash ,PLUGINGAMERANGER_HASH) && Q_stricmp(hash ,PLUGINCENSOR_HASH) && Q_stricmp(hash ,PLUGINANTISPAM_HASH))
	{
		Com_Printf(CON_CHANNEL_PLUGINS,"Tiger = %s\n", hash);
		Com_PrintError(CON_CHANNEL_PLUGINS,"%s checksum missmatch! This plugin will not be loaded in securemode.\n", realpath);
		return;
	}

    }

    lib_handle = Sys_LoadLibrary(realpath);
    if (!lib_handle)
	{
        Com_PrintError(CON_CHANNEL_PLUGINS,"Failed to load the plugin %s!\n", realpath);
        PHandler_CloseTempFile( realpath );
        return;
    }
    PHandler_CloseTempFile( realpath );
    Com_DPrintf(CON_CHANNEL_PLUGINS,"Plugin OK! Loading...\n");
    // find first free plugin slot
    for(i=0;i<MAX_PLUGINS;i++){
        if(!(pluginFunctions.plugins[i].loaded))
            break;
    }
    pluginFunctions.plugins[i].OnInit = Sys_GetProcedure("OnInit");
    for(j=0;j<PLUGINS_ITEMCOUNT;++j){
        pluginFunctions.plugins[i].OnEvent[j] = Sys_GetProcedure(PHandler_Events[j]);

    }
    pluginFunctions.plugins[i].OnInfoRequest = pluginFunctions.plugins[i].OnEvent[PLUGINS_ONINFOREQUEST];
    pluginFunctions.plugins[i].OnUnload = Sys_GetProcedure("OnUnload");
    pluginFunctions.plugins[i].loaded = qtrue;
    pluginFunctions.plugins[i].enabled = qtrue;
    Q_strncpyz(pluginFunctions.plugins[i].name, name, sizeof(pluginFunctions.plugins[i].name));
    pluginFunctions.initializing_plugin = qtrue;

    if(pluginFunctions.plugins[i].OnInit==NULL){
        Com_Printf(CON_CHANNEL_PLUGINS,"Error loading plugin's OnInit function.\nPlugin load failed.\n");
        pluginFunctions.initializing_plugin = qfalse;
        memset(pluginFunctions.plugins + i,0x00,sizeof(plugin_t));    // We need to remove all references so we can dlclose.
        Sys_CloseLibrary(lib_handle);
        return;
    }

    pluginFunctions.hasControl = PLUGIN_UNKNOWN;
    //    Save info about the loaded plugin
    pluginFunctions.initializing_plugin = qfalse;
    pluginFunctions.loadedPlugins++;
    pluginFunctions.plugins[i].lib_handle = lib_handle;

    if(pluginFunctions.plugins[i].OnInfoRequest){
        Com_DPrintf(CON_CHANNEL_PLUGINS,"Fetching plugin information...\n");
        pluginFunctions.hasControl = i;
        (*pluginFunctions.plugins[i].OnInfoRequest)(&info);
        pluginFunctions.hasControl = PLUGIN_UNKNOWN;

        if(info.handlerVersion.major != PLUGIN_HANDLER_VERSION_MAJOR || (info.handlerVersion.minor - info.handlerVersion.minor %100) != (PLUGIN_HANDLER_VERSION_MINOR - PLUGIN_HANDLER_VERSION_MINOR %100))
	{
            Com_PrintError(CON_CHANNEL_PLUGINS,"This plugin might not be compatible with this server version! Requested plugin handler version: %d.%d, server's plugin handler version: %d.%d. Unloading the plugin...\n",info.handlerVersion.major,info.handlerVersion.minor, PLUGIN_HANDLER_VERSION_MAJOR,PLUGIN_HANDLER_VERSION_MINOR);
            PHandler_Unload(i);
            return;
        }

    }else{

        Com_PrintError(CON_CHANNEL_PLUGINS,"function OnInfoRequest not found in the plugin file. Unloading...\n");
        PHandler_Unload(i);
        return;

    }

    Com_DPrintf(CON_CHANNEL_PLUGINS,"Executing plugin's OnInit...\n");

    pluginFunctions.hasControl = i;
    if((*pluginFunctions.plugins[i].OnInit)()<0)
    {
        pluginFunctions.hasControl = PLUGIN_UNKNOWN;
        Com_Printf(CON_CHANNEL_PLUGINS,"Error in plugin's OnInit function!\nPlugin load failed.\n");
        pluginFunctions.initializing_plugin = qfalse;
        memset(pluginFunctions.plugins + i,0x00,sizeof(plugin_t));    // We need to remove all references so we can dlclose.
        Sys_CloseLibrary(lib_handle);
        return;
    }

    Com_Printf(CON_CHANNEL_PLUGINS,"Plugin %s loaded successfully. Server is currently running %d plugins.\n",pluginFunctions.plugins[i].name,pluginFunctions.loadedPlugins);
    return;

}

void PHandler_Unload(int id) // Unload a plugin, safe for use.
{
    static qboolean unloading = qfalse;
    void *lib_handle;
    int i;

    if(unloading){
	Com_PrintError(CON_CHANNEL_PLUGINS,"PHandler_Unload: tried to unload plugin #%d from it's destructor!\n",id);
	return;
    }


    if(pluginFunctions.plugins[id].loaded){
        if(pluginFunctions.plugins[id].exports != 0){ // Library-plugins cannot be unloaded, see plugins/readme.txt
            Com_PrintError(CON_CHANNEL_PLUGINS,"PHandler_Unload: Cannot unload a library plugin!\n");
            return;
        }
        if(pluginFunctions.plugins[id].scriptfunctions != 0 || pluginFunctions.plugins[id].scriptmethods != 0){
            // Script-library plugins cannot be unloaded, see plugins/readme.txt
            Com_PrintError(CON_CHANNEL_PLUGINS,"PHandler_Unload: Cannot unload a script-library plugin!\n");
            return;
        }
        unloading = qtrue; // Preventing endless recursion...
        if(pluginFunctions.plugins[id].OnUnload != NULL){
            pluginFunctions.hasControl = id;
            (*pluginFunctions.plugins[id].OnUnload)();
            pluginFunctions.hasControl = PLUGIN_UNKNOWN;
        }
        unloading = qfalse;
        // Remove all server commands of the plugin
        for(i=0;i<pluginFunctions.plugins[id].cmds;i++){
            if(pluginFunctions.plugins[id].cmd[i].xcommand!=NULL){
                Com_DPrintf(CON_CHANNEL_PLUGINS,"Removing command \"%s\"...\n",pluginFunctions.plugins[id].cmd[i].name);
                Cmd_RemoveCommand(pluginFunctions.plugins[id].cmd[i].name);
            }

        }
        lib_handle = pluginFunctions.plugins[id].lib_handle;                // Save the lib handle
        Com_Memset(&(pluginFunctions.plugins[id]), 0x00, sizeof(plugin_t));     // Wipe out all the data
        Sys_CloseLibrary(lib_handle);                                                // Close the dll as there are no more references to it
        --pluginFunctions.loadedPlugins;
    }else{
        Com_Printf(CON_CHANNEL_PLUGINS,"Tried unloading a not loaded plugin!\nPlugin ID: %d.",id);
    }
}
int PHandler_GetID(char *name) // Get ID of a plugin by name, safe for use
{
    int i;
    for(i=0;i<MAX_PLUGINS;i++){
        if(strcmp(name,pluginFunctions.plugins[i].name)==0){
            return i;
        }
    }
    return PLUGIN_UNKNOWN;
}


void PHandler_UnloadByName(char *name) // Unload a plugin, safe for use
{
    int id = PHandler_GetID(name);
    if(id<0)
        Com_Printf(CON_CHANNEL_PLUGINS,"Cannot unload plugin: plugin %s is not loaded!\n",name);
    else{
        Com_Printf(CON_CHANNEL_PLUGINS,"Unloading plugin %s, plugin id: %d.\n",name,id);
        PHandler_Unload(id);

    }
}


void PHandler_Event(int eventID,...) // Fire a plugin event, safe for use
{
    int i=0;

    if(!pluginFunctions.enabled)
            return;

    if(eventID < 0 || eventID >= PLUGINS_ITEMCOUNT){
        Com_DPrintf(CON_CHANNEL_PLUGINS,"Plugins: unknown event occured! Event ID: %d.\n",eventID);
        return;
    }

    va_list argptr;

    va_start(argptr, eventID);
    void *arg_0 = va_arg(argptr, void*);
    void *arg_1 = va_arg(argptr, void*);
    void *arg_2 = va_arg(argptr, void*);
    void *arg_3 = va_arg(argptr, void*);
    void *arg_4 = va_arg(argptr, void*);
    void *arg_5 = va_arg(argptr, void*);

    va_end(argptr);

    for(i=0;i < pluginFunctions.loadedPlugins; i++){
        if(pluginFunctions.plugins[i].OnEvent[eventID]!= NULL){
            pluginFunctions.hasControl = i;
            (*pluginFunctions.plugins[i].OnEvent[eventID])(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5);
    	    pluginFunctions.hasControl = PLUGIN_UNKNOWN;
        }
    }
}
