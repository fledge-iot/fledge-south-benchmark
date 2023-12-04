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
		"plugin" :
			{ 
			"description" : "Simulated data generation for benchmark tests",
			"type" : "string",
		    "default" : PLUGIN_NAME,
			"readonly" : "true" 
			},
		"numAssets" : 
			{
			"description" : "Number of unique assets to simulate",
			"type" : "integer",
		    "default" : "1",
			"minimum" : "1",
			"order": "2",
			"displayName": "Number of Assets",
			"rule" : "value > 0"
			},
		"asset" : 
			{
			"description" : "Asset name prefix",
			"type" : "string",
			"default" : "Random",
			"order": "1",
			"displayName": "Asset Name"
			},
		"numReadingsPerPoll" :
			{
			"description" : "Number of readings to be returned per poll call",
			"type" : "integer",
			"default" : "1",
			"minimum" : "1",
			"maximum" : "100000",
			"order": "3",
			"displayName": "Readings Per Call"
			},
		"model": {
			"description": "The machine learning model to use inspect the captured image for defects",
			"displayName": "QA Model",
			"order": "4",
			"type": "bucket",
			"properties": "{\"constant\": {\"type\": \"MLModel\",\"name\": \"BadBottle\"}, \"key\": {\"product\": {\"description\": \"The bottle type being produced\", \"displayName\": \"Bottle Type\",\"type\": \"enumeration\",\"options\": [\"Wine Bottle\",\"Bear Bottle\",\"Whiskey Bottle\",\"Milk Bottle\"],\"default\": \"Milk Bottle\"}},\"properties\": {\"version\": {\"description\": \"The model version to use, 0 represents the latest version\",\"displayName\": \"Version\",\"type\": \"string\",\"default\": \"\"}}}",
			"default": "{\"type\": \"MLModel\",\"name\": \"BadBottle\",\"product\": \"Wine Bottle\",\"version\": \"0\" }"
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
	random->setNumReadingsPerPoll(numReadingsPerPoll);

	if (config->itemExists("model"))
	{
		Logger::getLogger()->info("model/bucket config item: type=%s ", config->getType("model").c_str());
		const string &s = config->getValue("model");
		Logger::getLogger()->info("model/bucket config item: value=%s ", s.c_str());
		const vector<pair<string,string>>* vec = config->parseBucketItemValue(s);
		if (vec)
		{
			for (const auto & v: *vec)
				Logger::getLogger()->info("BucketItemValue: key=%s, value=%s", v.first.c_str(), v.second.c_str());
		}
		else
			Logger::getLogger()->info("parseBucketItemValue() returned NULL");

		delete vec;
		
		const string &s2 = config->getItemAttribute("model", ConfigCategory::ItemAttribute::BUCKET_PROPERTIES_ATTR);
		Logger::getLogger()->info("model config item: bucketProperties=%s ", s2.c_str());
	}
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
	return random->takeReadings();
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
