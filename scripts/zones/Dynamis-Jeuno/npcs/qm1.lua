-----------------------------------
-- Area: Dynamis Jeuno
-- NPC:  Spawns Goatmug and Hexhands
-----------------------------------
package.loaded["scripts/zones/Dynamis-Jeuno/TextIDs"] = nil;
-----------------------------------

require("scripts/globals/keyitems");
require("scripts/zones/Dynamis-Jeuno/TextIDs");

-----------------------------------
-- onTrade
-----------------------------------

function onTrade(player,npc,trade)

local qm1 = GetNPCByID(17547510);
-- Goatmug
if (GetMobAction(17547396) == 0 and trade:hasItemQty(3356,1) and trade:hasItemQty(3853,3)) then
	player:setVar("DynaWeakener",3);
	SpawnMob(17547396):updateClaim(player);
    qm1:setStatus(STATUS_DISAPPEAR);
elseif (GetMobAction(17547396) == 0 and trade:hasItemQty(3356,1) and trade:hasItemQty(3853,2)) then
	player:setVar("DynaWeakener",2);
	SpawnMob(17547396):updateClaim(player);
	qm1:setStatus(STATUS_DISAPPEAR);
elseif (GetMobAction(17547396) == 0 and trade:hasItemQty(3356,1) and trade:hasItemQty(3853,1)) then
	player:setVar("DynaWeakener",1);
	SpawnMob(17547396):updateClaim(player);
	qm1:setStatus(STATUS_DISAPPEAR);
elseif (GetMobAction(17547396) == 0 and trade:hasItemQty(3356,1)) then
player:setVar("DynaWeakener",0);
	SpawnMob(17547396):updateClaim(player);
	qm1:setStatus(STATUS_DISAPPEAR);
end	

-- Hexhands

if (GetMobAction(17547493) == 0 and trade:hasItemQty(3420,1) and trade:hasItemQty(3853,3)) then
	player:setVar("DynaWeakener",3);
	SpawnMob(17547493):updateClaim(player);
	qm1:setStatus(STATUS_DISAPPEAR);
elseif (GetMobAction(17547493) == 0 and trade:hasItemQty(3420,1) and trade:hasItemQty(3853,2)) then
	player:setVar("DynaWeakener",2);
	SpawnMob(17547493):updateClaim(player);
	qm1:setStatus(STATUS_DISAPPEAR);
elseif (GetMobAction(17547493) == 0 and trade:hasItemQty(3420,1) and trade:hasItemQty(3853,1)) then
	player:setVar("DynaWeakener",1);
	SpawnMob(17547493):updateClaim(player);
	qm1:setStatus(STATUS_DISAPPEAR);
elseif (GetMobAction(17547493) == 0 and trade:hasItemQty(3420,1)) then
player:setVar("DynaWeakener",0);
	SpawnMob(17547493):updateClaim(player);
	qm1:setStatus(STATUS_DISAPPEAR);
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