#include "hal/gpio.hpp"
#include <unistd.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include "common/utils.hpp"

static constexpr const char* GPIO_PATH = "/sys/class/gpio/gpio";
static constexpr const char* GPIO_EXPORT = "/export";
static constexpr const char* GPIO_VALUE = "/value";
static constexpr const char* GPIO_ACTIVE_LOW = "/active_low";
static constexpr const char* GPIO_DIRECTION = "/direction";

void GPIO::exportPin(int pin) {
	std::ostringstream pathStream;
	pathStream << GPIO_PATH << pin << GPIO_VALUE;
	std::string path = pathStream.str();

	if (std::filesystem::exists(path)) return;

	std::ofstream exportFile(std::string(GPIO_PATH) + GPIO_EXPORT);
	if (exportFile.is_open()) {
		exportFile << pin;
		exportFile.close();
	} else {
		std::cerr << "Error opening GPIO export file for pin " << pin << std::endl;
	}

	// wait for pin to be exported
	sleepForMs(300);
}

void GPIO::configPin(headerType header, int pin, std::string setting) {
	std::stringstream commandStream;
	commandStream << "config-pin p" << header << "." << pin << " " << setting;
	runCommand(commandStream.str(), false);
}

int GPIO::getPinValue(int pin) {
	std::ostringstream pathStream;
	pathStream << GPIO_PATH << pin << GPIO_VALUE;
	std::string path = pathStream.str();

	std::ifstream valueFile(path);
	if (valueFile.is_open()) {
		int value;
		valueFile >> value;
		valueFile.close();
		return value;
	} else {
		std::cerr << "Error opening GPIO value file for pin " << pin << std::endl;
		return -1;
	}
}

void GPIO::setPinValue(int pin, int value) {
	std::ostringstream pathStream;
	pathStream << GPIO_PATH << pin << GPIO_VALUE;
	std::string path = pathStream.str();

	std::ofstream valueFile(path);
	if (valueFile.is_open()) {
		valueFile << value;
		valueFile.close();
	} else {
		std::cerr << "Error opening GPIO value file for pin " << pin << std::endl;
	}
}

void GPIO::setPinActiveLow(int pin, int activeLow) {
	std::ostringstream pathStream;
	pathStream << GPIO_PATH << pin << GPIO_ACTIVE_LOW;
	std::string path = pathStream.str();

	std::ofstream activeLowFile(path);
	if (activeLowFile.is_open()) {
		activeLowFile << activeLow;
		activeLowFile.close();
	} else {
		std::cerr << "Error opening GPIO active_low file for pin " << pin << std::endl;
	}
}

void GPIO::setPinDirection(int pin, std::string direction) {
	std::ostringstream pathStream;
	pathStream << GPIO_PATH << pin << GPIO_DIRECTION;
	std::string path = pathStream.str();

	std::ofstream directionFile(path);
	if (directionFile.is_open()) {
		directionFile << direction;
		directionFile.close();
	} else {
		std::cerr << "Error opening GPIO direction file for pin " << pin << std::endl;
	}
}

bool GPIO::isPinActive(int pin) {
	return getPinValue(pin) == 1;
}
