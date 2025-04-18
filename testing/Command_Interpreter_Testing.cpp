#include "Command_Interpreter.h"
#include <gtest/gtest.h>

TEST(CommandInterpreterTest, CreateCommandInterpreter) {
    testing::internal::CaptureStdout();

    auto pinNumbers = std::vector<int>{4, 5, 2, 3, 9, 7, 8, 6};

    auto pins = std::vector<PwmPin *>{};

    for (int pinNumber : pinNumbers) {
        pins.push_back(new HardwarePwmPin(pinNumber));
    }

    auto interpreter = new Command_Interpreter_RPi5(pins, std::vector<DigitalPin *>{});
    interpreter->initializePins();
    auto pinStatus = interpreter->readPins();
    std::string output = testing::internal::GetCapturedStdout();

    delete interpreter;

    std::string expectedOutput;
    for (int pinNumber : pinNumbers) {
        expectedOutput.append("Configure ");
        expectedOutput.append(std::to_string(pinNumber));
        expectedOutput.append(" HardPwm\nSet ");
        expectedOutput.append(std::to_string(pinNumber));
        expectedOutput.append(" PWM 1500\n");
    }
    ASSERT_EQ(pinStatus.size(), 8);
    ASSERT_EQ(pinStatus, (std::vector<int>{1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500}));
    ASSERT_EQ(output, expectedOutput);
}

TEST(CommandInterpreterTest, CreateCommandInterpreterWithDigitalPins) {
    testing::internal::CaptureStdout();

    auto pinNumbers = std::vector<int>{4, 5, 2, 3, 9, 7, 8, 6};

    auto pwmPins = std::vector<PwmPin *>{};

    for (int pinNumber : pinNumbers) {
        pwmPins.push_back(new HardwarePwmPin(pinNumber));
    }

    auto digital1 = new DigitalPin(8, ActiveLow);
    auto digital2 = new DigitalPin(9, ActiveHigh);
    auto digitalPins = std::vector<DigitalPin*>{digital1, digital2};

    auto interpreter = new Command_Interpreter_RPi5(pwmPins, digitalPins);
    interpreter->initializePins();
    std::string output = testing::internal::GetCapturedStdout();
    auto pinStatus = interpreter->readPins();

    delete interpreter;

    std::string expectedOutput;
    for (int pinNumber : pinNumbers) {
        expectedOutput.append("Configure ");
        expectedOutput.append(std::to_string(pinNumber));
        expectedOutput.append(" HardPwm\nSet ");
        expectedOutput.append(std::to_string(pinNumber));
        expectedOutput.append(" PWM 1500\n");
    }
    expectedOutput.append("Configure 8 Digital\nSet 8 Digital High\n");
    expectedOutput.append("Configure 9 Digital\nSet 9 Digital Low\n");

    ASSERT_EQ(pinStatus.size(), 10);
    ASSERT_EQ(pinStatus, (std::vector<int>{1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1, 0}));
    ASSERT_EQ(output, expectedOutput);
}

TEST(CommandInterpreterTest, BlindExecuteHardwarePwm) {
    testing::internal::CaptureStdout();

    const CommandComponent acceleration = {1500, 1900, 1100,
                                     1250, 1300, 1464, 1535,
                                     1536, std::chrono::milliseconds(2000)};

    auto pinNumbers = std::vector<int>{4, 5, 2, 3, 9, 7, 8, 6};

    auto pins = std::vector<PwmPin *>{};

    for (int pinNumber : pinNumbers) {
        pins.push_back(new HardwarePwmPin(pinNumber));
    }

    std::ofstream logFile = std::ofstream("/dev/null");

    auto interpreter = new Command_Interpreter_RPi5(pins, std::vector<DigitalPin*>{});
    interpreter->initializePins();
    auto startTime = std::chrono::system_clock::now();
    interpreter->blind_execute(acceleration, logFile);
    auto endTime = std::chrono::system_clock::now();
    std::string output = testing::internal::GetCapturedStdout();
    auto pinStatus = interpreter->readPins();

    delete interpreter;

    std::string expectedOutput;
    for (int pinNumber : pinNumbers) {
        expectedOutput.append("Configure ");
        expectedOutput.append(std::to_string(pinNumber));
        expectedOutput.append(" HardPwm\nSet ");
        expectedOutput.append(std::to_string(pinNumber));
        expectedOutput.append(" PWM 1500\n");
    }
    expectedOutput.append("Set 4 PWM 1500\n");
    expectedOutput.append("Set 5 PWM 1900\n");
    expectedOutput.append("Set 2 PWM 1100\n");
    expectedOutput.append("Set 3 PWM 1250\n");
    expectedOutput.append("Set 9 PWM 1300\n");
    expectedOutput.append("Set 7 PWM 1464\n");
    expectedOutput.append("Set 8 PWM 1535\n");
    expectedOutput.append("Set 6 PWM 1536\n");

    ASSERT_NEAR((endTime - startTime) / std::chrono::milliseconds(1), std::chrono::milliseconds(2000) /
        std::chrono::milliseconds(1), std::chrono::milliseconds(10) / std::chrono::milliseconds(1));
    ASSERT_EQ(pinStatus, (std::vector<int>{1500, 1900, 1100, 1250, 1300, 1464, 1535, 1536}));
    ASSERT_EQ(output, expectedOutput);
}

TEST(CommandInterpreterTest, BlindExecuteSoftwarePwm) {
    testing::internal::CaptureStdout();

    const CommandComponent acceleration = {1500, 1900, 1100,
                                           1250, 1300, 1464, 1535,
                                           1536, std::chrono::milliseconds(2000)};

    auto pinNumbers = std::vector<int>{4, 5, 2, 3, 9, 7, 8, 6};

    auto pins = std::vector<PwmPin *>{};

    for (int pinNumber : pinNumbers) {
        pins.push_back(new SoftwarePwmPin(pinNumber));
    }

    std::ofstream logFile = std::ofstream("/dev/null");

    auto interpreter = new Command_Interpreter_RPi5(pins, std::vector<DigitalPin*>{});
    interpreter->initializePins();
    auto startTime = std::chrono::system_clock::now();
    interpreter->blind_execute(acceleration, logFile);
    auto endTime = std::chrono::system_clock::now();
    std::string output = testing::internal::GetCapturedStdout();
    auto pinStatus = interpreter->readPins();

    delete interpreter;

    std::string expectedOutput;
    for (int pinNumber : pinNumbers) {
        expectedOutput.append("Configure ");
        expectedOutput.append(std::to_string(pinNumber));
        expectedOutput.append(" SoftPwm\nSet ");
        expectedOutput.append(std::to_string(pinNumber));
        expectedOutput.append(" PWM 1500\n");
    }
    expectedOutput.append("Set 4 PWM 1500\n");
    expectedOutput.append("Set 5 PWM 1900\n");
    expectedOutput.append("Set 2 PWM 1100\n");
    expectedOutput.append("Set 3 PWM 1250\n");
    expectedOutput.append("Set 9 PWM 1300\n");
    expectedOutput.append("Set 7 PWM 1464\n");
    expectedOutput.append("Set 8 PWM 1535\n");
    expectedOutput.append("Set 6 PWM 1536\n");

    ASSERT_NEAR((endTime - startTime) / std::chrono::milliseconds(1), std::chrono::milliseconds(2000) /
        std::chrono::milliseconds(1), std::chrono::milliseconds(10) / std::chrono::milliseconds(1));

    ASSERT_EQ(pinStatus, (std::vector<int>{1500, 1900, 1100, 1250, 1300, 1464, 1535, 1536}));
    ASSERT_EQ(output, expectedOutput);
}

