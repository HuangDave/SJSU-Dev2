#pragma once

#include <span>
#include <string_view>

#include "devices/communication/serial_uart.hpp"
#include "module.hpp"
#include "peripherals/gpio.hpp"

/// https://godbolt.org/z/jqcx8r

namespace sjsu
{
namespace bluetooth
{
class Zs040 final : public sjsu::Module<>
{
 public:
  enum class State : uint8_t
  {
    kAtCommand = 0,
    kData      = 1,
  };

  enum class BaudRate : char
  {
    kB115200 = '0',
    kB57600  = '1',
    kB38400  = '2',  // actual 9600
    kB19200  = '3',
    kB9600   = '4',  // actual 38400
  };

  enum class Role : char
  {
    kSlave  = '0',
    kMaster = '1',
    kSensor = '2',
    kBeacon = '3',
    kWeChat = '4',
  };

  enum class AuthType : char
  {
    kNoPassword        = '0',
    kPairing           = '1',
    kPairingAndBinding = '2',
  };

  struct ScanResult_t
  {
    /// Device index
    uint8_t idx;
    /// 6-byte MAC address
    char mac_address[12];
    /// The signal strength in units of dBm.
    char signal_strength[4];
  };

  struct Command  // NOLINT
  {
    /// Test command to determine if in AT mode
    static constexpr std::string_view kAt = "";

    /// Reset device to factory settings
    static constexpr std::string_view kFactoryReset = "+RENEW";

    /// Restart the device
    static constexpr std::string_view kSoftwareReset = "+RESET";

    /// Put the device in low-power mode.
    static constexpr std::string_view kSleep = "+SLEEP";

    /// Get the device's firmware version.
    static constexpr std::string_view kVersion = "+VERSION";

    /// Get the device's MAC address.
    static constexpr std::string_view kMacAddress = "+LADDR";

    /// Get/Set the device's configured baud rate for serial communication.
    static constexpr std::string_view kBaud = "+BAUD";

    /// Get/Set the device's current role.
    static constexpr std::string_view kRole = "+ROLE";

    ///
    static constexpr std::string_view kUuid = "+UUID";

    ///
    static constexpr std::string_view kCharacteristic = "+CHAR";

    ///
    static constexpr std::string_view kDeviceName = "+NAME";

    ///
    static constexpr std::string_view kPin = "+PIN";

    ///
    static constexpr std::string_view kScanDevices = "+INQ";

    ///
    static constexpr std::string_view kConnect = "+CONN";

    ///
    static constexpr std::string_view kIac = "+IAC";
  };

  static constexpr size_t kAtResponseBufferLength            = 30;
  static constexpr uint32_t kDefaultBaudRate                 = 9'600;
  static constexpr std::chrono::microseconds kDefaultTimeout = 500ms;

  /// Carriage return (CR) and line feed (LF).
  static constexpr std::string_view kCrLf = "\r\n";

  static constexpr std::string_view kAtResponseOk = "OK\r\n";

  /// Parses an AT Command response that has the following format and returns
  /// the value as an std::string_view:
  ///
  ///   1. <command>=<value>\r\n
  ///   2. <command>=<value>\r\nOK\r\n
  ///   3. <value>\r\n
  ///   4. OK\r\n
  ///
  /// @param buffer -
  /// @param command -
  /// @param cr_lf -
  /// @param check_for_ok -
  /// @returns The parsed response value or an empty string if response contains
  ///          only OK.
  static const std::string_view Parse(const std::string_view & buffer,
                                      const std::string_view command = "",
                                      const std::string_view cr_lf   = "\r\n",
                                      const bool check_for_ok        = false)
  {
    constexpr std::string_view kResponseOk  = "OK\r\n";
    constexpr std::string_view kErrorPrefix = "ERROR=";

    std::string_view value(buffer);

    if (value.starts_with(kErrorPrefix))
    {
      value.remove_prefix(
          std::min(value.find_first_not_of(kErrorPrefix), value.size()));
      value.remove_suffix(value.size() - value.find(cr_lf));
      // throw(std::errc::invalid_argument);
    }

    if (!command.empty())
    {
      value.remove_prefix(
          std::min(value.find_first_not_of(command), value.size()));
      value.remove_prefix(std::min(value.find_first_not_of("="), value.size()));
    }

    if (check_for_ok)
    {
      value.remove_suffix(value.size() - value.find(kResponseOk));
    }

    value.remove_suffix(value.size() - value.find(cr_lf));

    return value;
  }

  /// @param uart The UART peripheral used to communicate with the device.
  /// @param key_pion The device's chip enable pin. When driven low, the
  ///                 device will disconnect any connected bluetooth devices.
  /// @param state_pin The device's state pin.
  explicit Zs040(
      sjsu::Uart & uart,
      sjsu::Gpio & key_pin   = sjsu::GetInactive<sjsu::Gpio>(),
      sjsu::Gpio & state_pin = sjsu::GetInactive<sjsu::Gpio>()) noexcept
      : uart_(uart),
        serial_uart_({ uart_ }),
        key_pin_(key_pin),
        state_pin_(state_pin)
  {
  }

  virtual void ModuleInitialize() override
  {
    state_pin_.SetAsInput();
    key_pin_.SetAsOutput();
    key_pin_.SetHigh();

    uart_.settings.baud_rate = kDefaultBaudRate;
    uart_.Initialize();
  }

  // bool GetDeviceState() const
  // {
  //   return state_pin_.Read();
  // }

  // void EnterAtMode() const
  // {
  //   key_pin_.SetHigh();
  // }

  // void ExitAtMode() const
  // {
  //   key_pin_.SetLow();
  // }

  void SoftwareReset()
  {
    SendCommand(Command::kSoftwareReset);
    sjsu::Delay(500ms);
  }

  /// @returns The device's version.
  const std::string_view GetVersion()
  {
    return SendCommand(Command::kVersion);
  }

  /// @returns The device's MAC address as a string in the following format:
  ///          XX:XX:XX:XX:XX:XX.
  const std::string_view GetMacAddress()
  {
    return SendCommand(Command::kMacAddress);
  }

  Role SetRole(Role role)
  {
    const char kRoleSelect = sjsu::Value(role);
    return Role(SendCommand(Command::kRole, { &kRoleSelect })[0]);
  }

  /// @returns The current configured role.
  Role GetRole()
  {
    return Role(SendCommand(Command::kRole)[0]);
  }

  /// @param device_name The name to set, must be 18 bytes or less.
  const std::string_view SetDeviceName(const std::string_view device_name)
  {
    return SendCommand(Command::kDeviceName, { device_name }, true);
  }

  const std::string_view GetDeviceName()
  {
    return SendCommand(Command::kDeviceName);
  }

  bool SetBaudRate(const BaudRate baud,
                   const sjsu::UartSettings_t::StopBits stop_bits,
                   const sjsu::UartSettings_t::Parity parity)
  {
    const char baud_select = sjsu::Value(baud);
    const char stop_bits_select =
        static_cast<char>(sjsu::Value(stop_bits) + '0');
    // TODO: restrict to only kNone, kOdd, and kEven
    const char parity_select = static_cast<char>(sjsu::Value(parity) - '0');
    const auto response      = SendCommand(
        Command::kBaud, { &baud_select, &stop_bits_select, &parity_select });

    if (response == std::string_view(&baud_select))
    {
      return false;
    }

    // if no error
    // switch (baud)
    // {
    //   case BaudRate::kB115200: uart_.ConfigureBaudRate(115'200); break;
    //   case BaudRate::kB57600: uart_.ConfigureBaudRate(57'600); break;
    //   // case BaudRate::kB38400: uart_.ConfigureBaudRate(38'400); break;
    //   case BaudRate::kB38400: uart_.ConfigureBaudRate(9'600); break;
    //   case BaudRate::kB19200: uart_.ConfigureBaudRate(19'200); break;
    //   // case BaudRate::kB9600: uart_.ConfigureBaudRate(9'600); break;
    //   case BaudRate::kB9600: uart_.ConfigureBaudRate(38'400); break;
    // }

    return true;
  }

  BaudRate GetBaudRate()
  {
    return BaudRate(SendCommand(Command::kBaud)[0]);
  }

  /// @param uuid The 6 byte UUID string ranging between '0x0001' to '0xFFFE'.
  /// @returns
  const std::string_view SetUuid(const std::string_view uuid)
  {
    return SendCommand(Command::kUuid, { uuid });
  }

  /// @returns The 6 byte UUID string ranging between '0x0001' to '0xFFFE'.
  const std::string_view GetUuid()
  {
    return SendCommand(Command::kUuid);
  }

  const std::string_view SetCharacteristic(
      const std::string_view characteristic)
  {
    return SendCommand(Command::kCharacteristic, { characteristic });
  }

  const std::string_view GetCharacteristic()
  {
    return SendCommand(Command::kCharacteristic);
  }

  // ---------------------------------------------------------------------------
  // Master Mode
  // ---------------------------------------------------------------------------

  void Inquire()  // const std::string_view mode         = "0",
                  // const std::string_view device_limit = "7",
                  // const std::string_view timeout      = "48")
  {
    constexpr std::string_view kInqueryStart = "+INQS\r\n";
    constexpr std::string_view kInqueryEnd   = "+INQE\r\n";
    // 12 bytes for MAC and 2 extra bytes for "0x" prefix
    // constexpr size_t kMacLength = 12;

    // std::span<uint8_t> receive_buffer(
    //     reinterpret_cast<uint8_t *>(at_response_buffer_.data()),
    //     at_response_buffer_.size());

    // OK\r\n
    // +INQS\r\n
    // +INQ:1 0x882583F117DF -43\r\n
    // +INQE\r\n

    // auto resp = SendCommand(
    //     Command::kScanDevices, { mode, device_limit, timeout }, false);
    // if (resp != "OK")
    // {
    //   throw "Failed to inquire";
    // }

    // serial_uart_.ReadUntil(receive_buffer, kCrLf);
    // if (std::string_view(at_response_buffer_.data()) != kInqueryStart)
    // {
    //   return;
    // }

    // serial_uart_.ReadUntil(receive_buffer, kCrLf);
    // while (std::string_view(at_response_buffer_.data()) != kInqueryEnd)
    // {
    //   const size_t kSpace1 = resp.find(" ");
    //   const size_t kSpace2 = resp.find(" ", kSpace1 + 1);
    //   const size_t kEnd    = resp.find_last_of(kCrLf);
    //   sjsu::LogInfo("%.*s", resp.substr(0, kSpace1));
    //   sjsu::LogInfo("%.*s", resp.substr(kSpace1 + 3, kMacLength));
    //   sjsu::LogInfo("%.*s", resp.substr(kSpace2 + 1, kEnd));
    //   scan_result_count++;
    //   serial_uart_.ReadUntil(receive_buffer, kCrLf);
    // }

    // sjsu::LogInfo("%.*s", resp);

    uart_.Write("AT+INQ0,7,48\r\n");
    // serial_uart_.ReadUntil(kAtResponseOk);
    // serial_uart_.ReadUntil(kInqueryStart);
    // serial_uart_.ReadUntil(receive_buffer, kCrLf);
    // serial_uart_.ReadUntil(kCrLf);
    // sjsu::LogInfo("%s", at_response_buffer_.data());

    char buffer[100];
    std::span<uint8_t> receive_buffer(reinterpret_cast<uint8_t *>(buffer), 100);
    serial_uart_.ReadUntil(receive_buffer, kInqueryEnd, 2s, false);
    buffer[99] = '\0';
    sjsu::LogInfo("%s", buffer);
  }

  // void StopScan()
  // {
  //   SendCommand(Command::kScanDevices, "0");
  // }

  // void Connect(uint8_t device_index)
  // {
  //   const char index = static_cast<char>(device_index + '0');
  //   SendCommand(Command::kConnect, &index);
  // }

  // void GetConnectionStatus() {}

  // void Disconnect() {}

  const std::string_view SendCommand(
      const std::string_view & command,
      const std::initializer_list<std::string_view> params = {},
      const bool has_ok                                    = false)
  {
    // Sends the command in the following formats:
    //   AT<command>\r\n
    //   AT<command><param>\r\n
    //   AT<command><param1>,...,<paramN>\r\n
    constexpr std::string_view kPrefix = "AT";

    uart_.Write(kPrefix);
    uart_.Write(command);

    if (params.size() > 0)
    {
      auto iter = std::begin(params);
      uart_.Write(*iter);
      iter++;

      if (iter != std::end(params))
      {
        for (; iter != std::end(params); iter++)
        {
          uart_.Write(',');
          uart_.Write(*iter);
        }
      }
    }

    uart_.Write(kCrLf);

    std::span<uint8_t> receive_buffer(
        reinterpret_cast<uint8_t *>(at_response_buffer_.data()),
        at_response_buffer_.size());

    const int kBytesRead = serial_uart_.ReadUntil(receive_buffer, kCrLf);

    // Continue reading to get OK if it is part of the response
    if (has_ok)
    {
      serial_uart_.ReadUntil(kAtResponseOk, kDefaultTimeout, true);
    }

    return Parse(std::string_view(at_response_buffer_.data(), kBytesRead),
                 command,
                 kCrLf,
                 has_ok);
  }

  // Data Mode

  void Write(const std::span<const uint8_t> & data)
  {
    uart_.Write(data);
  }

  size_t Read(std::span<uint8_t> data)
  {
    return uart_.Read(data);
  }

 private:
  sjsu::Uart & uart_;
  sjsu::SerialUart serial_uart_;
  sjsu::Gpio & key_pin_;
  sjsu::Gpio & state_pin_;

  /// Buffer for holding responses in AT Command Mode.
  ///
  /// @note This will not be able to suport the AT+HELP command
  std::array<char, kAtResponseBufferLength> at_response_buffer_;

  std::array<ScanResult_t, 8> scan_results_;
  size_t scan_result_count = 0;
};  // namespace bluetooth
}  // namespace bluetooth
}  // namespace sjsu
