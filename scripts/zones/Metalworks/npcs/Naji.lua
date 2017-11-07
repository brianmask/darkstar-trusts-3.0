-----------------------------------
-- Area: Metalworks
-- NPC:  Naji
-- Involved in Quests: The doorman (finish), Riding on the Clouds 
-- Involved in Mission: Bastok 6-2
-- @pos 64 -14 -4 237
-----------------------------------
package.loaded["scripts/zones/Metalworks/TextIDs"] = nil;
package.loaded["scripts/globals/settings"] = nil;
-----------------------------------

require("scripts/globals/settings");
require("scripts/globals/keyitems");
require("scripts/globals/titles");
require("scripts/globals/quests");
require("scripts/globals/missions");
require("scripts/zones/Metalworks/TextIDs");

-----------------------------------
-- onTrade Action
-----------------------------------

function onTrade(player,npc,trade)
    local currentTokens = player:getVar("CurrentTokens_Naji"); 	
	if(player:getQuestStatus(JEUNO,RIDING_ON_THE_CLOUDS) == QUEST_ACCEPTED and player:getVar("ridingOnTheClouds_2") == 6) then
		if(trade:hasItemQty(1127,1) and trade:getItemCount() == 1) then -- Trade Kindred seal
			player:setVar("ridingOnTheClouds_2",0);
			player:tradeComplete();
			player:addKeyItem(SMILING_STONE);
			player:messageSpecial(KEYITEM_OBTAINED,SMILING_STONE);
		end
	end
	
        if ((player:getVar("NAJI_TRIB_FIGHT") == 3) and (player:getVar("TributeRank_Naji") == 0) and (trade:hasItemQty(65535, 1000))) then
	      player:PrintToPlayer("Naji : Thank you for your Tribute.",0x0D);
          player:PrintToPlayer("Naji's Attack is raised by 5 points! (Total: 5)", 0x15);
		  player:setVar("TrustAtt_Naji",5);
		  player:setVar("TributeRank_Naji",1);
	      currentTokens = currentTokens - 1;		  
        elseif ((player:getVar("NAJI_TRIB_FIGHT") == 3) and (player:getVar("TributeRank_Naji") == 1) and (trade:hasItemQty(65535, 2000))) then
	      player:PrintToPlayer("Naji : Thank you for your Tribute.",0x0D);
          player:PrintToPlayer("Naji's Accuracy is raised by 5 points! (Total: 5)", 0x15);
		  player:setVar("TrustAcc_Naji",5);	
		  player:setVar("TributeRank_Naji",2);
		  currentTokens = currentTokens - 2;
        elseif ((player:getVar("NAJI_TRIB_FIGHT") == 3) and (player:getVar("TributeRank_Naji") == 2) and (trade:hasItemQty(65535, 3000))) then
	      player:PrintToPlayer("Naji : Thank you for your Tribute.",0x0D);
          player:PrintToPlayer("Naji's Double Attack is raised by 2 points! (Total: 2%)", 0x15);
		  player:setVar("TrustDA_Naji",2);	
		  player:setVar("TributeRank_Naji",3);
		  currentTokens = currentTokens - 3;
        elseif ((player:getVar("NAJI_TRIB_FIGHT") == 3) and (player:getVar("TributeRank_Naji") == 3) and (trade:hasItemQty(65535, 4000))) then
	      player:PrintToPlayer("Naji : Thank you for your Tribute.",0x0D);
          player:PrintToPlayer("Naji's Attack is raised by 5 points! (Total: 10)", 0x15);
		  player:setVar("TrustAtt_Naji",10);	
		  player:setVar("TributeRank_Naji",4);
		  currentTokens = currentTokens - 4;
        elseif ((player:getVar("NAJI_TRIB_FIGHT") == 3) and (player:getVar("TributeRank_Naji") == 4) and (trade:hasItemQty(65535, 5000))) then
	      player:PrintToPlayer("Naji : Thank you for your Tribute.",0x0D);
          player:PrintToPlayer("Naji's Accuracy is raised by 5 points! (Total: 10)", 0x15);
		  player:setVar("TrustAcc_Naji",10);	
		  player:setVar("TributeRank_Naji",5);
		  currentTokens = currentTokens - 5;
        elseif ((player:getVar("NAJI_TRIB_FIGHT") == 3) and (player:getVar("TributeRank_Naji") == 5) and (trade:hasItemQty(65535, 10000))) then
	      player:PrintToPlayer("Naji : Thank you for your Tribute.",0x0D);
          player:PrintToPlayer("Naji's Double Attack is raised by 3 points! (Total: 5%)", 0x15);
		  player:setVar("TrustDA_Naji",5);	
		  player:setVar("TributeRank_Naji",6);
		  currentTokens = currentTokens - 10;
        elseif ((player:getVar("NAJI_TRIB_FIGHT") == 3) and (player:getVar("TributeRank_Naji") == 6) and (trade:hasItemQty(65535, 15000))) then
	      player:PrintToPlayer("Naji : Thank you for your Tribute.",0x0D);
          player:PrintToPlayer("Naji's Attack is raised by 5 points! (Total: 15)", 0x15);
		  player:setVar("TrustAtt_Naji",15);	
		  player:setVar("TributeRank_Naji",7);
		  currentTokens = currentTokens - 15;
        elseif ((player:getVar("NAJI_TRIB_FIGHT") == 3) and (player:getVar("TributeRank_Naji") == 7) and (trade:hasItemQty(65535, 30000))) then
	      player:PrintToPlayer("Naji : Thank you for your Tribute.",0x0D);
          player:PrintToPlayer("Naji's Accuracy is raised by 5 points! (Total: 15)", 0x15);
		  player:setVar("TrustAcc_Naji",15);	
		  player:setVar("TributeRank_Naji",8);
		  currentTokens = currentTokens - 20;
        elseif ((player:getVar("NAJI_TRIB_FIGHT") == 3) and (player:getVar("TributeRank_Naji") == 8) and (trade:hasItemQty(65535, 75000))) then
	      player:PrintToPlayer("Naji : Thank you for your Tribute.",0x0D);
          player:PrintToPlayer("Naji's Crit. Hit Rate is raised by 3%! (Total: 3%)", 0x15);
		  player:setVar("TrustCrit_Naji",3);	
		  player:setVar("TributeRank_Naji",9);	
          currentTokens = currentTokens - 30;		  
        elseif ((player:getVar("NAJI_TRIB_FIGHT") == 3) and (player:getVar("TributeRank_Naji") == 9) and (trade:hasItemQty(65535, 150000))) then
	      player:PrintToPlayer("Naji : Thank you for your Tribute.",0x0D);
          player:PrintToPlayer("Naji's Berserk Effect is raised by 1%! (Total: 1%)", 0x15);
		  player:setVar("TrustBerserk_Naji",1);	
		  player:setVar("TributeRank_Naji",10);
		  currentTokens = currentTokens - 35;
		end	
	
end;

-----------------------------------
-- onTrigger Action
-----------------------------------

function onTrigger(player,npc)
    local srank = player:getRank();
	local wrank = player:getRank();
	local brank = player:getRank();
	
	if (srank >= 4) and (player:hasKeyItem(RED_INSTITUTE_CARD)) and (player:hasSpell(897) == false) then  -- Sandy Rnak 4 or higher
	player:PrintToPlayer("Your Red Institute Card flashes brilliantly!", 0x1C);
    player:PrintToPlayer("Naji : Ah a Red Institute Card.  From now on, you can summon me to help you with your battles", 0xD);
    player:addSpell(897);	
	end
	if (wrank >= 4) and (player:hasKeyItem(GREEN_INSTITUTE_CARD)) and (player:hasSpell(897) == false) then  -- Sandy Rnak 4 or higher
	player:PrintToPlayer("Your Green Institute Card flashes brilliantly!", 0x1C);
    player:PrintToPlayer("Naji : Ah a Green Institute Card.  From now on, you can summon me to help you with your battles", 0xD);
    player:addSpell(897);
	end
	if (brank >= 4) and (player:hasKeyItem(BLUE_INSTITUTE_CARD)) and (player:hasSpell(897) == false) then  -- Sandy Rnak 4 or higher
	player:PrintToPlayer("Your Green Institute Card flashes brilliantly!", 0x1C);
    player:PrintToPlayer("Naji : Ah a Blue Institute Card.  From now on, you can summon me to help you with your battles", 0xD);
    player:addSpell(897);
	end




	
	if(player:hasKeyItem(YASINS_SWORD)) then -- The Doorman, WAR AF1
		player:startEvent(0x02ee);
	elseif(player:getCurrentMission(BASTOK) ~= 255) then
		local currentMission = player:getCurrentMission(BASTOK);
		
		if(currentMission == THE_ZERUHN_REPORT and player:hasKeyItem(ZERUHN_REPORT)) then
			if(player:seenKeyItem(ZERUHN_REPORT)) then
				player:startEvent(0x02C6,0);
			else
				player:startEvent(0x02C6,1);
			end
		elseif(currentMission == THE_CRYSTAL_LINE and player:hasKeyItem(C_L_REPORTS)) then
			player:startEvent(0x02c7);
		elseif(currentMission == THE_EMISSARY and player:hasKeyItem(KINDRED_REPORT)) then
			player:startEvent(0x02ca);
		elseif(currentMission == THE_EMISSARY) then
			if(player:hasKeyItem(LETTER_TO_THE_CONSULS_BASTOK) == false and player:getVar("MissionStatus") == 0) then
				player:startEvent(0x02c9);
			else
				player:showText(npc,GOOD_LUCK);
			end
		elseif(player:hasKeyItem(MESSAGE_TO_JEUNO_BASTOK) and player:getVar("MissionStatus") == 0) then
			player:startEvent(0x02d0);
		elseif(currentMission == DARKNESS_RISING and player:getVar("MissionStatus") == 1) then
			player:startEvent(0x02d1);
		elseif(player:hasKeyItem(BURNT_SEAL)) then
			player:startEvent(0x02d2);
		elseif(currentMission == THE_PIRATE_S_COVE and player:getVar("MissionStatus") == 0) then
			player:startEvent(0x02f9);
		elseif(currentMission == THE_PIRATE_S_COVE and player:getVar("MissionStatus") == 3) then
			player:startEvent(0x02fa);
		else
			player:startEvent(0x02bc);
		end
	elseif(player:hasKeyItem(YASINS_SWORD)) then -- The Doorman
		player:startEvent(0x02ee);
	else
		player:startEvent(0x02bc);
	end

	    ------- Trust Fight --------
	if ((mainlvl >= 71 and tribfight == 0 and (player:hasSpell(897)) and (player:getVar("FerretoryAura") >= 7) and (player:getVar("TRIB_FIGHT") ~= 1))) then
	player:PrintToPlayer("Naji : There is someone running around claming to me at Waughroon Shrine.  Please head there and I'll join you.", 0xD);
    player:PrintToPlayer("Naji : When you are ready, examine the Burning Circle in Waughroon Shrine and call me to your side.", 0xD);
	player:setVar("NAJI_TRIB_FIGHT",1);
    player:setVar("TRIB_FIGHT",1);	
	elseif (mainlvl >= 75 and tribfight == 2 and (player:hasSpell(897))) then
	player:PrintToPlayer("Naji : You have done well to help with the imposter investigation.  I am in your debt.", 0xD);
	player:PrintToPlayer("You are now able to collect Trust Tokens for Naji!", 0x15);
	player:setVar("NAJI_TRIB_FIGHT",3);
    player:setVar("TRIB_FIGHT",0);	
	end	
	
-- Handle Token Quest
  if ((player:getVar("NAJI_TRIB_FIGHT") == 3) and (player:getVar("TributeRank_Naji") == 0)) then
    player:PrintToPlayer("Naji : Bring me 1 of my Trust Tokens and 1,000 gil to raise my Attack by 5",0x0D);
  elseif ((player:getVar("NAJI_TRIB_FIGHT") == 3) and (player:getVar("TributeRank_Naji") == 1)) then
    player:PrintToPlayer("Naji : Bring me 2 of my Trust Tokens and 2,000 gil to raise my Accuracy by 5",0x0D);
  elseif ((player:getVar("NAJI_TRIB_FIGHT") == 3) and (player:getVar("TributeRank_Naji") == 2)) then
    player:PrintToPlayer("Naji : Bring me 3 of my Trust Tokens and 3,000 gil to raise my Double Attack by 2%",0x0D);
  elseif ((player:getVar("NAJI_TRIB_FIGHT") == 3) and (player:getVar("TributeRank_Naji") == 3)) then
    player:PrintToPlayer("Naji : Bring me 4 of my Trust Tokens and 4,000 gil to raise my Attack by 5",0x0D);
  elseif ((player:getVar("NAJI_TRIB_FIGHT") == 3) and (player:getVar("TributeRank_Naji") == 4)) then
    player:PrintToPlayer("Naji : Bring me 5 of my Trust Tokens and 5,000 gil to raise my Accuracy by 5",0x0D);
  elseif ((player:getVar("NAJI_TRIB_FIGHT") == 3) and (player:getVar("TributeRank_Naji") == 5)) then
    player:PrintToPlayer("Naji : Bring me 10 of my Trust Tokens and 10,000 gil to raise my Double Attack by 3%",0x0D);
  elseif ((player:getVar("NAJI_TRIB_FIGHT") == 3) and (player:getVar("TributeRank_Naji") == 6)) then
    player:PrintToPlayer("Naji : Bring me 15 of my Trust Tokens and 15,000 gil to raise my Attack by 5",0x0D);
  elseif ((player:getVar("NAJI_TRIB_FIGHT") == 3) and (player:getVar("TributeRank_Naji") == 7)) then
    player:PrintToPlayer("Naji : Bring me 20 of my Trust Tokens and 30,000 gil to raise my Accuracy by 5",0x0D);
  elseif ((player:getVar("NAJI_TRIB_FIGHT") == 3) and (player:getVar("TributeRank_Naji") == 8)) then
    player:PrintToPlayer("Naji : Bring me 30 of my Trust Tokens and 75,000 gil so I can raise my Critical Hitrate by 3%'",0x0D);
  elseif ((player:getVar("NAJI_TRIB_FIGHT") == 3) and (player:getVar("TributeRank_Naji") == 9)) then
    player:PrintToPlayer("Naji : Bring me 35 of my Trust Tokens and 150,000 gil to raise my Berserk by 1%",0x0D);
  end		
	
end;

-- 0x02c6  0x02c7  0x02bc  0x02c9  0x02ca  0x02cb  0x02cd  0x02d0  0x02d1  0x02ee  0x03f0  0x03f1  0x02f9
-- 0x02fa  0x030e  0x0325  0x034d  0x036d  0x03aa  0x03ab  0x03ac  0x03ad  0x03ae  0x03cb  0x03c9  0x03ca

-----------------------------------
-- onEventUpdate
-----------------------------------

function onEventUpdate(player,csid,option)
--printf("CSID: %u",csid);
--printf("RESULT: %u",option);
end;

-----------------------------------
-- onEventFinish
-----------------------------------
function onEventFinish(player,csid,option)
--printf("CSID: %u",csid);
--printf("RESULT: %u",option);

	if(csid == 0x02ee) then
		if(player:getFreeSlotsCount(0) >= 1) then
			player:addItem(18214, 1, 45, 7);
			player:messageSpecial(ITEM_OBTAINED, 18214); -- Razor Axe
			player:delKeyItem(YASINS_SWORD);
			player:setVar("theDoormanCS",0);
			player:addFame(BASTOK,BAS_FAME*30);
			player:completeQuest(BASTOK,THE_DOORMAN);
		else
           player:messageSpecial(ITEM_CANNOT_BE_OBTAINED, 18214); -- Razor Axe
	    end
	elseif(csid == 0x02C6) then
		player:delKeyItem(ZERUHN_REPORT);
		player:completeMission(BASTOK,THE_ZERUHN_REPORT);
	elseif(csid == 0x02c9) then
		player:addKeyItem(LETTER_TO_THE_CONSULS_BASTOK);
		player:messageSpecial(KEYITEM_OBTAINED,LETTER_TO_THE_CONSULS_BASTOK);
		player:setVar("MissionStatus",1);
	elseif(csid == 0x02d0 and option == 0 or csid == 0x02d1) then
		player:delKeyItem(MESSAGE_TO_JEUNO_BASTOK);
		player:addKeyItem(NEW_FEIYIN_SEAL);
		player:messageSpecial(KEYITEM_OBTAINED,NEW_FEIYIN_SEAL);
		player:setVar("MissionStatus",10);
	elseif(csid == 0x02d0 and option == 1) then
		player:delKeyItem(MESSAGE_TO_JEUNO_BASTOK);
		player:setVar("MissionStatus",1);
	elseif(csid == 0x02f9) then
		player:setVar("MissionStatus",1);
	elseif(csid == 0x02ca or csid == 0x02d2 or csid == 0x02fa) then
		finishMissionTimeline(player,1,csid,option);
	end
	
end;