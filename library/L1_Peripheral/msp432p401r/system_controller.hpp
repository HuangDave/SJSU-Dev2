#pragma once

#include <cstdint>

#include "L0_Platform/msp432p401r/msp432p401r.h"
#include "L1_Peripheral/system_controller.hpp"
#include "utility/bit.hpp"
#include "utility/build_info.hpp"
#include "utility/enum.hpp"
#include "utility/log.hpp"

namespace sjsu
{
namespace msp432p401r
{
/// SystemController class used to manage power control and various clock system
/// resources on the MSP432P401R MCU.
class SystemController final : public sjsu::SystemController
{
 public:
  /// Contains the device id for each of the available clock system modules.
  class Modules
  {
   public:
    //! @cond Doxygen_Suppress
    static constexpr auto kAuxiliaryClock       = PeripheralID::Define<0>();
    static constexpr auto kMasterClock          = PeripheralID::Define<1>();
    static constexpr auto kSubsystemMasterClock = PeripheralID::Define<2>();
    static constexpr auto kLowSpeedSubsystemMasterClock =
        PeripheralID::Define<3>();
    static constexpr auto kBackupClock           = PeripheralID::Define<4>();
    static constexpr auto kLowFrequencyClock     = PeripheralID::Define<5>();
    static constexpr auto kVeryLowFrequencyClock = PeripheralID::Define<6>();
    static constexpr auto kReferenceClock        = PeripheralID::Define<7>();
    static constexpr auto kModuleClock           = PeripheralID::Define<8>();
    static constexpr auto kSystemClock           = PeripheralID::Define<9>();
    //! @endcond
  };

  /// The available internal oscillators for the clock system module.
  enum class Oscillator : uint8_t
  {
    /// Low frequency oscillator (LFXT) with frequency of 32.768 kHz.
    kLowFrequency = 0b000,
    /// Ultra low power oscillator (VLO) with typical frequency of 9.4 kHz.
    kVeryLowFrequency = 0b001,
    /// Low frequency reference oscillator (REFO) that can be configured to
    /// output 32.768 kHz or 128 kHz.
    kReference = 0b010,
    /// Digitally controlled oscillator (DCO) that can be configured to
    /// generate a frequency between 1 MHZ and 48 MHz.
    kDigitallyControlled = 0b011,
    /// Low power oscillator with a typical frequency of 25 MHz.
    kModule = 0b100,
    /// High frequency oscillator (HFXT) which can be driven by an external
    /// oscillator or external square wave with frequency ranging from 1 MHz to
    /// 48 Mhz.
    kHighFrequency = 0b101,
  };

  /// The available system clocks used to drive various peripheral modules where
  /// kAuxiliary, kMaster, kSubsystemMaster, kLowSpeedSubsystemMaster, and
  /// kBackup are the primary clock signals.
  ///
  /// @see Figure 6-1. Clock System Block Diagram
  ///      https://www.ti.com/lit/ug/slau356i/slau356i.pdf#page=380
  enum class Clock : uint8_t
  {
    /// Auxiliary clock (ACLK) with a max frequency of 128 kHz.
    kAuxiliary = 0,
    /// Master clock (MCLK) that drives the CPU.
    kMaster,
    /// Subsystem master clock (HSMCLK).
    kSubsystemMaster,
    /// Low-speed subsystem master clock (SMCLK).
    kLowSpeedSubsystemMaster,
    /// Low speed backup domain clock (BCLK). The with maximum restricted
    /// frequency of 32.768 kHz.
    kBackup,
    /// Low frequency low power clock (LFXTCLK). This clock can be driven
    /// by the LFXT oscillator or an external oscillator with a frequency
    /// of 32.768 kHz or less in bypass mode.
    kLowFrequency,
    /// Very low frequency low power clock (VLOCLK).
    kVeryLowFrequency,
    /// Low frequency reference clock (REFOCLK).
    kReference,
    /// Low power module clock (MODCLK).
    kModule,
    /// System oscillator clock (SYSCLK).
    kSystem,
  };

  /// The available clock dividers for the primary clocks.
  enum class ClockDivider : uint8_t
  {
    //! @cond Doxygen_Suppress
    kDivideBy1   = 0b000,
    kDivideBy2   = 0b001,
    kDivideBy4   = 0b010,
    kDivideBy8   = 0b011,
    kDivideBy16  = 0b100,
    kDivideBy32  = 0b101,
    kDivideBy64  = 0b110,
    kDivideBy128 = 0b111,
    //! @endcond
  };

  /// Namespace containing the fixed clock rates of the available internal
  /// oscillators.
  ///
  /// @see 6.1 Clock System Introduction
  ///      https://www.ti.com/lit/ug/slau356i/slau356i.pdf#page=379
  struct InternalOscillator  // NOLINT
  {
    /// Clock rate for the very low power oscillator.
    static constexpr units::frequency::hertz_t kVeryLowFrequency = 9'400_Hz;
    /// Clock rate for the low power oscillator.
    static constexpr units::frequency::hertz_t kModule = 25_MHz;
    /// Internal system oscillator.
    static constexpr units::frequency::hertz_t kSystem = 5_MHz;
    /// Clock rates for the reference oscillator The reference oscillator is
    /// configuration to be either 32.768 kHz or 128 kHz.
    static constexpr std::array<units::frequency::hertz_t, 2> kReference = {
      32'768_Hz, 128_kHz
    };
  };

  /// Namespace containing the fixed clock rates of the available on-board
  /// external oscillators.
  ///
  /// @see http://www.ti.com/lit/ug/slau597f/slau597f.pdf
  struct ExternalOscillator  // NOLINT
  {
    /// Clock rate for the on-board external low frequency oscillator.
    static constexpr units::frequency::hertz_t kLowFrequency = 32'768_Hz;
    /// Clock rate for the on-board external high frequency oscillator.
    static constexpr units::frequency::hertz_t kHighFrequency = 48_MHz;
  };

  /// Namespace containing the bit masks for the Key Register (KEY) which locks
  /// or unlocks the other clock system registers.
  struct KeyRegister  // NOLINT
  {
    /// The CSKEY bit mask used for locking or unlocking the clock system
    /// registers.
    static constexpr bit::Mask kCsKey = bit::CreateMaskFromRange(0, 15);
  };

  /// Namespace containing the bit masks for the Control 0 Register (CTL0)
  /// which controls the configurations for the digitally controlled oscillator.
  struct Control0Register  // NOLINT
  {
    /// DCO tuning value bit mask.
    static constexpr bit::Mask kTuningSelect = bit::CreateMaskFromRange(0, 9);
    /// DCO frequency seelect bit mask.
    static constexpr bit::Mask kFrequencySelect =
        bit::CreateMaskFromRange(16, 18);
    /// DCO enable bit mask.
    static constexpr bit::Mask kEnable = bit::CreateMaskFromRange(23);
  };

  /// Namespace containing the bit masks for the Control 1 Register (CTL1)
  /// which controls the configurations for selecting the oscillator source and
  /// clock divider for the primary clock signals.
  struct Control1Register  // NOLINT
  {
    /// Master clock source select bit mask.
    static constexpr bit::Mask kMasterClockSourceSelect =
        bit::CreateMaskFromRange(0, 2);
    /// Subsystem master clock source select bit mask.
    static constexpr bit::Mask kSubsystemClockSourceSelect =
        bit::CreateMaskFromRange(4, 6);
    /// Auxiliary clock source select bit mask.
    static constexpr bit::Mask kAuxiliaryClockSourceSelect =
        bit::CreateMaskFromRange(8, 10);
    /// Backup clock source select bit mask.
    static constexpr bit::Mask kBackupClockSourceSelect =
        bit::CreateMaskFromRange(12);
    /// Master clock divider select bit mask.
    static constexpr bit::Mask kMasterClockDividerSelect =
        bit::CreateMaskFromRange(16, 18);
    /// Subsystem master clock divider select bit mask.
    static constexpr bit::Mask kSubsystemClockDividerSelect =
        bit::CreateMaskFromRange(20, 22);
    /// Auxiliary clock divider select bit mask.
    static constexpr bit::Mask kAuxiliaryClockDividerSelect =
        bit::CreateMaskFromRange(24, 26);
    /// Low speed subsystem master clock divider select bit mask.
    static constexpr bit::Mask kLowSpeedSubsystemClockDividerSelect =
        bit::CreateMaskFromRange(28, 30);
  };

  /// Namespace containing the bit masks for the Clock Enable Register (CLKEN).
  struct ClockEnableRegister  // NOLINT
  {
    /// Reference clock frequency select bit mask.
    static constexpr bit::Mask kReferenceFrequencySelect =
        bit::CreateMaskFromRange(15);
  };

  /// @see Figure 6-1. Clock System Block Diagram
  ///      https://www.ti.com/lit/ug/slau356i/slau356i.pdf#page=380
  struct ClockConfiguration_t
  {
    /// Configurations for the auxiliary clock module.
    struct
    {
      /// @note Only the reference clock, very low frequency clock, or the low
      ///       frequency clock can be used as the clock source.
      Oscillator clock_source = Oscillator::kReference;
      /// Clock divider for the auxiliary clock.
      ClockDivider divider = ClockDivider::kDivideBy1;
    } auxiliary = {};

    /// Configurations for the master clock module. This clock module is also
    /// used to drive the CPU clock.
    struct
    {
      /// On reset, the master clock is driven by the digitally controlled
      /// clock.
      Oscillator clock_source = Oscillator::kDigitallyControlled;
      /// Clock divider for the master clock.
      ClockDivider divider = ClockDivider::kDivideBy1;
    } master = {};

    /// Configurations for the subsystem master clock module.
    struct
    {
      /// On reset, the subsystem master clocks are driven by the DCO clock.
      ///
      /// @note The specified clock source is used to drive both the subsystem
      ///       master clock and the low speed subsystem master clock.
      Oscillator clock_source = Oscillator::kDigitallyControlled;
      /// Clock divider for the subsystem master clock.
      ClockDivider divider = ClockDivider::kDivideBy1;
      /// Clock divider for the low speed subsystem master clock.
      ClockDivider low_speed_divider = ClockDivider::kDivideBy1;
    } subsystem_master = {};

    /// Configurations for the backup clock module.
    struct
    {
      /// Clock source for the backup clock.
      ///
      /// @note Only the reference clock or the low frequency clock can be used
      ///       as the clock source.
      Oscillator clock_source = Oscillator::kReference;
    } backup = {};

    /// Configurations for the reference clock. The reference clock is
    /// configurable to output 32.768 kHz or 128 kHz.
    struct
    {
      /// When frequency_select = 0b0, the reference clock outputs 32.768 kHz.
      /// When frequency_select = 0b1, the reference clock outputs 128 kHz.
      uint8_t frequency_select = 0b0;
    } reference = {};

    /// Configurations for the digitally controlled (DCO) clock module.
    struct
    {
      /// On reset, the digitally controlled clock is used to drive the master
      /// clock and the subsystem master clocks. This value should be set to
      /// false if an alternate clock source is used to drive those clocks.
      bool enabled = true;
      /// The target DCO output frequency. On reset, DCO outputs 3 MHz.
      units::frequency::hertz_t frequency = 3_MHz;
    } dco = {};
  };

  /// Reference to the structure containing the clock system control registers.
  inline static CS_Type * clock_system = msp432p401r::CS;

  /// Reference to the device descriptor tag-length-value (TLV) structure
  /// containing the clock system calibration constants.
  inline static TLV_Type * device_descriptors = msp432p401r::TLV;

  /// The number of available clocks that can be used by the system.
  inline static constexpr size_t kClockPeripheralCount = 10;

  /// @param clock_configuration The desired clock configurations for the
  ///                            system.
  explicit constexpr SystemController(
      ClockConfiguration_t & clock_configuration)
      : clock_configuration_(clock_configuration)
  {
  }

  /// Initializes the system controller by configuring the DCO clock, reference
  /// clock, and all the primary clocks.
  void Initialize() override
  {
    // For an overview of the clock system, see:
    // https://www.ti.com/lit/ug/slau356i/slau356i.pdf#page=380

    // Internal oscillators with fixed frequencies.
    constexpr units::frequency::hertz_t kVlo =
        InternalOscillator::kVeryLowFrequency;
    constexpr units::frequency::hertz_t kMod = InternalOscillator::kModule;
    constexpr units::frequency::hertz_t kLfxt =
        ExternalOscillator::kLowFrequency;
    constexpr units::frequency::hertz_t kHfxt =
        ExternalOscillator::kHighFrequency;

    // =========================================================================
    // Step 1. Configure the DCO and Reference clocks
    // =========================================================================
    units::frequency::hertz_t dco  = ConfigureDcoClock();
    units::frequency::hertz_t refo = ConfigureReferenceClock();

    // =========================================================================
    // Step 2. Set the clock source for each of the primary clocks
    // =========================================================================
    SetClockSource(Clock::kAuxiliary,
                   clock_configuration_.auxiliary.clock_source);
    SetClockSource(Clock::kMaster, clock_configuration_.master.clock_source);
    SetClockSource(Clock::kSubsystemMaster,
                   clock_configuration_.subsystem_master.clock_source);
    SetClockSource(Clock::kBackup, clock_configuration_.backup.clock_source);

    // =========================================================================
    // Step 3. Configure primary clock dividers
    // =========================================================================
    SetClockDivider(Clock::kAuxiliary, clock_configuration_.auxiliary.divider);
    SetClockDivider(Clock::kMaster, clock_configuration_.master.divider);
    SetClockDivider(Clock::kSubsystemMaster,
                    clock_configuration_.subsystem_master.divider);
    SetClockDivider(Clock::kLowSpeedSubsystemMaster,
                    clock_configuration_.subsystem_master.low_speed_divider);

    // =========================================================================
    // Step 4. Determine the clock rate of each of the clocks based on the
    //         configured clock source and clock dividers.
    // =========================================================================
    units::frequency::hertz_t aclk  = 0_Hz;
    units::frequency::hertz_t mclk  = 0_Hz;
    units::frequency::hertz_t smclk = 0_Hz;
    units::frequency::hertz_t bclk  = 0_Hz;

    switch (clock_configuration_.auxiliary.clock_source)
    {
      case Oscillator::kLowFrequency: aclk = kLfxt; break;
      case Oscillator::kVeryLowFrequency: aclk = kVlo; break;
      case Oscillator::kReference: aclk = refo; break;
      default: break;
    }

    switch (clock_configuration_.master.clock_source)
    {
      case Oscillator::kLowFrequency: mclk = kLfxt; break;
      case Oscillator::kVeryLowFrequency: mclk = kVlo; break;
      case Oscillator::kReference: mclk = refo; break;
      case Oscillator::kDigitallyControlled: mclk = dco; break;
      case Oscillator::kModule: mclk = kMod; break;
      case Oscillator::kHighFrequency: mclk = kHfxt; break;
    }

    switch (clock_configuration_.subsystem_master.clock_source)
    {
      case Oscillator::kLowFrequency: smclk = kLfxt; break;
      case Oscillator::kVeryLowFrequency: smclk = kVlo; break;
      case Oscillator::kReference: smclk = refo; break;
      case Oscillator::kDigitallyControlled: smclk = dco; break;
      case Oscillator::kModule: smclk = kMod; break;
      case Oscillator::kHighFrequency: smclk = kHfxt; break;
    }

    switch (clock_configuration_.backup.clock_source)
    {
      case Oscillator::kLowFrequency: bclk = kLfxt; break;
      case Oscillator::kReference: bclk = refo; break;
      default: break;
    }

    clock_rates_[Value(Clock::kAuxiliary)] =
        aclk / (1 << Value(clock_configuration_.auxiliary.divider));
    clock_rates_[Value(Clock::kMaster)] =
        mclk / (1 << Value(clock_configuration_.master.divider));
    clock_rates_[Value(Clock::kSubsystemMaster)] =
        smclk / (1 << Value(clock_configuration_.subsystem_master.divider));
    clock_rates_[Value(Clock::kLowSpeedSubsystemMaster)] =
        smclk /
        (1 << Value(clock_configuration_.subsystem_master.low_speed_divider));
    clock_rates_[Value(Clock::kBackup)]    = bclk;
    clock_rates_[Value(Clock::kReference)] = refo;
  }

  /// @returns A pointer to the clock configuration object used to configure
  ///          this system controller.
  void * GetClockConfiguration() override
  {
    return &clock_configuration_;
  }

  /// @returns The clock rate frequency of a clock system module.
  units::frequency::hertz_t GetClockRate(PeripheralID peripheral) const override
  {
    if (peripheral.device_id >= kClockPeripheralCount)
    {
      return 0_MHz;
    }

    return clock_rates_[peripheral.device_id];
  }

  /// Configures the clock divider for one of the four primary clock signals
  /// (ACLK, MCLK, HSMCLK, or SMCLK).
  ///
  /// TODO(#): Migrate to using Error v2.
  ///
  /// @param clock   The clock to configure.
  /// @param divider The desired clock divider value. Only the following
  ///                dividers are available: 1, 2, 4, 8, 16, 32, 64, 128.
  void SetClockDivider(Clock clock, ClockDivider divider) const
  {
    SJ2_ASSERT_FATAL(
        Value(clock) <= Value(Clock::kLowSpeedSubsystemMaster),
        "Only the following clocks have a clock divider: kAuxiliary, kMaster, "
        "kSubsystemMaster, or kLowSpeedSubsystemMaster.");

    constexpr bit::Mask kDividerSelectMasks[] = {
      Control1Register::kAuxiliaryClockDividerSelect,
      Control1Register::kMasterClockDividerSelect,
      Control1Register::kSubsystemClockDividerSelect,
      Control1Register::kLowSpeedSubsystemClockDividerSelect,
    };
    UnlockClockSystemRegisters();
    {
      clock_system->CTL1 = bit::Insert(clock_system->CTL1, Value(divider),
                                       kDividerSelectMasks[Value(clock)]);
    }
    LockClockSystemRegisters();
    WaitForClockReadyStatus(clock);
  }

  bool IsPeripheralPoweredUp(PeripheralID) const override
  {
    return false;
  }

  // TODO(#): Migrate to using Error v2.
  void PowerUpPeripheral(PeripheralID) const override
  {
    SJ2_ASSERT_FATAL(false, "This function is not implemented.");
  }

  // TODO(#): Migrate to using Error v2.
  void PowerDownPeripheral(PeripheralID) const override
  {
    SJ2_ASSERT_FATAL(false, "This function is not implemented.");
  }

 private:
  /// Unlocks the clock system registers by writing the necessary value to the
  /// CSKEY register.
  void UnlockClockSystemRegisters() const
  {
    constexpr uint16_t kUnlockKey = 0x695A;
    clock_system->KEY =
        bit::Insert(clock_system->KEY, kUnlockKey, KeyRegister::kCsKey);
  }

  /// Locks the clock system registers by writing the necessary value to the
  /// CSKEY register.
  void LockClockSystemRegisters() const
  {
    constexpr uint16_t kLockKey = 0x0000;
    clock_system->KEY =
        bit::Insert(clock_system->KEY, kLockKey, KeyRegister::kCsKey);
  }

  /// Checks and waits for a clock signal to become stable after a frequency or
  /// divider configuration.
  ///
  /// TODO(#): Migrate to using Error v2.
  ///
  /// @note This feature is only available for the primary clock signals.
  ///
  /// @param clock The primary clock signal ready status to wait on.
  void WaitForClockReadyStatus(Clock clock) const
  {
    if constexpr (build::kPlatform == build::Platform::host)
    {
      return;
    }
    SJ2_ASSERT_FATAL(
        Value(clock) <= Value(Clock::kBackup),
        "Only the following clocks have a ready status: kAuxiliary, kMaster, "
        "kSubsystemMaster, kLowSpeedSubsystemMaster, or kBackup.");

    constexpr uint8_t kClockReadyBit = 24;
    const uint8_t kOffset            = Value(clock);
    const bit::Mask kReadyBitMask    = bit::CreateMaskFromRange(
        static_cast<uint8_t>(kClockReadyBit + kOffset));
    while (!bit::Read(clock_system->STAT, kReadyBitMask))
    {
      continue;
    }
  }

  /// Configures one of the five primary clock signals (ACLK, MCLK,
  /// HSMCLK / SMCLK, and BCLK) to be sourced by the specified oscillator.
  ///
  /// TODO(#): Migrate to using Error v2.
  ///
  /// @note When selecting the oscillator source for either SMCLK or SMCLK, the
  ///       oscillator will applied to both clock signals.
  ///
  /// @see https://www.ti.com/lit/ug/slau356i/slau356i.pdf#page=397
  ///
  /// @param clock      The primary clock to configure.
  /// @param oscillator The oscillator used to source the clock.
  void SetClockSource(Clock clock, Oscillator source) const
  {
    constexpr bit::Mask kPrimaryClockSelectMasks[] = {
      Control1Register::kAuxiliaryClockSourceSelect,
      Control1Register::kMasterClockSourceSelect,
      Control1Register::kSubsystemClockSourceSelect,
      Control1Register::kSubsystemClockSourceSelect,
      Control1Register::kBackupClockSourceSelect,
    };
    uint8_t select_value = Value(source);
    switch (clock)
    {
      case Clock::kMaster: [[fallthrough]];
      case Clock::kSubsystemMaster: [[fallthrough]];
      case Clock::kLowSpeedSubsystemMaster: break;
      case Clock::kAuxiliary:
        SJ2_ASSERT_FATAL(
            select_value <= Value(Oscillator::kReference),
            "The auxiliary clock can only be driven by kLowFrequency, "
            "kVeryLowFrequency, or kReference.");
        break;
      case Clock::kBackup:
        switch (source)
        {
          case Oscillator::kLowFrequency: break;
          case Oscillator::kReference:
          {
            select_value = 0b1;
          }
          break;
          default:
            SJ2_ASSERT_FATAL(false,
                             "The backup clock can only be driven by "
                             "kLowFrequency or kReference.");
            return;
        }
        break;
      default:
        SJ2_ASSERT_FATAL(
            false,
            "clock must be one of the five primary clocks: kAuxiliary, "
            "kMaster, kSubsystemMaster, kLowSpeedSubsystemMaster, or kBackup.");
        return;
    }
    clock_system->CTL1 = bit::Insert(clock_system->CTL1, select_value,
                                     kPrimaryClockSelectMasks[Value(clock)]);
  }

  /// Configures the DCO clock to generate a desired target frequency.
  ///
  /// TODO(#): Migrate to using Error v2.
  ///
  /// @see 6.2.8.3 DCO Ranges and Tuning in the MSP432P4xx Reference Manual.
  ///      https://www.ti.com/lit/ug/slau356i/slau356i.pdf#page=386
  ///
  /// @returns The DCO output frequency.
  units::frequency::hertz_t ConfigureDcoClock() const
  {
    const auto & dco_config = clock_configuration_.dco;
    const units::frequency::kilohertz_t kTargetFrequency = dco_config.frequency;

    // Configure DCO only if it is used as a clock source.
    if (dco_config.enabled)
    {
      // =======================================================================
      // Step 1. Ensure the target frequency is valid.
      // =======================================================================
      constexpr units::frequency::megahertz_t kMinimumDcoFrequency = 1_MHz;
      constexpr units::frequency::megahertz_t kMaximumDcoFrequency = 48_MHz;
      SJ2_ASSERT_FATAL(
          (kMinimumDcoFrequency <= kTargetFrequency) &&
              (kTargetFrequency <= kMaximumDcoFrequency),
          "The desired frequency must be between 1 MHz and 48 MHz.");

      // =======================================================================
      // Step 2. Determine the DCO tuning configuration values by finding the
      //         DCO frequency range, DCO constant, and DCO calibration values
      //         based on the desired target frequency.
      // =======================================================================
      uint8_t dco_frequency_select = 0b000;
      float dco_constant =
          static_cast<float>(device_descriptors->DCOIR_CONSTK_RSEL04);
      int32_t dco_calibration =
          static_cast<int32_t>(device_descriptors->DCOIR_FCAL_RSEL04);

      if (kTargetFrequency >= 32_MHz)
      {
        // target frequency is 32 MHz to 48 MHz
        dco_frequency_select = 0b101;
        dco_constant =
            static_cast<float>(device_descriptors->DCOIR_CONSTK_RSEL5);
        dco_calibration =
            static_cast<int32_t>(device_descriptors->DCOIR_FCAL_RSEL5);
      }
      else if (kTargetFrequency >= 16_MHz)
      {
        // target frequency is 16 MHz to 32 MHz
        dco_frequency_select = 0b100;
      }
      else if (kTargetFrequency >= 8_MHz)
      {
        // target frequency is 8 MHz to 16 MHz
        dco_frequency_select = 0b011;
      }
      else if (kTargetFrequency >= 4_MHz)
      {
        // target frequency is 4 MHz to 8 MHz
        dco_frequency_select = 0b010;
      }
      else if (kTargetFrequency >= 2_MHz)
      {
        // target frequency is 2 MHz to 4 MHz
        dco_frequency_select = 0b001;
      }
      else
      {
        // target frequency is 1 MHz to 2 MHz with center frequency of 1.5 MHz
        // use the default initialized values
      }

      // =======================================================================
      // Step 3. Calculate the signed 10-bit tuning value using Equation 6 from
      //         https://www.ti.com/lit/ug/slau356i/slau356i.pdf#page=387
      // =======================================================================
      constexpr units::frequency::kilohertz_t kDcoCenterFrequencies[] = {
        1500_kHz, 3_MHz, 6_MHz, 12_MHz, 24_MHz, 48_MHz
      };
      const auto kCenterFrequency = kDcoCenterFrequencies[dco_frequency_select];
      const float kFrequencyDifference =
          (kTargetFrequency - kCenterFrequency).to<float>();
      const float kCalibration =
          (1.0f + dco_constant * static_cast<float>(768 - dco_calibration));

      const float kDividend = kFrequencyDifference * kCalibration;
      const float kDivisor  = kTargetFrequency.to<float>() * dco_constant;

      const int16_t kTuningValue = static_cast<int16_t>(kDividend / kDivisor);

      // =======================================================================
      // Step 4. Configure the CSCTL0 register.
      // =======================================================================
      UnlockClockSystemRegisters();
      {
        uint32_t control0register = clock_system->CTL0;
        control0register          = bit::Insert(control0register, kTuningValue,
                                       Control0Register::kTuningSelect);
        control0register = bit::Insert(control0register, dco_frequency_select,
                                       Control0Register::kFrequencySelect);
        control0register =
            bit::Set(control0register, Control0Register::kEnable);
        clock_system->CTL0 = control0register;
      }
      LockClockSystemRegisters();
    }

    return kTargetFrequency;
  }

  /// Configures the reference clock to output either 32.768 kHz or 128 kHz.
  ///
  /// TODO(#): Migrate to using Error v2.
  ///
  /// @see Table 6-8. CSCLKEN Register Description
  ///      https://www.ti.com/lit/ug/slau356i/slau356i.pdf#page=401
  ///
  /// @returns The reference clock frequency.
  units::frequency::hertz_t ConfigureReferenceClock() const
  {
    const uint8_t kFrequencySelect =
        clock_configuration_.reference.frequency_select;

    SJ2_ASSERT_FATAL(kFrequencySelect <= 0b1,
                     "Invalid frequency_select code for configuring the "
                     "reference clock frequency.");

    clock_system->CLKEN =
        bit::Insert(clock_system->CLKEN, kFrequencySelect,
                    ClockEnableRegister::kReferenceFrequencySelect);

    return InternalOscillator::kReference[kFrequencySelect];
  }

  /// Clock system configurations.
  ClockConfiguration_t & clock_configuration_;
  /// Array containing the clock rates of all the clock system peripherals.
  std::array<units::frequency::hertz_t, kClockPeripheralCount> clock_rates_ = {
    0_Hz,                                   // auxiliary clock
    0_Hz,                                   // master clock
    0_Hz,                                   // subsystem master clock
    0_Hz,                                   // low speed subsystem master clock
    0_Hz,                                   // backup clock
    ExternalOscillator::kLowFrequency,      // low frequency clock
    InternalOscillator::kVeryLowFrequency,  // very low frequency clock
    0_Hz,                                   // reference clock
    InternalOscillator::kModule,            // module clock
    InternalOscillator::kSystem,            // system clock
  };
};
}  // namespace msp432p401r
}  // namespace sjsu