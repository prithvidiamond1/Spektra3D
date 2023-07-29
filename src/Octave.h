#pragma once

#include "Iir.h"

#include <cmath>

using namespace Iir;

class OctaveBandAnalyser {
private:
	int filterOrder;

	int sampleRate;
	int bandsPerOctave;

	float lowerLimitOfFreqRange;
	float upperLimitOfFreqRange;
	float refFrequency;

	std::vector<float> centerFreqsOfBands;

	Butterworth::BandPass<> bandPassFilter;
public:
	OctaveBandAnalyser();	// default constructor

	void setParams(int filterOrder, int sampleRate, int bandsPerOctave, float lowerLimitOfFreqRange,
		float upperLimitOfFreqRange, float refFrequency = 1000.0f);

	std::vector<float> getCenterFreqsOfBands();

	void analyseFrames(const std::vector<float>& inputFrames, std::vector<float>& outputVals);

};




