#pragma once


#define  DATABASE_STATUS_LABEL_ID      "lblDatabaseStatus"
#define  READER_STATUS_LABEL_ID        "lblReaderStatus"
#define  TABS_ID                       "switch"
#define  SUB_TABS_ID                   "switch_data"
#define  TEMPERATURE_DATA_CONTROL_ID   "TemperatureData"
#define  PATIENTS_CONTROL_ID           "Patients"
#define  NURSES_CONTROL_ID             "Nurses"
#define  SYNCHRONIZE_CONTROL_ID        "Synchronise"
#define  TEMPERATURE_DATA_OPTION_ID    "optTemperatureData"
#define  PATIENTS_OPTION_ID            "optPatients"
#define  NURSES_OPTION_ID              "optNurses"
#define  SYNCHRONIZE_OPTION_ID         "optSynchronise"
#define  PURE_DATA_CONTROL_ID          "PureData"
#define  CURVE_CONTROL_ID              "TemperatureCurve"
#define  PURE_DATA_OPTION_ID           "optPureData"
#define  CURVE_OPTION_ID               "optCurveData"

#define  TEMPERATURE_DATA_FILE         "TemperatureData.xml"
#define  PATIENTS_FILE                 "Patients.xml"
#define  NURSES_FILE                   "Nurses.xml"
#define  SYNCHRONIZE_FILE              "Synchronise.xml"
#define  PURE_DATA_FILE                "PureData.xml"
#define  CURVE_FILE                    "TemperatureCurve.xml"

#define  TEMPERATURE_DATA_INDEX        0
#define  PATIENTS_INDEX                1
#define  NURSES_INDEX                  2
#define  SYNCHRONIZE_INDEX             3
#define  PURE_DATA_INDEX               0
#define  CURVE_INDEX                   1

#define  SET_CONTROL_TEXT(ctrl,text)      do { if ( ctrl ) { ctrl->SetText(text); } } while( 0 )
#define  GET_CONTROL_TEXT(ctrl)           ( (ctrl == 0) ? "" : ctrl->GetText() )
#define  SET_CONTROL_ENABLED( ctrl, e )   do { if ( ctrl ) { ctrl->SetEnabled(e); } } while( 0 )