#include "../../development/include/cosem.h"
#include "../../development/include/gxobjects.h"
#include "../../development/include/dlmssettings.h"
// Define Logical Device Name.
static char LDN[17] = "Gurux";
// Define Flag ID.
static char FLAG_ID[3] = "GRX";
///////////////////////////////////////////////////////////////////////
/*Define all COSEM objects here so they are not use stack.*/
// 0.0.40.0.2.255 LLS Secret Key
extern gxAssociationLogicalName llsSecretKey;
// 0.0.15.0.0.255 Single-action Schedule for Billing Dates
extern gxActionSchedule singleActionScheduleForBillingDates;
// 0.0.13.0.0.255 Activity Calendar Active Time
extern gxActivityCalendar activityCalendarActiveTime;
// 0.0.1.0.0.255 Clock
extern gxClock clock1;
// 0.0.0.1.0.255 Cumulative Billing count
extern gxData cumulativeBillingCount;
// 0.0.42.0.0.255 Ch. 0 COSEM Logical device name
extern gxData cosemLogicalDeviceName;
// 0.0.94.91.0.255 Cumulative Tamper Count
extern gxData cumulativeTamperCount;
// 0.0.94.91.9.255 Meter Type
extern gxData meterType;
// 0.0.96.1.0.255 Meter Serial Number
extern gxData meterSerialNumber;
// 0.0.96.1.1.255 Manufacturer Name
extern gxData manufacturerName;
// 0.0.96.1.4.255 Meter Year Of Manufacture
extern gxData meterYearOfManufacture;
// 0.0.96.2.0.255 Cumulative Programming Count
extern gxData cumulativeProgrammingCount;
// 0.0.96.7.0.255 Ch. 0 No. of power failures in all three phases
extern gxData noOfPowerFailuresInAllThreePhases;
// 0.0.96.11.0.255 Ch. 0 Event code  #1
extern gxData eventCode1;
// 0.0.96.11.1.255 Event:Current Related
extern gxData eventCurrentRelated;
// 0.0.96.11.2.255 Event:Power Related
extern gxData eventPowerRelated;
// 0.0.96.11.3.255 Event:Transaction Related
extern gxData eventTransactionRelated;
// 0.0.96.11.4.255 Event:Others
extern gxData eventOthers;
// 0.0.96.11.5.255 Event:Non Roll Over Events
extern gxData eventNonRollOverEvents;
// 0.0.96.11.6.255 Event:Load Switch Status
extern gxData eventLoadSwitchStatus;
// 1.0.0.2.0.255 Firmware Version For Meter
extern gxData firmwareVersionForMeter;
// 1.0.0.4.2.255 Ch. 0 Transformer ratio - current (numerator)
extern gxData transformerRatioMinusCurrent;
// 1.0.0.4.3.255 Ch. 0 Transformer ratio - voltage (numerator)
extern gxData transformerRatioMinusVoltage;
// 1.0.0.8.0.255 Demand Integration Period
extern gxData demandIntegrationPeriod;
// 1.0.0.8.4.255 Profile Capture Period
extern gxData profileCapturePeriod;
// 1.0.0.8.5.255 Ch. 0 Recording interval 2, for load profile
extern gxData recordingInterval2ForLoadProfile;
// 1.0.0.1.2.255 Ch. 0 Time stamp of the most recent billing period closed (1)
extern gxData timeStampOfTheMostRecentBillingPeriodClosed;
// 1.0.1.6.0.255 Maximum Demand-kW
extern gxExtendedRegister maximumDemandKw;
// 1.0.2.6.0.255 Ch. 0 Sum Li Active power- (QII+QIII) Max. 1 Rate 0 (0 is total)
extern gxExtendedRegister sumLiActivePowerMinusMax1RateIsTotal;
// 1.0.9.6.0.255 Maximum Demand-kVA
extern gxExtendedRegister maximumDemandKva;
// 1.0.10.6.0.255 Ch. 0 Sum Li Apparent power- (QII+QIII) Max. 1 Rate 0 (0 is total)
extern gxExtendedRegister sumLiApparentPowerMinusMax1RateIsTotal;
// 0.0.22.0.0.255 Ch. 0 IEC HDLC setup
extern gxIecHdlcSetup iecHdlcSetup;
// 0.0.99.98.0.255 Voltage Related Events Profile
extern gxProfileGeneric voltageRelatedEventsProfile;
// 0.0.99.98.1.255 Current Related Events Profile
extern gxProfileGeneric currentRelatedEventsProfile;
// 0.0.99.98.2.255 Power Related Events Profile
extern gxProfileGeneric powerRelatedEventsProfile;
// 0.0.99.98.3.255 Transaction Events Profile
extern gxProfileGeneric transactionEventsProfile;
// 0.0.99.98.4.255 Other Tamper Events Profile
extern gxProfileGeneric otherTamperEventsProfile;
// 0.0.99.98.5.255 Non Roll Over Events Profile
extern gxProfileGeneric nonRollOverEventsProfile;
// 0.0.99.98.6.255 Control Events Profile
extern gxProfileGeneric controlEventsProfile;
// 1.0.94.91.0.255 Instantaneous Profile
extern gxProfileGeneric instantaneousProfile;
// 1.0.94.91.3.255 Scaler: Instantaneous Profile
extern gxProfileGeneric scalerInstantaneousProfile;
// 1.0.94.91.4.255 Scaler: Block Load Profile
extern gxProfileGeneric scalerBlockLoadProfile;
// 1.0.94.91.5.255 Scaler: Daily Load Profile
extern gxProfileGeneric scalerDailyLoadProfile;
// 1.0.94.91.6.255 Scaler: Billing Profile
extern gxProfileGeneric scalerBillingProfile;
// 1.0.94.91.7.255 Scaler: Events Profile
extern gxProfileGeneric scalerEventsProfile;
// 1.0.98.1.0.255 Billing Profile
extern gxProfileGeneric billingProfile;
// 1.0.99.1.0.255 Load Profile
extern gxProfileGeneric loadProfile;
// 1.0.99.2.0.255 Daily Load Profile
extern gxProfileGeneric dailyLoadProfile;
// 0.0.0.1.2.255 Billing Date Import Mode
extern gxRegister billingDateImportMode;
// 0.0.94.91.8.255 Cumulative power â€” OFF duration in min
extern gxRegister cumulativePowerOffDurationInMin;
// 1.0.1.7.0.255 Active Power-kW
extern gxRegister activePowerKw;
// 1.0.1.8.0.255 Cumulative Energy-kWh
extern gxRegister cumulativeEnergyKwh;
// 1.0.2.8.0.255 Cumulative Energy-kWh - Export
extern gxRegister cumulativeEnergyKwhExport;
// 1.0.3.7.0.255 Ch. 0 Sum Li Reactive power+ (QI+QII) Inst. value
extern gxRegister sumLiReactivePowerPlusInstValue;
// 1.0.5.8.0.255 Ch. 0 Sum Li Reactive power QI Time integral 1 Rate 0 (0 is total)
extern gxRegister sumLiReactivePowerQiTimeIntegral1RateIsTotal;
// 1.0.6.8.0.255 Ch. 0 Sum Li Reactive power QII Time integral 1 Rate 0 (0 is total)
extern gxRegister sumLiReactivePowerQiiTimeIntegral1RateIsTotal;
// 1.0.7.8.0.255 Ch. 0 Sum Li Reactive power QIII Time integral 1 Rate 0 (0 is total)
extern gxRegister sumLiReactivePowerQiiiTimeIntegral1RateIsTotal;
// 1.0.8.8.0.255 Ch. 0 Sum Li Reactive power QIV Time integral 1 Rate 0 (0 is total)
extern gxRegister sumLiReactivePowerQivTimeIntegral1RateIsTotal;
// 1.0.9.7.0.255 Apparent Power-KVA
extern gxRegister apparentPowerKva;
// 1.0.9.8.0.255 Cumulative Energy-kVAh - Import
extern gxRegister cumulativeEnergyKvahImport;
// 1.0.10.8.0.255 Cumulative Energy-kVAh - Export
extern gxRegister cumulativeEnergyKvahExport;
// 1.0.13.7.0.255 Signed Power Factor
extern gxRegister signedPowerFactor;
// 1.0.14.7.0.255 Frequency-Hz
extern gxRegister frequencyHz;
// 1.0.31.7.0.255 Ch. 0 L1 Current Inst. value
extern gxRegister l1CurrentInstValue;
// 1.0.32.7.0.255 Ch. 0 L1 Voltage Inst. value
extern gxRegister l1VoltageInstValue;
// 1.0.33.7.0.255 Ch. 0 L1 Power factor Inst. value
extern gxRegister l1PowerFactorInstValue;
// 1.0.51.7.0.255 Ch. 0 L2 Current Inst. value
extern gxRegister l2CurrentInstValue;
// 1.0.52.7.0.255 Ch. 0 L2 Voltage Inst. value
extern gxRegister l2VoltageInstValue;
// 1.0.53.7.0.255 Ch. 0 L2 Power factor Inst. value
extern gxRegister l2PowerFactorInstValue;
// 1.0.71.7.0.255 Ch. 0 L3 Current Inst. value
extern gxRegister l3CurrentInstValue;
// 1.0.72.7.0.255 Ch. 0 L3 Voltage Inst. value
extern gxRegister l3VoltageInstValue;
// 1.0.73.7.0.255 Ch. 0 L3 Power factor Inst. value
extern gxRegister l3PowerFactorInstValue;
// 1.0.16.8.0.255 Ch. 0 Sum Li Active power    (abs(QI+QIV)-abs(QII+QIII)) Time integral 1 Rate 0 (0 is total)
extern gxRegister sumLiActivePowerTimeIntegral1RateIsTotal;
// 1.0.31.27.0.255 Ch. 0 L1 Current Current avg. 5
extern gxRegister l1CurrentCurrentAvg5;
// 1.0.51.27.0.255 Ch. 0 L2 Current Current avg. 5
extern gxRegister l2CurrentCurrentAvg5;
// 1.0.71.27.0.255 Ch. 0 L3 Current Current avg. 5
extern gxRegister l3CurrentCurrentAvg5;
// 1.0.32.27.0.255 Ch. 0 L1 Voltage Current avg. 5
extern gxRegister l1VoltageCurrentAvg5;
// 1.0.52.27.0.255 Ch. 0 L2 Voltage Current avg. 5
extern gxRegister l2VoltageCurrentAvg5;
// 1.0.72.27.0.255 Ch. 0 L3 Voltage Current avg. 5
extern gxRegister l3VoltageCurrentAvg5;
// 1.0.1.29.0.255 Block Energy-kWh - Import
extern gxRegister blockEnergyKwhImport;
// 1.0.2.29.0.255 Block Energy-kWh - Export
extern gxRegister blockEnergyKwhExport;
// 1.0.16.29.0.255 Ch. 0 Sum Li Active power    (abs(QI+QIV)-abs(QII+QIII)) Time integral 5
extern gxRegister sumLiActivePowerTimeIntegral5;
// 1.0.9.29.0.255 Block Energy-kVAh - Import
extern gxRegister blockEnergyKvahImport;
// 1.0.10.29.0.255 Block Energy-kVAh - Export
extern gxRegister blockEnergyKvahExport;
// // 1.0.5.29.0.255 Ch. 0 Sum Li Reactive power QI Time integral 5
extern gxRegister sumLiReactivePowerQiTimeIntegral5;
// // 1.0.6.29.0.255 Ch. 0 Sum Li Reactive power QII Time integral 5
extern gxRegister sumLiReactivePowerQiiTimeIntegral5;
// // 1.0.7.29.0.255 Ch. 0 Sum Li Reactive power QIII Time integral 5
extern gxRegister sumLiReactivePowerQiiiTimeIntegral5;
// // 1.0.8.29.0.255 Ch. 0 Sum Li Reactive power QIV Time integral 5
extern gxRegister sumLiReactivePowerQivTimeIntegral5;
// // 1.0.94.91.13.255 Ch. 0 Identifiers for India
extern gxRegister identifiersForIndia;
// // 0.0.10.0.1.255 MD Reset Action
extern gxScriptTable mdResetAction;
///////////////////////////////////////////////////////////////////////
/*All objects.*/
static gxObject *ALL_OBJECTS[] = {
    BASE(llsSecretKey),
    BASE(singleActionScheduleForBillingDates),
    BASE(activityCalendarActiveTime),
    BASE(clock1),
    BASE(cumulativeBillingCount),
    BASE(cosemLogicalDeviceName),
    BASE(cumulativeTamperCount),
    BASE(meterType),
    BASE(meterSerialNumber),
    BASE(manufacturerName),
    BASE(meterYearOfManufacture),
    BASE(cumulativeProgrammingCount),
    BASE(noOfPowerFailuresInAllThreePhases),
    BASE(eventCode1),
    BASE(eventCurrentRelated),
    BASE(eventPowerRelated),
    BASE(eventTransactionRelated),
    BASE(eventOthers),
    BASE(eventNonRollOverEvents),
    BASE(eventLoadSwitchStatus),
    BASE(firmwareVersionForMeter),
    BASE(transformerRatioMinusCurrent),
    BASE(transformerRatioMinusVoltage),
    BASE(demandIntegrationPeriod),
    BASE(profileCapturePeriod),
    BASE(recordingInterval2ForLoadProfile),
    BASE(timeStampOfTheMostRecentBillingPeriodClosed),
    BASE(maximumDemandKw),
    BASE(sumLiActivePowerMinusMax1RateIsTotal),
    BASE(maximumDemandKva),
    BASE(sumLiApparentPowerMinusMax1RateIsTotal),
    BASE(iecHdlcSetup),
    BASE(voltageRelatedEventsProfile),
    BASE(currentRelatedEventsProfile),
    BASE(powerRelatedEventsProfile),
    BASE(transactionEventsProfile),
    BASE(otherTamperEventsProfile),
    BASE(nonRollOverEventsProfile),
    BASE(controlEventsProfile),
    BASE(instantaneousProfile),
    BASE(scalerInstantaneousProfile),
    BASE(scalerBlockLoadProfile),
    BASE(scalerDailyLoadProfile),
    BASE(scalerBillingProfile),
    BASE(scalerEventsProfile),
    BASE(billingProfile),
    BASE(loadProfile),
    BASE(dailyLoadProfile),
    BASE(billingDateImportMode),
    BASE(cumulativePowerOffDurationInMin),
    BASE(activePowerKw),
    BASE(cumulativeEnergyKwh),
    BASE(cumulativeEnergyKwhExport),
    BASE(sumLiReactivePowerPlusInstValue),
    BASE(sumLiReactivePowerQiTimeIntegral1RateIsTotal),
    BASE(sumLiReactivePowerQiiTimeIntegral1RateIsTotal),
    BASE(sumLiReactivePowerQiiiTimeIntegral1RateIsTotal),
    BASE(sumLiReactivePowerQivTimeIntegral1RateIsTotal),
    BASE(apparentPowerKva),
    BASE(cumulativeEnergyKvahImport),
    BASE(cumulativeEnergyKvahExport),
    BASE(signedPowerFactor),
    BASE(frequencyHz),
    BASE(l1CurrentInstValue),
    BASE(l1VoltageInstValue),
    BASE(l1PowerFactorInstValue),
    BASE(l2CurrentInstValue),
    BASE(l2VoltageInstValue),
    BASE(l2PowerFactorInstValue),
    BASE(l3CurrentInstValue),
    BASE(l3VoltageInstValue),
    BASE(l3PowerFactorInstValue),
    BASE(sumLiActivePowerTimeIntegral1RateIsTotal),
    BASE(l1CurrentCurrentAvg5),
    BASE(l2CurrentCurrentAvg5),
    BASE(l3CurrentCurrentAvg5),
    BASE(l1VoltageCurrentAvg5),
    BASE(l2VoltageCurrentAvg5),
    BASE(l3VoltageCurrentAvg5),
    BASE(blockEnergyKwhImport),
    BASE(blockEnergyKwhExport),
    BASE(sumLiActivePowerTimeIntegral5),
    BASE(blockEnergyKvahImport),
    BASE(blockEnergyKvahExport),
    BASE(sumLiReactivePowerQiTimeIntegral5),
    BASE(sumLiReactivePowerQiiTimeIntegral5),
    BASE(sumLiReactivePowerQiiiTimeIntegral5),
    BASE(sumLiReactivePowerQivTimeIntegral5),
    BASE(identifiersForIndia),
    BASE(mdResetAction),
};
///////////////////////////////////////////////////////////////////////
int addsingleActionScheduleForBillingDates();
int addactivityCalendarActiveTime();
int addllsSecretKey();
int addclock1();
int addcumulativeBillingCount();
int addcosemLogicalDeviceName();
int addcumulativeTamperCount();
int addmeterType();
int addmeterSerialNumber();
int addmanufacturerName();
int addmeterYearOfManufacture();
int addcumulativeProgrammingCount();
int addnoOfPowerFailuresInAllThreePhases();
int addeventCode1();
int addeventCurrentRelated();
int addeventPowerRelated();
int addeventTransactionRelated();
int addeventOthers();
int addeventNonRollOverEvents();
int addeventLoadSwitchStatus();
int addfirmwareVersionForMeter();
int addtransformerRatioMinusCurrent();
int addtransformerRatioMinusVoltage();
int adddemandIntegrationPeriod();
int addprofileCapturePeriod();
int addrecordingInterval2ForLoadProfile();
int addtimeStampOfTheMostRecentBillingPeriodClosed();
int addmaximumDemandKw();
int addsumLiActivePowerMinusMax1RateIsTotal();
int addmaximumDemandKva();
int addsumLiApparentPowerMinusMax1RateIsTotal();
int addiecHdlcSetup();
int addvoltageRelatedEventsProfile();
int addcurrentRelatedEventsProfile();
int addpowerRelatedEventsProfile();
int addtransactionEventsProfile();
int addotherTamperEventsProfile();
int addnonRollOverEventsProfile();
int addcontrolEventsProfile();
int addinstantaneousProfile();
int addscalerInstantaneousProfile();
int addscalerBlockLoadProfile();
int addscalerDailyLoadProfile();
int addscalerBillingProfile();
int addscalerEventsProfile();
int addbillingProfile();
int addloadProfile();
int adddailyLoadProfile();
int addbillingDateImportMode();
int addcumulativePowerOffDurationInMin();
int addactivePowerKw();
int addcumulativeEnergyKwh();
int addcumulativeEnergyKwhExport();
int addsumLiReactivePowerPlusInstValue();
int addsumLiReactivePowerQiTimeIntegral1RateIsTotal();
int addsumLiReactivePowerQiiTimeIntegral1RateIsTotal();
int addsumLiReactivePowerQiiiTimeIntegral1RateIsTotal();
int addsumLiReactivePowerQivTimeIntegral1RateIsTotal();
int addapparentPowerKva();
int addcumulativeEnergyKvahImport();
int addcumulativeEnergyKvahExport();
int addsignedPowerFactor();
int addfrequencyHz();
int addl1CurrentInstValue();
int addl1VoltageInstValue();
int addl1PowerFactorInstValue();
int addl2CurrentInstValue();
int addl2VoltageInstValue();
int addl2PowerFactorInstValue();
int addl3CurrentInstValue();
int addl3VoltageInstValue();
int addl3PowerFactorInstValue();
int addsumLiActivePowerTimeIntegral1RateIsTotal();
int addl1CurrentCurrentAvg5();
int addl2CurrentCurrentAvg5();
int addl3CurrentCurrentAvg5();
int addl1VoltageCurrentAvg5();
int addl2VoltageCurrentAvg5();
int addl3VoltageCurrentAvg5();
int addblockEnergyKwhImport();
int addblockEnergyKwhExport();
int addsumLiActivePowerTimeIntegral5();
int addblockEnergyKvahImport();
int addblockEnergyKvahExport();
int addsumLiReactivePowerQiTimeIntegral5();
int addsumLiReactivePowerQiiTimeIntegral5();
int addsumLiReactivePowerQiiiTimeIntegral5();
int addsumLiReactivePowerQivTimeIntegral5();
int addidentifiersForIndia();
int addmdResetAction();

int obj_InitObjects(dlmsServerSettings *settings);
