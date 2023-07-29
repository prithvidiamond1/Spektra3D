
#include "ALB.h"

ma_uint32 AudioLoopBack::ALB_sampleCountPerCallback;

void AudioLoopBack::ALB_displayVector(const std::vector<float>& inputVector)
{
	for (float value : inputVector) {
		std::cout << value << " ";
	}
	std::cout << std::endl;
}

void AudioLoopBack::ALB_dataCallback(ma_device* device, void* output, const void* input, ma_uint32 frameCount)
{
	ALB_sampleCountPerCallback = frameCount;
	device->pUserData = const_cast<void*>(input);

	(void)output;
}

void AudioLoopBack::ALB_start()
{
	ma_device_config ALB_deviceConfig = ma_device_config_init(ma_device_type_loopback);
	ALB_deviceConfig.capture.pDeviceID = NULL; //change this to capture from a different device
	ALB_deviceConfig.capture.format = ALB_captureFormat;
	ALB_deviceConfig.capture.channels = ALB_captureChannelCount;
	ALB_deviceConfig.sampleRate = ALB_captureSampleRate;
	ALB_deviceConfig.dataCallback = ALB_dataCallback;
	ALB_deviceConfig.pUserData = ALB_rawData;
	ALB_deviceConfig.periodSizeInMilliseconds = ALB_callbackPeriodMs;

	/* Loopback mode is currently only supported on WASAPI. */
	ma_backend backends[] = {
		ma_backend_wasapi
	};

	ma_result ALB_result = ma_device_init_ex(backends, sizeof(backends) / sizeof(backends[0]), NULL, &ALB_deviceConfig, &ALB_device);
	if (ALB_result != MA_SUCCESS) {
		// Error
		std::cout << "Error: MA_DEVICE_INIT failed!" << std::endl;
		return;
	}

	ALB_result = ma_device_start(&ALB_device);
	if (ALB_result != MA_SUCCESS) {
		ma_device_uninit(&ALB_device);
		// Error
		std::cout << "Error: MA_DEVICE_START failed!" << std::endl;
		return;
	}
}

void AudioLoopBack::ALB_getAudioData(std::vector<float>& leftChData, std::vector<float>& rightChData)
{
	// EVENTS HERE MUST OCCUR AS PART OF A LOOP
	void* ALB_deinterleavedLeftData = new float[ALB_sampleCountPerCallback];
	void* ALB_deinterleavedRightData = new float[ALB_sampleCountPerCallback];

	void** ALB_deinterleavedData = new void* [ALB_captureChannelCount];
	ALB_deinterleavedData[0] = ALB_deinterleavedLeftData;
	ALB_deinterleavedData[1] = ALB_deinterleavedRightData;

	ma_deinterleave_pcm_frames(ALB_captureFormat, ALB_captureChannelCount, ALB_sampleCountPerCallback, ALB_device.pUserData, ALB_deinterleavedData);

	float* ALB_castedLeftData = static_cast<float*>(ALB_deinterleavedLeftData);
	float* ALB_castedRightData = static_cast<float*>(ALB_deinterleavedRightData);

	leftChData = std::vector<float>(ALB_castedLeftData, ALB_castedLeftData + ALB_sampleCountPerCallback);
	rightChData = std::vector<float>(ALB_castedRightData, ALB_castedRightData + ALB_sampleCountPerCallback);

	delete[] ALB_deinterleavedData;
	delete[] ALB_deinterleavedLeftData;
	delete[] ALB_deinterleavedRightData;
}

void AudioLoopBack::ALB_finish()
{
	ma_device_uninit(&ALB_device);
}
