-----------------------------------
-- Area:  Dynamis Bastok
-- NPC:   qm2 (???)
-- Notes: Darkfound and Noondozer
-----------------------------------
package.loaded["scripts/zones/Dynamis-Bastok/TextIDs"] = nil;
-----------------------------------

require("scripts/globals/keyitems");
require("scripts/zones/Dynamis-Bastok/TextIDs");

-----------------------------------
-- onTrade
-----------------------------------

function onTrade(player,npc,trade)
	
if (GetMobAction(17539308) == 0 and trade:hasItemQty(3354,1) and trade:hasItemQty(3853,3)) then
	player:setVar("DynaWeakener",3);
	SpawnMob(17539308):updateClaim(player);
elseif (GetMobAction(17539308) == 0 and trade:hasItemQty(3354,1) and trade:hasItemQty(3853,2)) then
	player:setVar("DynaWeakener",2);
	SpawnMob(17539308):updateClaim(player);
elseif (GetMobAction(17539308) == 0 and trade:hasItemQty(3354,1) and trade:hasItemQty(3853,1)) then
	player:setVar("DynaWeakener",1);
	SpawnMob(17539308):updateClaim(player);
elseif (GetMobAction(17539308) == 0 and trade:hasItemQty(3354,1)) then
player:setVar("DynaWeakener",0);
	SpawnMob(17539308):updateClaim(player);
end	


if (GetMobAction(17539143) == 0 and trade:hasItemQty(3411,1) and trade:hasItemQty(3853,3)) then
	player:setVar("DynaWeakener",3);
	SpawnMob(17539143):updateClaim(player);
elseif (GetMobAction(17539143) == 0 and trade:hasItemQty(3411,1) and trade:hasItemQty(3853,2)) then
	player:setVar("DynaWeakener",2);
	SpawnMob(17539143):updateClaim(player);
elseif (GetMobAction(17539143) == 0 and trade:hasItemQty(3411,1) and trade:hasItemQty(3853,1)) then
	player:setVar("DynaWeakener",1);
	SpawnMob(17539143):updateClaim(player);
elseif (GetMobAction(17539143) == 0 and trade:hasItemQty(3411,1)) then
player:setVar("DynaWeakener",0);
	SpawnMob(17539143):updateClaim(player);
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