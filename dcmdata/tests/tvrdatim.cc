#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcvrda.h"
#include "dcvrtm.h"
#include "dcvrdt.h"
#include "dcdeftag.h"
#include "ofconsol.h"


int main()
{
    double timeZone;
    OFDate date;
    OFTime time;
    OFDateTime dateTime;
    OFString string;
    DcmDate dcmDate(DCM_StudyDate);
    DcmTime dcmTime(DCM_StudyTime);
    DcmDateTime dcmDateTime(DCM_DateTime);
    
    dcmDate.setCurrentDate();
    dcmDate.print(COUT);
    if (dcmDate.getOFDate(date).good())
        COUT << "current date: " << date << endl;
    else
        COUT << "current date: <invalid>" << endl;
    
    dcmTime.setCurrentTime();
    dcmTime.print(COUT);
    if (dcmTime.getOFTime(time).good())
        COUT << "current time: " << time << endl;
    else
        COUT << "current time: <invalid>" << endl;

    dcmTime.putString("12");
    dcmTime.print(COUT);
    if (dcmTime.getOFTime(time).good())
        COUT << "valid time: " << time << endl;
    dcmTime.putString("1203");
    dcmTime.print(COUT);
    if (dcmTime.getOFTime(time).good())
        COUT << "valid time: " << time << endl;
    dcmTime.putString("120315");
    dcmTime.print(COUT);
    if (dcmTime.getOFTime(time).good())
        COUT << "valid time: " << time << endl;
    dcmTime.putString("120301.99");
    dcmTime.print(COUT);
    if (dcmTime.getOFTime(time).good())
    {
        time.getISOFormattedTime(string, OFTrue /*seconds*/, OFTrue /*fraction*/, OFTrue /*timeZone*/);        
        COUT << "valid local time: " << string << endl;
    }

    dcmTime.putString("12:03:15");
    dcmTime.print(COUT);
    if (dcmTime.getOFTime(time).good())
        COUT << "valid time: " << time << endl;

    if (DcmTime::getTimeZoneFromString("+1130", timeZone).good())
        COUT << "time zone: " << timeZone << endl;
    if (DcmTime::getTimeZoneFromString("-0100", timeZone).good())
        COUT << "time zone: " << timeZone << endl;
    
    dcmDateTime.putString("200204101203+0500");
    dcmDateTime.print(COUT);
    if (dcmDateTime.getOFDateTime(dateTime).good())
        COUT << "valid date/time: " << dateTime << endl;
    dcmDateTime.setCurrentDateTime(OFTrue /*seconds*/, OFTrue /*fraction*/, OFTrue /*timeZone*/);
    dcmDateTime.print(COUT);
    if (dcmDateTime.getOFDateTime(dateTime).good())
        COUT << "current date/time: " << dateTime << endl;
    if (dateTime.getISOFormattedDateTime(string, OFTrue /*seconds*/, OFTrue /*fraction*/, OFTrue /*timeZone*/, OFFalse /*delimiter*/))
        COUT << "current date/time: " << string << endl;
    
    dcmDateTime.putString("20020410");
    dcmDateTime.print(COUT);
    if (dcmDateTime.getOFDateTime(dateTime).good())
    {
        dateTime.getISOFormattedDateTime(string, OFTrue /*seconds*/, OFFalse /*fraction*/, OFTrue /*timeZone*/);        
        COUT << "valid local date/time: " << string << endl;
    }
    
    return 0;
}
