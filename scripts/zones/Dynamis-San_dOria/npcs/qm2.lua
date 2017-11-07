-----------------------------------
-- Area:  Dynamis San d'Oria
-- NPC:   qm2 (???)
-- Notes: Spawns Fugbrag and Kratzvatz
-- Uses Chapter 3 Tome and Odious Leather
-----------------------------------
package.loaded["scripts/zones/Dynamis-San_dOria/TextIDs"] = nil;
-----------------------------------

require("scripts/globals/keyitems");
require("scripts/zones/Dynamis-San_dOria/TextIDs");

-----------------------------------
-- onTrade
-----------------------------------

function onTrade(player,npc,trade)
if (GetMobAction(17534979) == 0 and trade:hasItemQty(3353,1) and trade:hasItemQty(3853,3)) then
	player:setVar("DynaWeakener",3);
	SpawnMob(17534979):updateClaim(player);
elseif (GetMobAction(17534979) == 0 and trade:hasItemQty(3353,1) and trade:hasItemQty(3853,2)) then
	player:setVar("DynaWeakener",2);
	SpawnMob(17534979):updateClaim(player);
elseif (GetMobAction(17534979) == 0 and trade:hasItemQty(3353,1) and trade:hasItemQty(3853,1)) then
	player:setVar("DynaWeakener",1);
	SpawnMob(17534979):updateClaim(player);
elseif (GetMobAction(17534979) == 0 and trade:hasItemQty(3353,1)) then
player:setVar("DynaWeakener",0);
	SpawnMob(17534979):updateClaim(player);
end	

----- Need to replace secondary NPC ID below

if (GetMobAction(17535208) == 0 and trade:hasItemQty(3406,1) and trade:hasItemQty(3853,3)) then
	player:setVar("DynaWeakener",3);
	SpawnMob(17535208):updateClaim(player);
elseif (GetMobAction(17535208) == 0 and trade:hasItemQty(3406,1) and trade:hasItemQty(3853,2)) then
	player:setVar("DynaWeakener",2);
	SpawnMob(17535208):updateClaim(player);
elseif (GetMobAction(17535208) == 0 and trade:hasItemQty(3406,1) and trade:hasItemQty(3853,1)) then
	player:setVar("DynaWeakener",1);
	SpawnMob(17535208):updateClaim(player);
elseif (GetMobAction(17535208) == 0 and trade:hasItemQty(3406,1)) then
player:setVar("DynaWeakener",0);
	SpawnMob(17535208):updateClaim(player);
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