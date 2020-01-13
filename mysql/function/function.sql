DELIMITER //
DROP FUNCTION IF EXISTS dept_count;
CREATE FUNCTION dept_count(dept_name VARCHAR(20))
  RETURNS INTEGER
BEGIN
  DECLARE tmp_count, d_count INTEGER DEFAULT 0;
  SELECT count(*) INTO tmp_count from instructor where instructor.dept_name = dept_name;
  SET d_count = tmp_count;
  RETURN d_count;
END//
DELIMITER ;
