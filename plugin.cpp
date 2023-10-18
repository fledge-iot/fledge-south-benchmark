/*
 * Fledge south plugin.
 *
 * Copyright (c) 2018 Dianomic Systems
 *
 * Released under the Apache 2.0 Licence
 *
 * Author: Massimiliano Pinto
 */
#include <random.h>
#include <plugin_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string>
#include <logger.h>
#include <plugin_exception.h>
#include <config_category.h>
#include <version.h>
#include <stdexcept>

using namespace std;

#define PLUGIN_NAME "Benchmark"
static const char *default_config = QUOTE({
		"plugin" : { 
			"description" : "Simulated data generation for benchmark tests",
			"type" : "string",
		       	"default" : PLUGIN_NAME,
			"readonly" : "true" },
		"numAssets" : {
			"description" : "Number of unique assets to simulate",
			"type" : "integer",
		       	"default" : "1",
			"minimum" : "1",
			"order": "2",
			"displayName": "Number of Assets",
			"rule" : "value > 0"
			},
		"asset" : {
			"description" : "Asset name prefix",
			"type" : "string",
			"default" : "Random",
			"order": "1",
			"displayName": "Asset Name"
			},
		"numReadingsPerPoll" : {
			"description" : "Number of values to be returned per poll call",
			"type" : "integer",
			"default" : "1",
			"minimum" : "1",
			"maximum" : "100000",
			"order": "3",
			"displayName": "Values Per Call"
			}
		});
		  
/**
 * The Random plugin interface
 */
extern "C" {

/**
 * The plugin information structure
 */
static PLUGIN_INFORMATION info = {
	PLUGIN_NAME,              // Name
	VERSION,                  // Version
	0,    			  // Flags
	PLUGIN_TYPE_SOUTH,        // Type
	"2.0.0",                  // Interface version
	default_config		// Default configuration
};

/**
 * Return the information about this plugin
 */
PLUGIN_INFORMATION *plugin_info()
{
	return &info;
}

/**
 * Set plugin config into plugin handle
 */
void setPluginConfig(Random *random, ConfigCategory *config)
{
	if (!random || !config) 
		return;
	
	if (config->itemExists("asset"))
		random->setAssetName(config->getValue("asset"));

	unsigned int nAssets = 1;
	if (config->itemExists("numAssets"))
	{
		nAssets = stoul(config->getValue("numAssets"), nullptr, 0);
		if (nAssets <= 0)
		{
			throw runtime_error("The value of numAssets, number of unique assets to simulate, must be greater than 0");
		}
	}
	random->setNumAssets(nAssets);

	unsigned int numReadingsPerPoll = 1;
	if (config->itemExists("numReadingsPerPoll"))
	{
		numReadingsPerPoll = stoul(config->getValue("numReadingsPerPoll"), nullptr, 0);
		if (numReadingsPerPoll <= 0)
		{
			throw runtime_error("The value of numReadingsPerPoll, number of values to be returned per poll call, must be greater than 0");
		}
	}
	random->setNumValues(numReadingsPerPoll);
}

/**
 * Initialise the plugin, called to get the plugin handle
 */
PLUGIN_HANDLE plugin_init(ConfigCategory *config)
{
	Random *random = new Random();
	random->setAssetName("Random");
	
	Logger::getLogger()->info("Benchmark plugin config: %s", config->toJSON().c_str());
	setPluginConfig(random, config);
	
	return (PLUGIN_HANDLE)random;
}

/**
 * Poll for a plugin reading
 */
std::vector<Reading*>* plugin_poll(PLUGIN_HANDLE *handle)
{
	Random *random = (Random *)handle;
	return random->takeReading();
}

/**
 * Reconfigure the plugin
 */
void plugin_reconfigure(PLUGIN_HANDLE *handle, string& newConfig)
{
	Random *random = (Random *)*handle;
	Logger::getLogger()->info("Benchmark plugin new config: %s", newConfig.c_str());
	
	ConfigCategory configCategory(string("cfg"), newConfig);
	setPluginConfig(random, &configCategory);
}

/**
 * Shutdown the plugin
 */
void plugin_shutdown(PLUGIN_HANDLE *handle)
{
	Random *random = (Random *)handle;
	delete random;
}
};
