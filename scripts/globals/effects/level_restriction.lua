-----------------------------------
--  
-- 	EFFECT_LEVEL_RESTRICTION
-- 	
-----------------------------------
require("scripts/globals/keyitems");
-----------------------------------
-- onEffectGain Action
-----------------------------------

function onEffectGain(target,effect)
	target:levelRestriction(effect:getPower());
	local zone = target:getZoneID()
	if (target:getObjType() == PC) and (zone == 188) then
	target:PrintToPlayer("You have entered Dynamis");
	end
end;

-----------------------------------
-- onEffectTick Action
-----------------------------------

function onEffectTick(target,effect)
 local duration = target:getStatusEffect(EFFECT_LEVEL_RESTRICTION):getTimeRemaining();
 local zone = target:getZoneID();
 target:PrintToPlayer("Tick");
 
  
 
	local seconds = math.floor(duration / 1000);
if (target:getObjType() == TYPE_PC) and (zone == 185) or (zone == 186) or (zone == 187) or (zone == 188) or (zone == 134) or (zone == 135) then	
    if (target:getVar("Dynamis_Time_Remaining") < 5400000 and (target:getVar("Dynamis_Time_Remaining") > 5396000)) then
		local reduction = target:getVar("Dyna_KI_Counter");
		target:PrintToPlayer("You have 90 minutes remaining in Dynamis", 0xD);
		if (reduction == 0) then
			target:PrintToPlayer("You may re-enter Dynamis in 12 Hours", 0xD);	
		elseif (reduction == 1) then
			target:PrintToPlayer("Due to certain Key-Items, you may re-enter Dynamis in 8 Earth Hours.", 0xD);
		elseif (reduction == 2) then
			target:PrintToPlayer("Due to certain Key-Items, you may re-enter Dynamis in 6 Earth Hours.", 0xD);
		elseif (reduction == 3) then
			target:PrintToPlayer("Due to certain Key-Items, you may re-enter Dynamis in 4 Earth Hours.", 0xD);
		elseif (reduction == 4) then
			target:PrintToPlayer("Due to certain Key-Items, you may re-enter Dynamis in 3 Earth Hours.", 0xD);
		elseif (reduction == 5) then
			target:PrintToPlayer("Due to certain Key-Items, you may re-enter Dynamis in 2 Earth Hours.", 0xD);
		elseif (reduction == 6) then
			target:PrintToPlayer("Defeating all areas of Dynamis has granted you immediate re-entry.", 0xD);
		end	
	end

	if (target:getVar("Dynamis_Time_Remaining") > 180000) then
	    target:setVar("Dynamis_Time_Remaining",duration);
		-- target:PrintToPlayer(duration);
	end	
	if (target:getVar("Dynamis_Time_Remaining") < 9000) then
	    target:setVar("Dynamis_Time_Remaining",duration);
        target:PrintToPlayer("You have "..seconds.." second(s) remaining in Dynamis", 0xD);
	end	
	    if (target:getVar("Dynamis_Time_Remaining") < 30000 and (target:getVar("Dynamis_Time_Remaining") > 27500)) then
	    target:setVar("Dynamis_Time_Remaining",duration);
        target:PrintToPlayer("You have 30 seconds remaining in Dynamis", 0xD);
	end	
    if (target:getVar("Dynamis_Time_Remaining") < 180000 and (target:getVar("Dynamis_Time_Remaining") > 177500)) then
	    target:setVar("Dynamis_Time_Remaining",duration);
        target:PrintToPlayer("You have 3 minutes remaining in Dynamis", 0xD);
	end	
	if (target:getVar("Dynamis_Time_Remaining") <= 177000) then
	    target:setVar("Dynamis_Time_Remaining",duration);
       -- target:PrintToPlayer(duration);
	end	
end	


if (target:getObjType() == TYPE_PC) and (zone == 86) then
   if (duration < 180000) and (duration > 177500) then
   target:PrintToPlayer("You have 3 minutes remaining until you are ejected", 0xD);
   elseif (duration < 30000) and (duration > 27500) then
   target:PrintToPlayer("You have 30 seconds remaining until you are ejected", 0xD);
   end
end  

 
if (target:getObjType() == TYPE_PC) and (zone == 73) or (zone == 74) or (zone == 75) or (zone == 76) then	
	if (seconds < 1800) and (seconds > 1796) then
		target:PrintToPlayer("You have 30 minutes to complete your objective", 0xD);
	end
	if (seconds < 900) and (seconds > 896) then
	    target:PrintToPlayer("You have 15 minutes remaining until you are ejected", 0xD);
		-- target:PrintToPlayer(duration);
	end	


end






end;

-----------------------------------
-- onEffectLose Action
-----------------------------------

function onEffectLose(target,effect)
    local zone = target:getZoneID();
	target:levelRestriction(0);
	local chest = target:getVar("IxionChest");
	
	if (zone == 185) then
	target:setVar("ScyldMultiplier",0);
	target:delKeyItem(ALABASTER_GRANULES_OF_TIME);
    target:delKeyItem(CRIMSON_GRANULES_OF_TIME);
    target:delKeyItem(AZURE_GRANULES_OF_TIME);
    target:delKeyItem(AMBER_GRANULES_OF_TIME);
	target:setPos(161.000,-2.000,161.000,94,0xE6); -- Sandy
	elseif (zone == 186) then
	target:setVar("ScyldMultiplier",0);	
	target:delKeyItem(ALABASTER_GRANULES_OF_TIME);
    target:delKeyItem(CRIMSON_GRANULES_OF_TIME);
    target:delKeyItem(AZURE_GRANULES_OF_TIME);
    target:delKeyItem(AMBER_GRANULES_OF_TIME);	
	target:setPos(112.000,0.994,-72.000,127,0xEA); -- Bastok
	elseif (zone == 187) then
	target:setVar("ScyldMultiplier",0);
	target:delKeyItem(ALABASTER_GRANULES_OF_TIME);
    target:delKeyItem(CRIMSON_GRANULES_OF_TIME);
    target:delKeyItem(AZURE_GRANULES_OF_TIME);
    target:delKeyItem(AMBER_GRANULES_OF_TIME);	
	target:setPos(-217.000,1.000,-119.000,94,0xEF);  -- Windurst
	elseif (zone == 188) then
	target:setVar("ScyldMultiplier",0);
	target:delKeyItem(ALABASTER_GRANULES_OF_TIME);
    target:delKeyItem(CRIMSON_GRANULES_OF_TIME);
    target:delKeyItem(AZURE_GRANULES_OF_TIME);
    target:delKeyItem(AMBER_GRANULES_OF_TIME);	
	target:setPos(34.945,9.000,-49.667,111,0xF3);  -- Warp Back to Ru Lude Gardens
	elseif (zone == 134) then
	target:setVar("ScyldMultiplier",0);
	target:delKeyItem(ALABASTER_GRANULES_OF_TIME);
    target:delKeyItem(CRIMSON_GRANULES_OF_TIME);
    target:delKeyItem(AZURE_GRANULES_OF_TIME);
    target:delKeyItem(AMBER_GRANULES_OF_TIME);	
	target:setPos(-284.751,-39.923,-422.948,235,0x6F);  -- Warp Back to Glacier	
	elseif (zone == 86) and (chest == 1) then
	target:setVar("IxionChest",0);	
	target:setPos(-319,0,523,94,0x62); -- S Campaign [S]
	elseif (zone == 86) and (chest == 2) then
	target:setVar("IxionChest",0);
	target:setPos(618,25,429,94,0x5A); -- Pashhow Marshlands [S]
	end

					        
end;