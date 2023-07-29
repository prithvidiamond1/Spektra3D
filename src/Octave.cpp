#include "Octave.h"

OctaveBandAnalyser::OctaveBandAnalyser() {}

void OctaveBandAnalyser::setParams(int filterOrder, int sampleRate, int bandsPerOctave, float lowerLimitOfFreqRange,
	float upperLimitOfFreqRange, float refFrequency)
{
	this->filterOrder = filterOrder;
	this->sampleRate = sampleRate;
	this->bandsPerOctave = bandsPerOctave;
	this->lowerLimitOfFreqRange = lowerLimitOfFreqRange;
	this->upperLimitOfFreqRange = upperLimitOfFreqRange;
	this->refFrequency = refFrequency;

	int lowerN = (int)std::roundf(this->bandsPerOctave * std::log2f(this->lowerLimitOfFreqRange / this->refFrequency));
	int upperN = (int)std::roundf(this->bandsPerOctave * std::log2f(this->upperLimitOfFreqRange / this->refFrequency));

	for (int n = lowerN; n <= upperN; n++) {
		float centerFreq = this->refFrequency * std::powf(2, ((float)n / bandsPerOctave));
		this->centerFreqsOfBands.push_back(centerFreq);
	}
}

std::vector<float> OctaveBandAnalyser::getCenterFreqsOfBands()
{
	return this->centerFreqsOfBands;
}

void OctaveBandAnalyser::analyseFrames(const std::vector<float>& inputFrames, std::vector<float>& outputVals)
{
	for (int i = 0; i < this->centerFreqsOfBands.size(); i++) {
		float centerFreqOfBand = this->centerFreqsOfBands[i];
		float bandConst = std::sqrtf(std::powf(2, (1.0f / this->bandsPerOctave)));
		float lowerCutoffFreqOfBand = centerFreqOfBand / bandConst;
		float upperCutoffFreqOfBand = centerFreqOfBand * bandConst;

		this->bandPassFilter.setup(this->filterOrder, this->sampleRate, centerFreqOfBand, upperCutoffFreqOfBand - lowerCutoffFreqOfBand);
		
		float squaredSum = 0;
		for (int i = 0; i < inputFrames.size(); i++) {
			float filteredFrame = this->bandPassFilter.filter(inputFrames[i]);
			squaredSum += std::powf(filteredFrame, 2);
		}

		float rms = std::sqrtf(squaredSum / inputFrames.size());

		const float floorVal = 1e-5;
		float dBFS = -((20 * std::log10f(rms + floorVal)) - 20*std::log10f(floorVal));	// 1e-6 = floor value

		outputVals.push_back(dBFS);
	}
}