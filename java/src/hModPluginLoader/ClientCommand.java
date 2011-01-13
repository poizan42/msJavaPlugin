package hModPluginLoader;

public enum ClientCommand {
	intercom_return,
	
	user_teleport,
	user_getPosition,
	user_sethealth,
	
	chat_sendmsgTo,
	chat_sendmsg,
	chat_sendUserlist,
	
	logger_log,
	
	map_createPickupSpawn,
	map_setTime,
	map_getSpawn,
	map_getBlock,
	map_setBlock,
	map_saveWholeMap,
	map_getMapData_block,
	map_getMapData_meta,
	map_getMapData_skylight,
	map_getMapData_blocklight,
	
	config_has,
	config_iData,
	config_lData,
	config_fData,
	config_dData,
	config_sData,
	config_bData,
	
	plugin_getPluginVersion,
	plugin_setPluginVersion,
	plugin_addCallback
}
