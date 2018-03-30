#pragma once


#define  DATABASE_STATUS_LABEL_ID      "lblDatabaseStatus"
#define  READER_STATUS_LABEL_ID        "lblReaderStatus"
#define  TEMPERATURE_DATA_CONTROL_ID   "TemperatureData"
#define  PATIENTS_CONTROL_ID           "Patients"
#define  NURSES_CONTROL_ID             "Nurses"
#define  SYNCHRONIZE_CONTROL_ID        "Synchronise"

#define  TEMPERATURE_DATA_FILE         "TemperatureData.xml"
#define  PATIENTS_FILE                 "Patients.xml"
#define  NURSES_FILE                   "Nurses.xml"
#define  SYNCHRONIZE_FILE              "Synchronise.xml"

#define  SET_CONTROL_TEXT(ctrl,text)      do { if ( ctrl ) { ctrl->SetText(text); } } while( 0 )
#define  GET_CONTROL_TEXT(ctrl)           ( (ctrl == 0) ? "" : ctrl->GetText() )
#define  SET_CONTROL_ENABLED( ctrl, e )   do { if ( ctrl ) { ctrl->SetEnabled(e); } } while( 0 )