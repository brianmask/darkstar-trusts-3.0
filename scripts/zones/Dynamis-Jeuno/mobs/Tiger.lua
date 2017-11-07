-----------------------------------
-- Area: Dynamis Jeuno
-- NPC:  Tiger

-----------------------------------
package.loaded["scripts/zones/Dynamis-Windurst/TextIDs"] = nil;
-----------------------------------
require("scripts/globals/status");
require("scripts/globals/dynamis");
require("scripts/zones/Dynamis-Jeuno/TextIDs");

-----------------------------------
-- onMobSpawn Action
-----------------------------------

function onMobSpawn(mob)
local tiger = mob:getID()
GetMobByID(tiger):setDropID(5041); -- Default Drop ID
mob:setMod(MOD_ACC,260);
mob:setMod(MOD_EVA,295);
mob:setMod(MOD_DEF,250);

	
end;

-----------------------------------
-- onMobEngaged
-----------------------------------

function onMobEngaged(mob,target)
local vanatime = VanadielHour();
if (vanatime >= 12 and vanatime < 18) then
mob:setMod(MOD_ATTP,-20);
mob:setMod(MOD_MATT,-20);
mob:setMod(MOD_DELAY,-40);
elseif (vanatime >= 18) then
-- Neutral
elseif (vanatime >= 0 and vanatime < 6) then
mob:setMod(MOD_ACC,-30);
mob:setMod(MOD_ATTP,10);
end
    
end;


-----------------------------------
-- onWeaponskillHit
-----------------------------------
function onWeaponskillHit(mob, attacker, weaponskill)
-- Staggering Function
if (attacker:getObjType() == TYPE_PC) then
	local isweak = mob:getLocalVar("WeakenedTrigger");
	local tiger = mob:getID()
	
	local wsweakness = math.random(1,1000);
	if (wsweakness > 180) and (wsweakness < 401) and (isweak ~= 1) then
		mob:weaknessTrigger(0); -- Yellow Stagger Increase Scyld 55% of Triggers
		mob:addStatusEffect(EFFECT_TERROR,1,0,10);
		local randomscyld = math.random(10,20);
		local oldscyld = attacker:getVar("ScyldMultiplier");
		local newscyld = (randomscyld + oldscyld);
		attacker:setVar("ScyldMultiplier",newscyld);
		mob:setLocalVar("WeakenedTrigger",1);
		attacker:PrintToPlayer("You have been granted a "..randomscyld.."% scyld bonus.  Total Bonus: "..newscyld.."%.", 0x15);
	elseif (wsweakness > 80) and (wsweakness < 181) and (isweak ~= 1) then
		mob:weaknessTrigger(1); -- Blue Stagger drops Memoirs which grants 4-8 currency  15%
		mob:addStatusEffect(EFFECT_TERROR,1,0,10);
		GetMobByID(tiger):setDropID(6088);
		mob:setLocalVar("WeakenedTrigger",1);
	elseif (wsweakness > 20) and (wsweakness < 81) and (isweak ~= 1) then
		mob:weaknessTrigger(2); -- Red Stagger drops Pop Items 10%
		mob:addStatusEffect(EFFECT_TERROR,1,0,10);
		local itemdrop = math.random(1,4);
		if (itemdrop == 1) then
			GetMobByID(tiger):setDropID(6092); -- Fiendish Tome 17
		elseif (itemdrop == 2) then
			GetMobByID(tiger):setDropID(6093); -- Fiendish Tome 18
		elseif (itemdrop == 3) then
			GetMobByID(tiger):setDropID(6094); -- Fiendish Tome 19
		elseif (itemdrop == 4) then
			GetMobByID(tiger):setDropID(6095); -- Fiendish Tome 20
		end
		mob:setLocalVar("WeakenedTrigger",1);
	elseif (wsweakness < 21) and (isweak ~= 1) then
		mob:weaknessTrigger(3); -- White Stagger drops 100's
		mob:addStatusEffect(EFFECT_TERROR,1,0,10);
		local extracurrency = math.random(1,3);
		if (extracurrency == 1) then
			GetMobByID(tiger):setDropID(6089);	-- L.Jadeshell	
		elseif (extracurrency == 2) then
			GetMobByID(tiger):setDropID(6090); -- Montiont
		elseif (extracurrency == 3) then
			GetMobByID(tiger):setDropID(6091); -- 100 Byne
		end
		mob:setLocalVar("WeakenedTrigger",1);
	end

end



return 0;


end;






-----------------------------------
-- onMobFight Action
-- Randomly change Resistance based on Time
-----------------------------------
function onMobFight(mob,target)
if (mob:hasStatusEffect(EFFECT_TERROR)) then
mob:setMod(MOD_SLASHRES,1500);
mob:setMod(MOD_PIERCERES,1500);
mob:setMod(MOD_HTHRES,1500);
elseif (not mob:hasStatusEffect(EFFECT_TERROR)) then
mob:setMod(MOD_SLASHRES,1000);
mob:setMod(MOD_PIERCERES,1000);
mob:setMod(MOD_HTHRES,1000);
end


local vanatime = VanadielHour();
if (vanatime >= 6 and vanatime < 12) then
local randomTime = math.random(10,25);
local changeTime = mob:getLocalVar("changeTime");
if (mob:getBattleTime() - changeTime > randomTime) then
local changeres = math.random(1,3);

if (changeres == 1) then
if (target:getObjType() == TYPE_PC) then
target:PrintToPlayer("There is a change in the Monsters behavior.", 0x1C);
end
mob:setMod(MOD_SLASHRES,300);
mob:setMod(MOD_PIERCERES,1000);
mob:setMod(MOD_HTHRES,2000);
mob:setLocalVar("changeTime", mob:getBattleTime());
elseif (changeres == 2) then
if (target:getObjType() == TYPE_PC) then
target:PrintToPlayer("There is a change in the Monsters behavior.", 0x1C);
end
mob:setMod(MOD_SLASHRES,1000);
mob:setMod(MOD_PIERCERES,2000);
mob:setMod(MOD_HTHRES,300);
mob:setLocalVar("changeTime", mob:getBattleTime());
else
if (target:getObjType() == TYPE_PC) then
target:PrintToPlayer("There is a change in the Monsters behavior.", 0x1C);
end
mob:setMod(MOD_SLASHRES,3000);
mob:setMod(MOD_PIERCERES,300);
mob:setMod(MOD_HTHRES,2000);
mob:setLocalVar("changeTime", mob:getBattleTime());
end
end
end
end;

-----------------------------------
-- onMobDeath
-----------------------------------

function onMobDeath(mob,killer)
    local scyldmult = killer:getVar("ScyldMultiplier");
    local duration = 45;
	local defBoost = 15;
	local level = killer:getMainLvl();
	local scyld = math.floor((level - 65) * (1 + (scyldmult/100)));
	local stagger = mob:getLocalVar("MonsterStagger");
	
	if (killer:getObjType() == TYPE_PC) then
    local randombuff = math.random(1,100)
	if (randombuff >= 50) then
	killer:addStatusEffect(EFFECT_DEFENSE_BOOST,defBoost,0,duration);
    killer:PrintToPlayer("The monster has endowed you with a temporary Defense Bonus", 0xD);
	elseif (randombuff < 20) then
	local heal = killer:getMaxHP();
	local healmp = killer:getMaxMP();
	killer:addHP(heal);
	killer:addMP(healmp);
    killer:PrintToPlayer(string.format("%s recovers HP and MP", killer:getName()), 0x15);
	end
	killer:addCurrency("scyld", scyld);
	killer:PrintToPlayer(string.format("%s gains "..scyld.." scyld.", killer:getName()), 0x15);
	end
	
end;
