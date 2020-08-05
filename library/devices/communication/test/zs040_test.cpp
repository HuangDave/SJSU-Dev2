#include <array>

#include "L2_HAL/communication/bluetooth/zs040.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu::bluetooth
{
EMIT_ALL_METHODS(Zs040);

TEST_CASE("Zs040 Serial BLE Tests")
{
  Mock<Uart> mock_uart;
  Mock<Gpio> mock_key_pin;
  Mock<Gpio> mock_state_pin;

  SECTION("Parse Generic AT Response")
  {
    constexpr std::string_view kTestCommand = "+TEST";
    constexpr std::string_view kCrLf    = "\r\n";
    std::string_view buffer;

    SECTION("Response with CrLf")
    {
      CHECK(Zs040::Parse("+TEST=1\r\n", kTestCommand, kCrLf) == "1");
    }

    SECTION("Response with CrLf+OK") {}

    SECTION("Value only") {}

    SECTION("OK only") {}
  }
}
}  // namespace sjsu::bluetooth
