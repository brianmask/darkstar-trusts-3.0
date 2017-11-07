-----------------------------------
-- Area: Dynamis Jeuno
-- NPC:  Wilywox Tenderpalm
-- Boss Trigger for THF/WHM
-- Popped with Tome 20
-- Drops Odius Grenade
-----------------------------------

-----------------------------------
-- onMobSpawn Action
-----------------------------------

function onMobSpawn(mob)
mob:addMod(MOD_EVA,50);
mob:addMod(MOD_ACC,50);


end;


function onMobEngaged(mob,target)

    local hp = math.random(30,50)
    mob:setLocalVar("Benediciton", hp);

local weakener = target:getVar("DynaWeakener");
   if (weakener == 3) then
   mob:setMod(MOD_HPP,-75);
   mob:setMod(MOD_DEFP,-75);
   mob:setMod(MOD_ATTP,-75);
   mob:addMod(MOD_EVA,-30);
   mob:addMOd(MOD_ACC,-30);
if (target:getObjType() == TYPE_PC) then  
target:PrintToPlayer("You have significantly weakened the monster!", 0xD);
end
elseif (weakener == 2) then
   mob:setMod(MOD_HPP,-50);
   mob:setMod(MOD_DEFP,-20);
   mob:setMod(MOD_ATTP,-20);
   mob:addMod(MOD_EVA,-20);
   mob:addMOd(MOD_ACC,-20);
if (target:getObjType() == TYPE_PC) then  
target:PrintToPlayer("You have weakened the monster!", 0xD);
end
elseif (weakener == 1) then
   mob:setMod(MOD_HPP,-20);
   mob:setMod(MOD_DEFP,-10);
   mob:setMod(MOD_ATTP,-10);
   mob:addMod(MOD_EVA,-10);
   mob:addMOd(MOD_ACC,-10);
if (target:getObjType() == TYPE_PC) then	
target:PrintToPlayer("You have weakened the monster ever so slightly", 0xD);
end
elseif (weakener == 0) then
 -- mob:setMod(MOD_ACC,100);
 -- mob:setMod(MOD_EVA,100);
if (target:getObjType() == TYPE_PC) then 
target:PrintToPlayer("You have summoned a Monster.", 0xD); 
end 
end   
end;


-----------------------------------
-- onMobFight Action
-----------------------------------
function onMobFight(mob,target)

    local battletime = mob:getBattleTime();
	local bhp = mob:getLocalVar("Benediction");
    local pd = mob:getLocalVar("PD");
	local randpd = math.random(90, 120);

    if (battletime > pd + randpd) then
        mob:useMobAbility(437);
        mob:setLocalVar("PD", battletime);
    elseif (mob:getHPP() < bhp) then
        mob:useMobAbility(433);		
        mob:setLocalVar("Benediction", 0);		
    end

end;



-----------------------------------
-- onMobDeath
-----------------------------------

function onMobDeath(mob,killer)
local qm4 = GetNPCByID(17547513);
killer:setVar("DynaWeakener",0);
qm4:setStatus(STATUS_NORMAL);
	

end;

function onMobDespawn( mob )
local qm4 = GetNPCByID(17547513);

qm4:setStatus(STATUS_NORMAL);

end