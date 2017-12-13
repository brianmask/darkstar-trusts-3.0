/*
===========================================================================

Copyright (c) 2010-2015 Darkstar Dev Teams

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses/

This file is part of DarkStar-server source code.

===========================================================================
*/

#include "../../common/timer.h"
#include "../../common/utils.h"

#include <string.h>
#include <vector>
#include <math.h>

#include "battleutils.h"
#include "charutils.h"
#include "puppetutils.h"
#include "../grades.h"
#include "../map.h"
#include "petutils.h"
#include "zoneutils.h"
#include "../entities/mobentity.h"
#include "../entities/charentity.h"
#include "../entities/automatonentity.h"
#include "../ability.h"
#include "../modifier.h"
#include "../lua/luautils.h"

#include "../ai/ai_automaton_dummy.h"
#include "../ai/ai_pet_dummy.h"
#include "../ai/ai_mob_dummy.h"
#include "../ai/ai_ultimate_summon.h"

#include "../packets/char_sync.h"
#include "../packets/char_update.h"
#include "../packets/entity_update.h"
#include "../packets/pet_sync.h"


struct Pet_t
{
    look_t		look;		// ??????? ???
    string_t	name;		// ???
    ECOSYSTEM	EcoSystem;	// ???-???????

    uint8		minLevel;	// ??????????-?????????  ???????
    uint8		maxLevel;	// ???????????-????????? ???????

    uint8       name_prefix;
    uint8		size;		// ?????? ??????
    uint16		m_Family;
    uint32		time;		// ????? ????????????? (????? ?????????????? ??? ??????? ???????????? ?????? ???????)

    uint8		mJob;
    uint8		m_Element;
    float       HPscale;                             // HP boost percentage
    float       MPscale;                             // MP boost percentage

    uint16      cmbDelay;
    uint8 		speed;
    // stat ranks
    uint8        strRank;
    uint8        dexRank;
    uint8        vitRank;
    uint8        agiRank;
    uint8        intRank;
    uint8        mndRank;
    uint8        chrRank;
    uint8        attRank;
    uint8        defRank;
    uint8        evaRank;
    uint8        accRank;

    uint16       m_MobSkillList;

    // magic stuff
    bool hasSpellScript;
    uint16 spellList;

    // resists
    int16 slashres;
    int16 pierceres;
    int16 hthres;
    int16 impactres;

    int16 firedef;
    int16 icedef;
    int16 winddef;
    int16 earthdef;
    int16 thunderdef;
    int16 waterdef;
    int16 lightdef;
    int16 darkdef;

    int16 fireres;
    int16 iceres;
    int16 windres;
    int16 earthres;
    int16 thunderres;
    int16 waterres;
    int16 lightres;
    int16 darkres;

};

std::vector<Pet_t*> g_PPetList;

namespace petutils
{

    /************************************************************************
    *																		*
    *  ????????? ?????? ?????????? ????????									*
    *																		*
    ************************************************************************/

    void LoadPetList()
    {
        FreePetList();

        const int8* Query =
            "SELECT\
                pet_list.name,\
                modelid,\
                minLevel,\
                maxLevel,\
                time,\
                mobsize,\
                systemid,\
                mob_pools.familyid,\
                mob_pools.mJob,\
                pet_list.element,\
                (mob_family_system.HP / 100),\
                (mob_family_system.MP / 100),\
                mob_family_system.speed,\
                mob_family_system.STR,\
                mob_family_system.DEX,\
                mob_family_system.VIT,\
                mob_family_system.AGI,\
                mob_family_system.INT,\
                mob_family_system.MND,\
                mob_family_system.CHR,\
                mob_family_system.DEF,\
                mob_family_system.ATT,\
                mob_family_system.ACC, \
                mob_family_system.EVA, \
                hasSpellScript, spellList, \
                Slash, Pierce, H2H, Impact, \
                Fire, Ice, Wind, Earth, Lightning, Water, Light, Dark, \
                cmbDelay, name_prefix, mob_pools.skill_list_id \
                FROM pet_list, mob_pools, mob_family_system \
                WHERE pet_list.poolid = mob_pools.poolid AND mob_pools.familyid = mob_family_system.familyid";

        if (Sql_Query(SqlHandle, Query) != SQL_ERROR && Sql_NumRows(SqlHandle) != 0)
        {
            while (Sql_NextRow(SqlHandle) == SQL_SUCCESS)
            {
                Pet_t* Pet = new Pet_t();

                Pet->name.insert(0, Sql_GetData(SqlHandle, 0));

                memcpy(&Pet->look, Sql_GetData(SqlHandle, 1), 20);
                Pet->minLevel = (uint8)Sql_GetIntData(SqlHandle, 2);
                Pet->maxLevel = (uint8)Sql_GetIntData(SqlHandle, 3);
                Pet->time = Sql_GetUIntData(SqlHandle, 4);
                Pet->size = Sql_GetUIntData(SqlHandle, 5);
                Pet->EcoSystem = (ECOSYSTEM)Sql_GetIntData(SqlHandle, 6);
                Pet->m_Family = (uint16)Sql_GetIntData(SqlHandle, 7);
                Pet->mJob = (uint8)Sql_GetIntData(SqlHandle, 8);
                Pet->m_Element = (uint8)Sql_GetIntData(SqlHandle, 9);

                Pet->HPscale = Sql_GetFloatData(SqlHandle, 10);
                Pet->MPscale = Sql_GetFloatData(SqlHandle, 11);

                Pet->speed = (uint8)Sql_GetIntData(SqlHandle, 12);

                Pet->strRank = (uint8)Sql_GetIntData(SqlHandle, 13);
                Pet->dexRank = (uint8)Sql_GetIntData(SqlHandle, 14);
                Pet->vitRank = (uint8)Sql_GetIntData(SqlHandle, 15);
                Pet->agiRank = (uint8)Sql_GetIntData(SqlHandle, 16);
                Pet->intRank = (uint8)Sql_GetIntData(SqlHandle, 17);
                Pet->mndRank = (uint8)Sql_GetIntData(SqlHandle, 18);
                Pet->chrRank = (uint8)Sql_GetIntData(SqlHandle, 19);
                Pet->defRank = (uint8)Sql_GetIntData(SqlHandle, 20);
                Pet->attRank = (uint8)Sql_GetIntData(SqlHandle, 21);
                Pet->accRank = (uint8)Sql_GetIntData(SqlHandle, 22);
                Pet->evaRank = (uint8)Sql_GetIntData(SqlHandle, 23);

                Pet->hasSpellScript = (bool)Sql_GetIntData(SqlHandle, 24);

                Pet->spellList = (uint8)Sql_GetIntData(SqlHandle, 25);

                // resistances
                Pet->slashres = (uint16)(Sql_GetFloatData(SqlHandle, 26) * 1000);
                Pet->pierceres = (uint16)(Sql_GetFloatData(SqlHandle, 27) * 1000);
                Pet->hthres = (uint16)(Sql_GetFloatData(SqlHandle, 28) * 1000);
                Pet->impactres = (uint16)(Sql_GetFloatData(SqlHandle, 29) * 1000);

                Pet->firedef = 0;
                Pet->icedef = 0;
                Pet->winddef = 0;
                Pet->earthdef = 0;
                Pet->thunderdef = 0;
                Pet->waterdef = 0;
                Pet->lightdef = 0;
                Pet->darkdef = 0;

                Pet->fireres = (uint16)((Sql_GetFloatData(SqlHandle, 30) - 1) * -100);
                Pet->iceres = (uint16)((Sql_GetFloatData(SqlHandle, 31) - 1) * -100);
                Pet->windres = (uint16)((Sql_GetFloatData(SqlHandle, 32) - 1) * -100);
                Pet->earthres = (uint16)((Sql_GetFloatData(SqlHandle, 33) - 1) * -100);
                Pet->thunderres = (uint16)((Sql_GetFloatData(SqlHandle, 34) - 1) * -100);
                Pet->waterres = (uint16)((Sql_GetFloatData(SqlHandle, 35) - 1) * -100);
                Pet->lightres = (uint16)((Sql_GetFloatData(SqlHandle, 36) - 1) * -100);
                Pet->darkres = (uint16)((Sql_GetFloatData(SqlHandle, 37) - 1) * -100);

                Pet->cmbDelay = (uint16)Sql_GetIntData(SqlHandle, 38);
                Pet->name_prefix = (uint8)Sql_GetUIntData(SqlHandle, 39);
                Pet->m_MobSkillList = (uint16)Sql_GetUIntData(SqlHandle, 40);

                g_PPetList.push_back(Pet);
            }
        }
    }

    /************************************************************************
    *																		*
    *  ??????????? ?????? ?????????? ????????								*
    *																		*
    ************************************************************************/

    void FreePetList()
    {
        while (!g_PPetList.empty())
        {
            delete *g_PPetList.begin();
            g_PPetList.erase(g_PPetList.begin());
        }
    }

    void AttackTarget(CBattleEntity* PMaster, CBattleEntity* PTarget){
        DSP_DEBUG_BREAK_IF(PMaster->PPet == nullptr);

        CBattleEntity* PPet = PMaster->PPet;

        if (!PPet->StatusEffectContainer->HasPreventActionEffect())
        {
            PPet->PBattleAI->SetBattleTarget(PTarget);
            if (!(PPet->objtype == TYPE_PET && ((CPetEntity*)PPet)->m_PetID == PETID_ODIN || ((CPetEntity*)PPet)->m_PetID == PETID_BAHAMUT ||
			((CPetEntity*)PPet)->m_PetID == PETID_DARK_IXION || ((CPetEntity*)PPet)->m_PetID == PETID_LIGHT_IXION))
                PPet->PBattleAI->SetCurrentAction(ACTION_ENGAGE);
        }
    }

    void RetreatToMaster(CBattleEntity* PMaster){
        DSP_DEBUG_BREAK_IF(PMaster->PPet == nullptr);

        CBattleEntity* PPet = PMaster->PPet;

        if (!PPet->StatusEffectContainer->HasPreventActionEffect())
        {
            PPet->PBattleAI->SetCurrentAction(ACTION_DISENGAGE);
        }
    }

    uint16 GetJugWeaponDamage(CPetEntity* PPet)
    {
        float MainLevel = PPet->GetMLevel();
        return (uint16)(MainLevel * (MainLevel < 40 ? 1.4 - MainLevel / 100 : 1));
    }
    uint16 GetJugBase(CPetEntity * PMob, uint8 rank)
    {

        uint8 lvl = PMob->GetMLevel();
        if (lvl > 50){
            switch (rank){
            case 1:
                return (float)153 + (lvl - 50)*5.0;
            case 2:
                return (float)147 + (lvl - 50)*4.9;
            case 3:
                return (float)136 + (lvl - 50)*4.8;
            case 4:
                return (float)126 + (lvl - 50)*4.7;
            case 5:
                return (float)116 + (lvl - 50)*4.5;
            case 6:
                return (float)106 + (lvl - 50)*4.4;
            case 7:
                return (float)96 + (lvl - 50)*4.3;
            }
        }
        else {
            switch (rank){
            case 1:
                return (float)6 + (lvl - 1)*3.0;
            case 2:
                return (float)5 + (lvl - 1)*2.9;
            case 3:
                return (float)5 + (lvl - 1)*2.8;
            case 4:
                return (float)4 + (lvl - 1)*2.7;
            case 5:
                return (float)4 + (lvl - 1)*2.5;
            case 6:
                return (float)3 + (lvl - 1)*2.4;
            case 7:
                return (float)3 + (lvl - 1)*2.3;
            }
        }
        return 0;
    }
    uint16 GetBaseToRank(uint8 rank, uint16 lvl)
    {
        switch (rank)
        {
        case 1: return (5 + ((lvl - 1) * 50) / 100);
        case 2: return (4 + ((lvl - 1) * 45) / 100);
        case 3: return (4 + ((lvl - 1) * 40) / 100);
        case 4: return (3 + ((lvl - 1) * 35) / 100);
        case 5: return (3 + ((lvl - 1) * 30) / 100);
        case 6: return (2 + ((lvl - 1) * 25) / 100);
        case 7: return (2 + ((lvl - 1) * 20) / 100);
        }
        return 0;
    }

    void LoadJugStats(CPetEntity* PMob, Pet_t* petStats){
        //follows monster formulas but jugs have no subjob

        float growth = 1.0;
        uint8 lvl = PMob->GetMLevel();
		

        //give hp boost every 10 levels after 25
        //special boosts at 25 and 50
        if (lvl > 75){
            growth = 1.22;
        }
        else if (lvl > 65){
            growth = 1.20;
        }
        else if (lvl > 55){
            growth = 1.18;
        }
        else if (lvl > 50){
            growth = 1.16;
        }
        else if (lvl > 45){
            growth = 1.12;
        }
        else if (lvl > 35){
            growth = 1.09;
        }
        else if (lvl > 25){
            growth = 1.07;
        }

        PMob->health.maxhp = (int16)(17.0 * pow(lvl, growth) * petStats->HPscale);

        switch (PMob->GetMJob()){
        case JOB_PLD:
        case JOB_WHM:
        case JOB_BLM:
        case JOB_RDM:
        case JOB_DRK:
        case JOB_BLU:
        case JOB_SCH:
            PMob->health.maxmp = (int16)(15.2 * pow(lvl, 1.1075) * petStats->MPscale);
            break;
        }
		

        PMob->speed = petStats->speed;
        PMob->speedsub = petStats->speed;

        PMob->UpdateHealth();
        PMob->health.tp = 0;
        PMob->health.hp = PMob->GetMaxHP();
        PMob->health.mp = PMob->GetMaxMP();

        PMob->setModifier(MOD_DEF, GetJugBase(PMob, petStats->defRank));
        PMob->setModifier(MOD_EVA, GetJugBase(PMob, petStats->evaRank));
        PMob->setModifier(MOD_ATT, GetJugBase(PMob, petStats->attRank));
        PMob->setModifier(MOD_ACC, GetJugBase(PMob, petStats->accRank));

        PMob->m_Weapons[SLOT_MAIN]->setDamage(GetJugWeaponDamage(PMob));

        //reduce weapon delay of MNK
        if (PMob->GetMJob() == JOB_MNK){
            PMob->m_Weapons[SLOT_MAIN]->resetDelay();
        }

        uint16 fSTR = GetBaseToRank(petStats->strRank, PMob->GetMLevel());
        uint16 fDEX = GetBaseToRank(petStats->dexRank, PMob->GetMLevel());
        uint16 fVIT = GetBaseToRank(petStats->vitRank, PMob->GetMLevel());
        uint16 fAGI = GetBaseToRank(petStats->agiRank, PMob->GetMLevel());
        uint16 fINT = GetBaseToRank(petStats->intRank, PMob->GetMLevel());
        uint16 fMND = GetBaseToRank(petStats->mndRank, PMob->GetMLevel());
        uint16 fCHR = GetBaseToRank(petStats->chrRank, PMob->GetMLevel());

        uint16 mSTR = GetBaseToRank(grade::GetJobGrade(PMob->GetMJob(), 2), PMob->GetMLevel());
        uint16 mDEX = GetBaseToRank(grade::GetJobGrade(PMob->GetMJob(), 3), PMob->GetMLevel());
        uint16 mVIT = GetBaseToRank(grade::GetJobGrade(PMob->GetMJob(), 4), PMob->GetMLevel());
        uint16 mAGI = GetBaseToRank(grade::GetJobGrade(PMob->GetMJob(), 5), PMob->GetMLevel());
        uint16 mINT = GetBaseToRank(grade::GetJobGrade(PMob->GetMJob(), 6), PMob->GetMLevel());
        uint16 mMND = GetBaseToRank(grade::GetJobGrade(PMob->GetMJob(), 7), PMob->GetMLevel());
        uint16 mCHR = GetBaseToRank(grade::GetJobGrade(PMob->GetMJob(), 8), PMob->GetMLevel());

        PMob->stats.STR = (fSTR + mSTR) * 0.9;
        PMob->stats.DEX = (fDEX + mDEX) * 0.9;
        PMob->stats.VIT = (fVIT + mVIT) * 0.9;
        PMob->stats.AGI = (fAGI + mAGI) * 0.9;
        PMob->stats.INT = (fINT + mINT) * 0.9;
        PMob->stats.MND = (fMND + mMND) * 0.9;
        PMob->stats.CHR = (fCHR + mCHR) * 0.9;

    }
	
	
	    void LoadTrustStats(CPetEntity* PMob, Pet_t* petStats){
        //follows monster formulas but jugs have no subjob

        float growth = 1.0;
        uint8 lvl = PMob->GetMLevel();
		JOBTYPE mJob = PMob->GetMJob();
        JOBTYPE sJob = PMob->GetSJob();
		

        //give hp boost every 10 levels after 25
        //special boosts at 25 and 50
        if (lvl > 75){
            growth = 1.25;
        }
        else if (lvl > 65){
            growth = 1.28;
        }
        else if (lvl > 55){
            growth = 1.28;
        }
        else if (lvl > 50){
            growth = 1.25;
        }
        else if (lvl > 45){
            growth = 1.27;
        }
        else if (lvl > 35){
            growth = 1.29;
        }
        else if (lvl > 25){
            growth = 1.30;
        }
		else if (lvl > 4){
            growth = 1.43;
        }

        PMob->health.maxhp = (int16)(7.2 * pow(lvl, growth) * petStats->HPscale);

        switch (PMob->GetMJob()){
        case JOB_PLD:
        case JOB_WHM:
        case JOB_BLM:
        case JOB_RDM:
        case JOB_DRK:
        case JOB_BLU:
        case JOB_SCH:
            PMob->health.maxmp = (int16)(7.2 * pow(lvl, 1.1075) * petStats->MPscale);
            break;
        }
		
		if (((CPetEntity*)PMob)->m_PetID == PETID_KUPIPI){
		}
		/*if (mJob == 6){ //THF Add Traits and Weapon Damage Types
		ShowWarning(CL_GREEN"THF TRIGGERED!!! ADDING STATS\n" CL_RESET);
		PMob->setModifier(MOD_ACC, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PMob->GetMLevel())); //A+ Acc
		PMob->setModifier(MOD_EVA, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PMob->GetMLevel())); //A+ Evasion
		PMob->setModifier(MOD_ATT, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PMob->GetMLevel()));// A+ Attack
		PMob->setModifier(MOD_DEF, battleutils::GetMaxSkill(SKILL_POL, JOB_WAR, PMob->GetMLevel()));// B- Defense
		PMob->m_Weapons[SLOT_MAIN]->setDamage(floor(PMob->GetMLevel()*0.40f));// D:30 @75
		PMob->m_Weapons[SLOT_MAIN]->setDelay(floor(1000.0f*(180.0f / 60.0f))); //180 delay
		   if (lvl > 54){
		        PMob->setModifier(MOD_TRIPLE_ATTACK, 15);
				PMob->setModifier(MOD_TREASURE_HUNTER, 2);
		   }
		}
		else if (mJob == 12){ //SAM Add Traits and Weapon Damage Types
		ShowWarning(CL_GREEN"SAM TRIGGERED!!! ADDING STATS\n" CL_RESET);
		PMob->setModifier(MOD_ACC, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PMob->GetMLevel())); //A+ Acc
		PMob->setModifier(MOD_EVA, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PMob->GetMLevel())); //A+ Evasion
		PMob->setModifier(MOD_ATT, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PMob->GetMLevel()));// A+ Attack
		PMob->setModifier(MOD_DEF, battleutils::GetMaxSkill(SKILL_SYH, JOB_WAR, PMob->GetMLevel()));// B+ Defense
		PMob->m_Weapons[SLOT_MAIN]->setDamage(floor(PMob->GetMLevel()*0.93f));// D:30 @75
		PMob->m_Weapons[SLOT_MAIN]->setDelay(floor(1000.0f*(420.0f / 60.0f))); //420 delay
		   if (lvl > 49){
		        PMob->setModifier(MOD_DOUBLE_ATTACK, 15);
		   }
		}*/
		

        PMob->speed = petStats->speed;
        PMob->speedsub = petStats->speed;

        PMob->UpdateHealth();
        PMob->health.tp = 0;
        PMob->health.hp = PMob->GetMaxHP();
        PMob->health.mp = PMob->GetMaxMP();

       
        //PMob->setModifier(MOD_DEF, GetJugBase(PMob, petStats->defRank));
        //PMob->setModifier(MOD_EVA, GetJugBase(PMob, petStats->evaRank));
        //PMob->setModifier(MOD_ATT, GetJugBase(PMob, petStats->attRank));
        //PMob->setModifier(MOD_ACC, GetJugBase(PMob, petStats->accRank));

        //PMob->m_Weapons[SLOT_MAIN]->setDamage(GetJugWeaponDamage(PMob));
		
		
		 //PMob->setModifier(MOD_ACC, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PMob->GetMLevel()));
		    //Set A+ evasion
		 //PMob->setModifier(MOD_EVA, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PMob->GetMLevel()));

		//PMob->m_Weapons[SLOT_MAIN]->setDamage(floor(PMob->GetMLevel()*0.67f));

		
		

        //reduce weapon delay of MNK
        if (PMob->GetMJob() == JOB_MNK){
            PMob->m_Weapons[SLOT_MAIN]->resetDelay();
        }

        uint16 fSTR = GetBaseToRank(petStats->strRank, PMob->GetMLevel());
        uint16 fDEX = GetBaseToRank(petStats->dexRank, PMob->GetMLevel());
        uint16 fVIT = GetBaseToRank(petStats->vitRank, PMob->GetMLevel());
        uint16 fAGI = GetBaseToRank(petStats->agiRank, PMob->GetMLevel());
        uint16 fINT = GetBaseToRank(petStats->intRank, PMob->GetMLevel());
        uint16 fMND = GetBaseToRank(petStats->mndRank, PMob->GetMLevel());
        uint16 fCHR = GetBaseToRank(petStats->chrRank, PMob->GetMLevel());

        uint16 mSTR = GetBaseToRank(grade::GetJobGrade(PMob->GetMJob(), 2), PMob->GetMLevel());
        uint16 mDEX = GetBaseToRank(grade::GetJobGrade(PMob->GetMJob(), 3), PMob->GetMLevel());
        uint16 mVIT = GetBaseToRank(grade::GetJobGrade(PMob->GetMJob(), 4), PMob->GetMLevel());
        uint16 mAGI = GetBaseToRank(grade::GetJobGrade(PMob->GetMJob(), 5), PMob->GetMLevel());
        uint16 mINT = GetBaseToRank(grade::GetJobGrade(PMob->GetMJob(), 6), PMob->GetMLevel());
        uint16 mMND = GetBaseToRank(grade::GetJobGrade(PMob->GetMJob(), 7), PMob->GetMLevel());
        uint16 mCHR = GetBaseToRank(grade::GetJobGrade(PMob->GetMJob(), 8), PMob->GetMLevel());

        PMob->stats.STR = (fSTR + mSTR) * 0.9;
        PMob->stats.DEX = (fDEX + mDEX) * 0.9;
        PMob->stats.VIT = (fVIT + mVIT) * 0.9;
        PMob->stats.AGI = (fAGI + mAGI) * 0.9;
        PMob->stats.INT = (fINT + mINT) * 0.9;
        PMob->stats.MND = (fMND + mMND) * 0.9;
        PMob->stats.CHR = (fCHR + mCHR) * 0.9;
		
		

    }

    void LoadAutomatonStats(CCharEntity* PMaster, CPetEntity* PPet, Pet_t* petStats)
    {
        PPet->WorkingSkills.automaton_melee = PMaster->GetSkill(SKILL_AME);
        PPet->WorkingSkills.automaton_ranged = PMaster->GetSkill(SKILL_ARA);
        PPet->WorkingSkills.automaton_magic = PMaster->GetSkill(SKILL_AMA);

        // ?????????? ??????????, ?????? ??? ????????.
        float raceStat = 0;			// ???????? ????? HP ??? ?????? ?? ?????? ????.
        float jobStat = 0;			// ???????? ????? HP ??? ?????? ?? ?????? ????????? ?????????.
        float sJobStat = 0;			// ???????? ????? HP ??? ?????? ?? ?????? ????????? ?????????.
        int32 bonusStat = 0;			// ???????? ????? HP ??????? ??????????? ??? ?????????? ????????? ???????.
        int32 baseValueColumn = 0;	// ????? ??????? ? ??????? ??????????? HP
        int32 scaleTo60Column = 1;	// ????? ??????? ? ????????????? ?? 60 ??????
        int32 scaleOver30Column = 2;	// ????? ??????? ? ????????????? ????? 30 ??????
        int32 scaleOver60Column = 3;	// ????? ??????? ? ????????????? ????? 60 ??????
        int32 scaleOver75Column = 4;	// ????? ??????? ? ????????????? ????? 75 ??????
        int32 scaleOver60 = 2;			// ????? ??????? ? ????????????? ??? ??????? MP ????? 60 ??????
        int32 scaleOver75 = 3;			// ????? ??????? ? ????????????? ??? ??????? ?????? ????? 75-?? ??????

        uint8 grade;

        uint8 mlvl = PPet->GetMLevel();
        uint8 slvl = PPet->GetSLevel();
        JOBTYPE mjob = PPet->GetMJob();
        JOBTYPE sjob = PPet->GetSJob();
        // ?????? ???????? HP ?? main job
        int32 mainLevelOver30 = dsp_cap(mlvl - 30, 0, 30);			// ?????? ??????? +1HP ?????? ??? ????? 30 ??????
        int32 mainLevelUpTo60 = (mlvl < 60 ? mlvl - 1 : 59);			// ?????? ????? ???????? ?? 60 ?????? (???????????? ??? ?? ? ??? MP)
        int32 mainLevelOver60To75 = dsp_cap(mlvl - 60, 0, 15);		// ?????? ????? ??????? ????? 60 ??????
        int32 mainLevelOver75 = (mlvl < 75 ? 0 : mlvl - 75);			// ?????? ????? ??????? ????? 75 ??????

        //?????? ????????? ?????????? HP
        int32 mainLevelOver10 = (mlvl < 10 ? 0 : mlvl - 10);			// +2HP ?? ?????? ?????? ????? 10
        int32 mainLevelOver50andUnder60 = dsp_cap(mlvl - 50, 0, 10);	// +2HP ?? ?????? ?????? ? ?????????? ?? 50 ?? 60 ??????
        int32 mainLevelOver60 = (mlvl < 60 ? 0 : mlvl - 60);

        // ?????? raceStat jobStat bonusStat sJobStat
        // ?????? ?? ????

        grade = 4;

        raceStat = grade::GetHPScale(grade, baseValueColumn) +
            (grade::GetHPScale(grade, scaleTo60Column) * mainLevelUpTo60) +
            (grade::GetHPScale(grade, scaleOver30Column) * mainLevelOver30) +
            (grade::GetHPScale(grade, scaleOver60Column) * mainLevelOver60To75) +
            (grade::GetHPScale(grade, scaleOver75Column) * mainLevelOver75);

        // raceStat = (int32)(statScale[grade][baseValueColumn] + statScale[grade][scaleTo60Column] * (mlvl - 1));

        // ?????? ?? main job
        grade = grade::GetJobGrade(mjob, 0);

        jobStat = grade::GetHPScale(grade, baseValueColumn) +
            (grade::GetHPScale(grade, scaleTo60Column) * mainLevelUpTo60) +
            (grade::GetHPScale(grade, scaleOver30Column) * mainLevelOver30) +
            (grade::GetHPScale(grade, scaleOver60Column) * mainLevelOver60To75) +
            (grade::GetHPScale(grade, scaleOver75Column) * mainLevelOver75);

        // ?????? ???????? HP
        bonusStat = (mainLevelOver10 + mainLevelOver50andUnder60) * 2;
        PPet->health.maxhp = (int16)(raceStat + jobStat + bonusStat + sJobStat) * petStats->HPscale;
        
		
		PPet->health.hp = PPet->health.maxhp;

        //?????? ??????? MP
        raceStat = 0;
        jobStat = 0;
        sJobStat = 0;

        // ?????? MP ????.
        grade = 4;

        //???? ? main job ??? ?? ????????, ??????????? ??????? ????? ?? ?????? ?????? subjob ??????(??? ???????, ??? ? ???? ???? ?? ???????)
        if (!(grade::GetJobGrade(mjob, 1) == 0 && grade::GetJobGrade(sjob, 1) == 0))
        {
            //?????? ??????????? ???????? ??????
            raceStat = grade::GetMPScale(grade, 0) +
                grade::GetMPScale(grade, scaleTo60Column) * mainLevelUpTo60 +
                grade::GetMPScale(grade, scaleOver60) * mainLevelOver60;
        }

        //??? ??????? ?????????
        grade = grade::GetJobGrade(mjob, 1);
        if (grade > 0)
        {
            jobStat = grade::GetMPScale(grade, 0) +
                grade::GetMPScale(grade, scaleTo60Column) * mainLevelUpTo60 +
                grade::GetMPScale(grade, scaleOver60) * mainLevelOver60;
        }

        grade = grade::GetJobGrade(sjob, 1);
        if (grade > 0)
        {
            sJobStat = grade::GetMPScale(grade, 0) +
                grade::GetMPScale(grade, scaleTo60Column) * mainLevelUpTo60 +
                grade::GetMPScale(grade, scaleOver60) * mainLevelOver60;
        }

        PPet->health.maxmp = (int16)(raceStat + jobStat + sJobStat) * petStats->MPscale;
		PPet->health.mp = PPet->health.maxmp;
		

        uint16 fSTR = GetBaseToRank(petStats->strRank, PPet->GetMLevel());
        uint16 fDEX = GetBaseToRank(petStats->dexRank, PPet->GetMLevel());
        uint16 fVIT = GetBaseToRank(petStats->vitRank, PPet->GetMLevel());
        uint16 fAGI = GetBaseToRank(petStats->agiRank, PPet->GetMLevel());
        uint16 fINT = GetBaseToRank(petStats->intRank, PPet->GetMLevel());
        uint16 fMND = GetBaseToRank(petStats->mndRank, PPet->GetMLevel());
        uint16 fCHR = GetBaseToRank(petStats->chrRank, PPet->GetMLevel());

        uint16 mSTR = GetBaseToRank(grade::GetJobGrade(PPet->GetMJob(), 2), PPet->GetMLevel());
        uint16 mDEX = GetBaseToRank(grade::GetJobGrade(PPet->GetMJob(), 3), PPet->GetMLevel());
        uint16 mVIT = GetBaseToRank(grade::GetJobGrade(PPet->GetMJob(), 4), PPet->GetMLevel());
        uint16 mAGI = GetBaseToRank(grade::GetJobGrade(PPet->GetMJob(), 5), PPet->GetMLevel());
        uint16 mINT = GetBaseToRank(grade::GetJobGrade(PPet->GetMJob(), 6), PPet->GetMLevel());
        uint16 mMND = GetBaseToRank(grade::GetJobGrade(PPet->GetMJob(), 7), PPet->GetMLevel());
        uint16 mCHR = GetBaseToRank(grade::GetJobGrade(PPet->GetMJob(), 8), PPet->GetMLevel());

        uint16 sSTR = GetBaseToRank(grade::GetJobGrade(PPet->GetSJob(), 2), PPet->GetSLevel());
        uint16 sDEX = GetBaseToRank(grade::GetJobGrade(PPet->GetSJob(), 3), PPet->GetSLevel());
        uint16 sVIT = GetBaseToRank(grade::GetJobGrade(PPet->GetSJob(), 4), PPet->GetSLevel());
        uint16 sAGI = GetBaseToRank(grade::GetJobGrade(PPet->GetSJob(), 5), PPet->GetSLevel());
        uint16 sINT = GetBaseToRank(grade::GetJobGrade(PPet->GetSJob(), 6), PPet->GetSLevel());
        uint16 sMND = GetBaseToRank(grade::GetJobGrade(PPet->GetSJob(), 7), PPet->GetSLevel());
        uint16 sCHR = GetBaseToRank(grade::GetJobGrade(PPet->GetSJob(), 8), PPet->GetSLevel());

        PPet->stats.STR = fSTR + mSTR + sSTR;
        PPet->stats.DEX = fDEX + mDEX + sDEX;
        PPet->stats.VIT = fVIT + mVIT + sVIT;
        PPet->stats.AGI = fAGI + mAGI + sAGI;
        PPet->stats.INT = fINT + mINT + sINT;
        PPet->stats.MND = fMND + mMND + sMND;
        PPet->stats.CHR = fCHR + mCHR + sCHR;

        PPet->m_Weapons[SLOT_MAIN]->setSkillType(SKILL_AME);
        PPet->m_Weapons[SLOT_MAIN]->setDelay(floor(1000.0f*(petStats->cmbDelay / 60.0f))); //every pet should use this eventually
        PPet->m_Weapons[SLOT_MAIN]->setDamage((PPet->GetSkill(SKILL_AME) / 10) + 3);

        PPet->m_Weapons[SLOT_RANGED]->setSkillType(SKILL_ARA);
        PPet->m_Weapons[SLOT_RANGED]->setDamage((PPet->GetSkill(SKILL_ARA) / 9) * 2 + 3);

        CAutomatonEntity* PAutomaton = (CAutomatonEntity*)PPet;
		uint8 arki = 0;
		uint8 arkii = 0;
		uint8 arktotal = 0;
		if (PAutomaton->hasAttachment(8641))
		{
			arki = 4;
		}
		if (PAutomaton->hasAttachment(8641))
		{
			arki = 8;
		}

		arktotal = arki + arkii;

        switch (PAutomaton->getFrame())
        {
        case FRAME_HARLEQUIN:
            PPet->WorkingSkills.evasion = battleutils::GetMaxSkill(2, PPet->GetMLevel());
            PPet->setModifier(MOD_DEF, battleutils::GetMaxSkill(10, PPet->GetMLevel()));
			PPet->setModifier(MOD_HPP, arktotal);
			PPet->health.maxmp = ((int16)(raceStat + jobStat + sJobStat) * petStats->MPscale) /2;
		    PPet->health.mp = PPet->health.maxmp;
            break;
        case FRAME_VALOREDGE:
            PPet->WorkingSkills.evasion = battleutils::GetMaxSkill(5, PPet->GetMLevel());
            PPet->setModifier(MOD_DEF, battleutils::GetMaxSkill(5, PPet->GetMLevel()));
			PPet->setModifier(MOD_MP, 0);
			PPet->setModifier(MOD_HPP, arktotal);
            break;
        case FRAME_SHARPSHOT:
            PPet->WorkingSkills.evasion = battleutils::GetMaxSkill(1, PPet->GetMLevel());
            PPet->setModifier(MOD_DEF, battleutils::GetMaxSkill(11, PPet->GetMLevel()));
			PPet->setModifier(MOD_HPP, arktotal);
            break;
        case FRAME_STORMWAKER:
            PPet->WorkingSkills.evasion = battleutils::GetMaxSkill(10, PPet->GetMLevel());
            PPet->setModifier(MOD_DEF, battleutils::GetMaxSkill(12, PPet->GetMLevel()));
			PPet->setModifier(MOD_HPP, arktotal);
            break;
        }
		
    }

    void LoadAvatarStats(CPetEntity* PPet)
    {
        // ?????????? ??????????, ?????? ??? ????????.
        float raceStat = 0;			// ???????? ????? HP ??? ?????? ?? ?????? ????.
        float jobStat = 0;			// ???????? ????? HP ??? ?????? ?? ?????? ????????? ?????????.
        float sJobStat = 0;			// ???????? ????? HP ??? ?????? ?? ?????? ????????? ?????????.
        int32 bonusStat = 0;			// ???????? ????? HP ??????? ??????????? ??? ?????????? ????????? ???????.
        int32 baseValueColumn = 0;	// ????? ??????? ? ??????? ??????????? HP
        int32 scaleTo60Column = 1;	// ????? ??????? ? ????????????? ?? 60 ??????
        int32 scaleOver30Column = 2;	// ????? ??????? ? ????????????? ????? 30 ??????
        int32 scaleOver60Column = 3;	// ????? ??????? ? ????????????? ????? 60 ??????
        int32 scaleOver75Column = 4;	// ????? ??????? ? ????????????? ????? 75 ??????
        int32 scaleOver60 = 2;			// ????? ??????? ? ????????????? ??? ??????? MP ????? 60 ??????
        int32 scaleOver75 = 3;			// ????? ??????? ? ????????????? ??? ??????? ?????? ????? 75-?? ??????

        uint8 grade;

        uint8 mlvl = PPet->GetMLevel();
        JOBTYPE mjob = PPet->GetMJob();
        uint8 race = 3;					//Tarutaru

        // ?????? ???????? HP ?? main job
        int32 mainLevelOver30 = dsp_cap(mlvl - 30, 0, 30);			// ?????? ??????? +1HP ?????? ??? ????? 30 ??????
        int32 mainLevelUpTo60 = (mlvl < 60 ? mlvl - 1 : 59);			// ?????? ????? ???????? ?? 60 ?????? (???????????? ??? ?? ? ??? MP)
        int32 mainLevelOver60To75 = dsp_cap(mlvl - 60, 0, 15);		// ?????? ????? ??????? ????? 60 ??????
        int32 mainLevelOver75 = (mlvl < 75 ? 0 : mlvl - 75);			// ?????? ????? ??????? ????? 75 ??????

        //?????? ????????? ?????????? HP
        int32 mainLevelOver10 = (mlvl < 10 ? 0 : mlvl - 10);			// +2HP ?? ?????? ?????? ????? 10
        int32 mainLevelOver50andUnder60 = dsp_cap(mlvl - 50, 0, 10);	// +2HP ?? ?????? ?????? ? ?????????? ?? 50 ?? 60 ??????
        int32 mainLevelOver60 = (mlvl < 60 ? 0 : mlvl - 60);

        // ?????? raceStat jobStat bonusStat sJobStat
        // ?????? ?? ????

        grade = grade::GetRaceGrades(race, 0);

        raceStat = grade::GetHPScale(grade, baseValueColumn) +
            (grade::GetHPScale(grade, scaleTo60Column) * mainLevelUpTo60) +
            (grade::GetHPScale(grade, scaleOver30Column) * mainLevelOver30) +
            (grade::GetHPScale(grade, scaleOver60Column) * mainLevelOver60To75) +
            (grade::GetHPScale(grade, scaleOver75Column) * mainLevelOver75);

        // raceStat = (int32)(statScale[grade][baseValueColumn] + statScale[grade][scaleTo60Column] * (mlvl - 1));

        // ?????? ?? main job
        grade = grade::GetJobGrade(mjob, 0);

        jobStat = grade::GetHPScale(grade, baseValueColumn) +
            (grade::GetHPScale(grade, scaleTo60Column) * mainLevelUpTo60) +
            (grade::GetHPScale(grade, scaleOver30Column) * mainLevelOver30) +
            (grade::GetHPScale(grade, scaleOver60Column) * mainLevelOver60To75) +
            (grade::GetHPScale(grade, scaleOver75Column) * mainLevelOver75);

        // ?????? ???????? HP
        bonusStat = (mainLevelOver10 + mainLevelOver50andUnder60) * 2;
        if (PPet->m_PetID == PETID_ODIN || PPet->m_PetID == PETID_ALEXANDER || PPet->m_PetID == PETID_BAHAMUT || PPet->m_PetID == PETID_DARK_IXION ||
		PPet->m_PetID == PETID_LIGHT_IXION)
            bonusStat += 6800;
        PPet->health.maxhp = (int16)(raceStat + jobStat + bonusStat + sJobStat);
        PPet->health.hp = PPet->health.maxhp;

        //?????? ??????? MP
        raceStat = 0;
        jobStat = 0;
        sJobStat = 0;

        // ?????? MP ????.
        grade = grade::GetRaceGrades(race, 1);

        //???? ? main job ??? ?? ????????, ??????????? ??????? ????? ?? ?????? ?????? subjob ??????(??? ???????, ??? ? ???? ???? ?? ???????)
        if (grade::GetJobGrade(mjob, 1) == 0)
        {
        }
        else{
            //?????? ??????????? ???????? ??????
            raceStat = grade::GetMPScale(grade, 0) +
                grade::GetMPScale(grade, scaleTo60Column) * mainLevelUpTo60 +
                grade::GetMPScale(grade, scaleOver60) * mainLevelOver60;
        }

        //??? ??????? ?????????
        grade = grade::GetJobGrade(mjob, 1);
        if (grade > 0)
        {
            jobStat = grade::GetMPScale(grade, 0) +
                grade::GetMPScale(grade, scaleTo60Column) * mainLevelUpTo60 +
                grade::GetMPScale(grade, scaleOver60) * mainLevelOver60;
        }

        PPet->health.maxmp = (int16)(raceStat + jobStat + sJobStat); // ????????? ??????? MP
        PPet->health.mp = PPet->health.maxmp;
        //add in evasion from skill
        int16 evaskill = PPet->GetSkill(SKILL_EVA);
        int16 eva = evaskill;
        if (evaskill > 200){ //Evasion skill is 0.9 evasion post-200
            eva = 200 + (evaskill - 200)*0.9;
        }
        PPet->setModifier(MOD_EVA, eva);


        //?????? ??????? ?????????????
        uint8 counter = 0;
        for (uint8 StatIndex = 2; StatIndex <= 8; ++StatIndex)
        {
            // ?????? ?? ????
            grade = grade::GetRaceGrades(race, StatIndex);
            raceStat = grade::GetStatScale(grade, 0) + grade::GetStatScale(grade, scaleTo60Column) * mainLevelUpTo60;

            if (mainLevelOver60 > 0)
            {
                raceStat += grade::GetStatScale(grade, scaleOver60) * mainLevelOver60;
                if (mainLevelOver75 > 0)
                {
                    raceStat += grade::GetStatScale(grade, scaleOver75) * mainLevelOver75 - (mlvl >= 75 ? 0.01f : 0);
                }
            }

            // ?????? ?? ?????????
            grade = grade::GetJobGrade(mjob, StatIndex);
            jobStat = grade::GetStatScale(grade, 0) + grade::GetStatScale(grade, scaleTo60Column) * mainLevelUpTo60;

            if (mainLevelOver60 > 0)
            {
                jobStat += grade::GetStatScale(grade, scaleOver60) * mainLevelOver60;

                if (mainLevelOver75 > 0)
                {
                    jobStat += grade::GetStatScale(grade, scaleOver75) * mainLevelOver75 - (mlvl >= 75 ? 0.01f : 0);
                }
            }

            jobStat = jobStat * 1.5; //stats from subjob (assuming BLM/BLM for avatars)

            // ????? ????????
            WBUFW(&PPet->stats, counter) = (uint16)(raceStat + jobStat);
            counter += 2;
        }
    }

    /************************************************************************
    *																		*
    *																		*
    *																		*
    ************************************************************************/

    void SpawnPet(CBattleEntity* PMaster, uint32 PetID, bool spawningFromZone)
    {
        DSP_DEBUG_BREAK_IF(PMaster->PPet != nullptr);
        LoadPet(PMaster, PetID, spawningFromZone);

        CPetEntity* PPet = (CPetEntity*)PMaster->PPet;

		

        PPet->allegiance = PMaster->allegiance;
		
        PMaster->StatusEffectContainer->CopyConfrontationEffect(PPet);

        if (PetID == PETID_ALEXANDER || PetID == PETID_ODIN || PetID == PETID_BAHAMUT || PetID == PETID_DARK_IXION || PetID == PETID_LIGHT_IXION)
        {
		    if (PetID == PETID_DARK_IXION)
	        {
				apAction_t Action;
				PPet->m_ActionList.clear();

				Action.ActionTarget = PPet;
				Action.reaction = REACTION_NONE;
				Action.speceffect = SPECEFFECT_NONE;
				Action.animation = 607;
				ShowWarning(CL_GREEN"Setting to Spawn Animation 607!!! \n" CL_RESET);
				PPet->m_ActionList.push_back(Action);
				//PPet->loc.zone->PushPacket(PPet, CHAR_INRANGE, new CActionPacket(PPet));
			}

            PPet->PBattleAI = new CAIUltimateSummon(PPet);
        }
        else if (PetID >= PETID_HARLEQUINFRAME && PetID <= PETID_STORMWAKERFRAME)
        {
            PPet->PBattleAI = new CAIAutomatonDummy(PPet);
        }
        else
        {
            PPet->PBattleAI = new CAIPetDummy(PPet);
        }
        
        PPet->PBattleAI->SetLastActionTime(gettick());
      
        PPet->PBattleAI->SetCurrentAction(ACTION_SPAWN);
        

        PMaster->PPet = PPet;
        PPet->PMaster = PMaster;
       
       
        
		if (PMaster->getZone() == 60)
		{
			PMaster->PInstance->InsertPET(PPet);
			ShowWarning(CL_RED"INSTANCE INSERT PET" CL_RESET);
		}
		else
		{
			PMaster->loc.zone->InsertPET(PPet);
		}
      
         if (PMaster->objtype == TYPE_PC)
        {
            charutils::BuildingCharPetAbilityTable((CCharEntity*)PMaster, PPet, PetID);
            ((CCharEntity*)PMaster)->pushPacket(new CCharUpdatePacket((CCharEntity*)PMaster));
            ((CCharEntity*)PMaster)->pushPacket(new CPetSyncPacket((CCharEntity*)PMaster));

            // check latents affected by pets
            ((CCharEntity*)PMaster)->PLatentEffectContainer->CheckLatentsPetType(PetID);
                PMaster->ForParty([](CBattleEntity* PMember) {
                ((CCharEntity*)PMember)->PLatentEffectContainer->CheckLatentsPartyAvatar();
            });
        }
        // apply stats from previous zone if this pet is being transfered
        if (spawningFromZone == true)
        {
            PPet->health.tp = ((CCharEntity*)PMaster)->petZoningInfo.petTP;
            PPet->health.hp = ((CCharEntity*)PMaster)->petZoningInfo.petHP;
        }



    }
	
    void SpawnAlly(CBattleEntity* PMaster, uint32 PetID, bool spawningFromZone)
    {
        //Check to see if in full party
		//ShowWarning(CL_GREEN"SPAWN ALLY \n" CL_RESET);
		CCharEntity* PChar = (CCharEntity*)PMaster;
        uint16 partySize = PMaster->PAlly.size();
		int32 trustsize = charutils::GetVar(PChar, "Trustsize");
		
		//Load All Trust Tribute Variables
	    int32 attCur = charutils::GetVar(PChar, "TrustAtt_Cur");
	    int32 accCur = charutils::GetVar(PChar, "TrustAcc_Cur");
	    int32 defCur = charutils::GetVar(PChar, "TrustDEF_Cur");
	    int32 enmCur = charutils::GetVar(PChar, "TrustEnm_Cur");		
	    int32 trait1Cur = charutils::GetVar(PChar, "TrustJA_Cur");

	    int32 attExcen = charutils::GetVar(PChar, "TrustAtt_Excen");
	    int32 accExcen = charutils::GetVar(PChar, "TrustAcc_Excen");
	    int32 jumpExcen = charutils::GetVar(PChar, "TrustJump_Excen");  //-- SCRIPT
	    int32 enmExcen = charutils::GetVar(PChar, "TrustEnm_Excen");  //-- Enmity		
	    int32 trait1Excen = charutils::GetVar(PChar, "TrustJA_Excen"); //-- CORE

	    int32 attAya = charutils::GetVar(PChar, "TrustAtt_Ayame");
	    int32 accAya = charutils::GetVar(PChar, "TrustAcc_Ayame");
	    int32 stpAya = charutils::GetVar(PChar, "TrustSTP_Ayame");
	    int32 zanAya = charutils::GetVar(PChar, "TrustZan_Ayame");		
	    int32 trait1Aya = charutils::GetVar(PChar, "TrustJA_Ayame");     // Script Meditate + 20

	    int32 attNanaa = charutils::GetVar(PChar, "TrustAtt_Nanaa");
	    int32 accNanaa = charutils::GetVar(PChar, "TrustAcc_Nanaa");
	    int32 dexNanaa = charutils::GetVar(PChar, "TrustDEX_Nanaa");
	    int32 taNanaa = charutils::GetVar(PChar, "TrustTA_Nanaa");	    // Triple Attack
	    int32 trait1Nanaa = charutils::GetVar(PChar, "TrustTH_Nanaa");	// Treasure Hunter	
		
	    int32 attKup = charutils::GetVar(PChar, "TrustAtt_Kup");
	    int32 accKup = charutils::GetVar(PChar, "TrustAcc_Kup");
	    int32 curepotKup = charutils::GetVar(PChar, "TrustCure_Kup");  // Cure Potency
	    int32 curecastKup = charutils::GetVar(PChar, "TrustCast_Kup"); // Cure Spellcasting Time
	    int32 proKup = charutils::GetVar(PChar, "TrustPro_Kup");       // Core Pro V
	    int32 shellKup = charutils::GetVar(PChar, "TrustShell_Kup");   // Core Shell V		
		
		int32 attZeid = charutils::GetVar(PChar, "TrustAtt_Zeid");
		int32 accZeid = charutils::GetVar(PChar, "TrustAcc_Zeid");
		int32 occZeid = charutils::GetVar(PChar, "TrustOA_Zeid");
		int32 desZeid = charutils::GetVar(PChar, "TrustDB_Zeid");
		
		int32 attLion = charutils::GetVar(PChar, "TrustAtt_Lion");
		int32 accLion = charutils::GetVar(PChar, "TrustAcc_Lion");
		int32 agiLion = charutils::GetVar(PChar, "TrustAGI_Lion");
		int32 taLion = charutils::GetVar(PChar, "TrustTA_Lion");       // Triple Attack	
		int32 thLion = charutils::GetVar(PChar, "TrustTH_Lion");       // Treasure Hunter II
		
		int32 mattAdel = charutils::GetVar(PChar, "TrustMatt_Adel");
		int32 maccAdel = charutils::GetVar(PChar, "TrustMacc_Adel");
		int32 mpAdel = charutils::GetVar(PChar, "TrustMP_Adel");
		int32 subAdel = charutils::GetVar(PChar, "TrustSub_Adel");     // Script Sublimation Adel
		
		int32 attDarc = charutils::GetVar(PChar, "TrustAtt_Darc");
		int32 accDarc = charutils::GetVar(PChar, "TrustAcc_Darc");
		int32 mpDarc = charutils::GetVar(PChar, "TrustMP_Darc");
		int32 maccDarc = charutils::GetVar(PChar, "TrustMA_Darc");		
		int32 caDarc = charutils::GetVar(PChar, "TrustCA_Darc");       // Script Chain Affinity	

	    int32 attNaji = charutils::GetVar(PChar, "TrustAtt_Naji");
	    int32 accNaji = charutils::GetVar(PChar, "TrustAcc_Naji");
	    int32 daNaji = charutils::GetVar(PChar, "TrustDA_Naji");
	    int32 berserkNaji = charutils::GetVar(PChar, "TrustBerserk_Naji");	// Script Berserk		
	
		
		
		
        if (PMaster->PParty != nullptr)
        {
            for (uint8 i = 0; i < PMaster->PParty->members.size(); i++)
            {
                CBattleEntity* PPartyMember = PMaster->PParty->members[i];
                partySize = partySize + 1 + PPartyMember->PAlly.size();             
            }
        }
		else
        {
			partySize += 1;
        } 
        
		//Can't summon more than 5 Trusts
        if (partySize > 9){
            return;
        }
		
        if (PMaster->PAlly.size() > 3 && trustsize == 0)
        {
		    ShowWarning(CL_RED"Can't summon more than 3 Trusts \n" CL_RESET);
            PMaster->PAlly[2]->PBattleAI->SetCurrentAction(ACTION_FALL);
            PMaster->PAlly.pop_back();
        }		
        
        else if (PMaster->PAlly.size() > 4 && trustsize == 1)
        {
		    ShowWarning(CL_RED"Maximum Trusts Spawned \n" CL_RESET);
            PMaster->PAlly[3]->PBattleAI->SetCurrentAction(ACTION_FALL);
            PMaster->PAlly.pop_back();
        }
        if (PMaster->PParty == nullptr)
		{
            PMaster->PParty = new CParty(PMaster);
		}
        
        CPetEntity* PAlly = LoadAlly(PMaster, PetID, spawningFromZone);
        PAlly->allegiance = PMaster->allegiance;
        PMaster->StatusEffectContainer->CopyConfrontationEffect(PAlly);
		uint8 plvl = PAlly->GetMLevel();
		
		if (PetID == PETID_NANAA_MIHGO)
		{
		uint16 nmatt = (PAlly->GetMLevel() * 1.0) + attNanaa;
		uint16 nmacc = (PAlly->GetMLevel() * 0.5) + accNanaa;
		//ShowWarning(CL_GREEN"NANAA MIGHO TRIGGERED SPAWN ALLY!!! \n" CL_RESET);
		PAlly->setModifier(MOD_ACC, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel()) + nmacc); //A+ Acc
		PAlly->setModifier(MOD_EVA, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel())); //A+ Evasion
		PAlly->setModifier(MOD_ATT, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel()) + nmatt);// A+ Attack
		PAlly->setModifier(MOD_DEF, battleutils::GetMaxSkill(SKILL_POL, JOB_WAR, PAlly->GetMLevel()));// B- Defense
		PAlly->m_Weapons[SLOT_MAIN]->setDamage(floor(PAlly->GetMLevel()*0.46f) + 1);// D:35 @75
		PAlly->m_Weapons[SLOT_MAIN]->setDelay(floor(1000.0f*(200.0f / 60.0f))); //2000 delay
		PAlly->setModifier(MOD_ENMITY, -15);		
		PAlly->setModifier(MOD_CONVMPTOHP, 1);
		PAlly->setModifier(MOD_CONVHPTOMP, 1);
		PAlly->setModifier(MOD_FOOD_MPP, 1);
		PAlly->setModifier(MOD_FOOD_MP_CAP, 1);
		PAlly->setModifier(MOD_MPP, 1);
		PAlly->setModifier(MOD_HPP, 1);
		PAlly->setModifier(MOD_MOVE, 10);
		PAlly->health.maxhp = (int16)(14 + (3.75f*(plvl * 3.75f)));
		PAlly->UpdateHealth();
		   if (plvl > 74) {
		        PAlly->setModifier(MOD_DEX, dexNanaa);
				PAlly->setModifier(MOD_TREASURE_HUNTER, 1 + trait1Nanaa);
				PAlly->setModifier(MOD_TRIPLE_ATTACK, 5 + taNanaa);
		    }
		   else if (plvl > 54){
		        PAlly->setModifier(MOD_TRIPLE_ATTACK, 5);
				PAlly->setModifier(MOD_TREASURE_HUNTER, 1);
		    }
		}
	    else if (PetID == PETID_KUPIPI)
		{
		uint16 kupatt = (PAlly->GetMLevel() * 1.0) + attKup;
		uint16 kupacc = (PAlly->GetMLevel() * 0.5) + accKup; 
		
		PAlly->setModifier(MOD_ACC, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel()) + kupacc); //A+ Acc
		PAlly->setModifier(MOD_EVA, battleutils::GetMaxSkill(SKILL_POL, JOB_WAR, PAlly->GetMLevel())); //B- Evasion
		PAlly->setModifier(MOD_ATT, battleutils::GetMaxSkill(SKILL_POL, JOB_WAR, PAlly->GetMLevel()) + kupatt);// B- Attack
		PAlly->setModifier(MOD_DEF, battleutils::GetMaxSkill(SKILL_POL, JOB_WAR, PAlly->GetMLevel()));// B- Defense
		PAlly->setModifier(MOD_HEALING, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel()));// A+ Healing
		PAlly->setModifier(MOD_DIVINE, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel()));// A+ Divine Magic
		PAlly->m_Weapons[SLOT_MAIN]->setDamage(floor(PAlly->GetMLevel()*0.52f) + 3);// D:42 @75
		PAlly->m_Weapons[SLOT_MAIN]->setDelay(floor(1000.0f*(320.0f / 60.0f))); //320 delay
		PAlly->setModifier(MOD_ENMITY, -15);		
		PAlly->setModifier(MOD_CONVMPTOHP, 1);
		PAlly->setModifier(MOD_CONVHPTOMP, 1);
		PAlly->setModifier(MOD_FOOD_MPP, 1);
		PAlly->setModifier(MOD_FOOD_MP_CAP, 1);
		PAlly->setModifier(MOD_MPP, 1);
		PAlly->setModifier(MOD_HPP, 1);	
		PAlly->setModifier(MOD_MOVE, 10);
		PAlly->health.maxhp = (int16)(14 + (3.0f*(plvl * 3.75f))); 		
		PAlly->health.maxmp = (int16)(22 + (3.66f*(plvl * 3.66f))); 
		PAlly->UpdateHealth();
        PAlly->health.mp = PAlly->health.maxmp;
		   if (plvl > 74){
		   PAlly->setModifier(MOD_REFRESH, 3);
		   PAlly->setModifier(MOD_CURE_POTENCY, curepotKup);
		   PAlly->setModifier(MOD_CURE_CAST_TIME, curecastKup);
		   }
		   else if (plvl > 24){
		   PAlly->setModifier(MOD_REFRESH, 1);
		   }
		   
		}
		else if (PetID == PETID_NAJI)
		{
		uint16 moddatt = (PAlly->GetMLevel() * 1.0) + attNaji;
		uint16 moddacc = (PAlly->GetMLevel() * 0.5) + accNaji;
		//ShowWarning(CL_GREEN"NAJI TRIGGERED SPAWN ALLY!!! \n" CL_RESET);
		PAlly->setModifier(MOD_ACC, battleutils::GetMaxSkill(SKILL_SYH, JOB_WAR, PAlly->GetMLevel()) + moddacc); //B+ Acc
		PAlly->setModifier(MOD_EVA, battleutils::GetMaxSkill(SKILL_SYH, JOB_WAR, PAlly->GetMLevel())); //B+ Evasion
		PAlly->setModifier(MOD_ATT, battleutils::GetMaxSkill(SKILL_SYH, JOB_WAR, PAlly->GetMLevel()) + moddatt);// B+ Attack
		PAlly->setModifier(MOD_DEF, battleutils::GetMaxSkill(SKILL_POL, JOB_WAR, PAlly->GetMLevel()));// B- Defense
		PAlly->m_Weapons[SLOT_MAIN]->setDamage(floor(PAlly->GetMLevel()*0.56f));// D:42 @75
		PAlly->m_Weapons[SLOT_MAIN]->setDelay(floor(1000.0f*(240.0f / 60.0f))); //240 delay
		PAlly->setModifier(MOD_CONVMPTOHP, 1);
		PAlly->setModifier(MOD_CONVHPTOMP, 1);	
		PAlly->setModifier(MOD_FOOD_MPP, 1);
		PAlly->setModifier(MOD_FOOD_MP_CAP, 1);	
		PAlly->setModifier(MOD_ENMITY, -15);		
		PAlly->setModifier(MOD_MPP, 1);
		PAlly->setModifier(MOD_HPP, 1);
		PAlly->setModifier(MOD_MOVE, 10);
		PAlly->health.maxhp = (int16)(14 + (3.30f*(plvl * 4.15f))); 		
		PAlly->UpdateHealth();
		   if (plvl > 74){
				PAlly->setModifier(MOD_DOUBLE_ATTACK, 15 + daNaji);
		   }		
		   else if (plvl > 24){
				PAlly->setModifier(MOD_DOUBLE_ATTACK, 15);
		   }
		}
		else if (PetID == PETID_AYAME)
		{
		uint16 haste = 0;
		uint16 modatt = (PAlly->GetMLevel() * 0.3) + attAya;
		uint16 modacc = (PAlly->GetMLevel() * 0.66) + accAya;
		uint16 maxhaste = PAlly->GetMLevel();
		haste = (floor(maxhaste * 2.7));
		if (haste > 200){
		haste = 200;
		}
		uint16 bstr = (PAlly->GetMLevel() * 0.5);
		PAlly->setModifier(MOD_STR, bstr); //added str for WS		
		//ShowWarning(CL_GREEN"AYAME TRIGGERED SPAWN ALLY!!! \n" CL_RESET);
		PAlly->setModifier(MOD_ACC, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel()) + modacc); //A+ Acc
		PAlly->setModifier(MOD_EVA, battleutils::GetMaxSkill(SKILL_SYH, JOB_WAR, PAlly->GetMLevel())); //B+ Evasion
		PAlly->setModifier(MOD_ATT, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel()) + modatt);// A+ Attack
		PAlly->setModifier(MOD_DEF, battleutils::GetMaxSkill(SKILL_SYH, JOB_WAR, PAlly->GetMLevel()));// B+ Defense
		PAlly->m_Weapons[SLOT_MAIN]->setDamage(floor(PAlly->GetMLevel()*0.83f) + 12);// D:12 @1 / D:80 @ 75
		PAlly->m_Weapons[SLOT_MAIN]->setDelay(floor(1000.0f*(450.0f / 60.0f))); //420 delay
		PAlly->setModifier(MOD_HASTE_ABILITY, 102); //Constant Hasso
		PAlly->setModifier(MOD_ENMITY, -15);		
		PAlly->setModifier(MOD_HASTE_GEAR, haste);
		PAlly->setModifier(MOD_CONVMPTOHP, 1);
		PAlly->setModifier(MOD_CONVHPTOMP, 1);
		PAlly->setModifier(MOD_FOOD_MPP, 1);
		PAlly->setModifier(MOD_FOOD_MP_CAP, 1);	
		PAlly->setModifier(MOD_MPP, 1);
		PAlly->setModifier(MOD_HPP, 1);	
		PAlly->setModifier(MOD_MOVE, 10);
		PAlly->health.maxhp = (int16)(14 + (3.8f*(plvl * 4.0f)));
		PAlly->UpdateHealth();
        PAlly->health.hp = PAlly->health.maxhp;	
		   if (plvl > 74){
		        PAlly->setModifier(MOD_STORETP, 25 + stpAya);
				PAlly->setModifier(MOD_DOUBLE_ATTACK, 15);
				PAlly->setModifier(MOD_ZANSHIN, 15);
				PAlly->setModifier(MOD_TP_BONUS, (floor(PAlly->GetMLevel() + 26)));
		   }
		   else if (plvl > 49){
		        PAlly->setModifier(MOD_STORETP, 20);
				PAlly->setModifier(MOD_DOUBLE_ATTACK, 15);
				PAlly->setModifier(MOD_ZANSHIN, 10);
				PAlly->setModifier(MOD_TP_BONUS, (floor((PAlly->GetMLevel() / 2) + 1)));
		   }		   
		   else if (plvl > 29){
		        PAlly->setModifier(MOD_STORETP, 15);
				PAlly->setModifier(MOD_ZANSHIN, 25);
		   }
		   else if (plvl > 9){
		        PAlly->setModifier(MOD_STORETP, 10);
		   }
		}
		else if (PetID == PETID_CURILLA)
		{
		uint16 defrate = (floor(PAlly->GetMLevel() * 1.5)) + defCur;
		uint16 modstat = (floor(PAlly->GetMLevel() * 1.0)) + attCur;
		uint16 hpstat = (floor(PAlly->GetMLevel() * 3.2));
		uint16 accstat = (floor(PAlly->GetMLevel() * 0.5)) + accCur;
		uint16 shielddef = (floor(10 + (PAlly->GetMLevel() / 8)));
		//ShowWarning(CL_GREEN"CURILLA TRIGGERED SPAWN ALLY!!! \n" CL_RESET);
		PAlly->SetMJob(JOB_PLD);
		PAlly->setModifier(MOD_ACC, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel()) + accstat); //A+ Acc
		PAlly->setModifier(MOD_EVA, battleutils::GetMaxSkill(SKILL_SYH, JOB_WAR, PAlly->GetMLevel())); //B+ Evasion
		PAlly->setModifier(MOD_ATT, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel()) + modstat);// B+ Attack
		PAlly->setModifier(MOD_DEF, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel()) + defrate);// A+ Defense
		PAlly->setModifier(MOD_HEALING, battleutils::GetMaxSkill(SKILL_SYH, JOB_WAR, PAlly->GetMLevel()));// B+ Healing
		PAlly->setModifier(MOD_DIVINE, battleutils::GetMaxSkill(SKILL_SYH, JOB_WAR, PAlly->GetMLevel()));// A+ Divine Magic
		PAlly->setModifier(MOD_SHIELD, battleutils::GetMaxSkill(SKILL_SYH, JOB_WAR, PAlly->GetMLevel()));// A+ Shield Skill	
		PAlly->setModifier(MOD_ENMITY, 45);
		PAlly->setModifier(MOD_ENMITY_LOSS_REDUCTION, 20);
		PAlly->m_Weapons[SLOT_MAIN]->setDamage(floor(PAlly->GetMLevel()*0.48f) + 6);// D:42 @75
		PAlly->m_Weapons[SLOT_MAIN]->setDelay(floor(1000.0f*(260.0f / 60.0f))); //260 delay
		PAlly->m_Weapons[SLOT_SUB]->setShieldSize(3);
		uint8 curillashieldsize = PAlly->m_Weapons[SLOT_SUB]->getShieldSize();
		//PAlly->m_Weapons[SLOT_SUB]->IsShield();
		PAlly->setModifier(MOD_CONVMPTOHP, 1);
		PAlly->setModifier(MOD_CONVHPTOMP, 1);
		PAlly->setModifier(MOD_FOOD_MPP, 1);
		PAlly->setModifier(MOD_FOOD_MP_CAP, 1);	
		PAlly->setModifier(MOD_MPP, 1);
		PAlly->setModifier(MOD_HPP, 1);	
        PAlly->setModifier(MOD_MOVE, 10);		
		PAlly->m_Weapons[SLOT_SUB]->addModifier(new CModifier(MOD_DEF, shielddef));
		PAlly->health.maxhp = (int16)(15 + hpstat + (3.66f*(plvl * 4.10f)));
		PAlly->health.maxmp = (int16)(15 + (2.72f*(plvl * 2.72f))); 
		PAlly->UpdateHealth();
        PAlly->health.mp = PAlly->health.maxmp;
		   if (plvl > 74){
		        PAlly->setModifier(MOD_SHIELD_MASTERY_TP, trait1Cur);
		   }
		   if (plvl > 49){
				PAlly->setModifier(MOD_DOUBLE_ATTACK, 10);

		   }
		   if (plvl > 29){
				PAlly->setModifier(MOD_REFRESH, 1);
		   }
		}
		else if (PetID == PETID_EXCENMILLE)
		{
		uint16 haste = 0;
		uint16 exeatt = (PAlly->GetMLevel() * 1.0) + attExcen;
		uint16 exeacc = (PAlly->GetMLevel() * 0.5) + accExcen;
		uint16 maxhaste = PAlly->GetMLevel();
		haste = (floor(maxhaste * 2.7));
		if (haste > 200){
		haste = 200;
		}
		//ShowWarning(CL_GREEN"EXCENMILLE TRIGGERED SPAWN ALLY!!! \n" CL_RESET);
		PAlly->setModifier(MOD_ACC, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel()) + exeatt); //A+ Acc
		PAlly->setModifier(MOD_EVA, battleutils::GetMaxSkill(SKILL_SYH, JOB_WAR, PAlly->GetMLevel())); //B+ Evasion
		PAlly->setModifier(MOD_ATT, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel()) + exeacc);// A+ Attack
		PAlly->setModifier(MOD_DEF, battleutils::GetMaxSkill(SKILL_SYH, JOB_WAR, PAlly->GetMLevel()));// B+ Defense
		PAlly->m_Weapons[SLOT_MAIN]->setDamage(floor(PAlly->GetMLevel()*0.96f) + 15);// D:15 @5 / D:87 @ 75
		PAlly->m_Weapons[SLOT_MAIN]->setDelay(floor(1000.0f*(492.0f / 60.0f))); //492 delay
		PAlly->setModifier(MOD_HASTE_GEAR, haste);
		PAlly->setModifier(MOD_ENMITY, -15);		
		PAlly->setModifier(MOD_CONVMPTOHP, 1);
		PAlly->setModifier(MOD_CONVHPTOMP, 1);
		PAlly->setModifier(MOD_FOOD_MPP, 1);
		PAlly->setModifier(MOD_FOOD_MP_CAP, 1);	
		PAlly->setModifier(MOD_MPP, 1);
		PAlly->setModifier(MOD_HPP, 1);	
        PAlly->setModifier(MOD_MOVE, 10);		
		PAlly->health.maxhp = (int16)(11 + (3.5*(plvl * 4.5f))); 
		PAlly->UpdateHealth();
        PAlly->health.hp = PAlly->health.maxhp;
		    if (plvl > 74)
			{
			  PAlly->setModifier(MOD_DOUBLE_ATTACK, 10);
			  PAlly->setModifier(MOD_HASTE_ABILITY, 52); //Constant Subjob Hasso
			  PAlly->setModifier(MOD_STORETP, 15); // Small Store TP Bonus			
			  PAlly->setModifier(MOD_ENMITY, -enmExcen);
			}
			
			else if (plvl > 49)
		    {
			  PAlly->setModifier(MOD_DOUBLE_ATTACK, 10);
			  PAlly->setModifier(MOD_HASTE_ABILITY, 52); //Constant Subjob Hasso
			  PAlly->setModifier(MOD_STORETP, 15); // Small Store TP Bonus
		    }
		}
		else if (PetID == PETID_BLUE)
		{
		uint16 bmoddatt = (PAlly->GetMLevel() * 1.0) + attDarc;
		uint16 bmoddacc = (PAlly->GetMLevel() * 0.5) + accDarc;
		uint16 bstr = (PAlly->GetMLevel() * 0.5);
		uint16 bdex = (PAlly->GetMLevel() * 0.4);
		PAlly->setModifier(MOD_STR, bstr); //added str for spells
		PAlly->setModifier(MOD_DEX, bdex); //added dex for spells
		PAlly->setModifier(MOD_ACC, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel()) + bmoddacc); //A+ Acc
		PAlly->setModifier(MOD_EVA, battleutils::GetMaxSkill(SKILL_SYH, JOB_WAR, PAlly->GetMLevel())); //B+ Evasion
		PAlly->setModifier(MOD_ATT, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel()) + bmoddatt);// A+ Attack
		PAlly->setModifier(MOD_DEF, battleutils::GetMaxSkill(SKILL_POL, JOB_WAR, PAlly->GetMLevel()));// B- Defense
		PAlly->setModifier(MOD_BLUE, battleutils::GetMaxSkill(SKILL_SYH, JOB_WAR, PAlly->GetMLevel()));// B+ Blue Magic
		PAlly->setModifier(MOD_DUAL_WIELD, 20); // Dual Wield
	    PAlly->setModifier(MOD_CONVMPTOHP, 1);
		PAlly->setModifier(MOD_CONVHPTOMP, 1);
		PAlly->setModifier(MOD_FOOD_MPP, 1);
		PAlly->setModifier(MOD_FOOD_MP_CAP, 1);	
		PAlly->setModifier(MOD_MPP, 1);
		PAlly->setModifier(MOD_HPP, 1);	
		PAlly->setModifier(MOD_MOVE, 10);
		PAlly->setModifier(MOD_ENMITY, -15);		
		PAlly->m_Weapons[SLOT_MAIN]->setDamage(floor(PAlly->GetMLevel()*0.54f));// D:40 @75
		PAlly->m_Weapons[SLOT_MAIN]->setDelay(floor(1000.0f*(240.0f / 60.0f))); //240 delay
        PAlly->health.maxmp = (int16)(15 + (3.66f*(plvl * 2.95f))); 
		PAlly->health.maxhp = (int16)(22 + (3.85f*(plvl * 3.66f))); 
		PAlly->UpdateHealth();
        PAlly->health.mp = PAlly->health.maxmp;
        PAlly->health.mp = PAlly->health.maxhp;		
		   if (plvl > 74){
				PAlly->setModifier(MOD_DOUBLE_ATTACK, 3);
				PAlly->setModifier(MOD_REFRESH, 1);
				PAlly->setModifier(MOD_MACC, maccDarc);
			    PAlly->setModifier(MOD_MP, mpDarc);				
		   }
		   else if (plvl > 60){
				PAlly->setModifier(MOD_REFRESH, 1);
		   }
		}
		else if (PetID == PETID_ADELHIED)
		{
		uint16 moddatt = (PAlly->GetMLevel() * 1.0);
		uint16 moddacc = (PAlly->GetMLevel() * 0.5);
		uint16 modmab = (PAlly->GetMLevel() / 2);
		uint16 modmacc = (PAlly->GetMLevel());
		PAlly->setModifier(MOD_ACC, battleutils::GetMaxSkill(SKILL_POL, JOB_WAR, PAlly->GetMLevel()) + moddacc); //B+ Acc
		PAlly->setModifier(MOD_EVA, battleutils::GetMaxSkill(SKILL_POL, JOB_WAR, PAlly->GetMLevel())); //B+ Evasion
		PAlly->setModifier(MOD_ATT, battleutils::GetMaxSkill(SKILL_POL, JOB_WAR, PAlly->GetMLevel()) + moddatt);// B+ Attack
		PAlly->setModifier(MOD_DEF, battleutils::GetMaxSkill(SKILL_POL, JOB_WAR, PAlly->GetMLevel()));// B- Defense
		PAlly->setModifier(MOD_HEALING, battleutils::GetMaxSkill(SKILL_H2H, JOB_WAR, PAlly->GetMLevel()));// D Healing
		PAlly->setModifier(MOD_DIVINE, battleutils::GetMaxSkill(SKILL_H2H, JOB_WAR, PAlly->GetMLevel()));// D Divine
		PAlly->setModifier(MOD_ELEM, battleutils::GetMaxSkill(SKILL_H2H, JOB_WAR, PAlly->GetMLevel()));// D Elemental
		PAlly->setModifier(MOD_ENFEEBLE, battleutils::GetMaxSkill(SKILL_H2H, JOB_WAR, PAlly->GetMLevel()));// D Enfeebling		
		// PAlly->setModifier(MOD_MATT, modmab); //Magic Attack Bonus	
		PAlly->setModifier(MOD_MACC, modmacc); //Magic Accuracy Bonus
		PAlly->m_Weapons[SLOT_MAIN]->setDamage(floor(PAlly->GetMLevel()*0.49f));// D:37 @75
		PAlly->m_Weapons[SLOT_MAIN]->setDelay(floor(1000.0f*(340.0f / 60.0f))); //340 delay	
		PAlly->setModifier(MOD_CONVMPTOHP, 1);
		PAlly->setModifier(MOD_CONVHPTOMP, 1);
		PAlly->setModifier(MOD_FOOD_MPP, 1);
		PAlly->setModifier(MOD_FOOD_MP_CAP, 1);
		PAlly->setModifier(MOD_ENMITY, -15);		
		PAlly->setModifier(MOD_MPP, 1);
		PAlly->setModifier(MOD_HPP, 1);
		PAlly->setModifier(MOD_MOVE, 10);
		PAlly->health.maxmp = (int16)(22 + (3.46f*(plvl * 3.46f))); 
		PAlly->health.maxhp = (int16)(24 + (3.70f*(plvl * 3.70f))); 		
		PAlly->UpdateHealth();
        PAlly->health.mp = PAlly->health.maxmp;
		if (plvl > 74){
			PAlly->setModifier(MOD_MATT, 28 + mattAdel);
			PAlly->setModifier(MOD_MACC, maccAdel);			
			PAlly->setModifier(MOD_SUBLIMATION_BONUS, 7);
			PAlly->setModifier(MOD_REFRESH, 3);
			PAlly->setModifier(MOD_FASTCAST, 20);
			PAlly->setModifier(MOD_MP, mpAdel); 
		}
        else if (plvl > 50){
			PAlly->setModifier(MOD_MATT, 24);
			PAlly->setModifier(MOD_SUBLIMATION_BONUS, 5);
			PAlly->setModifier(MOD_FASTCAST, 10);
		}
		else if (plvl > 25){
			PAlly->setModifier(MOD_MATT, 15);
			PAlly->setModifier(MOD_REFRESH, 1);
		}
		else {
			PAlly->setModifier(MOD_MATT, 10);
		}	
        }
		
		else if (PetID == PETID_LION)
		{
	    uint16 haste = 0;
		uint16 bmoddatt = (PAlly->GetMLevel() * 1.0);
		uint16 bmoddacc = (PAlly->GetMLevel() * 0.5);		
		uint16 bstr = (PAlly->GetMLevel() * 0.5);
		uint16 bdex = (PAlly->GetMLevel() * 0.4);
		uint16 bagi = (PAlly->GetMLevel() * 0.5);
		
		
		uint16 maxhaste = PAlly->GetMLevel();
		haste = (floor(maxhaste * 2.7));
		if (haste > 200){
		haste = 200;
		}		
		PAlly->setModifier(MOD_STR, bstr); //added str 
		PAlly->setModifier(MOD_DEX, bdex); //added dex	
		if (plvl >= 75) //Addin Trust Tributes
		{
		    PAlly->setModifier(MOD_AGI, bagi + agiLion); //added agi + trust tribute
			PAlly->setModifier(MOD_ATT, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel()) + bmoddatt + attLion);// A+ Attack	
			PAlly->setModifier(MOD_ACC, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel()) + bmoddacc + accLion); //A+ Acc			
		}
		else
		{
		    PAlly->setModifier(MOD_AGI, bagi); //added agi	
			PAlly->setModifier(MOD_ATT, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel()) + bmoddatt);// A+ Attack	
			PAlly->setModifier(MOD_ACC, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel()) + bmoddacc); //A+ Acc
		}
		PAlly->setModifier(MOD_EVA, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel())); //A+ Evasion	
		PAlly->setModifier(MOD_DEF, battleutils::GetMaxSkill(SKILL_POL, JOB_WAR, PAlly->GetMLevel()));// B- Defense
	    PAlly->setModifier(MOD_CONVMPTOHP, 1);
		PAlly->setModifier(MOD_CONVHPTOMP, 1);
		PAlly->setModifier(MOD_FOOD_MPP, 1);
		PAlly->setModifier(MOD_FOOD_MP_CAP, 1);	
		PAlly->setModifier(MOD_MPP, 1);
		PAlly->setModifier(MOD_HPP, 1);	
        PAlly->setModifier(MOD_MOVE, 10);
		PAlly->setModifier(MOD_HASTE_GEAR, haste);
		PAlly->setModifier(MOD_ENMITY, -15);
        PAlly->setModifier(MOD_GRAVITYRES, 10);		
		PAlly->m_Weapons[SLOT_MAIN]->setDamage(floor(PAlly->GetMLevel()*0.43f));// D:32 @75
		PAlly->m_Weapons[SLOT_MAIN]->setDelay(floor(1000.0f*(250.0f / 60.0f))); //180 delay			
		PAlly->health.maxhp = (int16)(24 + (3.83f*(plvl * 3.63f))); 
		PAlly->UpdateHealth();
        PAlly->health.mp = PAlly->health.maxhp;		
		   	if (plvl > 74){
		   		PAlly->setModifier(MOD_TRIPLE_ATTACK, 5); // Double Attack	
       		   	PAlly->setModifier(MOD_TREASURE_HUNTER, 1 + thLion); // Treasure Hunter 1					
		   } 
		   else if (plvl > 54){
		   		PAlly->setModifier(MOD_TRIPLE_ATTACK, 3); // Double Attack	
       		   	PAlly->setModifier(MOD_TREASURE_HUNTER, 1); // Treasure Hunter 1	
 				
		   }		   
		   else if (plvl >= 25){
				PAlly->setModifier(MOD_TREASURE_HUNTER, 1); // Treasure Hunter 1
		   }
		   else if (plvl >= 15){
		   		PAlly->setModifier(MOD_TREASURE_HUNTER, 1); // Treasure Hunter 1
		   }		   
		}

		else if (PetID == PETID_PRISHE)
		{
	    uint16 haste = 0;
		uint16 bmoddatt = (PAlly->GetMLevel() * 1.0);
		uint16 bmoddacc = (PAlly->GetMLevel() * 0.5);		
		uint16 bstr = (PAlly->GetMLevel() * 0.5);
		uint16 bdex = (PAlly->GetMLevel() * 0.4);
		uint16 bagi = (PAlly->GetMLevel() * 0.5);
		uint16 maxhaste = PAlly->GetMLevel();
		haste = (floor(maxhaste * 2.7));
		if (haste > 200){
		haste = 200;
		}		
		PAlly->setModifier(MOD_STR, bstr); //added str 
		PAlly->setModifier(MOD_DEX, bdex); //added dex
		PAlly->setModifier(MOD_AGI, bagi); //added agi		
		PAlly->setModifier(MOD_ACC, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel()) + bmoddacc); //A+ Acc
		PAlly->setModifier(MOD_EVA, battleutils::GetMaxSkill(SKILL_POL, JOB_WAR, PAlly->GetMLevel())); //B- Evasion
		PAlly->setModifier(MOD_ATT, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel()) + bmoddatt);// A+ Attack		
		PAlly->setModifier(MOD_DEF, battleutils::GetMaxSkill(SKILL_POL, JOB_WAR, PAlly->GetMLevel()));// B- Defense
		PAlly->setModifier(MOD_HTH, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel()));// A+ H2H Skill	
	    PAlly->setModifier(MOD_CONVMPTOHP, 1);
		PAlly->setModifier(MOD_CONVHPTOMP, 1);
		PAlly->setModifier(MOD_FOOD_MPP, 1);
		PAlly->setModifier(MOD_FOOD_MP_CAP, 1);	
		PAlly->setModifier(MOD_MPP, 1);
		PAlly->setModifier(MOD_HPP, 1);	
        PAlly->setModifier(MOD_MOVE, 10);
		PAlly->setModifier(MOD_HASTE_GEAR, haste);
        PAlly->m_Weapons[SLOT_MAIN]->setSkillType(SKILL_H2H);
		PAlly->setModifier(MOD_ENMITY, -15);		
		//PAlly->m_Weapons[SLOT_MAIN]->setDamage((floor(PAlly->GetMLevel()*0.2)) + 3.0f);// D:32 @75
		PAlly->m_Weapons[SLOT_MAIN]->setDelay(floor(1000.0f*(400.0f / 60.0f))); //+48 Delay.  Default H2H delay is 480	
        PAlly->m_Weapons[SLOT_SUB]->setSkillType(SKILL_H2H);
		PAlly->m_Weapons[SLOT_SUB]->setDamage((floor(PAlly->GetMLevel()*0.2)) + 3.0f);// D:32 @75		
        PAlly->SetMJob(JOB_MNK);	
		PAlly->health.maxhp = (int16)(29 + (4.2*(plvl * 3.7f))); 
		PAlly->health.maxmp = (int16)(12 + (3.45f*(plvl * 2.66f))); 
		PAlly->UpdateHealth();
        PAlly->health.mp = PAlly->health.maxmp;
        PAlly->health.mp = PAlly->health.maxhp;		
		   if (plvl >= 75){
		   		PAlly->setModifier(MOD_MARTIAL_ARTS, 100); // 300 
				PAlly->setModifier(MOD_KICK_ATTACK, 13);
                PAlly->setModifier(MOD_DUAL_WIELD, 1);				
		   }
		   else if (plvl >= 71){
		   		PAlly->setModifier(MOD_MARTIAL_ARTS, 80); // 320
				PAlly->setModifier(MOD_KICK_ATTACK, 13);
                PAlly->setModifier(MOD_DUAL_WIELD, 1);				
		   }		   
		   else if (plvl >= 61){
		   		PAlly->setModifier(MOD_MARTIAL_ARTS, 80); // 320
				PAlly->setModifier(MOD_KICK_ATTACK, 10);
                PAlly->setModifier(MOD_DUAL_WIELD, 1);				
		   }
		   else if (plvl >= 51){
		   		PAlly->setModifier(MOD_MARTIAL_ARTS, 60); // 340
				PAlly->setModifier(MOD_KICK_ATTACK, 10);
                PAlly->setModifier(MOD_DUAL_WIELD, 1);				
		   }		   
		   else if (plvl >= 46){
		   		PAlly->setModifier(MOD_MARTIAL_ARTS, 60); // 340
				PAlly->setModifier(MOD_DUAL_WIELD, 1);
		   }
		   else if (plvl >= 31){
		   		PAlly->setModifier(MOD_MARTIAL_ARTS, 40); // 360
                PAlly->setModifier(MOD_DUAL_WIELD, 1);				
		   }		   
		   else if (plvl >= 16){
		   		PAlly->setModifier(MOD_MARTIAL_ARTS, 20); // 380
                PAlly->setModifier(MOD_DUAL_WIELD, 1);				
		   }		   
		}		
		
		
		
		
		else if (PetID == PETID_ULMIA)
		{
		uint16 moddatt = (PAlly->GetMLevel() * 1.0);
		uint16 moddacc = (PAlly->GetMLevel() * 0.5);
		uint16 bchr = (PAlly->GetMLevel() * 1.3);
		PAlly->setModifier(MOD_CHR, bchr); //added chr for spells		
		//ShowWarning(CL_GREEN"NAJI TRIGGERED SPAWN ALLY!!! \n" CL_RESET);
		PAlly->setModifier(MOD_ACC, battleutils::GetMaxSkill(SKILL_SYH, JOB_WAR, PAlly->GetMLevel()) + moddacc); //B+ Acc
		PAlly->setModifier(MOD_EVA, battleutils::GetMaxSkill(SKILL_SYH, JOB_WAR, PAlly->GetMLevel())); //B+ Evasion
		PAlly->setModifier(MOD_ATT, battleutils::GetMaxSkill(SKILL_SYH, JOB_WAR, PAlly->GetMLevel()) + moddatt);// B+ Attack
		PAlly->setModifier(MOD_DEF, battleutils::GetMaxSkill(SKILL_POL, JOB_WAR, PAlly->GetMLevel()));// B- Defense
		PAlly->setModifier(MOD_SINGING, battleutils::GetMaxSkill(SKILL_EVA, JOB_WAR, PAlly->GetMLevel()));// C Singing
		PAlly->setModifier(MOD_STRING, battleutils::GetMaxSkill(SKILL_EVA, JOB_WAR, PAlly->GetMLevel()));// C String		
		//PAlly->m_Weapons[SLOT_MAIN]->setDamage(floor(PAlly->GetMLevel()*0.56f));// D:42 @75
		//PAlly->m_Weapons[SLOT_MAIN]->setDelay(floor(1000.0f*(240.0f / 60.0f))); //240 delay
		PAlly->m_Weapons[SLOT_RANGED]->setSkillType(SKILL_STR);
		PAlly->setModifier(MOD_CONVMPTOHP, 1);
		PAlly->setModifier(MOD_CONVHPTOMP, 1);	
		PAlly->setModifier(MOD_FOOD_MPP, 1);
		PAlly->setModifier(MOD_FOOD_MP_CAP, 1);	
		PAlly->setModifier(MOD_MPP, 1);
		PAlly->setModifier(MOD_HPP, 1);
		PAlly->setModifier(MOD_MOVE, 10);
		PAlly->health.maxhp = (int16)(14 + (3.30f*(plvl * 4.15f))); 
		PAlly->health.maxmp = (int16)(10 + (1.52*(plvl * 3.46f))); 		
		PAlly->UpdateHealth();	
        PAlly->health.mp = PAlly->health.maxmp;
        PAlly->health.mp = PAlly->health.maxhp;			
		}
		else if (PetID == PETID_LUZAF)
		{
		uint16 bmoddatt = (PAlly->GetMLevel() * 1.0);
		uint16 bmoddacc = (PAlly->GetMLevel() * 0.5);
		uint16 bmodratt = (PAlly->GetMLevel() * 1.0);
		uint16 bmodracc = (PAlly->GetMLevel() * 1.0);		
		uint16 bstr = (PAlly->GetMLevel() * 0.5);
		uint16 bdex = (PAlly->GetMLevel() * 0.4);
		uint16 bagi = (PAlly->GetMLevel() * 0.5);	
		PAlly->setModifier(MOD_STR, bstr); //added str 
		PAlly->setModifier(MOD_DEX, bdex); //added dex
		PAlly->setModifier(MOD_AGI, bagi); //added dex	
		PAlly->setModifier(MOD_ENMITY, -15);		
		PAlly->setModifier(MOD_ACC, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel()) + bmoddacc); //A+ Acc
		PAlly->setModifier(MOD_RACC, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel()) + bmodracc); //A+ Acc		
		PAlly->setModifier(MOD_EVA, battleutils::GetMaxSkill(SKILL_SYH, JOB_WAR, PAlly->GetMLevel())); //B+ Evasion
		PAlly->setModifier(MOD_ATT, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel()) + bmoddatt);// A+ Attack
		PAlly->setModifier(MOD_RATT, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel()) + bmodratt);// A+ R.Attack			
		PAlly->setModifier(MOD_DEF, battleutils::GetMaxSkill(SKILL_POL, JOB_WAR, PAlly->GetMLevel()));// B- Defense

	    PAlly->setModifier(MOD_CONVMPTOHP, 1);
		PAlly->setModifier(MOD_CONVHPTOMP, 1);
		PAlly->setModifier(MOD_FOOD_MPP, 1);
		PAlly->setModifier(MOD_FOOD_MP_CAP, 1);	
		PAlly->setModifier(MOD_MPP, 1);
		PAlly->setModifier(MOD_HPP, 1);	
        PAlly->setModifier(MOD_MOVE, 10);		
		PAlly->m_Weapons[SLOT_MAIN]->setDamage(floor(PAlly->GetMLevel()*0.54f));// D:40 @75
		PAlly->m_Weapons[SLOT_MAIN]->setDelay(floor(1000.0f*(340.0f / 60.0f))); //240 delay
		PAlly->m_Weapons[SLOT_SUB]->setDamage(floor(PAlly->GetMLevel()*0.40f));// D:30 @75
		PAlly->m_Weapons[SLOT_SUB]->setDelay(floor(1000.0f*(200.0f / 60.0f))); //200 delay
		PAlly->m_Weapons[SLOT_RANGED]->setDamage(floor(PAlly->GetMLevel()*0.70f));// D:52 @75		
		PAlly->m_Weapons[SLOT_RANGED]->setDelay(floor(1000.0f*(480.0f / 60.0f))); //480 delay		
		PAlly->health.maxhp = (int16)(22 + (3.81f*(plvl * 3.62f))); 
		PAlly->UpdateHealth();
        PAlly->health.mp = PAlly->health.maxhp;		
		   if (plvl > 74){
				PAlly->setModifier(MOD_MATT, 30); // Magic Attack Bonus	
		   		PAlly->setModifier(MOD_DUAL_WIELD, 20); // Dual Wield					
		   }
		   else if (plvl > 49){
		   		PAlly->setModifier(MOD_DUAL_WIELD, 20); // Dual Wield			
		   }
		   else if (plvl > 34){
				PAlly->setModifier(MOD_MATT, 15); // Magic Attack Bonus
		   }		   
		   else if (plvl >= 10){
		   		PAlly->setModifier(MOD_DUAL_WIELD, 10); // Dual Wield
		   }		   
		}
		else if (PetID == PETID_GESSHO)
		{
	    uint16 haste = 0;
		uint16 bmoddatt = (PAlly->GetMLevel() * 1.0);
		uint16 bmoddacc = (PAlly->GetMLevel() * 0.5);		
		uint16 bstr = (PAlly->GetMLevel() * 0.5);
		uint16 bdex = (PAlly->GetMLevel() * 0.4);
		uint16 bagi = (PAlly->GetMLevel() * 0.5);
		uint16 maxhaste = PAlly->GetMLevel();
		haste = (floor(maxhaste * 2.7));
		if (haste > 200){
		haste = 200;
		}		
		PAlly->setModifier(MOD_STR, bstr); //added str 
		PAlly->setModifier(MOD_DEX, bdex); //added dex
		PAlly->setModifier(MOD_AGI, bagi); //added agi		
		PAlly->setModifier(MOD_ACC, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel()) + bmoddacc); //A+ Acc
		PAlly->setModifier(MOD_EVA, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel())); //A+ Evasion
		PAlly->setModifier(MOD_ATT, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel()) + bmoddatt);// A+ Attack		
		PAlly->setModifier(MOD_DEF, battleutils::GetMaxSkill(SKILL_POL, JOB_WAR, PAlly->GetMLevel()));// B- Defense
		PAlly->setModifier(MOD_NINJUTSU, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel()));// A+ Ninjutsu	
	    PAlly->setModifier(MOD_CONVMPTOHP, 1);
		PAlly->setModifier(MOD_CONVHPTOMP, 1);
		PAlly->setModifier(MOD_FOOD_MPP, 1);
		PAlly->setModifier(MOD_FOOD_MP_CAP, 1);	
		PAlly->setModifier(MOD_MPP, 1);
		PAlly->setModifier(MOD_HPP, 1);	
        PAlly->setModifier(MOD_MOVE, 10);
		PAlly->setModifier(MOD_HASTE_GEAR, haste);
		PAlly->setModifier(MOD_ENMITY, 30);		
		PAlly->m_Weapons[SLOT_MAIN]->setDamage(floor(PAlly->GetMLevel()*0.43f));// D:40 @75
		PAlly->m_Weapons[SLOT_MAIN]->setDelay(floor(1000.0f*(165.0f / 60.0f))); //227 delay		
		PAlly->health.maxhp = (int16)(22 + (3.81f*(plvl * 3.62f))); 
		PAlly->UpdateHealth();
        PAlly->health.mp = PAlly->health.maxhp;		
		   if (plvl > 74){
				PAlly->setModifier(MOD_MATT, 25); // Magic Attack Bonus	
				PAlly->setModifier(MOD_DOUBLE_ATTACK, 25); // Double Attack
		   }
		   else if (plvl > 49){
		   		PAlly->setModifier(MOD_DOUBLE_ATTACK, 20); // Double Attack
				PAlly->setModifier(MOD_MATT, 25); // Magic Attack Bonus				
		   }		   
		   else if (plvl >= 25){
		   		PAlly->setModifier(MOD_DOUBLE_ATTACK, 10); // Double Attack
				PAlly->setModifier(MOD_MATT, 15); // Magic Attack Bonus
		   }		   
		}
		else if (PetID == PETID_ZEID)
		{
		uint16 haste = 0;
		uint16 moddatt = (PAlly->GetMLevel() * 0.75);
		uint16 moddacc = (PAlly->GetMLevel() * 0.75);
		uint16 bstr = (PAlly->GetMLevel() * 0.4);
		uint16 bdex = (PAlly->GetMLevel() * 0.4);
		uint16 bagi = (PAlly->GetMLevel() * 0.5);
		uint16 maxhaste = PAlly->GetMLevel();
		haste = (floor(maxhaste * 2.7));
		if (haste > 200){
		haste = 200;
		}		
		PAlly->setModifier(MOD_STR, bstr); //added str 
		PAlly->setModifier(MOD_DEX, bdex); //added dex
		PAlly->setModifier(MOD_AGI, bagi); //added agi	
		if (plvl >= 75) //Addin Trust Tributes
		{
			PAlly->setModifier(MOD_ACC, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel()) + moddacc + accZeid); //A+ Acc
		    PAlly->setModifier(MOD_ATT, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel()) + moddatt + attZeid);// A+ Attack			
		}
		else
		{
			PAlly->setModifier(MOD_ACC, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel()) + moddacc); //A+ Acc
		    PAlly->setModifier(MOD_ATT, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel()) + moddatt);// A+ Attack		
		}
		PAlly->setModifier(MOD_ACC, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel()) + moddacc); //A+ Acc
		PAlly->setModifier(MOD_EVA, battleutils::GetMaxSkill(SKILL_SYH, JOB_WAR, PAlly->GetMLevel())); //B+ Evasion
		PAlly->setModifier(MOD_ATT, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel()) + moddatt);// A+ Attack
		PAlly->setModifier(MOD_DEF, battleutils::GetMaxSkill(SKILL_POL, JOB_WAR, PAlly->GetMLevel()));// B- Defense
		PAlly->setModifier(MOD_DARK, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PAlly->GetMLevel()));// A+ Ninjutsu	
		PAlly->m_Weapons[SLOT_MAIN]->setDamage((floor(PAlly->GetMLevel()*0.92f) + 11));// D:80 @75
		PAlly->m_Weapons[SLOT_MAIN]->setDelay(floor(1000.0f*(470.0f / 60.0f))); //500 delay
		PAlly->setModifier(MOD_CONVMPTOHP, 1);
		PAlly->setModifier(MOD_CONVHPTOMP, 1);	
		PAlly->setModifier(MOD_FOOD_MPP, 1);
		PAlly->setModifier(MOD_FOOD_MP_CAP, 1);	
		PAlly->setModifier(MOD_ENMITY, -15);		
		PAlly->setModifier(MOD_MPP, 1);
		PAlly->setModifier(MOD_HPP, 1);
		PAlly->setModifier(MOD_MOVE, 10);
	    PAlly->setModifier(MOD_PARALYZERES, 10);	
		PAlly->health.maxhp = (int16)(20 + (3.35f*(plvl * 4.18f))); 
        PAlly->health.maxmp = (int16)(12 + (2.65f*(plvl * 2.50f))); 
		PAlly->UpdateHealth();
        PAlly->health.mp = PAlly->health.maxmp;			
		   if (plvl > 74){
				PAlly->setModifier(MOD_DOUBLE_ATTACK, 15);
				PAlly->setModifier(MOD_OCCULT_ACUMEN, occZeid);
				PAlly->setModifier(MOD_STORETP, 25); // Small Store TP Bonus
		   }
	       else if (plvl > 24){
				PAlly->setModifier(MOD_DOUBLE_ATTACK, 15);
				PAlly->setModifier(MOD_STORETP, 10); // Small Store TP Bonus
		   }
		}		
		
		
        PAlly->PBattleAI = new CAIPetDummy(PAlly);
        PAlly->PBattleAI->SetLastActionTime(gettick());
        PAlly->PBattleAI->SetCurrentAction(ACTION_SPAWN);
        PAlly->PMaster = PMaster;

		
		if (PMaster->getZone() == 60)
		{
			PMaster->PInstance->InsertPET(PAlly);
			ShowWarning(CL_RED"INSTANCE INSERT Ally" CL_RESET);
		}
		else
		{
			PMaster->loc.zone->InsertPET(PAlly);
		}
		
        //PMaster->loc.zone->InsertPET(PAlly);
        PMaster->PParty->ReloadParty();	
        if (PMaster->objtype == TYPE_PC)
         {
          static_cast<CCharEntity*>(PMaster)->resetPetZoningInfo();
         }		 

    }
	
    void SpawnMobPet(CBattleEntity* PMaster, uint32 PetID)
    {
        // this is ONLY used for mob smn elementals / avatars
        /*
        This should eventually be merged into one big spawn pet method.
        At the moment player pets and mob pets are totally different. We need a central place
        to manage pet families and spawn them.
        */

        // grab pet info
        Pet_t* petData = g_PPetList.at(PetID);
        CMobEntity* PPet = (CMobEntity*)PMaster->PPet;

        PPet->look = petData->look;
        PPet->name = petData->name;
        PPet->SetMJob(petData->mJob);
        PPet->m_EcoSystem = petData->EcoSystem;
        PPet->m_Family = petData->m_Family;
        PPet->m_Element = petData->m_Element;
        PPet->HPscale = petData->HPscale;
        PPet->MPscale = petData->MPscale;
        PPet->m_HasSpellScript = petData->hasSpellScript;

        PPet->allegiance = PMaster->allegiance;
        PMaster->StatusEffectContainer->CopyConfrontationEffect(PPet);

        if (PPet->m_EcoSystem == SYSTEM_AVATAR || PPet->m_EcoSystem == SYSTEM_ELEMENTAL)
        {
            // assuming elemental spawn
            PPet->setModifier(MOD_DMGPHYS, -50); //-50% PDT
        }

        PPet->m_SpellListContainer = mobSpellList::GetMobSpellList(petData->spellList);

        PPet->setModifier(MOD_SLASHRES, petData->slashres);
        PPet->setModifier(MOD_PIERCERES, petData->pierceres);
        PPet->setModifier(MOD_HTHRES, petData->hthres);
        PPet->setModifier(MOD_IMPACTRES, petData->impactres);

        PPet->setModifier(MOD_FIREDEF, petData->firedef); // These are stored as floating percentages
        PPet->setModifier(MOD_ICEDEF, petData->icedef); // and need to be adjusted into modifier units.
        PPet->setModifier(MOD_WINDDEF, petData->winddef); // Higher DEF = lower damage.
        PPet->setModifier(MOD_EARTHDEF, petData->earthdef); // Negatives signify increased damage.
        PPet->setModifier(MOD_THUNDERDEF, petData->thunderdef); // Positives signify reduced damage.
        PPet->setModifier(MOD_WATERDEF, petData->waterdef); // Ex: 125% damage would be 1.25, 50% damage would be 0.50
        PPet->setModifier(MOD_LIGHTDEF, petData->lightdef); // (1.25 - 1) * -1000 = -250 DEF
        PPet->setModifier(MOD_DARKDEF, petData->darkdef); // (0.50 - 1) * -1000 = 500 DEF

        PPet->setModifier(MOD_FIRERES, petData->fireres); // These are stored as floating percentages
        PPet->setModifier(MOD_ICERES, petData->iceres); // and need to be adjusted into modifier units.
        PPet->setModifier(MOD_WINDRES, petData->windres); // Higher RES = lower damage.
        PPet->setModifier(MOD_EARTHRES, petData->earthres); // Negatives signify lower resist chance.
        PPet->setModifier(MOD_THUNDERRES, petData->thunderres); // Positives signify increased resist chance.
        PPet->setModifier(MOD_WATERRES, petData->waterres);
        PPet->setModifier(MOD_LIGHTRES, petData->lightres);
        PPet->setModifier(MOD_DARKRES, petData->darkres);
    }

    void DetachPet(CBattleEntity* PMaster)
    {
        DSP_DEBUG_BREAK_IF(PMaster->PPet == nullptr);
        DSP_DEBUG_BREAK_IF(PMaster->objtype != TYPE_PC);

        CBattleEntity* PPet = PMaster->PPet;
        CCharEntity* PChar = (CCharEntity*)PMaster;

        PPet->PBattleAI->SetCurrentAction(ACTION_FALL);

        if (PPet->objtype == TYPE_MOB){
            CMobEntity* PMob = (CMobEntity*)PPet;

            if (!PMob->isDead()){
                // mobs charm wears off whist fighting another mob. Both mobs now attack player since mobs are no longer enemies
                if (PMob->PBattleAI != nullptr && PMob->PBattleAI->GetBattleTarget() != nullptr && PMob->PBattleAI->GetBattleTarget()->objtype == TYPE_MOB){
                    ((CMobEntity*)PMob->PBattleAI->GetBattleTarget())->PEnmityContainer->Clear();
                    ((CMobEntity*)PMob->PBattleAI->GetBattleTarget())->PEnmityContainer->UpdateEnmity(PChar, 0, 0);
                }

                //clear the ex-charmed mobs enmity
                PMob->PEnmityContainer->Clear();

                // charm time is up, mob attacks player now
                if (PMob->PEnmityContainer->IsWithinEnmityRange(PMob->PMaster))
                {
                    PMob->PEnmityContainer->UpdateEnmity(PChar, 0, 0);
                }
                else
                {
                    PMob->m_OwnerID.clean();
                    PMob->updatemask |= UPDATE_STATUS;
                }

                // dirty exp if not full
                PMob->m_giveExp = PMob->GetHPP() == 100;

                CAIPetDummy* PPetAI = (CAIPetDummy*)PPet->PBattleAI;
                //master using leave command
                if (PMaster->PBattleAI->GetCurrentAction() == ACTION_JOBABILITY_FINISH && PMaster->PBattleAI->GetCurrentJobAbility()->getID() == 55 || PChar->loc.zoning || PChar->isDead()){
                    PMob->PEnmityContainer->Clear();
                    PMob->m_OwnerID.clean();
                    PMob->updatemask |= UPDATE_STATUS;
                }

            }
            else {
                PMob->m_OwnerID.clean();
                PMob->updatemask |= UPDATE_STATUS;
            }

            PMob->isCharmed = false;
            PMob->allegiance = ALLEGIANCE_MOB;
            PMob->charmTime = 0;
            PMob->PMaster = nullptr;

            delete PMob->PBattleAI;
            PMob->PBattleAI = new CAIMobDummy(PMob);
            PMob->PBattleAI->SetLastActionTime(gettick());

            if (PMob->isDead())
                PMob->PBattleAI->SetCurrentAction(ACTION_FALL);
            else
                PMob->PBattleAI->SetCurrentAction(ACTION_DISENGAGE);

        }
        else if (PPet->objtype == TYPE_PET){
            CPetEntity* PPetEnt = (CPetEntity*)PPet;

            if (PPetEnt->getPetType() == PETTYPE_AVATAR)
                PMaster->setModifier(MOD_AVATAR_PERPETUATION, 0);

            ((CCharEntity*)PMaster)->PLatentEffectContainer->CheckLatentsPetType(-1);
            PMaster->ForParty([](CBattleEntity* PMember){
                ((CCharEntity*)PMember)->PLatentEffectContainer->CheckLatentsPartyAvatar();
            });

            if (PPetEnt->getPetType() != PETTYPE_AUTOMATON){
                PPetEnt->PMaster = nullptr;
            }
            PChar->removePetModifiers(PPetEnt);
            charutils::BuildingCharPetAbilityTable(PChar, PPetEnt, 0);// blank the pet commands
        }

        PChar->PPet = nullptr;
        PChar->pushPacket(new CCharUpdatePacket(PChar));
    }

    /************************************************************************
    *																		*
    *																		*
    *																		*
    ************************************************************************/

    void DespawnPet(CBattleEntity* PMaster)
    {
        DSP_DEBUG_BREAK_IF(PMaster->PPet == nullptr);

        CBattleEntity* PPet = PMaster->PPet;


        // mob was not reset properly on death/uncharm
        // reset manually
        if (PPet->isCharmed && PMaster->objtype == TYPE_MOB)
        {
            PPet->isCharmed = false;
            PMaster->charmTime = 0;

            delete PPet->PBattleAI;
			//luautils::OnMobDeath(this, nullptr);
            PPet->PBattleAI = new CAIMobDummy((CMobEntity*)PMaster);
            PPet->PBattleAI->SetLastActionTime(gettick());
            PPet->PBattleAI->SetCurrentAction(ACTION_FALL);

            ShowDebug("An ex charmed mob was not reset properly, Manually resetting it.\n");
            return;
        }


        petutils::DetachPet(PMaster);
        // when Ally dies
        uint8 counter = 0;
        if (PMaster != nullptr && PMaster->PAlly.size() != 0){
            for (auto ally : PMaster->PAlly)
            {
                //if (ally == this)
                //{
                    PMaster->PAlly.erase(PMaster->PAlly.begin() + counter);
                    break;
                //}
                counter++;
            }
            PMaster->PParty->ReloadParty();
        }
		
    }

    void MakePetStay(CBattleEntity* PMaster)
    {

        CPetEntity* PPet = (CPetEntity*)PMaster->PPet;

        if (PPet != nullptr && !PPet->StatusEffectContainer->HasPreventActionEffect())
        {
            PPet->PBattleAI->SetCurrentAction(ACTION_NONE);
        }
    }

    int16 PerpetuationCost(uint32 id, uint8 level)
    {
        int16 cost = 0;
        if (id >= 0 && id <= 7)
        {
            if (level < 19)
                cost = 1;
            else if (level < 38)
                cost = 2;
            else if (level < 57)
                cost = 3;
            else if (level < 75)
                cost = 4;
            else if (level < 81)
                cost = 5;
            else if (level < 91)
                cost = 6;
            else
                cost = 7;
        }
        else if (id == 8)
        {
            if (level < 10)
                cost = 1;
            else if (level < 18)
                cost = 2;
            else if (level < 27)
                cost = 3;
            else if (level < 36)
                cost = 4;
            else if (level < 45)
                cost = 5;
            else if (level < 54)
                cost = 6;
            else if (level < 63)
                cost = 7;
            else if (level < 72)
                cost = 8;
            else if (level < 81)
                cost = 9;
            else if (level < 91)
                cost = 10;
            else
                cost = 11;
        }
        else if (id == 9)
        {
            if (level < 8)
                cost = 1;
            else if (level < 15)
                cost = 2;
            else if (level < 22)
                cost = 3;
            else if (level < 30)
                cost = 4;
            else if (level < 37)
                cost = 5;
            else if (level < 45)
                cost = 6;
            else if (level < 51)
                cost = 7;
            else if (level < 59)
                cost = 8;
            else if (level < 66)
                cost = 9;
            else if (level < 73)
                cost = 10;
            else if (level < 81)
                cost = 11;
            else if (level < 91)
                cost = 12;
            else
                cost = 13;
        }
        else if (id <= 16)
        {
            if (level < 10)
                cost = 3;
            else if (level < 19)
                cost = 4;
            else if (level < 28)
                cost = 5;
            else if (level < 38)
                cost = 6;
            else if (level < 47)
                cost = 7;
            else if (level < 56)
                cost = 8;
            else if (level < 65)
                cost = 9;
            else if (level < 68)
                cost = 10;
            else if (level < 71)
                cost = 11;
            else if (level < 74)
                cost = 12;
            else if (level < 81)
                cost = 13;
            else if (level < 91)
                cost = 14;
            else
                cost = 15;
        }

        return cost;
    }

    /*
    Familiars a pet.
    */
    void Familiar(CBattleEntity* PPet)
    {

        /*
            Boost HP by 10%
            Increase charm duration up to 30 mins
            boost stats by 10%
            */

        // only increase time for charmed mobs
        if (PPet->objtype == TYPE_MOB && PPet->isCharmed)
        {
            // increase charm duration
            // 30 mins - 1-5 mins
            PPet->charmTime += 1800000 - dsprand::GetRandomNumber(300000u);
        }

        float rate = 0.10f;

        // boost hp by 10%
        uint16 boost = (float)PPet->health.maxhp * rate;

        PPet->health.maxhp += boost;
        PPet->health.hp += boost;
        PPet->UpdateHealth();

        // boost stats by 10%
        PPet->addModifier(MOD_ATTP, rate * 100.0f);
        PPet->addModifier(MOD_ACCP, rate * 100.0f);
        PPet->addModifier(MOD_EVAP, rate * 100.0f);
        PPet->addModifier(MOD_DEFP, rate * 100.0f);

    }
	   
    CPetEntity* LoadAlly(CBattleEntity* PMaster, uint32 PetID, bool spawningFromZone)
    {
	    //ShowWarning(CL_GREEN"LOAD ALLY \n" CL_RESET);
        DSP_DEBUG_BREAK_IF(PetID >= g_PPetList.size());
        Pet_t* PPetData = g_PPetList.at(PetID);
        PETTYPE petType = PETTYPE_TRUST;
        CPetEntity* PPet = new CPetEntity(petType);
				
        PPet->m_Weapons[SLOT_MAIN]->setDelay(floor(1000.0f*(240.0f / 60.0f)));

        if (PPet->m_PetID == PETID_PRISHE)
		{
		    PPet->SetMJob(JOB_MNK);
			PPet->m_Weapons[SLOT_MAIN]->resetDelay();
		}
		

        PPet->SetMLevel(PMaster->GetMLevel());
        PPet->SetSLevel(PMaster->GetMLevel() / 2);
        LoadTrustStats(PPet, PPetData);
        PPet->loc = PMaster->loc;
        PPet->loc.p = nearPosition(PMaster->loc.p, PET_ROAM_DISTANCE, M_PI);
        
        PPet->look = g_PPetList.at(PetID)->look;
        PPet->name = g_PPetList.at(PetID)->name;
        PPet->m_name_prefix = g_PPetList.at(PetID)->name_prefix;
        PPet->m_MobSkillList = g_PPetList.at(PetID)->m_MobSkillList;
        //PPet->SetMJob(g_PPetList.at(PetID)->mJob);
        PPet->m_Element = g_PPetList.at(PetID)->m_Element;
        PPet->m_PetID = PetID;
        
        FinalizePetStatistics(PMaster, PPet);
		PPet->PetSkills = battleutils::GetMobSkillList(PPet->m_MobSkillList);
		PPet->status = STATUS_NORMAL;
		PPet->m_ModelSize += g_PPetList.at(PetID)->size;
		PPet->m_EcoSystem = g_PPetList.at(PetID)->EcoSystem;
        PMaster->PAlly.insert(PMaster->PAlly.begin(), PPet);
        return PPet;
        
    }
    	

    void LoadPet(CBattleEntity* PMaster, uint32 PetID, bool spawningFromZone)
    {
        DSP_DEBUG_BREAK_IF(PetID >= g_PPetList.size());
        if (PMaster->GetMJob() != JOB_DRG && PetID == PETID_WYVERN) {
            return;
        }

        Pet_t* PPetData = g_PPetList.at(PetID);

        if (PMaster->objtype == TYPE_PC)
            ((CCharEntity*)PMaster)->petZoningInfo.petID = PetID;

        PETTYPE petType = PETTYPE_JUG_PET;

        if (PetID <= PETID_CAIT_SITH)
        {
            petType = PETTYPE_AVATAR;
        }
        //TODO: move this out of modifying the global pet list
        else if (PetID == PETID_WYVERN)
        {
            petType = PETTYPE_WYVERN;

            const int8* Query =
                "SELECT\
                pet_name.name,\
                char_pet.wyvernid\
                FROM pet_name, char_pet\
                WHERE pet_name.id = char_pet.wyvernid AND \
                char_pet.charid = %u";

            if (Sql_Query(SqlHandle, Query, PMaster->id) != SQL_ERROR && Sql_NumRows(SqlHandle) != 0)
            {
                while (Sql_NextRow(SqlHandle) == SQL_SUCCESS)
                {
                    uint16 wyvernid = (uint16)Sql_GetIntData(SqlHandle, 1);

                    if (wyvernid != 0)
                    {
                        g_PPetList.at(PetID)->name.clear();
                        g_PPetList.at(PetID)->name.insert(0, Sql_GetData(SqlHandle, 0));
                    }
                }
            }
        }
        /*
        else if (PetID==PETID_ADVENTURING_FELLOW)
        {
        petType = PETTYPE_ADVENTURING_FELLOW;

        const int8* Query =
        "SELECT\
        pet_name.name,\
        char_pet.adventuringfellowid\
        FROM pet_name, char_pet\
        WHERE pet_name.id = char_pet.adventuringfellowid";

        if ( Sql_Query(SqlHandle, Query) != SQL_ERROR && Sql_NumRows(SqlHandle) != 0)
        {
        while (Sql_NextRow(SqlHandle) == SQL_SUCCESS)
        {
        uint16 adventuringfellowid = (uint16)Sql_GetIntData(SqlHandle, 1);

        if (adventuringfellowid != 0)
        {
        g_PPetList.at(PetID)->name.clear();
        g_PPetList.at(PetID)->name.insert(0, Sql_GetData(SqlHandle, 0));
        }
        }
        }
        }
        */
        else if (PetID == PETID_CHOCOBO)
        {
            petType = PETTYPE_CHOCOBO;

            const int8* Query =
                "SELECT\
                char_pet.chocoboid\
                FROM char_pet\
                char_pet.charid = %u";

            if (Sql_Query(SqlHandle, Query, PMaster->id) != SQL_ERROR && Sql_NumRows(SqlHandle) != 0)
            {
                while (Sql_NextRow(SqlHandle) == SQL_SUCCESS)
                {
                    uint32 chocoboid = (uint32)Sql_GetIntData(SqlHandle, 0);

                    if (chocoboid != 0)
                    {
                        uint16 chocoboname1 = chocoboid & 0x0000FFFF;
                        uint16 chocoboname2 = chocoboid >>= 16;

                        g_PPetList.at(PetID)->name.clear();

                        Query =
                            "SELECT\
                            pet_name.name\
                            FROM pet_name\
                            WHERE pet_name.id = %u OR pet_name.id = %u";

                        if (Sql_Query(SqlHandle, Query, chocoboname1, chocoboname2) != SQL_ERROR && Sql_NumRows(SqlHandle) != 0)
                        {
                            while (Sql_NextRow(SqlHandle) == SQL_SUCCESS)
                            {
                                if (chocoboname1 != 0 && chocoboname2 != 0)
                                {
                                    g_PPetList.at(PetID)->name.insert(0, Sql_GetData(SqlHandle, 0));
                                }
                            }
                        }
                    }
                }
            }
        }
        else if (PetID == PETID_HARLEQUINFRAME || PetID == PETID_VALOREDGEFRAME || PetID == PETID_SHARPSHOTFRAME || PetID == PETID_STORMWAKERFRAME)
        {
            petType = PETTYPE_AUTOMATON;
        }
        CPetEntity* PPet = nullptr;
        if (petType == PETTYPE_AUTOMATON && PMaster->objtype == TYPE_PC)
            PPet = ((CCharEntity*)PMaster)->PAutomaton;
		else 
			PPet = new CPetEntity(petType);

        PPet->loc = PMaster->loc;

        // spawn me randomly around master
        PPet->loc.p = nearPosition(PMaster->loc.p, PET_ROAM_DISTANCE, M_PI);

        if (petType != PETTYPE_AUTOMATON)
        {
            PPet->look = g_PPetList.at(PetID)->look;
            PPet->name = g_PPetList.at(PetID)->name;
        }
        else
        {
            PPet->look.size = MODEL_AUTOMATON;
        }
        PPet->m_name_prefix = g_PPetList.at(PetID)->name_prefix;
        PPet->m_Family = g_PPetList.at(PetID)->m_Family;
        PPet->m_MobSkillList = g_PPetList.at(PetID)->m_MobSkillList;
        PPet->SetMJob(g_PPetList.at(PetID)->mJob);
        PPet->m_Element = g_PPetList.at(PetID)->m_Element;
        PPet->m_PetID = PetID;


        if (PPet->getPetType() == PETTYPE_AVATAR){
            if (PMaster->GetMJob() == JOB_SMN){
                PPet->SetMLevel(PMaster->GetMLevel());
            }
            else if (PMaster->GetSJob() == JOB_SMN){
                PPet->SetMLevel(PMaster->GetSLevel());
            }
            else{ //should never happen
                ShowDebug("%s summoned an avatar but is not SMN main or SMN sub! Please report. \n", PMaster->GetName());
                PPet->SetMLevel(1);
            }
            LoadAvatarStats(PPet); //follows PC calcs (w/o SJ)
            PPet->setModifier(MOD_DMGPHYS, -50); //-50% PDT
            if (PPet->GetMLevel() >= 70){
                PPet->setModifier(MOD_MATT, 32);
            }
            else if (PPet->GetMLevel() >= 50){
                PPet->setModifier(MOD_MATT, 28);
            }
            else if (PPet->GetMLevel() >= 30){
                PPet->setModifier(MOD_MATT, 24);
            }
            else if (PPet->GetMLevel() >= 10){
                PPet->setModifier(MOD_MATT, 20);
            }
            PPet->m_Weapons[SLOT_MAIN]->setDelay(floor(1000.0f*(320.0f / 60.0f)));
            if (PetID == PETID_FENRIR){
                PPet->m_Weapons[SLOT_MAIN]->setDelay(floor(1000.0*(280.0f / 60.0f)));
            }
            PPet->m_Weapons[SLOT_MAIN]->setDamage(floor(PPet->GetMLevel()*0.74f));
            if (PetID == PETID_CARBUNCLE){
                PPet->m_Weapons[SLOT_MAIN]->setDamage(floor(PPet->GetMLevel()*0.67f));
            }

            //Set B+ weapon skill (assumed capped for level derp)
            //attack is madly high for avatars (roughly x2)
            PPet->setModifier(MOD_ATT, 2 * battleutils::GetMaxSkill(SKILL_CLB, JOB_WHM, PPet->GetMLevel()));
            PPet->setModifier(MOD_ACC, battleutils::GetMaxSkill(SKILL_CLB, JOB_WHM, PPet->GetMLevel()));
            //Set E evasion and def
            PPet->setModifier(MOD_EVA, battleutils::GetMaxSkill(SKILL_THR, JOB_WHM, PPet->GetMLevel()));
            PPet->setModifier(MOD_DEF, battleutils::GetMaxSkill(SKILL_THR, JOB_WHM, PPet->GetMLevel()));
            // cap all magic skills so they play nice with spell scripts
            for (int i = SKILL_DIV; i <= SKILL_BLU; i++) {
                uint16 maxSkill = battleutils::GetMaxSkill((SKILLTYPE)i, PPet->GetMJob(), PPet->GetMLevel());
                if (maxSkill != 0) {
                    PPet->WorkingSkills.skill[i] = maxSkill;
                }
                else //if the mob is WAR/BLM and can cast spell
                {
                    // set skill as high as main level, so their spells won't get resisted
                    uint16 maxSubSkill = battleutils::GetMaxSkill((SKILLTYPE)i, PPet->GetSJob(), PPet->GetMLevel());

                    if (maxSubSkill != 0)
                    {
                        PPet->WorkingSkills.skill[i] = maxSubSkill;
                    }
                }
            }

            
            if (PMaster->objtype == TYPE_PC)
            {
                CCharEntity* PChar = (CCharEntity*)PMaster;
                PPet->addModifier(MOD_MATT, PChar->PMeritPoints->GetMeritValue(MERIT_AVATAR_MAGICAL_ATTACK, PChar));
                PPet->addModifier(MOD_ATT, PChar->PMeritPoints->GetMeritValue(MERIT_AVATAR_PHYSICAL_ATTACK, PChar));
                PPet->addModifier(MOD_MACC, PChar->PMeritPoints->GetMeritValue(MERIT_AVATAR_MAGICAL_ACCURACY, PChar));
                PPet->addModifier(MOD_ACC, PChar->PMeritPoints->GetMeritValue(MERIT_AVATAR_PHYSICAL_ACCURACY, PChar));
            }

            PMaster->addModifier(MOD_AVATAR_PERPETUATION, PerpetuationCost(PetID, PPet->GetMLevel()));
        }
        else if (PPet->getPetType() == PETTYPE_JUG_PET){
            PPet->m_Weapons[SLOT_MAIN]->setDelay(floor(1000.0f*(240.0f / 60.0f)));
            //TODO: Base off the caps + merits depending on jug type

            // get random lvl
            uint8 highestLvl = PMaster->GetMLevel();

			if (highestLvl > PPetData->maxLevel)
            {
				highestLvl = PPetData->maxLevel;
			}

			// Increase the pet's level by the bonus.
			CCharEntity* PChar = (CCharEntity*)PMaster;
			highestLvl += PChar->PMeritPoints->GetMeritValue(MERIT_BEAST_AFFINITY, PChar);

            // 0-2 lvls lower, less Monster Gloves(+1/+2) bonus
            highestLvl -= dsprand::GetRandomNumber(3 - dsp_cap(PChar->getMod(MOD_JUG_LEVEL_RANGE), 0, 2));

            PPet->SetMLevel(highestLvl);
            LoadJugStats(PPet, PPetData); //follow monster calcs (w/o SJ)
        }
        else if (PPet->getPetType() == PETTYPE_WYVERN){
			LoadWyvernStatistics(PMaster, PPet, false);
        }
        else if (PPet->getPetType() == PETTYPE_AUTOMATON && PMaster->objtype == TYPE_PC)
        {
            CAutomatonEntity* PAutomaton = (CAutomatonEntity*)PPet;
			AUTOHEADTYPE head = PAutomaton->getHead();
            switch (PAutomaton->getFrame())
            {
            case FRAME_HARLEQUIN:
                PPet->SetMJob(JOB_RDM);
                PPet->SetSJob(JOB_WAR);
				PPet->setModifier(MOD_ATTP, 5);
                break;
            case FRAME_VALOREDGE:
                PPet->SetMJob(JOB_WAR);
                PPet->SetSJob(JOB_WAR);
                break;
            case FRAME_SHARPSHOT:
                PPet->SetMJob(JOB_RNG);
                PPet->SetSJob(JOB_PUP);
                break;	
            case FRAME_STORMWAKER:
                PPet->SetMJob(JOB_WHM);
                PPet->SetSJob(JOB_RDM);
                {
				switch (PAutomaton->getHead())
				{
				case HEAD_STORMWAKER:
				    PPet->SetMJob(JOB_RDM);
                    PPet->SetSJob(JOB_RDM);
					break;
				case HEAD_SPIRITREAVER:
					PPet->SetMJob(JOB_BLM);
                    PPet->SetSJob(JOB_RDM);
					break;
				case HEAD_IMMORTAL:
					PPet->SetMJob(JOB_BLU);
                    PPet->SetSJob(JOB_WAR);
					break;	
					}
				}	
				break;
            }
            //TEMP: should be MLevel when unsummoned, and PUP level when summoned
			
            if (PMaster->GetMJob() == JOB_PUP){
                PPet->SetMLevel(PMaster->GetMLevel());
				PPet->SetSLevel(PMaster->GetMLevel() / 2);
            }
            else if (PMaster->GetSJob() == JOB_PUP){
                PPet->SetMLevel(PMaster->GetSLevel());
				PPet->SetSLevel(PMaster->GetSLevel() / 2);
            }
			

            
            LoadAutomatonStats((CCharEntity*)PMaster, PPet, g_PPetList.at(PetID)); //temp
			
            /*
                CCharEntity* PChar = (CCharEntity*)PMaster;
				//Melee Skill
				PChar->setModifier(MOD_AUTO_MELEE_SKILL, PChar->PMeritPoints->GetMeritValue(MERIT_AUTOMATION_SKILLS, PChar));
				ShowWarning(CL_RED"LOADING NEW AUTOMATON  MELEE SKILL STATS\n" CL_RESET);
				//Archery Skill
				PChar->setModifier(MOD_AUTO_RANGED_SKILL, PChar->PMeritPoints->GetMeritValue(MERIT_AUTOMATION_SKILLS, PChar));
				//Magic Skill
				PChar->setModifier(MOD_AUTO_MAGIC_SKILL, PChar->PMeritPoints->GetMeritValue(MERIT_AUTOMATION_SKILLS, PChar));
				//Fine Tuning Merits
                PPet->setModifier(MOD_ACC, PChar->PMeritPoints->GetMeritValue(MERIT_FINE_TUNING, PChar));
                PPet->setModifier(MOD_RACC, PChar->PMeritPoints->GetMeritValue(MERIT_FINE_TUNING, PChar));
                PPet->setModifier(MOD_EVA, PChar->PMeritPoints->GetMeritValue(MERIT_FINE_TUNING, PChar));
                PPet->setModifier(MOD_MDEF, PChar->PMeritPoints->GetMeritValue(MERIT_FINE_TUNING, PChar));
				//Optimization
				PPet->setModifier(MOD_ATTP, PChar->PMeritPoints->GetMeritValue(MERIT_OPTIMIZATION, PChar));
                PPet->setModifier(MOD_DEFP, PChar->PMeritPoints->GetMeritValue(MERIT_OPTIMIZATION, PChar));
                PPet->setModifier(MOD_MATT, PChar->PMeritPoints->GetMeritValue(MERIT_OPTIMIZATION, PChar));
			*/	
            
        }
       /* else if (PPet->getPetType() == PETTYPE_TRUST)
        {
		    CPetEntity* PPet = (CPetEntity*)PMaster->PPet;
		    if (PetID == PETID_NANAA_MIHGO){
			}
		    //Set A+ weapon skill
		    PPet->setModifier(MOD_ACC, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PPet->GetMLevel()));
		    //Set A+ evasion
		    PPet->setModifier(MOD_EVA, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PPet->GetMLevel()));
            PPet->SetMLevel(PMaster->GetMLevel());
            PPet->SetSLevel(PMaster->GetMLevel() / 2);
            LoadTrustStats(PPet, PPetData);  
        }*/
		
		FinalizePetStatistics(PMaster, PPet);
		PPet->PetSkills = battleutils::GetMobSkillList(PPet->m_MobSkillList);
		PPet->status = STATUS_NORMAL;
		PPet->m_ModelSize += g_PPetList.at(PetID)->size;
		PPet->m_EcoSystem = g_PPetList.at(PetID)->EcoSystem;

        PMaster->PPet = PPet;
    }

	void LoadWyvernStatistics(CBattleEntity* PMaster, CPetEntity* PPet, bool finalize) {
		//set the wyvern job based on master's SJ
		if (PMaster->GetSJob() != JOB_NON){
			PPet->SetSJob(PMaster->GetSJob());
		}
		PPet->SetMJob(JOB_DRG);
		PPet->SetMLevel(PMaster->GetMLevel());
		
		uint8 plvl = PMaster->GetMLevel();

		LoadAvatarStats(PPet); //follows PC calcs (w/o SJ)
		PPet->m_Weapons[SLOT_MAIN]->setDelay(floor(1000.0f*(320.0f / 60.0f))); //320 delay
		PPet->m_Weapons[SLOT_MAIN]->setDamage(1 + floor(PPet->GetMLevel()*0.9f));
		//Set A+ weapon skill
		PPet->setModifier(MOD_ATT, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PPet->GetMLevel()));
		PPet->setModifier(MOD_ACC, battleutils::GetMaxSkill(SKILL_GAX, JOB_WAR, PPet->GetMLevel()));
		//Set B- evasion and def
		PPet->setModifier(MOD_EVA, battleutils::GetMaxSkill(SKILL_POL, JOB_WAR, PPet->GetMLevel()));
		PPet->setModifier(MOD_DEF, battleutils::GetMaxSkill(SKILL_POL, JOB_WAR, PPet->GetMLevel()));
		
		if (plvl > 50){
			PPet->setModifier(MOD_MATT, 75);
		}
		else if (plvl > 25){
			PPet->setModifier(MOD_MATT, 35);
		}
		else {
			PPet->setModifier(MOD_MATT, 15);
		}

		if (finalize) {
			FinalizePetStatistics(PMaster, PPet);
		}
	}

	void FinalizePetStatistics(CBattleEntity* PMaster, CPetEntity* PPet) {
		//set C magic evasion
		PPet->setModifier(MOD_MEVA, battleutils::GetMaxSkill(SKILL_ELE, JOB_RDM, PPet->GetMLevel()));
		PPet->health.tp = 0;
		PPet->UpdateHealth();

		PMaster->applyPetModifiers(PPet);
	}

}; // namespace petutils
