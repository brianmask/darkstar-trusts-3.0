-----------------------------------
-- Area: Balga's Dais
-- NPC:  Black Dragon
-- Mission 2-3 BCNM Fight
-----------------------------------

require("scripts/globals/titles");

-----------------------------------
-- onMobSpawn Action
-----------------------------------

function onMobSpawn(mob)
mob:setLocalVar("astralflows",1);
end;

function onSpellPrecast(mob, spell)
if (mob:getHPP() <= 75) and (mob:getHPP() > 50) then
	if(spell:getID() == 165) then -- thunder II > Thunder III
	mob:setLocalVar("Thunder",3);
    elseif(spell:getID() == 194) then -- thundaga > Thundaga II
    mob:setLocalVar("Thundaga",2);
	end
elseif (mob:getHPP() <= 50) and (mob:getHPP() > 25) then
	if(spell:getID() == 165) then -- thunder II > Thunder IV
 	mob:setLocalVar("Thunder",4);
	elseif(spell:getID() == 194) then -- thundaga > Thundaga III
	mob:setLocalVar("Thundaga",3);
	end
elseif (mob:getHPP() <= 25) then
    print("Less than 25%");
	if(spell:getID() == 165) then -- thunder II > Thunder V
 	mob:setLocalVar("Thunder",5);
	elseif(spell:getID() == 194) then -- thundaga > Thundaga IV	
 	mob:setLocalVar("Thundaga",4);
	end	
end	
end;

function onMobFight(mob, target)

local astralFlows = mob:getLocalVar("astralflows");
local afTrigger = math.random(25,50);
local thunder = mob:getLocalVar("Thunder");
local thundaga = mob:getLocalVar("Thundaga");

if (thunder == 3) then
	mob:castSpell(166);
    mob:setLocalVar("Thunder",1);
elseif (thunder == 4) then
	mob:castSpell(167);
    mob:setLocalVar("Thunder",1);
elseif (thunder == 5) then
	mob:castSpell(168);
    mob:setLocalVar("Thunder",1);	
end

if (thunder == 2) then
	mob:castSpell(195);
    mob:setLocalVar("Thundaga",1);
elseif (thunder == 3) then
	mob:castSpell(196);
    mob:setLocalVar("Thundaga",1);
elseif (thunder == 4) then
	mob:castSpell(197);
    mob:setLocalVar("Thundaga",1);	
end



if (mob:getHPP() <= afTrigger) and (astralFlows == 1) then
mob:useMobAbility(2078);
mob:setLocalVar("astralflows",2);
end




end;


-----------------------------------
-- onMobDeath Action
-----------------------------------

function onMobDeath(mob,killer)
    local ixion = killer:getVar("DarkIxionWin");
	killer:PrintToPlayer("Dark Ixion: A mere mortal defeat me...???",0x0D);
	killer:setVar("IxionKill",1);
	if (ixion ~= 1) then
	killer:setVar("DarkIxionWin",1);
	killer:PrintToPlayer("You may now summon Dark Ixion using Thunder Spirit pact under Astral Flow",0x15);
	end

	



end;