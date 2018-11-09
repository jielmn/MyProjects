/*
Navicat MySQL Data Transfer

Source Server         : 192.168.0.99
Source Server Version : 50720
Source Host           : 192.168.0.99:3306
Source Database       : luhe_temp

Target Server Type    : MYSQL
Target Server Version : 50720
File Encoding         : 65001

Date: 2018-11-09 15:08:27
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
) ENGINE=InnoDB AUTO_INCREMENT=517 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for nurseinfo
-- ----------------------------
DROP TABLE IF EXISTS `nurseinfo`;
CREATE TABLE `nurseinfo` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `nurseid` varchar(16) NOT NULL,
  `name` varchar(16) NOT NULL,
  `departid` int(11) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=44 DEFAULT CHARSET=utf8;

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
-- Table structure for patientreltag
-- ----------------------------
DROP TABLE IF EXISTS `patientreltag`;
CREATE TABLE `patientreltag` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `patientid` varchar(255) NOT NULL,
  `tagid` varchar(255) NOT NULL,
  `reltime` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `latest` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=132 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for readerinfo
-- ----------------------------
DROP TABLE IF EXISTS `readerinfo`;
CREATE TABLE `readerinfo` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `readerid` varchar(255) DEFAULT NULL,
  `departid` int(11) DEFAULT NULL,
  `bedid` int(11) DEFAULT NULL,
  `readertype` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `readerid` (`readerid`)
) ENGINE=InnoDB AUTO_INCREMENT=47 DEFAULT CHARSET=utf8;

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
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for station
-- ----------------------------
DROP TABLE IF EXISTS `station`;
CREATE TABLE `station` (
  `ip` varchar(16) NOT NULL,
  `name` varchar(32) NOT NULL,
  `atime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`ip`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for tempermonitor
-- ----------------------------
DROP TABLE IF EXISTS `tempermonitor`;
CREATE TABLE `tempermonitor` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `readerid` varchar(16) NOT NULL,
  `tagid` varchar(16) NOT NULL,
  `temper` int(11) NOT NULL,
  `mtime` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `nurseid` varchar(16) DEFAULT NULL,
  `bodyindex` tinyint(4) DEFAULT '0',
  `islast` tinyint(4) DEFAULT '0',
  `isbind` tinyint(4) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=4304 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for users
-- ----------------------------
DROP TABLE IF EXISTS `users`;
CREATE TABLE `users` (
  `username` varchar(255) DEFAULT NULL,
  `password` varchar(255) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- View structure for roles
-- ----------------------------
DROP VIEW IF EXISTS `roles`;
CREATE ALGORITHM=UNDEFINED DEFINER=`root`@`%` SQL SECURITY DEFINER VIEW `roles` AS select `users`.`username` AS `username`,'manager' AS `role` from `users` ;
