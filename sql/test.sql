create table test (id int auto_increment primary key, a int, b varbinary (128))
engine = innodb;

CREATE TABLE IF NOT EXISTS `tbl`(
   `id` INT UNSIGNED AUTO_INCREMENT,
   `title` VARCHAR(100) NOT NULL,
   `author` VARCHAR(40) NOT NULL,
   `submission_date` DATE,
   PRIMARY KEY ( `id` )
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

INSERT INTO tbl 
    (title, author, submission_date)
    VALUES
    ("吃吃吃", "augus", NOW());
