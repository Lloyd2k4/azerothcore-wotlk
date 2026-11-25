-- Syndicate Magus: use Frostbolt 15530 instead of 12675 for the repeating in-combat cast
UPDATE `smart_scripts` SET `action_param1` = 15530 WHERE `entryorguid` = 2591 AND `source_type` = 0 AND `id` = 1;
