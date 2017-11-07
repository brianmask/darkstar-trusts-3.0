-----------------------------------
-- Area: Dynamis Beaucedine
-- NPC:  Angra Mainyu
-- Mega Boss
-----------------------------------

require("scripts/globals/titles");
require("scripts/globals/keyitems");
require("scripts/globals/status");



function onMobSpawn(mob)
    local qm0 = GetNPCByID(17326800);
	qm0:setStatus(STATUS_DISAPPEAR);

end;

-----------------------------------
-- onMobInitialize Action
-----------------------------------

function onMobInitialize(mob)
end;

-----------------------------------
-- onMobEngaged
-----------------------------------

function onMobEngaged(mob,target)
	

	
end;

-----------------------------------
-- onMobFight Action
-----------------------------------

function onMobFight(mob,target)
	

	
end;

-- Return the selected spell ID.
function onMonsterMagicPrepare(mob, target)
    
	if (mob:getHPP() <= 25) then
		return 244; -- Death
	else
		-- Can cast Blindga, Death, Graviga, Silencega, and Sleepga II.
		-- Casts Graviga every time before he teleports.
		
		rnd = math.random();
		
		if (rnd < 0.2) then
			return 361; -- Blindga
		elseif (rnd < 0.4) then
			 return 244; -- Death
		elseif (rnd < 0.6) then
			
			return 366; -- Graviga
		elseif (rnd < 0.8) then
			return 274; -- Sleepga II
		else
			return 359; -- Silencega
		end
	end
    
end;

-----------------------------------
-- onMobDeath
-----------------------------------

function onMobDeath(mob,killer)

    local qm0 = GetNPCByID(17326800);
	
	killer:addTitle(DYNAMISBEAUCEDINE_INTERLOPER); -- Add title
	killer:setVar("DynaBeaucedine_Win",1);
	
	if (killer:hasKeyItem(HYDRA_CORPS_INSIGNIA) == false and killer:getObjType() == TYPE_PC) then
		killer:addKeyItem(HYDRA_CORPS_INSIGNIA);
		killer:messageSpecial(KEYITEM_OBTAINED,HYDRA_CORPS_INSIGNIA);
	end
	 qm0:setStatus(STATUS_NORMAL);
	
	
end;

function onMobDespawn(mob)

    local qm0 = GetNPCByID(17326800);
	

	qm0:setStatus(STATUS_NORMAL);

end;
