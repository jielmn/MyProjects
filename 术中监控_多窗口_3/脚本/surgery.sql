/*
Navicat MySQL Data Transfer

Source Server         : 192.168.0.123
Source Server Version : 50560
Source Host           : 192.168.0.123:3306
Source Database       : surgery

Target Server Type    : MYSQL
Target Server Version : 50560
File Encoding         : 65001

Date: 2018-12-12 17:32:02
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for department
-- ----------------------------
DROP TABLE IF EXISTS `department`;
CREATE TABLE `department` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `departname` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for logininfo
-- ----------------------------
DROP TABLE IF EXISTS `logininfo`;
CREATE TABLE `logininfo` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `username` varchar(255) DEFAULT NULL,
  `logintime` varchar(255) DEFAULT NULL,
  `loginip` varchar(255) DEFAULT NULL,
  `loginpart` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=580 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for patientinfo
-- ----------------------------
DROP TABLE IF EXISTS `patientinfo`;
CREATE TABLE `patientinfo` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `patientid` varchar(16) NOT NULL,
  `patientname` varchar(16) NOT NULL,
  `gender` char(1) DEFAULT NULL,
  `bedid` varchar(16) DEFAULT NULL,
  `age` tinyint(4) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=4077 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for staff
-- ----------------------------
DROP TABLE IF EXISTS `staff`;
CREATE TABLE `staff` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `STFID` varchar(255) DEFAULT NULL,
  `STFNAME` varchar(255) DEFAULT NULL,
  `STFMOBIL` varchar(255) DEFAULT NULL,
  `STFEMAIL` varchar(255) DEFAULT NULL,
  `EXT` varchar(255) DEFAULT NULL,
  `EXT1` varchar(255) DEFAULT NULL,
  `EXT2` varchar(255) DEFAULT NULL,
  `EXT3` varchar(255) DEFAULT NULL,
  `EXT4` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for tagbands
-- ----------------------------
DROP TABLE IF EXISTS `tagbands`;
CREATE TABLE `tagbands` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `tagid` varchar(255) CHARACTER SET utf8 NOT NULL DEFAULT '',
  `patientid` varchar(255) CHARACTER SET utf8 DEFAULT '',
  `patientpart` varchar(255) CHARACTER SET utf8 DEFAULT '',
  PRIMARY KEY (`id`,`tagid`)
) ENGINE=InnoDB AUTO_INCREMENT=53 DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for temperature
-- ----------------------------
DROP TABLE IF EXISTS `temperature`;
CREATE TABLE `temperature` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `tag_id` varchar(16) NOT NULL,
  `collect_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `temperature` int(11) NOT NULL,
  `grid_index` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for users
-- ----------------------------
DROP TABLE IF EXISTS `users`;
CREATE TABLE `users` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `username` varchar(255) NOT NULL,
  `password` varchar(255) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `username` (`username`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8;

-- ----------------------------
-- View structure for bindmap
-- ----------------------------
DROP VIEW IF EXISTS `bindmap`;
CREATE ALGORITHM=UNDEFINED DEFINER=`root`@`%` SQL SECURITY DEFINER VIEW `bindmap` AS select `surgery`.`bindings`.`tag_id` AS `tagid`,`surgery`.`bindings`.`patient_name` AS `patientname`,`surgery`.`bindings`.`tag_name` AS `tagname`,`surgery`.`bindings`.`grid_index` AS `gridindex` from `bindings` ;

-- ----------------------------
-- View structure for roles
-- ----------------------------
DROP VIEW IF EXISTS `roles`;
CREATE ALGORITHM=UNDEFINED DEFINER=`root`@`%` SQL SECURITY DEFINER VIEW `roles` AS select `users`.`username` AS `username`,'manager' AS `role` from `users` ;
