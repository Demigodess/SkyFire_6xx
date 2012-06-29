SET @CGUID := 88615;
DELETE FROM `creature` WHERE `guid` BETWEEN @CGUID+00 AND @CGUID+22;
INSERT INTO `creature` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`position_x`,`position_y`,`position_z`,`orientation`,`spawntimesecs`,`spawndist`,`MovementType`) VALUES
(@CGUID+00,39190,631,12,1,505.1563,-2536.863,1069.598,0,10,10,1), -- Wicked Spirit
(@CGUID+01,39190,631,12,1,502.6493,-2531.328,1069.598,0,10,10,1), -- Wicked Spirit
(@CGUID+02,39190,631,12,1,503.8246,-2508.141,1069.598,0,10,10,1), -- Wicked Spirit
(@CGUID+03,39190,631,12,1,495.3629,-2538.215,1069.598,0,10,10,1), -- Wicked Spirit
(@CGUID+04,39190,631,12,1,506.8715,-2515.552,1069.598,0,10,10,1), -- Wicked Spirit
(@CGUID+05,39190,631,12,1,481.3021,-2509.222,1069.598,0,10,10,1), -- Wicked Spirit
(@CGUID+06,39190,631,12,1,481.5382,-2527.332,1069.598,0,10,10,1), -- Wicked Spirit
(@CGUID+07,39190,631,12,1,500.0807,-2523.289,1069.598,0,10,10,1), -- Wicked Spirit
(@CGUID+08,39190,631,12,1,486.6632,-2517.747,1069.598,0,10,10,1), -- Wicked Spirit
(@CGUID+09,39190,631,12,1,513.8802,-2511.795,1069.596,0,10,10,1), -- Wicked Spirit
(@CGUID+10,39190,631,12,1,499.6406,-2500.783,1069.656,0,10,10,1), -- Wicked Spirit
(@CGUID+11,39190,631,12,1,513.7101,-2530.931,1069.596,0,10,10,1), -- Wicked Spirit
(@CGUID+12,39190,631,12,1,502.6811,-2522.542,1069.598,0,10,10,1), -- Wicked Spirit
(@CGUID+13,39190,631,12,1,518.7483,-2521.240,1069.596,0,10,10,1), -- Wicked Spirit
(@CGUID+14,39190,631,12,1,478.9774,-2517.446,1069.598,0,10,10,1), -- Wicked Spirit
(@CGUID+15,39190,631,12,1,489.0104,-2538.535,1069.598,0,10,10,1), -- Wicked Spirit
(@CGUID+16,39190,631,12,1,511.6320,-2522.373,1069.598,0,10,10,1), -- Wicked Spirit
(@CGUID+17,39190,631,12,1,500.8976,-2527.198,1069.598,0,10,10,1), -- Wicked Spirit
(@CGUID+18,39190,631,12,1,484.7934,-2533.592,1069.598,0,10,10,1), -- Wicked Spirit
(@CGUID+19,39190,631,12,1,490.4132,-2504.323,1069.598,0,10,10,1), -- Wicked Spirit
(@CGUID+20,39190,631,12,1,485.0298,-2527.724,1069.598,0,10,10,1), -- Wicked Spirit
(@CGUID+21,39190,631,12,1,494.2899,-2511.384,1069.598,0,10,10,1), -- Wicked Spirit
(@CGUID+22,39190,631,12,1,491.7170,-2532.490,1069.598,0,10,10,1); -- Wicked Spirit