-----------------------------------
-- Area: Windurst Walls
-- NPC:  Trail Markings
-- Dynamis-Windurst Enter
-- @pos -216 0 -94 239
-----------------------------------
package.loaded["scripts/zones/Windurst_Walls/TextIDs"] = nil;
-----------------------------------

require("scripts/globals/settings");
require("scripts/globals/keyitems");
-- require("scripts/globals/dynamis");
require("scripts/zones/Windurst_Walls/TextIDs");

-----------------------------------
-- onTrade Action
-----------------------------------

function onTrade(player,npc,trade)
end; 

-----------------------------------
-- onTrigger Action
-----------------------------------

function onTrigger(player,npc)
	
	if (player:getVar("Dynamis_Status") == 1) then
		player:startEvent(0x01c7); -- cs with Cornelia
	elseif (player:getVar("DynaWindurst_Win") == 1) then
		player:startEvent(0x01d1,HYDRA_CORPS_LANTERN); -- Win CS
	elseif (player:hasKeyItem(VIAL_OF_SHROUDED_SAND)) then
		local firstDyna = 0;
		local realDay = os.time();
		local dynaWaitxDay = player:getVar("dynaWaitxDay");
		local dynatime = player:getVar("Dynamis_Reenter");
		
		if (checkFirstDyna(player,3)) then  -- First Dyna-Windurst => CS
			firstDyna = 1; 
		end
		
		if (player:getMainLvl() < DYNA_LEVEL_MIN) then
			player:messageSpecial(PLAYERS_HAVE_NOT_REACHED_LEVEL,DYNA_LEVEL_MIN);
		-- elseif ((dynaWaitxDay + (BETWEEN_2DYNA_WAIT_TIME * 24 * 60 * 60)) < realDay or player:getVar("DynamisID") == GetServerVariable("[DynaSandoria]UniqueID")) then
		   elseif (dynatime < realDay) then
			player:startEvent(0x01c4,3,firstDyna,0,BETWEEN_2DYNA_WAIT_TIME,64,VIAL_OF_SHROUDED_SAND,4236,4237);
		else
			dayRemaining = math.floor((dynatime - realDay)/3456);
			player:messageSpecial(YOU_CANNOT_ENTER_DYNAMIS,dayRemaining,3);
		end
	else
		player:messageSpecial(STRANDS_OF_GRASS_HERE);
	end
	
end;

-----------------------------------
-- onEventUpdate
-----------------------------------

function onEventUpdate(player,csid,option)
--printf("CSID: %u",csid);
--printf("updateRESULT: %u",option);
end;

-----------------------------------
-- onEventFinish
-----------------------------------

function onEventFinish(player,csid,option)
--printf("CSID: %u",csid);
-- printf("finishRESULT: %u",option);
	
	if (csid == 0x01c7) then
		player:addKeyItem(VIAL_OF_SHROUDED_SAND);
		player:messageSpecial(KEYITEM_OBTAINED,VIAL_OF_SHROUDED_SAND);
		player:setVar("Dynamis_Status",0);
	elseif (csid == 0x01d1) then
		player:setVar("DynaWindurst_Win",0);
	elseif (csid == 0x01c4 and option == 0) then
		if (checkFirstDyna(player,3)) then
			player:setVar("Dynamis_Status",player:getVar("Dynamis_Status") + 8);
		end
		-- Store information about Key Items Gained and relay re-enter time to variable
		
		
		local d1 = 0;
		local d2 = 0;
		local d3 = 0;
		local d4 = 0;
		local d5 = 0;
		local d6 = 0;
		local reduction = 0;
		local reentertime = 43200;
	
	
		if (player:hasKeyItem(HYDRA_CORPS_COMMAND_SCEPTER)) then
		d1 = 1;
		end
		if (player:hasKeyItem(HYDRA_CORPS_EYEGLASS)) then
		d2 = 1;
		end
		if (player:hasKeyItem(HYDRA_CORPS_LANTERN)) then
		d3 = 1;
		end
		if (player:hasKeyItem(HYDRA_CORPS_TACTICAL_MAP)) then
		d4 = 1;
		end
		if (player:hasKeyItem(HYDRA_CORPS_INSIGNIA)) then
		d5 = 1;
		end
		if (player:hasKeyItem(HYDRA_CORPS_BATTLE_STANDARD)) then	
		d6 = 1;
		end
	
		reduction = d1 + d2 + d3 + d4 + d5 + d6;
	
		if (reduction == 1) then
		reentertime = 28800;
		player:setVar("Dyna_KI_Counter",1);
		elseif (reduction == 2) then	
		reentertime = 21600;
		player:setVar("Dyna_KI_Counter",2);			
		elseif (reduction == 3) then
		reentertime = 14400;
		player:setVar("Dyna_KI_Counter",3);			
		elseif (reduction == 4) then
		reentertime = 10800;
		player:setVar("Dyna_KI_Counter",4);			
		elseif (reduction == 5) then
		reentertime = 7200;
		player:setVar("Dyna_KI_Counter",5);			
		elseif (reduction == 6) then
		reentertime = 0;
		player:setVar("Dyna_KI_Counter",6);			
		else
		reentertime = 43200;
		player:setVar("Dyna_KI_Counter",0);			
		end
		
		player:setVar("Dyna_Reduction", reentertime);
	
			
				
		player:setPos(-221.988,1.000,-120.184,0,0xbb);
	end
	
end;