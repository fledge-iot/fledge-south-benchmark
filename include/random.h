#ifndef _RANDOM_H
#define _RANDOM_H
/*
 * Fledge south service plugin
 *
 * Copyright (c) 2018 Dianomic Systems
 *
 * Released under the Apache 2.0 Licence
 *
 * Author: Mark Riddoch, Massimiliano Pinto
 */
#include <reading.h>

class Random {
	public:
		Random();
		~Random();
		std::vector<Reading*>*	takeReading();
		std::string	getAssetName() { return m_assetName; }
		unsigned long	getNumAssets() { return m_numAssets; }
		void	setAssetName(const std::string& assetName) { m_assetName = assetName; }
		void	setNumAssets(unsigned long numAssets) { if (numAssets > 0) { m_numAssets = numAssets; } }
		void	setNumValues(unsigned long numValues) { if (numValues > 0) { m_numValues = numValues; } }

	private:
		long		m_lastValue;
		std::string	m_assetName;
		long            m_numAssets;
		long            m_numValues;
		long            m_assetCount;
};
#endif
