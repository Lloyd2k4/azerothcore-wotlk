-- Bloodfen Raptor: make the Raptor Capture Credit cast triggered so it is
-- not blocked by the SmartAI crowd-control guard (capture spell stuns the raptor)
UPDATE `smart_scripts` SET `action_param2` = 2 WHERE `entryorguid` = 4351 AND `source_type` = 0 AND `id` = 0;
