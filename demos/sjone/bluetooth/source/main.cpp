#include <string_view>

#include "devices/communication/bluetooth/zs040.hpp"
#include "peripherals/lpc17xx/gpio.hpp"
#include "peripherals/lpc17xx/uart.hpp"
#include "utility/log.hpp"

namespace
{
constexpr std::string_view kBleName = "Some Device";
constexpr auto kBleRole             = sjsu::bluetooth::Zs040::Role::kMaster;
}  // namespace

int main()
{
  sjsu::LogInfo("Starting Bluetooth (UART) Example...");

  sjsu::lpc17xx::Uart & uart2 = sjsu::lpc17xx::GetUart<2>();
  sjsu::lpc17xx::Gpio state_pin(0, 0);
  sjsu::lpc17xx::Gpio key_pin(0, 1);

  sjsu::bluetooth::Zs040 zs040(uart2, key_pin, state_pin);

  zs040.Initialize();
  // zs040.Enable();

    sjsu::LogInfo("Version:     %.*s", zs040.GetVersion());
    // sjsu::LogInfo("Baud:        %c", sjsu::Value(zs040.GetBaudRate()));
    sjsu::LogInfo("Role:        %c", sjsu::Value(zs040.GetRole()));
    // sjsu::LogInfo("Role:        %c", sjsu::Value(zs040.SetRole(kBleRole)));
    sjsu::LogInfo("MAC Address: %.*s", zs040.GetMacAddress());
    sjsu::LogInfo("Set Name:    %.*s", zs040.SetDeviceName(kBleName));
    sjsu::LogInfo("Device Name: %.*s", zs040.GetDeviceName());
    sjsu::LogInfo("UUID:        %.*s", zs040.GetUuid());
  try
  {
    // sjsu::LogInfo("State:       %.*s", zs040.SendCommand("+STATE"));
    // sjsu::LogInfo("Baud:       %.*s",
    //               zs040.SendCommand("+BAUD", { "2", "0", "0" }));

    // constexpr auto kDataRate = sjsu::bluetooth::Zs040::BaudRate::kB9600;
    // sjsu::LogInfo("Set Baud:    %c",
    // sjsu::Value(zs040.SetBaudRate(kDataRate)));

    zs040.Inquire();
  }
  catch (const std::string_view error)
  {
    sjsu::LogInfo("Inq error %.*s", error);
  }
  catch (const std::system_error & error)
  {
    sjsu::LogInfo("Inq sys err: %s", error.what());
  }

  sjsu::Halt();

  return 0;
}
