-- phpMyAdmin SQL Dump
-- version 3.3.8
-- http://www.phpmyadmin.net
--
-- Serveur: localhost
-- G�n�r� le : Ven 24 Juin 2011 � 08:08
-- Version du serveur: 6.0.0
-- Version de PHP: 5.2.9-2

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Base de donn�es: `dspdb`
--

-- --------------------------------------------------------

--
-- Structure de la table `char_vars`
--

DROP TABLE IF EXISTS `total_online`;
CREATE TABLE IF NOT EXISTS `total_online` (
  `lineID` INT(10) NOT NULL AUTO_INCREMENT,
  `call_date` int(10) unsigned NOT NULL,
  `players_online` int(11) NOT NULL,
  PRIMARY KEY (`lineID`,`call_date`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;