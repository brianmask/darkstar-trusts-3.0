-----------------------------------
-- Area:  Dynamis San d'Oria
-- NPC:   qm1 (???)
-- Notes: Spawns  Gadgqok and Rokgevok
-- Uses Chapter 2 Tome and Odious Scale
-----------------------------------
package.loaded["scripts/zones/Dynamis-San_dOria/TextIDs"] = nil;
-----------------------------------

require("scripts/globals/keyitems");
require("scripts/zones/Dynamis-San_dOria/TextIDs");

-----------------------------------
-- onTrade
-----------------------------------

function onTrade(player,npc,trade)
if (GetMobAction(17535006) == 0 and trade:hasItemQty(3353,1) and trade:hasItemQty(3853,3)) then
	player:setVar("DynaWeakener",3);
	SpawnMob(17535006):updateClaim(player);
elseif (GetMobAction(17535006) == 0 and trade:hasItemQty(3353,1) and trade:hasItemQty(3853,2)) then
	player:setVar("DynaWeakener",2);
	SpawnMob(17535006):updateClaim(player);
elseif (GetMobAction(17535006) == 0 and trade:hasItemQty(3353,1) and trade:hasItemQty(3853,1)) then
	player:setVar("DynaWeakener",1);
	SpawnMob(17535006):updateClaim(player);
elseif (GetMobAction(17535006) == 0 and trade:hasItemQty(3353,1)) then
player:setVar("DynaWeakener",0);
	SpawnMob(17535006):updateClaim(player);
end	



if (GetMobAction(17535207) == 0 and trade:hasItemQty(3405,1) and trade:hasItemQty(3853,3)) then
	player:setVar("DynaWeakener",3);
	SpawnMob(17535207):updateClaim(player);
elseif (GetMobAction(17535207) == 0 and trade:hasItemQty(3405,1) and trade:hasItemQty(3853,2)) then
	player:setVar("DynaWeakener",2);
	SpawnMob(17535207):updateClaim(player);
elseif (GetMobAction(17535207) == 0 and trade:hasItemQty(3405,1) and trade:hasItemQty(3853,1)) then
	player:setVar("DynaWeakener",1);
	SpawnMob(17535207):updateClaim(player);
elseif (GetMobAction(17535207) == 0 and trade:hasItemQty(3405,1)) then
player:setVar("DynaWeakener",0);
	SpawnMob(17535207):updateClaim(player);
end

end;

-----------------------------------
-- onTrigger
-----------------------------------

function onTrigger(player,npc)
	
		player:messageSpecial(NOTHING_OUT_OF_ORDINARY);		
	
end;

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
end;