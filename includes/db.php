<?php

// Настройки
require_once('configs/config.php');
// Библиотека для работы с БД - http://dklab.ru/lib/DbSimple
require_once('includes/DbSimple/Generic.php');

// Массив настроек
global $AoWoWconf;

// Подключение к БД мангос (версия 3 :) )
$DB = DbSimple_Generic::connect("mysql://".$AoWoWconf['world']['user'].":".$AoWoWconf['world']['pass']."@".$AoWoWconf['world']['host']."/".$AoWoWconf['world']['db']);
$DB->setErrorHandler('databaseErrorHandler');
$DB->setIdentPrefix($AoWoWconf['world']['aowow']);
$DB->query('SET NAMES ?', 'utf8');
// Подключение к БД auth
if($AoWoWconf['auth'])
{
	$rDB = DbSimple_Generic::connect("mysql://".$AoWoWconf['auth']['user'].":".$AoWoWconf['auth']['pass']."@".$AoWoWconf['auth']['host']."/".$AoWoWconf['auth']['db']);
	$rDB->setErrorHandler('databaseErrorHandler');
	$rDB->query('SET NAMES ?', 'utf8');
}
// Код обработчика ошибок SQL.
function databaseErrorHandler($message, $info)
{
	// Если использовалась @, ничего не делать.
	if (!error_reporting()) return;
	// Выводим подробную информацию об ошибке.
	echo "SQL Error: $message<br><pre>"; 
	print_r($info);
	echo "</pre>";
	exit();
}

// Для отладки разкомментировать строку ниже
if($AoWoWconf['debug'])
	$DB->setLogger('myLogger');

function myLogger($db, $sql)
{
	global $smarty;
	$smarty->uDebug('!DbSimple', $sql, 5000);
}

?>