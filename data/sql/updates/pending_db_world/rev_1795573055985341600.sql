-- Crushridge Mage: use Frostbolt 15530 instead of 9672 for the repeating in-combat cast
UPDATE `smart_scripts` SET `action_param1` = 15530 WHERE `entryorguid` = 2255 AND `source_type` = 0 AND `id` = 1;
