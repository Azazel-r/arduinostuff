/*
 * ESP32PWM.cpp
 *
 *  Created on: Sep 22, 2018
 *      Author: hephaestus
 */

#include <ESP32PWM.h>
#include "esp32-hal-ledc.h"

// initialize the class variable ServoCount
int ESP32PWM::PWMCount = -1;              // the total number of attached servos
bool  ESP32PWM::explicateAllocationMode=false;
ESP32PWM * ESP32PWM::ChannelUsed[NUM_PWM]; // used to track whether a channel is in service
long ESP32PWM::timerFreqSet[4] = { -1, -1, -1, -1 };
int ESP32PWM::timerCount[4] = { 0, 0, 0, 0 };

static const char* TAG = "ESP32PWM";

// The ChannelUsed array elements are 0 if never used, 1 if in use, and -1 if used and disposed
// (i.e., available for reuse)
/**
 * allocateTimer
 * @param a timer number 0-3 indicating which timer to allocate in this library
 * Switch to explicate allocation mode
 *
 */
void ESP32PWM::allocateTimer(int timerNumber){
	if(timerNumber<0 || timerNumber>3)
		return;
	if(ESP32PWM::explicateAllocationMode==false){
		ESP32PWM::explicateAllocationMode=true;
		for(int i=0;i<4;i++)
			ESP32PWM::timerCount[i]=4;// deallocate all timers to start mode
	}
	ESP32PWM::timerCount[timerNumber]=0;
}

ESP32PWM::ESP32PWM() {
	resolutionBits = 8;
	pwmChannel = -1;
	pin = -1;
	myFreq = -1;
	if (PWMCount == -1) {
		for (int i = 0; i < NUM_PWM; i++)
			ChannelUsed[i] = NULL; // load invalid data into the storage array of pin mapping
		PWMCount = PWM_BASE_INDEX; // 0th channel does not work with the PWM system
	}
}

ESP32PWM::~ESP32PWM() {
	if (attached()) {
#ifdef ESP_ARDUINO_VERSION_MAJOR
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
		ledcDetach(pin);
#else
		ledcDetachPin(pin);
#endif
#else
		ledcDetachPin(pin);
#endif
	}
	deallocate();
}

double ESP32PWM::_ledcSetupTimerFreq(uint8_t pin, double freq,
		uint8_t bit_num, uint8_t channel) {

#ifdef ESP_ARDUINO_VERSION_MAJOR
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
	return ledcAttachChannel(pin, freq, bit_num, channel);
#else
	return ledcSetup(channel, freq, bit_num);
#endif
#else
	return ledcSetup(channel, freq, bit_num);
#endif

}

int ESP32PWM::timerAndIndexToChannel(int timerNum, int index) {
	int localIndex = 0;
	for (int j = 0; j < NUM_PWM; j++) {
		if (((j / 2) % 4) == timerNum) {
			if (localIndex == index) {
				return j;
			}
			localIndex++;
		}
	}
	return -1;
}
int ESP32PWM::allocatenext(double freq) {
	long freqlocal = (long) freq;
	if (pwmChannel < 0) {
		for (int i = 0; i < 4; i++) {
			bool freqAllocated = ((timerFreqSet[i] == freqlocal)
					|| (timerFreqSet[i] == -1));
			if (freqAllocated && timerCount[i] < 4) {
				if (timerFreqSet[i] == -1) {
					//Serial.println("Starting timer "+String(i)+" at freq "+String(freq));
					timerFreqSet[i] = freqlocal;
				}
				//Serial.println("Free channel timer "+String(i)+" at freq "+String(freq)+" remaining "+String(4-timerCount[i]));

				timerNum = i;
				for (int index=0; index<4; ++index)
				{
					int myTimerNumber = timerAndIndexToChannel(timerNum,index);
					if ((myTimerNumber >= 0)  && (!ChannelUsed[myTimerNumber]))
					{
						pwmChannel = myTimerNumber;
// 						Serial.println(
// 							"PWM on ledc channel #" + String(pwmChannel)
// 									+ " using 'timer " + String(timerNum)
// 									+ "' to freq " + String(freq) + "Hz");
						ChannelUsed[pwmChannel] = this;
						timerCount[timerNum]++;
						PWMCount++;
						myFreq = freq;
						return pwmChannel;
					}
				}
			} else {
//				if(timerFreqSet[i]>0)
//					Serial.println("Timer freq mismatch target="+String(freq)+" on timer "+String(i)+" was "+String(timerFreqSet[i]));
//				else
//					Serial.println("Timer out of channels target="+String(freq)+" on timer "+String(i)+" was "+String(timerCount[i]));
			}
		}
	} else {
		return pwmChannel;
	}
	ESP_LOGE(TAG, 
			"ERROR All PWM timers allocated! Can't accomodate %.3f Hz\r\nHalting...", freq);
	while (1)
		;
}
void ESP32PWM::deallocate() {
	if (pwmChannel < 0)
		return;
	ESP_LOGE(TAG, "PWM deallocating LEDc #%d",pwmChannel);
	timerCount[getTimer()]--;
	if (timerCount[getTimer()] == 0) {
		timerFreqSet[getTimer()] = -1; // last pwn closed out
	}
	timerNum = -1;
	attachedState = false;
	ChannelUsed[pwmChannel] = NULL;
	pwmChannel = -1;
	PWMCount--;

}

int ESP32PWM::getChannel() {
	if (pwmChannel < 0) {
		ESP_LOGE(TAG, "FAIL! must setup() before using get channel!");
	}
	return pwmChannel;
}

double ESP32PWM::setup(double freq, uint8_t resolution_bits) {
	checkFrequencyForSideEffects(freq);

	resolutionBits = resolution_bits;
	if (attached()) {
#ifdef ESP_ARDUINO_VERSION_MAJOR
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
		ledcDetach(pin);
		double val = ledcAttachChannel(getPin(), freq, resolution_bits, getChannel());
#else
		ledcDetachPin(pin);
		double val = ledcSetup(getChannel(), freq, resolution_bits);
#endif
#else
		ledcDetachPin(pin);
		double val = ledcSetup(getChannel(), freq, resolution_bits);
#endif

		attachPin(pin);
		return val;
	}
#ifdef ESP_ARDUINO_VERSION_MAJOR
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
	return ledcAttachChannel(getPin(), freq, resolution_bits, getChannel());
#else
	return ledcSetup(getChannel(), freq, resolution_bits);
#endif
#else
	return ledcSetup(getChannel(), freq, resolution_bits);
#endif
}
double ESP32PWM::getDutyScaled() {
	return mapf((double) myDuty, 0, (double) ((1 << resolutionBits) - 1), 0.0,
			1.0);
}
void ESP32PWM::writeScaled(double duty) {
	write(mapf(duty, 0.0, 1.0, 0, (double) ((1 << resolutionBits) - 1)));
}
void ESP32PWM::write(uint32_t duty) {
	myDuty = duty;
#ifdef ESP_ARDUINO_VERSION_MAJOR
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
	ledcWrite(getPin(), duty);
#else
	ledcWrite(getChannel(), duty);
#endif
#else
	ledcWrite(getChannel(), duty);
#endif
}
void ESP32PWM::adjustFrequencyLocal(double freq, double dutyScaled) {
	timerFreqSet[getTimer()] = (long) freq;
	myFreq = freq;
	if (attached()) {
#ifdef ESP_ARDUINO_VERSION_MAJOR
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
		ledcDetach(pin);
		// Remove the PWM during frequency adjust
		_ledcSetupTimerFreq(getPin(), freq, resolutionBits, getChannel());
		writeScaled(dutyScaled);
		ledcAttachChannel(getPin(), freq, resolutionBits, getChannel()); // re-attach the pin after frequency adjust
#else
		ledcDetachPin(pin);
		// Remove the PWM during frequency adjust
		_ledcSetupTimerFreq(getPin(), freq, resolutionBits, getChannel());
		writeScaled(dutyScaled);
		ledcAttachPin(pin, getChannel()); // re-attach the pin after frequency adjust
#endif
#else
		ledcDetachPin(pin);
		// Remove the PWM during frequency adjust
		_ledcSetupTimerFreq(getPin(), freq, resolutionBits, getChannel());
		writeScaled(dutyScaled);
		ledcAttachPin(pin, getChannel()); // re-attach the pin after frequency adjust
#endif

	} else {
		_ledcSetupTimerFreq(getPin(), freq, resolutionBits, getChannel());
		writeScaled(dutyScaled);
	}
}
void ESP32PWM::adjustFrequency(double freq, double dutyScaled) {
	if(dutyScaled<0)
		dutyScaled=getDutyScaled();
	writeScaled(dutyScaled);
	for (int i = 0; i < timerCount[getTimer()]; i++) {
		int pwm = timerAndIndexToChannel(getTimer(), i);
		if (ChannelUsed[pwm] != NULL) {
			if (ChannelUsed[pwm]->myFreq != freq) {
				ChannelUsed[pwm]->adjustFrequencyLocal(freq,
						ChannelUsed[pwm]->getDutyScaled());
			}
		}
	}
}
double ESP32PWM::writeTone(double freq) {
	for (int i = 0; i < timerCount[getTimer()]; i++) {
		int pwm = timerAndIndexToChannel(getTimer(), i);
		if (ChannelUsed[pwm] != NULL) {
			if (ChannelUsed[pwm]->myFreq != freq) {
				ChannelUsed[pwm]->adjustFrequencyLocal(freq,
						ChannelUsed[pwm]->getDutyScaled());
			}
			write(1 << (resolutionBits-1)); // writeScaled(0.5);
		}
	}

	return 0;
}
double ESP32PWM::writeNote(note_t note, uint8_t octave) {
	const uint16_t noteFrequencyBase[12] = {
			//   C        C#       D        Eb       E        F       F#        G       G#        A       Bb        B
			4186, 4435, 4699, 4978, 5274, 5588, 5920, 6272, 6645, 7040, 7459,
			7902 };

	if (octave > 8 || note >= NOTE_MAX) {
		return 0;
	}
	double noteFreq = (double) noteFrequencyBase[note]
			/ (double) (1 << (8 - octave));
	return writeTone(noteFreq);
}
uint32_t ESP32PWM::read() {
#ifdef ESP_ARDUINO_VERSION_MAJOR
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
	return ledcRead(getPin());
#else
	return ledcRead(getChannel());
#endif
#else
	return ledcRead(getChannel());
#endif

}
double ESP32PWM::readFreq() {
	return myFreq;
}
void ESP32PWM::attach(int p) {
	pin = p;
	attachedState = true;
}
void ESP32PWM::attachPin(uint8_t pin) {

	if (hasPwm(pin)) {
		attach(pin);
		bool success=true;
#ifdef ESP_ARDUINO_VERSION_MAJOR
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
		success=ledcAttachChannel(pin, readFreq(), resolutionBits, getChannel());
#else
		ledcAttachPin(pin, getChannel());
#endif
#else
		ledcAttachPin(pin, getChannel());
#endif
		if(success)
			return;
		ESP_LOGE(TAG, "ERROR PWM channel failed to configure on pin %d!", pin);
		return;
	}
		
#if defined(CONFIG_IDF_TARGET_ESP32S2)
						ESP_LOGE(TAG, "ERROR PWM channel unavailable on pin requested! %d PWM available on: 1-21,26,33-42",pin);
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
						ESP_LOGE(TAG, "ERROR PWM channel unavailable on pin requested! %d PWM available on: 1-21,35-45,47-48",pin);
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
						ESP_LOGE(TAG, "ERROR PWM channel unavailable on pin requested! %d PWM available on: 1-10,18-21",pin);
#else
						ESP_LOGE(TAG, "ERROR PWM channel unavailable on pin requested! %d PWM available on: 2,4,5,12-19,21-23,25-27,32-33",pin);
#endif

}
void ESP32PWM::attachPin(uint8_t pin, double freq, uint8_t resolution_bits) {

	if (hasPwm(pin)){
        this->pin = pin;
		int ret=setup(freq, resolution_bits);
		ESP_LOGW(TAG, "Pin Setup %d with code %d",pin,ret);
	    attach(pin);
	}
	else
		ESP_LOGE(TAG, "ERROR Pin Failed %d ",pin);
}
void ESP32PWM::detachPin(int pin) {
#ifdef ESP_ARDUINO_VERSION_MAJOR
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)

	ledcDetach(pin);
#else
	ledcDetachPin(pin);
#endif
#else
	ledcDetachPin(pin);
#endif
	deallocate();
}
/* Side effects of frequency changes happen because of shared timers
 *
 * LEDC Chan to Group/Channel/Timer Mapping
 ** ledc: 0  => Group: 0, Channel: 0, Timer: 0
 ** ledc: 1  => Group: 0, Channel: 1, Timer: 0
 ** ledc: 2  => Group: 0, Channel: 2, Timer: 1
 ** ledc: 3  => Group: 0, Channel: 3, Timer: 1
 ** ledc: 4  => Group: 0, Channel: 4, Timer: 2
 ** ledc: 5  => Group: 0, Channel: 5, Timer: 2
 ** ledc: 6  => Group: 0, Channel: 6, Timer: 3
 ** ledc: 7  => Group: 0, Channel: 7, Timer: 3
 ** ledc: 8  => Group: 1, Channel: 0, Timer: 0
 ** ledc: 9  => Group: 1, Channel: 1, Timer: 0
 ** ledc: 10 => Group: 1, Channel: 2, Timer: 1
 ** ledc: 11 => Group: 1, Channel: 3, Timer: 1
 ** ledc: 12 => Group: 1, Channel: 4, Timer: 2
 ** ledc: 13 => Group: 1, Channel: 5, Timer: 2
 ** ledc: 14 => Group: 1, Channel: 6, Timer: 3
 ** ledc: 15 => Group: 1, Channel: 7, Timer: 3
 */

bool ESP32PWM::checkFrequencyForSideEffects(double freq) {

	allocatenext(freq);
	for (int i = 0; i < timerCount[getTimer()]; i++) {
		int pwm = timerAndIndexToChannel(getTimer(), i);

		if (pwm == pwmChannel)
			continue;
		if (ChannelUsed[pwm] != NULL)
			if (ChannelUsed[pwm]->getTimer() == getTimer()) {
				double diff = abs(ChannelUsed[pwm]->myFreq - freq);
				if (abs(diff) > 0.1) {
					ESP_LOGW(TAG, 
							"\tWARNING PWM channel %d	\
							 shares a timer with channel %d\n	\
							\tchanging the frequency to %.3f		\
							Hz will ALSO change channel %d	\
							\n\tfrom its previous frequency of %.3f Hz\n "
								,pwmChannel, pwm, freq, pwm, ChannelUsed[pwm]->myFreq);
					ChannelUsed[pwm]->myFreq = freq;
				}
			}
	}
	return true;
}

ESP32PWM* pwmFactory(int pin) {
	for (int i = 0; i < NUM_PWM; i++)
		if (ESP32PWM::ChannelUsed[i] != NULL) {
			if (ESP32PWM::ChannelUsed[i]->getPin() == pin)
				return ESP32PWM::ChannelUsed[i];
		}
	return NULL;
}
