-----------------------------------
-- Ability: Sublimation
-- Gradually creates a storage of MP while reducing your HP. The effect ends once an MP limit is reached, or your HP has gone too low. The stored MP is then transferred to your MP pool by using the ability a second time.
-- Obtained: Scholar Level 35
-- Recast Time: 30 seconds after the ability is reactivated
-- Duration (Charging): Until MP stored is 25% of Max HP or until HP = 50%
-- Duration (Charged): 2 hours
-----------------------------------

require("scripts/globals/settings");
require("scripts/globals/status");

-----------------------------------
-- onAbilityCheck
-----------------------------------

function onAbilityCheck(pet,target,ability)
    
end;


-----------------------------------
-- onUseAbility
-----------------------------------

function onPetAbility(target, pet, skill)

    local sublimationComplete = pet:getStatusEffect(EFFECT_TRUST_SUBLIMATION_COMPLETE);
    local sublimationCharging = pet:getStatusEffect(EFFECT_TRUST_SUBLIMATION_ACTIVATED);
    local mp = 0;

    if sublimationComplete ~= nil then
        mp = sublimationComplete:getPower();
        local maxmp = pet:getMaxMP();
        local currmp = pet:getMP();
        if ( mp + currmp > maxmp ) then
            mp = maxmp - currmp;
        end
        pet:addMP(mp);
        pet:delStatusEffectSilent(EFFECT_TRUST_SUBLIMATION_COMPLETE);
        -- ability:setMsg(451);
    elseif sublimationCharging ~= nil then
        mp = sublimationCharging:getPower();
        local maxmp = pet:getMaxMP();
        local currmp = pet:getMP();
        if ( mp + currmp > maxmp ) then
            mp = maxmp - currmp;
        end
        pet:addMP(mp);
        pet:delStatusEffectSilent(EFFECT_TRUST_SUBLIMATION_ACTIVATED);
        -- ability:setMsg(451);
    else
        local refresh = pet:getStatusEffect(EFFECT_REFRESH);
        if refresh == nil or refresh:getSubPower() < 3 then
            pet:delStatusEffect(EFFECT_REFRESH);
            pet:addStatusEffectEx(EFFECT_TRUST_SUBLIMATION_ACTIVATED,EFFECT_SUBLIMATION_ACTIVATED,0,3,7200);
        else
            ability:setMsg(323);
        end
    end
    return mp;
end;