#include <array>
#include <cstdint>
#include <span>
#include <string_view>

#include "peripherals/uart.hpp"
#include "utility/error_handling.hpp"
#include "utility/log.hpp"
#include "utility/time/time.hpp"
#include "utility/time/timeout_timer.hpp"

namespace sjsu
{
class SerialUart
{
 public:
  static constexpr std::chrono::nanoseconds kDefaultTimeout = 2s;

  explicit SerialUart(sjsu::Uart & uart) : uart_port_(uart) {}

  // Reads to provided buffer and checks to see if the end of the read matches
  // the end char array.  Returns size of read or -1 if end char array doesn't
  // match the read char array.
  int ReadUntil(std::span<uint8_t> buffer,
                const std::string_view & end,
                std::chrono::nanoseconds timeout = kDefaultTimeout,
                bool throw_on_failure            = true)
  {
    struct ReadUntil_t
    {
      std::span<uint8_t> buffer;
      const std::string_view & end;
      size_t string_length     = 0;
      uint32_t buffer_position = 0;
      uint32_t end_position    = 0;
      bool success             = false;
    };

    std::fill(buffer.begin(), buffer.end(), 0);

    ReadUntil_t until = {
      .buffer        = buffer,
      .end           = end,
      .string_length = end.size(),
    };

    Wait(timeout, [this, &until]() {
      if (until.end_position >= until.string_length)
      {
        until.success = true;
        return true;
      }
      if (!uart_port_.HasData())
      {
        return false;
      }

      uint32_t buf_pos      = until.buffer_position % until.buffer.size();
      until.buffer[buf_pos] = uart_port_.Read();

      if (until.buffer[buf_pos] == until.end[until.end_position])
      {
        until.end_position++;
      }
      else if (until.buffer[buf_pos] == until.end[0])
      {
        until.end_position = 1;
      }
      else
      {
        until.end_position = 0;
      }

      until.buffer_position++;
      return false;
    });

    if (until.success)
    {
      return until.buffer_position;
    }

    if (throw_on_failure)
    {
      debug::HexdumpDebug(buffer.data(), buffer.size());
      throw Exception(std::errc::timed_out,
                      "Did not get the expected response from device.");
    }

    return -1;
  }

  // Default buffer size for ReadUntil method
  template <size_t kBufferSize = 64>
  int ReadUntil(const std::string_view & end,
                std::chrono::nanoseconds timeout = kDefaultTimeout,
                bool throw_on_failure            = false)
  {
    std::array<uint8_t, kBufferSize> buffer = { 0 };
    return ReadUntil(buffer, end, timeout, throw_on_failure);
  }

 private:
  sjsu::Uart & uart_port_;
};
}  // namespace sjsu