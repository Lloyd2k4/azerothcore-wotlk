-- Muckshell Scrabbler: use Lightning Bolt 15234 instead of 9532 for the repeating in-combat cast
UPDATE `smart_scripts` SET `action_param1` = 15234, `comment` = 'Muckshell Scrabbler - In Combat - Cast ''15234''' WHERE `entryorguid` = 4404 AND `source_type` = 0 AND `id` = 0;
