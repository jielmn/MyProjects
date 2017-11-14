DROP PROCEDURE IF EXISTS `proc_update_date`;
DELIMITER ;;
CREATE PROCEDURE `proc_update_date`()
BEGIN
    #Routine body goes here...
    DECLARE max_day DATETIME;
    select max(x) into @max_day from gzqx;
    -- select @max_day;

    update gzqx set x=DATE_ADD(x, INTERVAL DATEDIFF(CURDATE(),@max_day) DAY);
END
;;
DELIMITER ; 