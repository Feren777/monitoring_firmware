BEGIN
DECLARE done INT DEFAULT FALSE;
DECLARE lastId BIGINT DEFAULT NULL;
DECLARE lastCreated TIMESTAMP DEFAULT NULL;
DECLARE lastValue DOUBLE DEFAULT 0;
DECLARE currentId BIGINT DEFAULT NULL;
DECLARE currentCreated TIMESTAMP DEFAULT NULL;
DECLARE currentValue DOUBLE DEFAULT 0;
DECLARE deltaValue DOUBLE DEFAULT -1;
DECLARE deltaTime DOUBLE DEFAULT -1;
DECLARE channelScan CURSOR FOR SELECT DATAID, CREATED, VALUE FROM `monitor_drainmax.data` WHERE `channel_Id` = channelId AND `analyzed` = 0 ORDER BY CREATED ASC;
DECLARE CONTINUE HANDLER FOR NOT FOUND SET done = TRUE;
OPEN channelScan;
read_loop: LOOP
  SET lastId = currentId;
  SET lastCreated = currentCreated;
  SET lastValue = currentValue;
  FETCH channelScan INTO currentId, currentCreated, currentValue;
  IF done THEN
    LEAVE read_loop;
  END IF;
  IF (lastId IS NOT NULL) THEN
    SET deltaValue = currentValue - lastValue;
    SET deltaTime = TIME_TO_SEC(TIMEDIFF(currentCreated, lastCreated));
    IF (deltaTime = 0) THEN
      SET deltaTime = 1;
    END IF;
  INSERT INTO monitor_drainmax.statistics (data_Id, data_previous_Id, value_delta, time_delta_sec, effeciency) VALUES (currentId, lastId, deltaValue, deltaTime, deltaValue / deltaTime);
  END IF;
END LOOP;
CLOSE channelScan;
UPDATE monitor_drainmax.data SET analyzed = 1 WHERE channel_id = channelId AND analyzed = 0;
END