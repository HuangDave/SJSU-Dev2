#pragma once

#include <cstdint>

#include "L0_Platform/stm32f10x/stm32f10x.h"
#include "L1_Peripheral/system_controller.hpp"
#include "utility/bit.hpp"
#include "utility/enum.hpp"

namespace sjsu
{
namespace stm32f10x
{
/// System controller for stm32f10x that controls clock sources, clock speed,
/// clock outputs control, and peripheral enabling
class SystemController : public sjsu::SystemController
{
 public:
  /// Number of bits between each enable register
  static constexpr uint32_t kBits = 32;

  /// Namespace containing the set of all peripherals id on the stm32f4xx mcu
  /// family.
  class Peripherals
  {
   public:
    /// Bit position of AHB
    static constexpr uint32_t kAHB = kBits * 0;

    //! @cond Doxygen_Suppress
    static constexpr auto kDma1  = PeripheralID::Define<kAHB + 0>();
    static constexpr auto kDma2  = PeripheralID::Define<kAHB + 1>();
    static constexpr auto kSram  = PeripheralID::Define<kAHB + 2>();
    static constexpr auto kFlitf = PeripheralID::Define<kAHB + 4>();
    static constexpr auto kCrc   = PeripheralID::Define<kAHB + 6>();
    static constexpr auto kFsmc  = PeripheralID::Define<kAHB + 8>();
    static constexpr auto kSdio  = PeripheralID::Define<kAHB + 10>();
    //! @endcond

    /// Bit position of APB1
    static constexpr uint32_t kAPB1 = kBits * 1;

    //! @cond Doxygen_Suppress
    static constexpr auto kTimer2         = PeripheralID::Define<kAPB1 + 0>();
    static constexpr auto kTimer3         = PeripheralID::Define<kAPB1 + 1>();
    static constexpr auto kTimer4         = PeripheralID::Define<kAPB1 + 2>();
    static constexpr auto kTimer5         = PeripheralID::Define<kAPB1 + 3>();
    static constexpr auto kTimer6         = PeripheralID::Define<kAPB1 + 4>();
    static constexpr auto kTimer7         = PeripheralID::Define<kAPB1 + 5>();
    static constexpr auto kTimer12        = PeripheralID::Define<kAPB1 + 6>();
    static constexpr auto kTimer13        = PeripheralID::Define<kAPB1 + 7>();
    static constexpr auto kTimer14        = PeripheralID::Define<kAPB1 + 8>();
    static constexpr auto kWindowWatchdog = PeripheralID::Define<kAPB1 + 11>();
    static constexpr auto kSpi2           = PeripheralID::Define<kAPB1 + 14>();
    static constexpr auto kSpi3           = PeripheralID::Define<kAPB1 + 15>();
    static constexpr auto kUsart2         = PeripheralID::Define<kAPB1 + 17>();
    static constexpr auto kUsart3         = PeripheralID::Define<kAPB1 + 18>();
    static constexpr auto kUart4          = PeripheralID::Define<kAPB1 + 19>();
    static constexpr auto kUart5          = PeripheralID::Define<kAPB1 + 20>();
    static constexpr auto kI2c1           = PeripheralID::Define<kAPB1 + 21>();
    static constexpr auto kI2c2           = PeripheralID::Define<kAPB1 + 22>();
    static constexpr auto kUsb            = PeripheralID::Define<kAPB1 + 23>();
    static constexpr auto kCan1           = PeripheralID::Define<kAPB1 + 25>();
    static constexpr auto kBackupClock    = PeripheralID::Define<kAPB1 + 27>();
    static constexpr auto kPower          = PeripheralID::Define<kAPB1 + 28>();
    static constexpr auto kDac            = PeripheralID::Define<kAPB1 + 29>();
    //! @endcond

    /// Bit position of AHB2
    static constexpr uint32_t kAPB2 = kBits * 2;

    //! @cond Doxygen_Suppress
    static constexpr auto kAFIO    = PeripheralID::Define<kAPB2 + 0>();
    static constexpr auto kGpioA   = PeripheralID::Define<kAPB2 + 2>();
    static constexpr auto kGpioB   = PeripheralID::Define<kAPB2 + 3>();
    static constexpr auto kGpioC   = PeripheralID::Define<kAPB2 + 4>();
    static constexpr auto kGpioD   = PeripheralID::Define<kAPB2 + 5>();
    static constexpr auto kGpioE   = PeripheralID::Define<kAPB2 + 6>();
    static constexpr auto kGpioF   = PeripheralID::Define<kAPB2 + 7>();
    static constexpr auto kGpioG   = PeripheralID::Define<kAPB2 + 8>();
    static constexpr auto kAdc1    = PeripheralID::Define<kAPB2 + 9>();
    static constexpr auto kAdc2    = PeripheralID::Define<kAPB2 + 10>();
    static constexpr auto kTimer1  = PeripheralID::Define<kAPB2 + 11>();
    static constexpr auto kSpi1    = PeripheralID::Define<kAPB2 + 12>();
    static constexpr auto kTimer8  = PeripheralID::Define<kAPB2 + 13>();
    static constexpr auto kUsart1  = PeripheralID::Define<kAPB2 + 14>();
    static constexpr auto kAdc3    = PeripheralID::Define<kAPB2 + 15>();
    static constexpr auto kTimer9  = PeripheralID::Define<kAPB2 + 19>();
    static constexpr auto kTimer10 = PeripheralID::Define<kAPB2 + 20>();
    static constexpr auto kTimer11 = PeripheralID::Define<kAPB2 + 21>();
    //! @endcond

    /// Bit position of systems outside of any bus
    static constexpr uint32_t kBeyond = kBits * 3;

    //! @cond Doxygen_Suppress
    static constexpr auto kCpu         = PeripheralID::Define<kBeyond + 0>();
    static constexpr auto kSystemTimer = PeripheralID::Define<kBeyond + 1>();
    static constexpr auto kI2s         = PeripheralID::Define<kBeyond + 2>();
    //! @endcond
  };

  /// List of all of the enable registers in the order corresponding to the
  /// their constants in the Peripherals namespace.
  static inline volatile uint32_t * enable[] = {
    &RCC->AHBENR,
    &RCC->APB1ENR,
    &RCC->APB2ENR,
  };

  /// Pointer to the Clock Control register
  static inline RCC_TypeDef * clock_control = RCC;

  /// Pointer to the flash control register
  static inline FLASH_TypeDef * flash = FLASH;

  /// Available dividers for the APB bus
  enum class APBDivider
  {
    kDivideBy1  = 0,
    kDivideBy2  = 0b100,
    kDivideBy4  = 0b101,
    kDivideBy8  = 0b110,
    kDivideBy16 = 0b111,
  };

  /// Available dividers for the AHB bus
  enum class AHBDivider
  {
    kDivideBy1   = 0,
    kDivideBy2   = 0b1000,
    kDivideBy4   = 0b1001,
    kDivideBy8   = 0b1010,
    kDivideBy16  = 0b1011,
    kDivideBy64  = 0b1100,
    kDivideBy128 = 0b1101,
    kDivideBy256 = 0b1110,
    kDivideBy512 = 0b1111,
  };

  /// Available dividers for the ADC bus
  enum class AdcDivider
  {
    kDivideBy2 = 0b00,
    kDivideBy4 = 0b01,
    kDivideBy6 = 0b10,
    kDivideBy8 = 0b11,
  };

  /// Available clock sources available for the system clock
  enum class SystemClockSelect
  {
    kHighSpeedInternal = 0b00,
    kHighSpeedExternal = 0b01,
    kPll               = 0b10,
  };

  /// Bit masks for the CFGR register
  struct ClockConfigurationRegisters  // NOLINT
  {
    /// Controls which clock signal is sent to the MCO pin
    static constexpr auto kMco = bit::CreateMaskFromRange(24, 26);
    /// Sets the USB clock divider
    static constexpr auto kUsbPrescalar = bit::CreateMaskFromRange(22);
    /// Sets the PLL multiplier
    static constexpr auto kPllMul = bit::CreateMaskFromRange(18, 21);
    /// If set to 1, will divide the HSE signal by 2 before sending to PLL
    static constexpr auto kHsePreDivider = bit::CreateMaskFromRange(17);
    /// Sets which source the PLL will take as input
    static constexpr auto kPllSource = bit::CreateMaskFromRange(16);
    /// Sets the clock divider for the ADC peripherals
    static constexpr auto kAdcDivider = bit::CreateMaskFromRange(14, 15);
    /// Sets the divider for peripherals on the APB2 bus
    static constexpr auto kAPB2Divider = bit::CreateMaskFromRange(11, 13);
    /// Sets the divider for peripherals on the APB1 bus
    static constexpr auto kAPB1Divider = bit::CreateMaskFromRange(8, 10);
    /// Sets the divider for peripherals on the AHB bus
    static constexpr auto kAHBDivider = bit::CreateMaskFromRange(4, 7);
    /// Used to check if the system clock has taken the new system clock
    /// settings.
    static constexpr auto kSystemClockStatus = bit::CreateMaskFromRange(2, 3);
    /// Set which clock will be used for the system clock .
    static constexpr auto kSystemClockSelect = bit::CreateMaskFromRange(0, 1);
  };

  /// Bit masks for the CR register
  struct ClockControlRegisters  // NOLINT
  {
    /// Indicates if the PLL is enabled and ready
    static constexpr auto kPllReady = bit::CreateMaskFromRange(25);
    /// Used to enable the PLL
    static constexpr auto kPllEnable = bit::CreateMaskFromRange(24);
    /// Indicates if the external oscillator is ready for use
    static constexpr auto kExternalOscReady = bit::CreateMaskFromRange(17);
    /// Used to enable the external oscillator
    static constexpr auto kExternalOscEnable = bit::CreateMaskFromRange(16);
  };

  /// PLL frequency multiplication options.
  enum class PllMultiply
  {
    kMultiplyBy2  = 0b0000,
    kMultiplyBy3  = 0b0001,
    kMultiplyBy4  = 0b0010,
    kMultiplyBy5  = 0b0011,
    kMultiplyBy6  = 0b0100,
    kMultiplyBy7  = 0b0101,
    kMultiplyBy8  = 0b0110,
    kMultiplyBy9  = 0b0111,
    kMultiplyBy10 = 0b1000,
    kMultiplyBy11 = 0b1001,
    kMultiplyBy12 = 0b1010,
    kMultiplyBy13 = 0b1011,
    kMultiplyBy14 = 0b1100,
    kMultiplyBy15 = 0b1101,
    kMultiplyBy16 = 0b1110,
  };

  /// Bitmasks for the BDCR register
  struct RtcRegisters  // NOLINT
  {
    /// Will reset all clock states for the RTC
    static constexpr auto kBackupDomainReset = bit::CreateMaskFromRange(16);
    /// Enables the RTC clock
    static constexpr auto kRtcEnable = bit::CreateMaskFromRange(15);
    /// Selects the clock source for the RTC
    static constexpr auto kRtcSourceSelect = bit::CreateMaskFromRange(8, 9);
    /// Indicates if the LSE is ready for use
    static constexpr auto kLowSpeedOscReady = bit::CreateMaskFromRange(1);
    /// Used to enable the LSE
    static constexpr auto kLowSpeedOscEnable = bit::CreateMaskFromRange(0);
  };

  /// Available clock sources for the RTC
  enum class RtcSource
  {
    kNoClock                       = 0b00,
    kLowSpeedInternal              = 0b01,
    kLowSpeedExternal              = 0b10,
    kHighSpeedExternalDividedBy128 = 0b11,
  };

  /// Available clock sources for the PLL
  enum class PllSource
  {
    kHighSpeedInternal           = 0b0,
    kHighSpeedExternal           = 0b1,
    kHighSpeedExternalDividedBy2 = 0b11,
  };

  /// Available dividers for the USB peripheral
  enum class UsbDivider
  {
    kDivideBy1 = 1,
    /// Divide by 1.5
    kDivideBy1Point5 = 0,
  };

  /// RM0008 page 126 describes the clock tree for the stm32f10x
  struct ClockConfiguration  // NOLINT
  {
    /// Defines the frequency of the high speed external clock signal
    units::frequency::hertz_t high_speed_external = 0_MHz;

    /// Defines the frequency of the low speed external clock signal.
    units::frequency::hertz_t low_speed_external = 0_MHz;

    /// Defines the configuration of the PLL
    struct
    {
      bool enable          = false;
      PllSource source     = PllSource::kHighSpeedInternal;
      PllMultiply multiply = PllMultiply::kMultiplyBy2;
      struct
      {
        UsbDivider divider = UsbDivider::kDivideBy1Point5;
      } usb = {};
    } pll = {};

    /// Defines which clock source will be use for the system.
    /// @warning System will lock up in the following situations:
    ///          - Select PLL, but PLL is not enabled
    ///          - Select PLL, but PLL frequency is too high
    ///          - Select High Speed External, but the frequency is kept at
    ///            0_Mhz.
    SystemClockSelect system_clock = SystemClockSelect::kHighSpeedInternal;

    /// Defines the configuration for the RTC
    struct
    {
      bool enable      = false;
      RtcSource source = RtcSource::kLowSpeedInternal;
    } rtc;

    /// Defines the configuration of the dividers beyond system clock mux.
    struct
    {
      AHBDivider divider = AHBDivider::kDivideBy1;
      /// Maximum rate of 36 MHz
      struct
      {
        APBDivider divider = APBDivider::kDivideBy1;
      } apb1 = {};

      struct
      {
        APBDivider divider = APBDivider::kDivideBy1;
        struct
        {
          /// Maximum of 14 MHz
          AdcDivider divider = AdcDivider::kDivideBy2;
        } adc = {};
      } apb2 = {};
    } ahb = {};
  };

  /// Constant for the frequency of the LSE
  static constexpr auto kLowSpeedInternal = 20_kHz;

  /// Constant for the frequency of the HSE
  static constexpr auto kHighSpeedInternal = 8_MHz;

  /// Constant for the frequency of the Flash Controller
  static constexpr auto kFlashClock = kHighSpeedInternal;

  /// Constant for the frequency of the Watch Dog Peripheral
  static constexpr auto kWatchdogClockRate = kLowSpeedInternal;

  /// @param config - clock configuration structure
  explicit constexpr SystemController(ClockConfiguration & config)
      : config_(config)
  {
  }

  // Clock tree 8. page 86

  void Initialize() override
  {
    units::frequency::hertz_t system_clock = 0_Hz;

    // =========================================================================
    // Step 1. Select internal clock source for everything.
    //         Make sure PLLs are not clock sources for everything.
    // =========================================================================
    // Step 1.1 Set SystemClock to HSI
    clock_control->CFGR = bit::Insert(
        clock_control->CFGR, Value(SystemClockSelect::kHighSpeedInternal),
        ClockConfigurationRegisters::kSystemClockSelect);

    // Step 1.4 Reset RTC clock registers
    clock_control->BDCR =
        bit::Set(clock_control->BDCR, RtcRegisters::kBackupDomainReset);

    // Manually clear the RTC reset bit
    clock_control->BDCR =
        bit::Clear(clock_control->BDCR, RtcRegisters::kBackupDomainReset);

    // =========================================================================
    // Step 2. Disable PLL and external clock sources
    // =========================================================================
    // Step 2.1 Disable PLLs
    clock_control->CR =
        bit::Clear(clock_control->CR, ClockControlRegisters::kPllEnable);

    // Step 2.1 Disable External Oscillators
    clock_control->CR = bit::Clear(clock_control->CR,
                                   ClockControlRegisters::kExternalOscEnable);

    // =========================================================================
    // Step 3. Enable External Oscillators
    // =========================================================================
    // Step 3.1 Enable High speed external Oscillator
    if (config_.high_speed_external != 0_MHz)
    {
      clock_control->CR = bit::Set(clock_control->CR,
                                   ClockControlRegisters::kExternalOscEnable);

      while (!bit::Read(clock_control->CR,
                        ClockControlRegisters::kExternalOscReady))
      {
        continue;
      }
    }

    // Step 3.2 Enable Low speed external Oscillator
    if (config_.low_speed_external != 0_MHz)
    {
      clock_control->BDCR =
          bit::Insert(clock_control->BDCR, 1, RtcRegisters::kLowSpeedOscEnable);

      while (!bit::Read(clock_control->BDCR, RtcRegisters::kLowSpeedOscReady))
      {
        continue;
      }
    }

    // =========================================================================
    // Step 4. Set oscillator source for PLLs
    // =========================================================================
    if (config_.pll.source == PllSource::kHighSpeedExternalDividedBy2)
    {
      clock_control->CFGR = bit::Set(
          clock_control->CFGR, ClockConfigurationRegisters::kHsePreDivider);
    }
    else
    {
      clock_control->CFGR = bit::Clear(
          clock_control->CFGR, ClockConfigurationRegisters::kHsePreDivider);
    }

    clock_control->CFGR =
        bit::Insert(clock_control->CFGR, Value(config_.pll.source),
                    ClockConfigurationRegisters::kPllSource);

    // =========================================================================
    // Step 5. Setup PLLs and enable them where necessary
    // =========================================================================
    if (config_.pll.enable)
    {
      clock_control->CFGR =
          bit::Insert(clock_control->CFGR, Value(config_.pll.multiply),
                      ClockConfigurationRegisters::kPllMul);

      clock_control->CR =
          bit::Set(clock_control->CR, ClockControlRegisters::kPllEnable);

      while (!bit::Read(clock_control->CR, ClockControlRegisters::kPllReady))
      {
        continue;
      }

      switch (config_.pll.source)
      {
        case PllSource::kHighSpeedInternal:
          pll_clock_rate_ = kHighSpeedInternal / 2;
          break;
        case PllSource::kHighSpeedExternal:
          pll_clock_rate_ = config_.high_speed_external;
          break;
        case PllSource::kHighSpeedExternalDividedBy2:
          pll_clock_rate_ = config_.high_speed_external / 2;
          break;
      }

      // Multiply the PLL clock up to the correct rate.
      pll_clock_rate_ = pll_clock_rate_ * (Value(config_.pll.multiply) + 2);
    }

    // =========================================================================
    // Step 6. Setup peripheral dividers
    // =========================================================================
    // Step 6.1 Set USB divider
    clock_control->CFGR =
        bit::Insert(clock_control->CFGR, Value(config_.pll.usb.divider),
                    ClockConfigurationRegisters::kUsbPrescalar);

    // Step 6.2 Set AHB divider
    clock_control->CFGR =
        bit::Insert(clock_control->CFGR, Value(config_.ahb.divider),
                    ClockConfigurationRegisters::kAHBDivider);

    // Step 6.3 Set APB1 divider
    clock_control->CFGR =
        bit::Insert(clock_control->CFGR, Value(config_.ahb.apb1.divider),
                    ClockConfigurationRegisters::kAPB1Divider);

    // Step 6.4 Set APB2 divider
    clock_control->CFGR =
        bit::Insert(clock_control->CFGR, Value(config_.ahb.apb2.divider),
                    ClockConfigurationRegisters::kAPB2Divider);

    // Step 6.5 Set ADC divider
    clock_control->CFGR =
        bit::Insert(clock_control->CFGR, Value(config_.ahb.apb2.adc.divider),
                    ClockConfigurationRegisters::kAdcDivider);

    // =========================================================================
    // Step 7. Set System Clock and RTC Clock
    // =========================================================================
    uint32_t target_clock_source = Value(config_.system_clock);

    // Step 7.1 Set the Flash wait states appropriately prior to setting the
    //          system clock frequency. Failure to do this will cause the system
    //          to be unable to read from flash, resulting in the platform
    //          locking up. See p.60 of RM0008 for the Flash ACR register
    if (config_.system_clock == SystemClockSelect::kPll)
    {
      if (pll_clock_rate_ <= 24_MHz)
      {
        // 0 Wait states
        flash->ACR = sjsu::bit::Insert(flash->ACR, 0b000,
                                       sjsu::bit::CreateMaskFromRange(0, 2));
      }
      else if (24_MHz <= pll_clock_rate_ && pll_clock_rate_ <= 48_MHz)
      {
        // 1 Wait state
        flash->ACR = sjsu::bit::Insert(flash->ACR, 0b001,
                                       sjsu::bit::CreateMaskFromRange(0, 2));
      }
      else
      {
        // 2 Wait states
        flash->ACR = sjsu::bit::Insert(flash->ACR, 0b010,
                                       sjsu::bit::CreateMaskFromRange(0, 2));
      }
    }

    // Step 7.2 Set system clock source
    // NOTE: return error if clock = SystemClockSelect::kHighSpeedExternal and
    //       high speed external is not enabled.
    clock_control->CFGR =
        bit::Insert(clock_control->CFGR, Value(config_.system_clock),
                    ClockConfigurationRegisters::kSystemClockSelect);

    while (bit::Extract(clock_control->CFGR,
                        ClockConfigurationRegisters::kSystemClockStatus) !=
           target_clock_source)
    {
      continue;
    }

    switch (config_.system_clock)
    {
      case SystemClockSelect::kHighSpeedInternal:
        system_clock = kHighSpeedInternal;
        break;
      case SystemClockSelect::kHighSpeedExternal:
        system_clock = config_.high_speed_external;
        break;
      case SystemClockSelect::kPll: system_clock = pll_clock_rate_; break;
    }

    // Step 7.3 Set the RTC oscillator source
    clock_control->BDCR =
        bit::Insert(clock_control->BDCR, Value(config_.rtc.source),
                    RtcRegisters::kRtcSourceSelect);

    // Step 7.4 Enable/Disable the RTC
    clock_control->BDCR = bit::Insert(clock_control->BDCR, config_.rtc.enable,
                                      RtcRegisters::kRtcEnable);

    // =========================================================================
    // Step 8. Define the clock rates for the system
    // =========================================================================
    switch (config_.ahb.divider)
    {
      case AHBDivider::kDivideBy1: ahb_clock_rate_ = system_clock / 1; break;
      case AHBDivider::kDivideBy2: ahb_clock_rate_ = system_clock / 2; break;
      case AHBDivider::kDivideBy4: ahb_clock_rate_ = system_clock / 4; break;
      case AHBDivider::kDivideBy8: ahb_clock_rate_ = system_clock / 8; break;
      case AHBDivider::kDivideBy16: ahb_clock_rate_ = system_clock / 16; break;
      case AHBDivider::kDivideBy64: ahb_clock_rate_ = system_clock / 64; break;
      case AHBDivider::kDivideBy128:
        ahb_clock_rate_ = system_clock / 128;
        break;
      case AHBDivider::kDivideBy256:
        ahb_clock_rate_ = system_clock / 256;
        break;
      case AHBDivider::kDivideBy512:
        ahb_clock_rate_ = system_clock / 512;
        break;
    }

    switch (config_.ahb.apb1.divider)
    {
      case APBDivider::kDivideBy1:
        apb1_clock_rate_ = ahb_clock_rate_ / 1;
        break;
      case APBDivider::kDivideBy2:
        apb1_clock_rate_ = ahb_clock_rate_ / 2;
        break;
      case APBDivider::kDivideBy4:
        apb1_clock_rate_ = ahb_clock_rate_ / 4;
        break;
      case APBDivider::kDivideBy8:
        apb1_clock_rate_ = ahb_clock_rate_ / 8;
        break;
      case APBDivider::kDivideBy16:
        apb1_clock_rate_ = ahb_clock_rate_ / 16;
        break;
    }

    switch (config_.ahb.apb2.divider)
    {
      case APBDivider::kDivideBy1:
        apb2_clock_rate_ = ahb_clock_rate_ / 1;
        break;
      case APBDivider::kDivideBy2:
        apb2_clock_rate_ = ahb_clock_rate_ / 2;
        break;
      case APBDivider::kDivideBy4:
        apb2_clock_rate_ = ahb_clock_rate_ / 4;
        break;
      case APBDivider::kDivideBy8:
        apb2_clock_rate_ = ahb_clock_rate_ / 8;
        break;
      case APBDivider::kDivideBy16:
        apb2_clock_rate_ = ahb_clock_rate_ / 16;
        break;
    }

    switch (config_.rtc.source)
    {
      case RtcSource::kNoClock: rtc_clock_rate_ = 0_Hz; break;
      case RtcSource::kLowSpeedInternal:
        rtc_clock_rate_ = kLowSpeedInternal;
        break;
      case RtcSource::kLowSpeedExternal:
        rtc_clock_rate_ = config_.low_speed_external;
        break;
      case RtcSource::kHighSpeedExternalDividedBy128:
        rtc_clock_rate_ = config_.high_speed_external / 128;
        break;
    }

    switch (config_.pll.usb.divider)
    {
      case UsbDivider::kDivideBy1: usb_clock_rate_ = pll_clock_rate_; break;
      case UsbDivider::kDivideBy1Point5:
        usb_clock_rate_ = (pll_clock_rate_ * 2) / 3;
        break;
    }

    switch (config_.ahb.apb1.divider)
    {
      case APBDivider::kDivideBy1:
        timer_apb1_clock_rate_ = apb1_clock_rate_;
        break;
      default: timer_apb1_clock_rate_ = apb1_clock_rate_ * 2; break;
    }

    switch (config_.ahb.apb2.divider)
    {
      case APBDivider::kDivideBy1:
        timer_apb2_clock_rate_ = apb2_clock_rate_;
        break;
      default: timer_apb2_clock_rate_ = apb2_clock_rate_ * 2; break;
    }

    switch (config_.ahb.apb2.adc.divider)
    {
      case AdcDivider::kDivideBy2:
        adc_clock_rate_ = apb2_clock_rate_ / 2;
        break;
      case AdcDivider::kDivideBy4:
        adc_clock_rate_ = apb2_clock_rate_ / 4;
        break;
      case AdcDivider::kDivideBy6:
        adc_clock_rate_ = apb2_clock_rate_ / 6;
        break;
      case AdcDivider::kDivideBy8:
        adc_clock_rate_ = apb2_clock_rate_ / 8;
        break;
    }
  }

  /// @return the a pointer to the clock configuration object used to configure
  /// this system controller.
  void * GetClockConfiguration() override
  {
    return &config_;
  }

  /// @return the clock rate frequency of a peripheral
  units::frequency::hertz_t GetClockRate(PeripheralID id) const override
  {
    switch (id.device_id)
    {
      case Peripherals::kI2s.device_id: return pll_clock_rate_;
      case Peripherals::kUsb.device_id: return usb_clock_rate_;
      case Peripherals::kFlitf.device_id: return kHighSpeedInternal;

      // Arm Cortex running clock rate.
      // This code does not utilize the /8 clock for the system timer, thus the
      // clock rate for that subsystem is equal to the CPU running clock.
      case Peripherals::kSystemTimer.device_id: [[fallthrough]];
      case Peripherals::kCpu.device_id: return ahb_clock_rate_;

      // APB1 Timers
      case Peripherals::kTimer2.device_id: [[fallthrough]];
      case Peripherals::kTimer3.device_id: [[fallthrough]];
      case Peripherals::kTimer4.device_id: [[fallthrough]];
      case Peripherals::kTimer5.device_id: [[fallthrough]];
      case Peripherals::kTimer6.device_id: [[fallthrough]];
      case Peripherals::kTimer7.device_id: [[fallthrough]];
      case Peripherals::kTimer12.device_id: [[fallthrough]];
      case Peripherals::kTimer13.device_id: [[fallthrough]];
      case Peripherals::kTimer14.device_id: return timer_apb1_clock_rate_;

      // APB2 Timers
      case Peripherals::kTimer1.device_id: [[fallthrough]];
      case Peripherals::kTimer8.device_id: [[fallthrough]];
      case Peripherals::kTimer9.device_id: [[fallthrough]];
      case Peripherals::kTimer10.device_id: [[fallthrough]];
      case Peripherals::kTimer11.device_id: return timer_apb2_clock_rate_;

      case Peripherals::kAdc1.device_id: [[fallthrough]];
      case Peripherals::kAdc2.device_id: [[fallthrough]];
      case Peripherals::kAdc3.device_id: return adc_clock_rate_;
    }

    if (id.device_id < Peripherals::kAPB1)
    {
      return ahb_clock_rate_;
    }

    if (Peripherals::kAPB1 <= id.device_id && id.device_id < Peripherals::kAPB2)
    {
      return apb1_clock_rate_;
    }

    if (Peripherals::kAPB2 <= id.device_id &&
        id.device_id < Peripherals::kBeyond)
    {
      return apb2_clock_rate_;
    }

    return 0_Hz;
  }

  bool IsPeripheralPoweredUp(PeripheralID id) const override
  {
    return bit::Read(*EnableRegister(id), EnableBitPosition(id));
  }

  void PowerUpPeripheral(PeripheralID id) const override
  {
    *EnableRegister(id) = bit::Set(*EnableRegister(id), EnableBitPosition(id));
  }

  void PowerDownPeripheral(PeripheralID id) const override
  {
    *EnableRegister(id) =
        bit::Clear(*EnableRegister(id), EnableBitPosition(id));
  }

 private:
  volatile uint32_t * EnableRegister(PeripheralID id) const
  {
    return enable[id.device_id / kBits];
  }

  uint32_t EnableBitPosition(PeripheralID id) const
  {
    return id.device_id % kBits;
  }

  ClockConfiguration & config_;
  units::frequency::hertz_t rtc_clock_rate_        = 0_Hz;
  units::frequency::hertz_t usb_clock_rate_        = 0_Hz;
  units::frequency::hertz_t pll_clock_rate_        = 0_Hz;
  units::frequency::hertz_t ahb_clock_rate_        = 0_Hz;
  units::frequency::hertz_t apb1_clock_rate_       = 0_Hz;
  units::frequency::hertz_t apb2_clock_rate_       = 0_Hz;
  units::frequency::hertz_t timer_apb1_clock_rate_ = 0_Hz;
  units::frequency::hertz_t timer_apb2_clock_rate_ = 0_Hz;
  units::frequency::hertz_t adc_clock_rate_        = 0_Hz;
};
}  // namespace stm32f10x
}  // namespace sjsu
