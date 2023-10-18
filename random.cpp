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
std::vector<Reading*>* Random::takeReading()
{
	std::vector<Reading*> *readings = new std::vector<Reading *>;
	for (int i=0; i < m_numReadingsPerPoll; i++)
	{
		m_lastValue += ((rand() % 100) > 50 ? 1 : -1) * ((rand() % 100) / 20);
		DatapointValue value(m_lastValue);
		Reading *in = new Reading(m_assetName+std::to_string(m_assetCount++ % m_numAssets + 1), new Datapoint("random", value));
		readings->push_back(in);
	}

	return readings;
}

