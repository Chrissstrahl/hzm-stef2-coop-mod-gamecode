//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// Based upon code from the HaZardModding Coop Mod Level Scripts created at 2006
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING SERVER RELATED FUNCTIONS FOR THE HZM CO-OP MOD

//HAZARDMODDING CO-OP SCRIPT MODIFICATION �2006-2018 SOME RIGHTS RESERVED AND
//PRIMARY (IP)INTELLECTUAL PROPERTY ON THE HZM COOP MOD HELD BY CHRISTIAN SEBASTIAN STRAHL, ALIAS CHRISSSTRAHL.

//YOU ARE EXPLICITE FORBIDDEN TO PUBLISH A MODIFIED VARIANT OF THIS CODE,
//ANY MATERIALS OR INTELLECTUAL PROPERTY OF THIS FILE WITHOUT THE EXPLICIT
//WRITTEN PERMISSION OF THE RESPECTIVE OWNERS!

//YOU MAY USE CODE PARTS AS LONG AS THEY DO NOT COMPROMISE THE GAME SAFTY
//LOCAL AND INTERNATIONAL LAWS, AS WELL AS VIOLATE UPON THE ENDCLIENT ITS PRIVACY

//CONTACT: chrissstrahl@yahoo.de [Christian Sebastian Strahl, Germany]

#ifndef __COOPSERVER_HPP__
#define __COOPSERVER_HPP__
#include "_pch_cpp.h"

//we wana see if we can memorize all models and animations loadad
//and then compare them to when new ones are spawned or chached
//the reason is that we need to keep track of the tiki model cache usage
//the tikicache does not get reset after mapload, so we are struggeling
//with a server side buffer that is bigger than the client buffer
//making models behave strangely or even disapear or disfunction
//ultimatively braking the game functunality and making players
//stuck in the mission, which is obviousely not what we want.
//
//NOW THIS is a atempt to keep dynamically track of the cache,
//by monitoring the tikis and skas loadad. Forcing the server to quit
//once the cache is full, so that it shall automatically be restarted
//by a batch file or linux script with a empty tiki model chace again.
//
//I could not find any other way to do this, since we do not have access
//to the part or code that is suppose to manage this. Also these commands
//have been tested and did not help with a dedicated server: flushtiki,
//killserver, startserver
static int iTIKIS = 0;
static int iSKAS = 0;
static int iSPRITES = 0;

#define COOP_MAX_SAFE_TIKI_LOAD		150	//was 200//was 250
#define COOP_MAX_SAFE_SKA_LOAD		90 //was 110//was 140
#define COOP_MAX_SAFE_COMBINED_LOAD	230 //was 250

#define COOP_DEFAULT_TEAMICON	false
#define COOP_DEFAULT_LASTMANSTANDING	false
#define COOP_DEFAULT_RESPAWNTIME	0
#define COOP_DEFAULT_MAXSPEED		300
#define COOP_DEFAULT_FF				0.25f
#define COOP_DEFAULT_SKILL			1
#define COOP_DEFAULT_AWARDS			1
#define COOP_DEFAULT_DEADBODIES		0 //[b607]
#define COOP_DEFAULT_STASISTIME		7 //[b607]
#define COOP_DEFAULT_AIRACCELERATE	2 //[b607]

#define COOP_MAX_SETUP_TRIES		5 //how often do we try to run setup

//TEST VALUES; CHANGE BACK TO PREVIOUSE !!!!!
/*
#define COOP_MAX_SAFE_TIKI_LOAD		15
#define COOP_MAX_SAFE_SKA_LOAD		9
#define COOP_MAX_SAFE_COMBINED_LOAD	23
*/
int coop_serverConfigstringRemoveCombatSounds(str sActorname);//[b607] chrissstrahl - used to remove combatsounds for named actor
void coop_serverConfigstringRemoveNonCoopStrings(); //[b607] chrissstrahl - try to minimize the usage of configstrings due to cl_parsegamestate issue
int coop_serverConfigstringRemove(str sRem); //[b607] chrissstrahl - try to minimize the usage of configstrings due to cl_parsegamestate issue
bool coop_serverCheckEndMatch(void);
bool coop_serverLmsCheckFailure( void );
bool coop_serverError( str sError , bool bFatal );
void coop_serverInizializeGameVars( void );
void coop_serverSaveGameVars( const str &name , const str &value );
bool coop_serverManageReboot( str sMapToLoad );
bool coop_serverManageReboot(str sMapToLoad, Player* player); //added [b607]
void coop_serverMonitorTikiCache( str sData );
void coop_serverManageClientData( str sMap );
void coop_serverSaveClientData( Player *player );
void coop_serverSaveClientDataWrite( Player *player );
void coop_serverSaveAllClientData( void );
void coop_serverResetClientData( Player *player );
void coop_serverResetAllClientData( void );
void coop_afterChristmasEvent_deleteTrees( void );
void coop_serverCoop();
void coop_serverSetup( void );
bool coop_serverRunScriptThread( str scriptThread );
void coop_serverManageAi( void );
str coop_serverModifiedFile( str standardPath );
void coop_serverThink( void );

#endif /* coopServer.hpp */
