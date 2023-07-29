#pragma once

#include "miniaudio.h"

#include <iostream>
#include <vector>
#include <cmath>

class AudioLoopBack {
private:
	void* ALB_rawData;
	ma_device ALB_device;

public:
	const int ALB_callbackPeriodMs = 64;
	const int ALB_captureSampleRate = 48000;
	const int ALB_captureChannelCount = 2;
	const ma_format ALB_captureFormat = ma_format_f32;
	
	static ma_uint32 ALB_sampleCountPerCallback;

	static void ALB_dataCallback(ma_device* ALB_device, void* ALB_output, const void* ALB_input, ma_uint32 frameCount);

	static void ALB_displayVector(const std::vector<float>& inputVector);

	void ALB_start();
	void ALB_getAudioData(std::vector<float>& leftChData, std::vector<float>& rightChData);
	void ALB_finish();
};