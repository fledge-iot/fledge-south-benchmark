/*
 * Fledge south service plugin
 *
 * Copyright (c) 2018 Dianomic Systems
 *
 * Released under the Apache 2.0 Licence
 *
 * Author: Massimiliano Pinto
 */
#include <random.h>
#include <reading.h>

/**
 * Constructor for the random "sensor"
 */
Random::Random() : m_numAssets(1), m_numReadingsPerPoll(1)
{
	srand(time(0));
	m_lastValue = rand() % 100;
	m_assetCount = 0;
}

/**
 * Destructor for the random "sensor"
 */
Random::~Random()
{
}

/**
 * Take a reading from the random "sensor"
 */
std::vector<Reading*>* Random::takeReadings()
{
	std::vector<Reading*> *readings = new std::vector<Reading *>;
	for (int i=0; i < m_numReadingsPerPoll; i++)
	{
		m_lastValue += ((rand() % 100) > 50 ? 1 : -1) * ((rand() % 100) / 20);
		DatapointValue value(m_lastValue);

		std::vector<Datapoint *> values;
		values.emplace_back(new Datapoint("random", value));
		// crete more datapoints
		for (int j=1; j < m_numDatapoints; j++)
		{
			std::string dpName = "random"+ std::to_string(j+1);
			m_lastValue += ((rand() % 100) > 50 ? 1 : -1) * ((rand() % 100) / 20);
			DatapointValue dpValue(m_lastValue);
			values.emplace_back(new Datapoint(dpName, dpValue));
		}

		Reading *in = new Reading(m_assetName+std::to_string(m_assetCount++ % m_numAssets + 1), values);
		readings->push_back(in);
	}

	return readings;
}

