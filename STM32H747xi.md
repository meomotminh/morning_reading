## FDCAN1, FDCAN2

CAN subsystem consist of 2 CAN modules. a shared message RAM memory and a clock calibration unit.

Both AN modeuls compliant with ISO 11898-1 and CAN FD protocol specification version 1.0

FDCAN1 supports time triggered CAN (TT-FDCAN), including event synchronized time-triggered communication, global system time, and clock drift compensation. FDCAN1 contains additional registers specific to the time triggered feature. CAN FD option can be used together with event-triggered and time-triggered CAN communication

A 10 KB message RAM memory implements filters, receive FIFOS, receive buffer, transmit event FIFOs, transmit buffer and trigger for TTFDCAN. This message RAM is shared between FDCAN1 and FDCAN2 modules

The common clock caliration unit is optional. can be used to calibrated clock for both FDCAN1 and FDCAN2 from HSI internal RC oscillator and the PLL, by evaluating CAN messages received by FDCAN1

CAN subsystem I/O signals

- fdcan_ker_clk     Digital Input       CAN subsystem kernel clock input
- fdcan_pclk        Digital Input       CAN subsytem APB interface clock input
- fdcan_cal_it      Digital Output      FDCAN caliration interrupt
- fdcan1_intr0_it   Digital Output      FDCAN1 interrupt0
- fdcan1_intr1_it   Digital Output      FDCAN1 interrupt1
- fdcan2_intr0_it   Digital Output      FDCAN2 interrupt0
- fdcan2_intr1_it   Digital Output      FDCAN2 interrupt1
- fdcan1_swt[0:3]   Digital input       Stop watch trigger input
- fdcan1_evt[0:3]   Digital Input       Event Trigger Input
- fdcan1_ts[0:15]   Digital Input       External timestamp verctor
- fdcan1_soc        Digital Output      Start of Cycle pulse
- fdcan1_rtp        Digital Output      Register time mark pulse
- fdcan1_tmp        Digital Output      TRigger time mark pulse

CAN subsystem I/O pins

- FDCAN1_RX         Digital Input       FDCAN1 receive pin
- FDCAN2_RX         Digital Input       FDCAN2 receive pin
- FDCAN1_TX         Digital Output      FDCAN1 transmit pin
- FDCAN2_TX         Digital Output      FDCAN2 transmit pin

FDCAN main features

- Conform with CAN protocol ver 2
- CAN FD with max 64 data bytes
- TTCAN protocol level 1 and level 2 completely in hardware
- Event Synch time-triggered communication supported
- CAN error logging
- AUTOSAR and J1939 support
- Improved acceptance filtering
- 2 configurable receive FIFOS
- Separate signaling on reception of high priority message
- up t 64 dedicated receive buffer
- up to 32 dedicated transmitt buffer
- configuratble transmit FIFO queue
- configuratble transmit event FIFO
- FDCAN modules share the same message RAM
- programmable loop-back test mode
- maskable module interrupts
- 2 clock domains: APB bus interface and CAN core kernel clock
- power down support


- Dual interrupt lines:
    - FDCAN peripheral provides 2 interrupt lines fdcan_intr0_it & fdcan_int1_it 
    - EINT0 & EINT1 in FDCAN_ILE to enable or disable
- CAN Core
    - contain protocol controller and receive/transmit shift registers
    - handle all ISO 11898-1:2015 protocol function and support both 11-bit and 29-bit identifier
- Sync
    - sync signals from the APB clock domain to the CAN kernel clock domain and vice versa
- Tx Handler
    - control msg transfer from msg RAM to the CAN core. max of 32 Tx buffers can be configured for transmission. Tx buffers can be used as dedicated Tx buffers as Tx FIFO or as combination of them
    - A Tx event FIFO stores Tx timestamps together with corresponding message ID
    - On FDCAN1, Tx handler also implemnet frame sync which control time-triggered communication, sync itself to reference message on CAN bus
- Rx Handler
    - control transfer of received msg from CAN core to external message RAM
    - support 2 receive FIFOs
    - a dedicated Rx buffer used to srtoed only msg with a specific ID
    - an Rx timestamp is stored together with each msg
    - upto 128 filters can be defined for 11 bit IDs
- ADB interface
    - connect FDCAN to APB bus
- Message RAM interface
    - connect FDCAN access to an external 10 KB message RAM through a RAM controller

## OPerating modes

- Software Initialization:
    - set INIT in FDCAN_CCCR (by reset or by going Bus_Off)
    - While INIT is set, msg tranfer is stopped, status of CAN bus output FDCAN_TX is recessive (high). not change any config register. clearing INIT finish software initialization. afterwards bit tream processor (BSP) synch itself to the data transfer on the CAN bus by waitign for 11 consecutive recessive bits (Bus_Idle)
    - Access to FDCAN config registers is only enabled when both INIT bit in FDCAN_CCCR and CCE bit in FDCAN_CCCR register are set
    - CCE bit in FDCAN_CCCR can only be set/clear while INIT bit in FDCAN_CCCR is set
    - CCE bit in FDCAN_CCCR is auto cleared then INIT bit in FDCAN_CCCR is cleared
    - the following register are reset when CCE bit in FDCAN_CCCR is set:
        - FDCAN_HPMS - high priority msg status
        - FDCAN_RXF0S - Rx FIFO 0 status
        - FDCAN_RXF1S - Rx FIFO 1 status
        - FDCAN_TXFQS - Tx FIFO status
        - FDCAN_TXBRP - Tx buffer request pending
        - FDCAN_TXBTO - Tx buffer transmission occurred
        - FDCAN_TXBCF - Tx buffer cancellation finished
        - FDCAN_TXEFS - Tx event FIFO status
        - FDCAN_TTOST - TT (time trigger) operation status (FDCAN1)
        - FDCAN_TTLGT - TT local and global time, only FDCAN_TTLGT.GT is reset
        - FDCAN_TTCTC - TT cycle time and count (FDCAN1)
        - FDCAN_TTCSM - TT cycle sync mark (FDCAN1)
    - The timeout counter TOC bit in FDCAN_TOCV register is preset to TOP bit in FDCAN_TOCC when CCE bit is set
    - Tx handler and Rx handler are held in idle state while CCE is set
    - following register can be written only when CCE is cleared:
        - FDCAN_TXBAR - Tx buffer add request
        - FDCAN_TXBCR - Tx buffer cancellation request
    - TEST bit in FDCAN_CCCR and MON bit in FDCAN_CCCR can only set by software while both INIT and CCE are set. Both bits can be reset any time. DAR bit in FDCAN_CCCR only set/cleared while both INIT and CCE are set

# NOrmal operation

- default operating mode after hardware reset is event-driven CAN communication withou time Triggers (FDCAN_TTOCF.OM = 00). required that both INIT and CCE in FDCAN_CCCR are set before TT operation mode can be changed
- Once FDDCAN is initialized and INIT bit cleared, FDCAN synch itself to CAN bus and ready for communication
- After passing acceptance filter, received msg including msg ID and DLC are stored into dedicated Rx buffer or Rx FIFO0 or Rx FIFO1
- for msg to be transmitted Tx buffers and T FIFO or Tx queue can be inititialized . Auto transmission on reception of remote frames is not supported

## CAN FD operation

- 2 variant in CAN FD protocol: 
    - Long frame mode (LFM): data field of CAN frame > 8
    - Fast frame mode (FFM): control field, data field, CRC are transmitted with higher bitrate than the beginning and end of the frame
        - FDF 1 signifies CAN FD frame: res and BRS decide whether bitrate inside this CAN FD frame is switched (res 0 and BRS 1)
        - FDF 0 signifies classic CAN frame
    - if protocol exception handling is enabled (FDCAN_CCCR.PXHD = 0), operation state to change from receiver (FDCAN_PSR.ACT = 10) to Integrating (FDCAN_PSR.ACT = 00)
    -  if protocol exception disabled (FDCAN_CCCR.PXHD = 1) FDCAN will treat res 1 as a form error and respond with error frame
    
- CAN FD operation is enabled by FDCAN_CCCR.FDOE = 1 -> CAN FD frame is enable.
- Classic CAN frame is always possible.
- FDF determined whether FD or classic CAN frame

## Chagne mode during CAN operation is only recommended:
    - failure rate in CAN FD data phase is significant higher thatn CAN FD arbitration phase -> disable CAN FD bitrate switching option
    - dueing system startup all nodes trasmitting classic CAN until it is verified that they can communicate CAN FD, if true all nodes switch to CAN FD
    - wake-up msg in CAN partial networking have to be transmitted in classic CAN
    - end-of-line programming in case not all nodes are CAN FD capable. Non CAN FD nodes are held in Silent mode until programming has completed then all nodes switch back to classic CAN

    In FDCAN, DLC 
    9: 12
    10: 16
    11: 20
    12: 24
    13: 32
    14: 48
    15: 64

    in CAN FD fast frames, bit timing will be switched inside the fame, after BRS bit if BRS = 1
    in CAN FD arbitration phase, nominal CAN bit timeing is used as defined by bit timing and prescaler register FDCAN_NBTP. 
    in following CAN FD data phase, fast CAN bit timing is used as defined by the fast bit timing and prescaler register FDCAN_DBTP.
    switch back at CRC demimiter or when error detected

- max bitrate in CAN FD data phase depend on FDCAN kernel clock frequency (clock 20 MHz, shorted configurable bit ime of 3 time quanta (tq) -> bitrate of data phase is 5 MBit/s)

- in both data frame formats, CAN FD long frames and CAN FD fast frames, value of ESI is determined by the transmitter error state at the statrt of transmisson. in FD CAN , ESI is allway 0. Data length of CAN FD remote frames is 0

- if FDCAN Tx buffer is configured for CAN FD transmission with DLC > 8, 1st 8 bytes transmitted as configured in the Tx buffer while the remaining is padded with 0xCC. when FDCAN receive FDCAN frame with DLC > 8, 1st 8 bytes are stored into mathching Tx buffer of Rx FIFO. teh remaining bytes are discarded

## Transceiver Delay Compensation

- During data phase of a CAN FD transmission only 1 node is transmitting, all other are receivers
- when transmitting via pin FDCAN_TX, protocol controller receive the transmitted data from its local CAN transceiver via pin FDCAN_RX.
- received data is delayed by CAN transceiver loop delay, in case this delay is greater than TSEG1 (time segment before sample point), a bit error is detected
- without transceiver delay compensation, the bitrate in the data phase of CAN FD frame is limited by the transceiver loop delay
- FDCAN implements delay compensation mechanism to compensate CAN transceiver loop delay, enabling transmission with higher bitrates during CAN FD independent of delay of CAN transceiver
- To check for bit errors durign data phase of transmitting node, the delayed transmit data is compared against the received data at the Secondary Sample Point SSP. if bit error is detected, transmitter will react on this bit error at the next following regular sample point. DUring arbitration phase, delay compensation is disabled
- transmitter delay compensation enables config where data bit ime is shorted than the transmitter delay, enabled by FDCAN_DBTP.TDC

- FDCAN_TDCR.TDCO transmitter delay compensation offset
- FDCAN_PSR.TDCV show actual transmitter delay compensation value, cleard then INIT is set and update at each transmission of an FD frame wihle FCAN_DBTC.TDC is set.
- sum of delay from FDCANx_TX to FDCANx_RX and transmitter delay compensation FDCAN_TDCR.TDCO < 6 bit times in the data phase
- and < 127 mtq
- data phase end at sample point of CRC delimiter, stops checking received bits at SSPs

# Restricted Operation Mode

- in restricted op mode, node receive data and remote froame and give ack to valid frames, but not send data frames...
- if error, does not send dominant bits, wait for the occurrance of bus idle to resynch
- error counters frozen while error logging is active. The software can set FDCAN into restricted operation mode by setting bit FDCAN_CCCR.ASM. The bit can only be set by software when both FDCAN_CCCR.CCE and FDCAN_CCCR.INIT set
- restricted 

# Bus Monitoring Mode

- FDCAN is set in bus monitoring mode by setting FDCAN_CCCR.MON bit or when error level S3. In bus monitoring mode FDCAN able to receive valid data frames and valid remote frames but not transmission. send only 1 on CAN bus, if FDCAN required to send 0, the bit is rerouted internally so that FDCAN monitors this 0.
- bus monitoring mode can be used to analyse the traffic on a CAN bus without affecting it by the transmission of 0.

# Disabled Automatic Retransmission (DAR) mode

- by default automatic retransmission is enabled
- to support time-triggered communication, automatic retransmission can be disabled via FDCAN_CCCR.DAR
- in DAR mode, all transmission are auto canceled after they started on the CAN bus
- Tx buffer Tx request pending bit FDCAN_TXBRP.TRPx is reset after successfully transmission.
    - FDCAN_TXBTO.TOx set
    - FDCAN_TXBCF.CFx not set
    - FDCAN_TXBTO.TOx set
    - FDCAN_TXBCF.CFx set
    - FDCAN_TXBTO.TOx not set
    - FDCAN_TXBCF.CFx set

# Power down mode

- FDCAN can be set into power down mode controlled by clock stop request input via FDCAN_CCCR.CSR = 1
- when all pending transmission have completed, FDCAN waits until bus idle state is detected. Then FDCAN set FDCAN_CCCR.INIT to 1  -> FDCAN_CCCR.CSA to 1.
- To leave power down mode, the app has to turn on the module clock before resetting CC control register FDCAN_CCCR.CSR

# Test mode

- To enable write access to FDCAN test reguster, FDCAN_CCCR.TEST = 1 thus enabling the configuration of test modes
- 4 output functinos FDCAN_TX by programming FDCAN_TEST.TX:
    - serial data output can drive CAN sample point signal to monitor the FDCAN bit timing and can drive the CAN Sample Point signal to monitor FDCAN bit timing ad can drive constant dominant or recessive values. 
- Test modes should be used for prouction tests or self test only

## External loop back mode

- FDCAN_TEST.LBCK = 1
- treat its own transmitted msg as received msg and stored into Rx FIFO
- hardware self-test, ignore ack errors in loop back mode. In this mode FDCAN Perform an internal feedback from its transmit output to its receive input. actual value of FDCAN_RX is disregarded

## Internal Loop back mode

- FDCAN_TEST.LBCK = 1 and FDCAN_CCCR.MON = 1
- use for "Hot Selftest", FDCAN can be tested without affecting a runnign CAN system connected 

## Application Watchdog (FDCAN1)

- reading register FDCAN_TTOST
- when app watchdog not served in time, FDCAN_TTOST.AWE is set, all TTCAN communication is stopped and FDCAN1 is set into bus monitoring mode
- TT app watchdog can be disabled by programming the app watchdog limit FDCAN_TTOCF.AWL = 0x00

## Timestamp generation

- Prescaler FDCAN_TSCC.TCP can be configured to clock counter in multiples of CAN bit times
- counter is reader via FDCAN_TSCV.TCV 
- Write reset to 0
- wrap around -> FDCAN_IR.TSW is set
- on start of frame reception/transmission, counter value is captured and stored into timestamp seection of a Rx buffer or Tx event FIFO
- FDCAN_TSCC.TSS 16-bit timestamp

## Timeout Counter

- 16-bit timeout counter
- prescaler FDCAN_TSCC.TCP
- FDCAN_TOCC
- actual counter value FDCAN_TOCV.TOC
- operating mode selected by FDCAN_TOCC.TOS
    - countinuos mode, counter start when FDCAN_CCCR.INIT is reset
    - control by FIFOs, empty FIFO preset counter to FDCAN_TOCC.TOP
    - reach 0 -> FDCAN_IR_TOO SET

## Messag RAM

## Rx handling


## Reset and Clock Control (RCC)

RCC block manage clock and reset generation for the whole microcontroller, which embeds 2 CPU: M7 and M4, CPU1 and CPU2
RCC block is located in D3 domain

### Main Features

- Reset block:
    - Generation of local and system reset
    - bidirectional pin reset allowing to reset the microcontroller or external devices
    - HOld boot function
    - WWDG reset support
- Clock generation block
    - generate and dispatch of clocks for the complete device
    - 3 separate PLLs using integer or fractional ratios
    - possibility to change PLL fractional ratios on-the-fly
    - Smart clock gating 
    - 2 external oscillators:
        - HSE crystal from 4 to 48 MHz
        - LSE 32kHz crystals
    - 4 internal oscillators:
        - HSI
        - 48 MHz RC oscillator (HSI48)
        - CSI
        - LSI
    - Buffered clock outputs for external devices
    - Generation of 2 type of interrupts:
        - clock security mangaement
        - general interrupt for other events
    - Clock generation handling in Stop and Standy mode

## RCC reset block functional description
Several sources can generate a reset:
    - an external device via NRST pin
    - a failure on the supply voltage applied to VDD
    - a watchdog timeout
    - a software command
Reset scope depend on the source that generate the reset. 3 reset categories exist:
    - power-on/off reset
    - system reset
    - local resets
### Power on/off reset
- activate when VDD is below a threshold levle
### System Reset
- reset from NRST pin 
- reset from power-on/off 
- reset from brownout reset
- reset from independent watchdogs
- software reset from M4 core: SFT2RESET
- software reset form M7 core: SFT1RESET
- reset from window watchdogs
- reset from low-power mode security reset
### LOcal resets
- Domain reset
    - D1,DD2 domain exit DStandby
### Reset Source Identification
- CPU can check reset source by checking reset flags in RCC_C1_RSR and RCC_C2_RSR
- each CPU can reset the flags of its own register by setting RMVF bit without interfering wit other CPU

## RCC Clock block functional description:
- HSI, HSE, LSE, LSI, CSI, HSI48
- 3 PLLs
- offer 2 clock outputs MCO1 and MCO2
- SCGU block (System Clock Generation Unit) contain several prescaler
- PKSU block (Peripheral Kernel clock Selection Unit) provide dynamic switches allowing a large choice of kernel clock distribution to peripheral
- PKEU (Peripheral Kernel Clock Enable Unit) and SCEU (System Clock Enable Unit) perform peripheral clock gating and bus interface matrix clock gating

- Peripheral Clocks:
    - bus interface clocks: to access its register (AHB, APB or AXI)
    - kernel clocks
- CPU clocks
- Bus matrix Clock

- External Clock Source (HSE Bypass)
    - external clock source must be provided to OSC_IN pin
    - selected by setting HSEBYP and HSEON on RCC_CR
- External crytal/ceramic resonator
    - setting HSEBYP bit to 0 and HSEON to 1

- HSERDY of RCC_CR indicate HSE is stable or not

- LSE similar

- HSI oscillator: default clock, high-speed internal RC oscillator : 8, 16, 32 or 64 MHz via HSIDIV
    - time interval between kernal clock request and clock really available
    - frequency accuracy
- CSI oscillator: 4 MHz
    - Low cost
    - faster startup
    - very low-power consumption
- HSI48 : 48 MHz
    - aim at providign high precisiono clock to the USB peripheral by means of a special Clock Recovery System (CRS) circuitry
- LSI: 32kHz for watchdog and Auto-wakeup unit

## Clock Security System (CSS)
- HSECSSON = 1, cannot directly clear HSECSSON but by reset

## MCO1/MCO2

## PLL
    - PLL1 provide clocks to CPUs and some peripheral
    - PLL2 & PLL3 generate kernel clock for peripherals
    - 2 embedded VCOs
        - a wide-range VCO(VCOH)
        - low-frequency VCO(VCOL)
    - input frequency range:
        - 1 to 2 MHz when VCOL is used
        - 2 to 16 MHz when VCOH
    - interger or fractional mode
    - 13-bit Sigma-Delta modulator
    - each PLL offer 3 outputs with post-divider
    - control via RCC_PLLxDIVR, RCC_PLLxFRACR, RCC_PLLCFGR and RCC_CR
    - frequency must range from 1 to 16 MHz (DIVMx divider of RCC_PLLCKSELR)
    - PLLxRGE of RCC_PLLCFGR set according to reference input frequency to guarantee optimal performance
    - PLL can be enaled by setting PLLxON, PLLxRDY indicate PLL is ready

## Output frequency computation
- integer mode: SH_REG = 0
    - FVCO = FREF_BK * DIVN

## Initialization phase
- initialize PLLs register according to the requied f
    - set PLLxFRACEN = 0 for interger mode
    - set PLLxFRACEN = 1 for fractional mode
- once PLLxON = 1, wait unti PLLxRDY = 1
- once PLLxRDY = 1 -> ready to use
- if tune PLLx f on-the-fly
    - PLLxFRACEN = 0
    - new value uploaded into PLLxFRACR
    - PLLxFRACEN = 1

## FDCAN clock_
- FDCANSEL : hse_ck | pll1_q_ck | pll2_q_ck
- FDCANEN : enable
- FDCANLPEN : low power enable
- fdcan_ker_ck


## Transceiver Delay compensation

During data phase of CAN FD, only 1 node is transmitting, others are receivers. Length of bus line has no impact. When transmitting via FDCAN_TX protocol controller receive transmitted data from its local CAN transceiver via FDCAN_RX. received data is delayed by CAN transceiver loop delay. in case this delay greatedr than TSEG1, a bit error is detected. Without transceiver delay compensation, bitrate in the data phase of a CAN FD frame is limited by the transceiver loop delay

- CHeck for bit error during data phase of transmitting, delayde transmit data is compared against received data at Secondary Sample Point SSP. If bit error is detected, transmitter will react on this bit error at the next following regular sample point.

## Timestamp generation:

For timestamp generation the FDCAN supplies a 16-bit wraparound counter. a prescaler FDCAN_TSCC.TCP can be configured to clock the counter in multiples of CAN bit times (1..16). The counter is readable via FDCAN_TSCV.TCV. When timecounter wrap around interrupt flag FDCAN_IR.TSW is set.

On start of frame reception/transmission the counter value is captured and stored into the timestamp section of a Rx or Tx FIFO

FDCAN_TSCC.TSS

## Timeout counter:

16-bit timeout counter, use the same prescaler controlled by FDCAN_TSCC.TCP as timestamp counter. Timeout counter is configured via FDCAN_TOCC. the actual counter value can be read from FDCAN_TOCV.TOC

- Interrupt FDCAN_IR.TOO is set

## Acceptance Filter

FDCAN can configure 2 sets of acceptance filters, 1 for standard ID and 1 for Extended ID. These filters can be assigned to Rx Buffer, Rx FIFO 0 or Rx FIFO 1. For acceptance filtering each list of filters is executed from element #0 until the first matching element. Acceptance filtering stop at the first matching element

- each filter element can be configured as
    - range filter 
    - filter for 1 or 2 dedicated ID
    - classic bit mask filter
- each filter element is configurable for acceptance or rejection filtering
- each filter element can be enable/disable
- check sequentially, stop with the first matching filter element

- Global filter configuration (FDCAN_GFC)
- Standard ID filter configuration (FDCAN_SIDFC)
- Extended ID filter configuration (FDCAN_XIDFC)
- Extended ID AND Mask (FDCAN_XIDAM)

- Depend on the config of the filter element (SFEC/EFEC) a match trigger 1 of the following actions:
    - Store receive frame in FIFO 0 or 1
    - Store receive frame in Rx buffer
    - Store receive frame in Rx buffer and generate pulse at filter event pin
    - Reject received frame
    - Set high priority message interrupt flag FDCAN_IR.HPM
    - Set high priority message interrupt flag FDCAN_IR.HPM and store received frame in FIFO 0 or FIFO 1

Acceptance filtering is started after the complete ID has been received. After acceptance filtering completed, if a matching Rx buffer or Rx FIFO has been found, message handler start writing the received message data in portion of 32 bit to the matching Rx buffer or Rx FIFO

## Range Filter

- FIlter match for all received frames with message ID in range defined by SF1ID/SF2ID and EF1ID/EF2ID
    - EFT = 00: message ID of received frames is AND with the extended ID AND Mask before the range filter is applied
    - EFT = 11: the extended ID AND Mask is not used for range filtering

## Filter for dedicated IDs

- to filter for 1 specific message ID, the filter element has to be configured with SF1ID = SF2ID and EF1ID = EF2ID

## Classic Bit mask filter

- SF1ID/EF1ID is used as message UD filter while SF2ID/EF2ID is used as filter mask
- 0 bit at filter mask will mask out the corresponding bit position of the configured ID filter -> not relevant
- in case a

## TTCAN

### Reference Message
- level 1 _ data length must be at least 1
- level 0,2 _ data length must be at least 4

### TTCAN Timing
- Network Time Unit(NTU) is the unit in which all times are measured
- initial reference trigger offset FDCAN_TTOC.IRTO 7-bit how long a backup time master wait before transmiss reference mssag 
- FDCAN_TTOCF.OM operate in TTCAN level 0, 1 or 2.

## Trigger memory

- is part of the external message RAM to which the TTCAN is connected to
- upto 64 trigger elements
- a trigger memory element consist of time mark TM, cycle code CC, trigger type TYPE, filter type FTYPE, message number MNR, message status count MSC, time mark event internal TMIN, time mark event external TMEX
- time mark define at which cycle time a trigger become active

## Trigger Types

- Tx_Ref_Trigger (TYPE = 0000) and 



## Reference Manual

### TX Handler:

- Control msg transfer ffrom message RAM to CAN core. max of 32 Tx Buffers can be configured for trnasmission. Tx buffer can be used as dedicated Tx buffer, Tx FIFO, part of a Tx que or combination. Tx event FIFO store Tx timestamps together with message ID
- on FDCAN1, Tx handler also implement Frame Synch Entity (FSE) which control time-triggered communication. Synch itself with the reference message on the CAN bus, control cycle time and global time and handle transmissions according to the predefined mssage schedule, the system matrix. also handles time marks of the sstem matrix are linked to the messages in the message RAM. Stop watch trigger, event trigger and time mark interrupt

### Rx Handler:

- control transfer of received msg from CAN core to the external msg RAM. Rx handler support 2 receive FIFOs, upto 64 Rx buffer to stored all msg . A didicated Rx buffer is to store only msg with a specific ID. Rx timestamp is stored together with each msg. 128 filters can be defined for 11-bit IDs and 64 filters for 29-bit ID

### Message RAM interface

### CAN FD operation

- 2 variants in the CAN FD protocol: LFM (Long Frame Mode), FFM (Fast Frame MOdee)
- FDF bit 0 signify a CAN FD frame, 1 signify a classic CAN frame

### Transceiver delay compensation

- delay > TSEG1
- to check for bit errors during data phase, delayed transmit data is compared against the received data at the Secondary Sample Point (SSP)

### Application watchdog (FDCAN1 only)

- served by reading register FDCAN_TTOST. TT Application watchdog should not be disabled in TT Application

- Timestamp generation:
    - prescaler FDCAN_TSCC.TCP
    - counter FDCAN_TSCV.TCV
    - TimeStamp store in Rx buffer/Rx FIFO (RXTS[15:0]) or Tx event FIFO (TXTS[15:0])

- Timeout counter:
    - prescaler FDCAN_TSCC.TCP
    - Config FDCAN_TOCC
    - Value FDCAN_TOCV.TOC
    - operation mode: FDCAN_TOCC.TOS
        - Continuous mode: counter start when INIT is 0. Write to FDCAN_TOCV preset counter to value FDCAN_TOCC.TOP
        - if controlled by 1 of FIFO, empty FIFO preset counter to FDCAN_TOCC.TOP. Down counting started when 1st FIFO element is stored. Write to FDCAN_TOCV has no effect
        - counter reach 0 -> interrupt flag FDCAN_IR.TOO is set

### Message RAM

- width 32 bits
- can allocate up to 2560 words 
- address words not single bytes
- [15:2] is considered

### Rx Handling

control acceptance filtering, transfer of received msg to Tx buffer or to 1 of 2 Rx FIFO, Rx FIFO put and get indices

- Acceptnce filter:
    - 2 sets
    - list of filters is executed from #0 until first matching elemnet
    - Global filter config FDCAN_GFC
    - Standard IF filter config FDCAN_SIDFC
    - Extended ID filter config FDCAN_XIDFC
    - Extended ID AND mask FDCAN_XIDAM

- RxFIFO:
    - blocking mode
        - when Rx FIFO full, no further message is written to the Rx FIFO until 1 msg read out 
        - FDCAN_IR.RFNL (msg discarded) 
    - overwrite mode
        - full -> overwrite

### Trigger memory
- is part of external message RAM to whcih TTCAN is connceted to
- up to 64 trigger elements
- a trigger memory element consist of:
    -  time mark (TM)
    - cycle code (CC)
    - trigger type (TYPE)
    - filter type (FTYPE)
    - message number (MNR)
    - message status count (MSC)
    - time mark event internal (TMIN)
    - time mark event external (TMEX)

- time mark define at which cycle tmie a trigger become active, sorted by time marks
- msg number & cycle code ignored for type Tx_Ref_Trigger, Tx_Ref_Trigger_Gap, Watch_Trigger, Watch_Trigger_Gap and End_of_List
- in case of Transmit trigger, Tx handler start to read msg from message RAM 





