package msJavaPlugin;

public enum ClientCommand {
	intercom_return,

	plugin_hasPluginVersion,
	plugin_getPluginVersion,
	plugin_setPluginVersion,
	plugin_remPluginVersion,
	
	plugin_hasPointer,
	plugin_getPointer,
	plugin_setPointer,
	plugin_remPointer,
	
	plugin_hasHook,
	plugin_getHook,
	plugin_setHook,
	plugin_remHook,
	
	plugin_hasCallback,
	plugin_addCallback,
	plugin_remCallback,
	plugin_doUntilTrue,
	plugin_doUntilFalse,
	plugin_doAll,
	
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
	map_getMapData,
	
	getMapData_block,
	getMapData_meta,
	getMapData_skylight,
	getMapData_blocklight,
	
	config_has,
	config_iData,
	config_lData,
	config_fData,
	config_dData,
	config_sData,
	config_bData
}
