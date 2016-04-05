
// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2016 Robotcub Consortium
* Author: Marco Accame
* CopyPolicy: Released under the terms of the GNU GPL v2.0.
*
*/

// general purpose stuff.
#include <string>
#include <iostream>
#include <string.h>

// Yarp Includes
#include <yarp/os/Time.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <stdarg.h>
#include <stdio.h>
#include <yarp/dev/PolyDriver.h>
#include <ace/config.h>
#include <ace/Log_Msg.h>


// specific to this device driver.
#include <serviceParser.h>

#include <yarp/os/LogStream.h>
#include "EoAnalogSensors.h"


#include "EoProtocol.h"
#include "EoProtocolMN.h"
#include "EoProtocolAS.h"

#include <yarp/os/NetType.h>

#ifdef WIN32
#pragma warning(once:4355)
#endif



using namespace yarp;
using namespace yarp::os;
using namespace yarp::dev;

ServiceParser::ServiceParser()
{
    // how do i reset variable as_service?

    as_service.type = eomn_serv_NONE;

    as_service.properties.canboards.resize(0);
    as_service.properties.sensors.resize(0);

    as_service.settings.acquisitionrate = 0;
    as_service.settings.enabledsensors.resize(0);
}

// marco.accame: localize the strings used inside the xml files in a single place. and then use a ....

bool ServiceParser::convert(ConstString const &fromstring, eOmn_serv_type_t& toservicetype, bool& formaterror)
{
    const char *t = fromstring.c_str();

    if(0 == strcmp(t, "enumServiceSTRAIN"))
    {
        toservicetype = eomn_serv_AS_strain;
    }
    else if(0 == strcmp(t, "enumServiceMAIS"))
    {
        toservicetype = eomn_serv_AS_mais;
    }
    else if(0 == strcmp(t, "enumServiceINERTIALS"))
    {
        toservicetype = eomn_serv_AS_inertial;
    }
    else if(0 == strcmp(t, "enumServiceNONE"))
    {
        toservicetype = eomn_serv_NONE;
    }
    else
    {
        yWarning() << "ServiceParser::convert():" << t << "is not a legal string for eOmn_serv_type_t";
        toservicetype = eomn_serv_NONE;
        formaterror = true;
        return false;
    }

    return true;
}


bool ServiceParser::convert(ConstString const &fromstring, eObrd_cantype_t& tobrdcantype, bool& formaterror)
{
    const char *t = fromstring.c_str();

    if(0 == strcmp(t, "enumCanBrdMC4"))
    {
        tobrdcantype = eobrd_cantype_mc4;
    }
    else if(0 == strcmp(t, "enumCanBrdSTRAIN"))
    {
        tobrdcantype = eobrd_cantype_strain;
    }
    else if(0 == strcmp(t, "enumCanBrdMAIS"))
    {
        tobrdcantype = eobrd_cantype_mais;
    }
    else if(0 == strcmp(t, "enumCanBrdMTB"))
    {
        tobrdcantype = eobrd_cantype_mtb;
    }
    else if(0 == strcmp(t, "enumCanBrdFOC"))
    {
        tobrdcantype = eobrd_cantype_foc;
    }
    else if(0 == strcmp(t, "enumCanBrdNONE"))
    {
        tobrdcantype = eobrd_cantype_none;
    }
    else
    {
        yWarning() << "ServiceParser::convert():" << t << "is not a legal string for eObrd_cantype_t";
        tobrdcantype = eobrd_cantype_unknown;
        formaterror = true;
        return false;
    }

    return true;
}

bool ServiceParser::convert(ConstString const &fromstring, bool& tobool, bool& formaterror)
{
    const char *t = fromstring.c_str();

    if(0 == strcmp(t, "true"))
    {
        tobool = true;
    }
    else if(0 == strcmp(t, "false"))
    {
        tobool = false;
    }
    else if(0 == strcmp(t, "TRUE"))
    {
        tobool = true;
    }
    else if(0 == strcmp(t, "FALSE"))
    {
        tobool = false;
    }
    else if(0 == strcmp(t, "1"))
    {
        tobool = true;
    }
    else if(0 == strcmp(t, "0"))
    {
        tobool = false;
    }
    else
    {
        yWarning() << "ServiceParser::convert():" << t << "is not a legal string for bool";
        tobool = false;
        formaterror = true;
        return false;
    }

    return true;
}


bool ServiceParser::convert(const int number, uint8_t& tou8, bool& formaterror)
{
    if((number >= 0) && (number < 256))
    {
        tou8 = number;
    }
    else
    {
        yWarning() << "ServiceParser::convert():" << number << "is not a legal value for uint8_t";
        tou8 = 0;
        formaterror = true;
        return false;
    }

    return true;
}

bool ServiceParser::convert(const int number, uint16_t& tou16, bool& formaterror)
{
    if((number >= 0) && (number < (64*1024)))
    {
        tou16 = number;
    }
    else
    {
        yWarning() << "ServiceParser::convert():" << number << "is not a legal value for uint16_t";
        tou16 = 0;
        formaterror = true;
        return false;
    }

    return true;
}


bool convert(ConstString const &fromstring, eOas_sensor_t& tosensortype, bool& formaterror)
{
    const char *t = fromstring.c_str();

    if(0 == strcmp(t, "enumSnsrSTRAIN"))
    {
        tosensortype = eoas_strain;
    }
    else if(0 == strcmp(t, "enumSnsrMAIS"))
    {
        tosensortype = eoas_mais;
    }
    else if(0 == strcmp(t, "enumSnsrAccMTBint"))
    {
        tosensortype = eoas_accel_mtb_int;
    }
    else if(0 == strcmp(t, "enumSnsrAccMTBext"))
    {
        tosensortype = eoas_accel_mtb_ext;
    }
    else if(0 == strcmp(t, "enumSnsrGyrMTBext"))
    {
        tosensortype = eoas_gyros_mtb_ext;
    }
    else if(0 == strcmp(t, "enumSnsrAccSTlis3x"))
    {
        tosensortype = eoas_accel_st_lis3x;
    }
    else if(0 == strcmp(t, "enumSnsrGyrSTl3g4200d"))
    {
        tosensortype = eoas_gyros_st_l3g4200d;
    }
    else if(0 == strcmp(t, "enumSnsrNONE"))
    {
        tosensortype = eoas_none;
    }
    else
    {
        yWarning() << "ServiceParser::convert():" << t << "is not a legal string for eOas_sensor_t";
        tosensortype = eoas_unknown;
        formaterror = true;
        return false;
    }

    return true;
}


bool ServiceParser::convert(ConstString const &fromstring, const uint8_t strsize, char *str, bool &formaterror)
{
    const char *t = fromstring.c_str();

    if(strsize <= strlen(t))
    {
        snprintf(str, strsize, "%s", t);
    }
    else
    {
        yWarning() << "ServiceParser::convert():" << t << "is not a legal string for a char[] of max size" << strsize;
        formaterror = true;
        return false;
    }

    return true;
}


bool ServiceParser::convert(ConstString const &fromstring, eObrd_location_t &location, bool &formaterror)
{
    // it is actually a micro-parser: PRE-num
    // at

    const char *t = fromstring.c_str();
    int len = strlen(t);

    if(len > 15)
    {
        yWarning() << "ServiceParser::convert():" << t << "is not a legal string for a eObrd_location_t because it is too long with size =" << len;
        formaterror = true;
        return false;
    }

    char prefix[16] = {0};
    sscanf(t, "%3c", prefix);
    if(0 == strcmp(prefix, "LOC"))
    {
        int adr = 0;
        sscanf(t, "%3c-%d", prefix, &adr);
        location.place = eobrd_place_loc;
        location.address.local = adr;
    }
    else if(0 == strcmp(prefix, "CAN"))
    {
        int bus = 0;
        int adr = 0;
        sscanf(t, "%3c%d-%d", prefix, &bus, &adr);
        // verify bus being eitehr 1 or 2, and adr being 1 ----- 14
        if((1 != bus) && (2 != bus))
        {
            yWarning() << "ServiceParser::convert():" << t << "is not a legal string for a eObrd_location_t because we can have either CAN1 or CAN2";
            formaterror = true;
            return false;
        }
        if((0 == adr) || (adr > 14))
        {
            yWarning() << "ServiceParser::convert():" << t << "is not a legal string for a eObrd_location_t because CAN address is in range [1, 14]";
            formaterror = true;
            return false;
        }
        location.place = eobrd_place_can;
        location.address.oncan.port = (1 == bus) ? (eOcanport1) : (eOcanport2);
        location.address.oncan.addr = adr;
        location.address.oncan.insideindex = eomn_serv_caninsideindex_none;
    }
    else if(0 == strcmp(prefix, "MC4"))
    {
        char mc4can[8] = {0};
        int bus = 0;
        int adr = 0;
        int sub = 0;
        sscanf(t, "%6c%d-%d-%d", mc4can, &bus, &adr, &sub);
        if(0 != strcmp(mc4can, "MC4CAN"))
        {
            yWarning() << "ServiceParser::convert():" << t << "is not a legal string for a eObrd_location_t because if it begins with MC4 then CAN should follow";
            formaterror = true;
            return false;
        }
        // verify bus being eitehr 1 or 2, and adr being 1 ----- 14, and sub 0, 1
        if((1 != bus) && (2 != bus))
        {
            yWarning() << "ServiceParser::convert():" << t << "is not a legal string for a eObrd_location_t because we can have either CAN1 or CAN2";
            formaterror = true;
            return false;
        }
        if((0 == adr) || (adr > 14))
        {
            yWarning() << "ServiceParser::convert():" << t << "is not a legal string for a eObrd_location_t because CAN address is in range [1, 14]";
            formaterror = true;
            return false;
        }
        if((0 != sub) && (1 != sub))
        {
            yWarning() << "ServiceParser::convert():" << t << "is not a legal string for a eObrd_location_t because in MC4CNx-adr-SUB, SUB address is in range [0, 1]";
            formaterror = true;
            return false;
        }
        location.place = eobrd_place_can;
        location.address.oncan.port = (1 == bus) ? (eOcanport1) : (eOcanport2);
        location.address.oncan.addr = adr;
        location.address.oncan.insideindex = (0 == sub) ? (eomn_serv_caninsideindex_first) : (eomn_serv_caninsideindex_second);
    }
    else
    {
        yWarning() << "ServiceParser::convert():" << t << "is not a legal string for a eObrd_location_t because it does not begin with LOC or CAN";
        formaterror = true;
        return false;
    }

    return true;
}


bool ServiceParser::check_analog(Searchable &config, eOmn_serv_type_t type)
{
    bool formaterror = false;
    // so far we check for eomn_serv_AS_mais / strain / inertial only
    if((eomn_serv_AS_mais != type) && (eomn_serv_AS_strain != type) && (eomn_serv_AS_inertial != type))
    {
        yError() << "ServiceParser::check() is called with wrong type";
        return false;
    }

    // format is SERVICE{ type, PROPERTIES{ CANBOARDS, SENSORS }, SETTINGS }

    // check whether we#include "EOnv_hid.h" have the proper type

    if(false == config.check("type"))
    {
        yError() << "ServiceParser::check() cannot find SERVICE.type";
        return false;
    }
    else
    {
        Bottle b_type(config.find("type").asString());
        if(false == convert(b_type.get(1).asString(), as_service.type, formaterror))
        {
            yError() << "ServiceParser::check() has found unknown SERVICE.type = " << b_type.toString();
            return false;
        }
        if(type != as_service.type)
        {
            yError() << "ServiceParser::check() has found wrong SERVICE.type = " << as_service.type << "it must be" << "TODO: tostring() function";
            return false;
        }
    }

    // check whether we have the proper groups

    Bottle b_PROPERTIES = Bottle(config.findGroup("PROPERTIES"));
    if(b_PROPERTIES.isNull())
    {
        yError() << "ServiceParser::check() cannot find PROPERTIES";
        return false;
    }
    else
    {
        Bottle b_PROPERTIES_CANBOARDS = Bottle(b_PROPERTIES.findGroup("CANBOARDS"));
        if(b_PROPERTIES_CANBOARDS.isNull())
        {
            yError() << "ServiceParser::check() cannot find PROPERTIES.CANBOARDS";
            return false;
        }
        else
        {
            // now get type, useGlobalParams, PROTOCOL.major/minor, FIRMWARE.major/minor/build and see their sizes. the must be all equal.
            // for mais and strain and so far for intertials it must be numboards = 1.

            Bottle b_PROPERTIES_CANBOARDS_type = b_PROPERTIES_CANBOARDS.findGroup("type");
            if(b_PROPERTIES_CANBOARDS_type.isNull())
            {
                yError() << "ServiceParser::check() cannot find PROPERTIES.CANBOARDS.type";
                return false;
            }
            Bottle b_PROPERTIES_CANBOARDS_useGlobalParams = b_PROPERTIES_CANBOARDS.findGroup("useGlobalParams");
            if(b_PROPERTIES_CANBOARDS_useGlobalParams.isNull())
            {
                yError() << "ServiceParser::check() cannot find PROPERTIES.CANBOARDS.useGlobalParams";
                return false;
            }
            Bottle b_PROPERTIES_CANBOARDS_PROTOCOL = Bottle(b_PROPERTIES_CANBOARDS.findGroup("PROTOCOL"));
            if(b_PROPERTIES_CANBOARDS_PROTOCOL.isNull())
            {
                yError() << "ServiceParser::check() cannot find PROPERTIES.CANBOARDS.PROTOCOL";
                return false;
            }
            Bottle b_PROPERTIES_CANBOARDS_PROTOCOL_major = Bottle(b_PROPERTIES_CANBOARDS_PROTOCOL.findGroup("major"));
            if(b_PROPERTIES_CANBOARDS_PROTOCOL_major.isNull())
            {
                yError() << "ServiceParser::check() cannot find PROPERTIES.CANBOARDS.PROTOCOL.major";
                return false;
            }
            Bottle b_PROPERTIES_CANBOARDS_PROTOCOL_minor = Bottle(b_PROPERTIES_CANBOARDS_PROTOCOL.findGroup("minor"));
            if(b_PROPERTIES_CANBOARDS_PROTOCOL_minor.isNull())
            {
                yError() << "ServiceParser::check() cannot find PROPERTIES.CANBOARDS.PROTOCOL.minor";
                return false;
            }
            Bottle b_PROPERTIES_CANBOARDS_FIRMWARE = Bottle(b_PROPERTIES_CANBOARDS.findGroup("FIRMWARE"));
            if(b_PROPERTIES_CANBOARDS_FIRMWARE.isNull())
            {
                yError() << "ServiceParser::check() cannot find PROPERTIES.CANBOARDS.FIRMWARE";
                return false;
            }
            Bottle b_PROPERTIES_CANBOARDS_FIRMWARE_major = Bottle(b_PROPERTIES_CANBOARDS_FIRMWARE.findGroup("major"));
            if(b_PROPERTIES_CANBOARDS_FIRMWARE_major.isNull())
            {
                yError() << "ServiceParser::check() cannot find PROPERTIES.CANBOARDS.FIRMWARE.major";
                return false;
            }
            Bottle b_PROPERTIES_CANBOARDS_FIRMWARE_minor = Bottle(b_PROPERTIES_CANBOARDS_FIRMWARE.findGroup("minor"));
            if(b_PROPERTIES_CANBOARDS_FIRMWARE_minor.isNull())
            {
                yError() << "ServiceParser::check() cannot find PROPERTIES.CANBOARDS.FIRMWARE.minor";
                return false;
            }
            Bottle b_PROPERTIES_CANBOARDS_FIRMWARE_build = Bottle(b_PROPERTIES_CANBOARDS_FIRMWARE.findGroup("build"));
            if(b_PROPERTIES_CANBOARDS_FIRMWARE_build.isNull())
            {
                yError() << "ServiceParser::check() cannot find PROPERTIES.CANBOARDS.FIRMWARE.build";
                return false;
            }

            int tmp = b_PROPERTIES_CANBOARDS_type.size();
            int numboards = tmp - 1;    // first position of bottle contains the tag "type"

            // check if all other fields have the same size.
            if( (tmp != b_PROPERTIES_CANBOARDS_useGlobalParams.size()) ||
                (tmp != b_PROPERTIES_CANBOARDS_PROTOCOL_major.size()) ||
                (tmp != b_PROPERTIES_CANBOARDS_PROTOCOL_minor.size()) ||
                (tmp != b_PROPERTIES_CANBOARDS_FIRMWARE_major.size()) ||
                (tmp != b_PROPERTIES_CANBOARDS_FIRMWARE_minor.size()) ||
                (tmp != b_PROPERTIES_CANBOARDS_FIRMWARE_build.size())
              )
            {
                yError() << "ServiceParser::check() in PROPERTIES.CANBOARDS some param has inconsistent length";
                return false;
            }


            as_service.properties.canboards.resize(numboards);

            formaterror = false;
            for(int i=0; i<as_service.properties.canboards.size(); i++)
            {
                servCanBoard_t item = as_service.properties.canboards.at(i);

                convert(b_PROPERTIES_CANBOARDS_type.get(i+1).asString(), item.type, formaterror);
                convert(b_PROPERTIES_CANBOARDS_useGlobalParams.get(i+1).asString(), item.useglobalparams, formaterror);

                convert(b_PROPERTIES_CANBOARDS_PROTOCOL_major.get(i+1).asInt(), item.protocol.major, formaterror);
                convert(b_PROPERTIES_CANBOARDS_PROTOCOL_minor.get(i+1).asInt(), item.protocol.minor, formaterror);

                convert(b_PROPERTIES_CANBOARDS_FIRMWARE_major.get(i+1).asInt(), item.firmware.major, formaterror);
                convert(b_PROPERTIES_CANBOARDS_FIRMWARE_minor.get(i+1).asInt(), item.firmware.minor, formaterror);
                convert(b_PROPERTIES_CANBOARDS_FIRMWARE_build.get(i+1).asInt(), item.firmware.build, formaterror);

            }

            // in here we could decide to return false if any previous conversion function has returned error
            // bool fromStringToBoolean(string str, bool &anyerror); // inside: if error then .... be sure to set error = true. dont set it to false.

            if(true == formaterror)
            {
                yError() << "ServiceParser::check() has detected an illegal format for some of the params of PROPERTIES.CANBOARDS some param has inconsistent length";
                return false;
            }
        }

        Bottle b_PROPERTIES_SENSORS = Bottle(b_PROPERTIES.findGroup("SENSORS"));
        if(b_PROPERTIES_SENSORS.isNull())
        {
            yError() << "ServiceParser::check() cannot find PROPERTIES.SENSORS";
            return false;
        }
        else
        {

            Bottle b_PROPERTIES_SENSORS_id = Bottle(b_PROPERTIES_SENSORS.findGroup("id"));
            if(b_PROPERTIES_SENSORS_id.isNull())
            {
                yError() << "ServiceParser::check() cannot find PROPERTIES.SENSORS.id";
                return false;
            }
            Bottle b_PROPERTIES_SENSORS_type = Bottle(b_PROPERTIES_SENSORS.findGroup("type"));
            if(b_PROPERTIES_SENSORS_type.isNull())
            {
                yError() << "ServiceParser::check() cannot find PROPERTIES.SENSORS.type";
                return false;
            }
            Bottle b_PROPERTIES_SENSORS_location = Bottle(b_PROPERTIES_SENSORS.findGroup("location"));
            if(b_PROPERTIES_SENSORS_location.isNull())
            {
                yError() << "ServiceParser::check() cannot find PROPERTIES.SENSORS.location";
                return false;
            }

            int tmp = b_PROPERTIES_SENSORS_id.size();
            int numsensors = tmp - 1;    // first position of bottle contains the tag "id"

            // check if all other fields have the same size.
            if( (tmp != b_PROPERTIES_SENSORS_type.size()) ||
                (tmp != b_PROPERTIES_SENSORS_location.size())
              )
            {
                yError() << "ServiceParser::check() in PROPERTIES.SENSORS some param has inconsistent length";
                return false;
            }


            as_service.properties.sensors.resize(numsensors);

            formaterror = false;
            for(int i=0; i<as_service.properties.sensors.size(); i++)
            {
                servAnalogSensor_t item = as_service.properties.sensors.at(i);

                //ConstString s_id = b_PROPERTIES_SENSORS_id.get(i+1).asString();
                //ConstString s_type = b_PROPERTIES_SENSORS_type.get(i+1).asString();
                //ConstString s_location = b_PROPERTIES_SENSORS_location.get(i+1).asString();

                //snprintf(item.id, sizeof(item.id), "%s", s_id.c_str()); // todo: function which fills a string into a string and verifies its length.

                convert(b_PROPERTIES_SENSORS_id.get(i+1).asString(), sizeof(item.id), item.id, formaterror);
                convert(b_PROPERTIES_SENSORS_type.get(i+1).asString(), item.type, formaterror);
                convert(b_PROPERTIES_SENSORS_location.get(i+1).asString(), item.location, formaterror);
            }

            // in here we could decide to return false if any previous conversion function has returned error
            // bool fromStringToBoolean(string str, bool &anyerror); // inside: if error then .... be sure to set error = true. dont set it to false.

            if(true == formaterror)
            {
                yError() << "ServiceParser::check() has detected an illegal format for some of the params of PROPERTIES.SENSORS some param has inconsistent length";
                return false;
            }

        }

    }

    Bottle b_SETTINGS = Bottle(config.findGroup("SETTINGS"));
    if(b_SETTINGS.isNull())
    {
        yError() << "ServiceParser::check() cannot find SETTINGS";
        return false;
    }
    else
    {

        Bottle b_SETTINGS_acquisitionRate = Bottle(b_SETTINGS.findGroup("acquisitionRate"));
        if(b_SETTINGS_acquisitionRate.isNull())
        {
            yError() << "ServiceParser::check() cannot find SETTINGS.acquisitionRate";
            return false;
        }
        Bottle b_SETTINGS_enabledSensors = Bottle(b_SETTINGS.findGroup("enabledSensors"));
        if(b_SETTINGS_enabledSensors.isNull())
        {
            yError() << "ServiceParser::check() cannot find SETTINGS.enabledSensors";
            return false;
        }

        int numenabledsensors = b_SETTINGS_enabledSensors.size();    // first position of bottle contains the tag "enabledSensors"

        // the enabled must be <= the sensors.
        if( numenabledsensors > as_service.properties.sensors.size() )
        {
            yError() << "ServiceParser::check() in SETTINGS.enabledSensors there are too many items with respect to supported sensors:" << numenabledsensors << "vs." << as_service.properties.sensors.size();
            return false;
        }

        convert(b_SETTINGS_acquisitionRate.get(1).asInt(), as_service.settings.acquisitionrate, formaterror);
        //as_service.settings.acquisitionrate = b_SETTINGS_acquisitionRate.get(1).asInt();

        as_service.settings.enabledsensors.resize(0);

        for(int i=0; i<numenabledsensors; i++)
        {
            servAnalogSensor_t founditem = {0};

            ConstString s_enabled_id = b_SETTINGS_enabledSensors.get(i+1).asString();
            const char *str = s_enabled_id.c_str();

            // we must now search inside the whole vector<> as_service.properties.sensors if we find an id which matches s_enabled_id ....
            // if we dont, ... we issue a warning.
            // if we find, ... we do a pushback of it inside
            bool found = false;
            // i decide to use a brute force search ... for now
            for(int n=0; n<as_service.properties.sensors.size(); n++)
            {
                servAnalogSensor_t item = as_service.properties.sensors.at(n);
                if(0 == strcmp(item.id, str))
                {
                    found = true;
                    break;
                }
            }

            if(true == found)
            {
                as_service.settings.enabledsensors.push_back(founditem);
            }

        }

        // in here we issue an error if we dont have at least one enabled sensor

        if(0 == as_service.settings.enabledsensors.size())
        {
            yError() << "ServiceParser::check() could not find any item in SETTINGS.enabledSensors which matches what in PROPERTIES.SENSORS.id";
            return false;
        }

    }

    // now we may have one or more sections which are specific of the device ...

    // only strain so far.

    if(eomn_serv_AS_strain == type)
    {
        Bottle b_STRAIN_SETTINGS = Bottle(config.findGroup("STRAIN_SETTINGS"));
        if(b_STRAIN_SETTINGS.isNull())
        {
            yError() << "ServiceParser::check() cannot find STRAIN_SETTINGS";
            return false;
        }
        else
        {

            Bottle b_STRAIN_SETTINGS_useCalibration = Bottle(b_STRAIN_SETTINGS.findGroup("useCalibration"));
            if(b_STRAIN_SETTINGS_useCalibration.isNull())
            {
                yError() << "ServiceParser::check() cannot find STRAIN_SETTINGS.useCalibration";
                return false;
            }

            formaterror = false;
            convert(b_STRAIN_SETTINGS_useCalibration.get(1).asString(), as_strain_settings.useCalibration, formaterror);

            if(true == formaterror)
            {
                yError() << "ServiceParser::check() has detected an illegal format for paramf STRAIN_SETTINGS.useCalibration";
                return false;
            }
        }
    }




    // we we are in here we have the struct filled with all variables ... some validations are still due to the calling device.
    // for instance, if embObjMais

    return true;
}


bool ServiceParser::parseService(Searchable &config, servConfigMais_t &maisconfig)
{
    if(false == check_analog(config, eomn_serv_AS_mais))
    {
        yError() << "ServiceParser::parseService() has received an invalid SERVICE group for mais";
        return false;
    }

    // now we extract values ... so far we dont make many checks ... we just assume the vector<> are of size 1.
    servCanBoard_t themais_props = as_service.properties.canboards.at(0);
    servAnalogSensor_t themais_sensor = as_service.properties.sensors.at(0);

    maisconfig.acquisitionrate = as_service.settings.acquisitionrate;

    memset(&maisconfig.ethservice.configuration, 0, sizeof(maisconfig.ethservice.configuration));

    maisconfig.ethservice.configuration.type = eomn_serv_AS_mais;
    memcpy(&maisconfig.ethservice.configuration.data.as.mais.version.protocol, &themais_props.protocol, sizeof(eOmn_canprotocolversion_t));
    memcpy(&maisconfig.ethservice.configuration.data.as.mais.version.firmware, &themais_props.firmware, sizeof(eOmn_canfirmwareversion_t));

    memcpy(&maisconfig.ethservice.configuration.data.as.mais.canloc, &themais_sensor.location.address.oncan, sizeof(eOmn_serv_canlocation_t));


    return true;
}

bool ServiceParser::parseService(Searchable &config, servConfigStrain_t &strainconfig)
{
    if(false == check_analog(config, eomn_serv_AS_strain))
    {
        yError() << "ServiceParser::parseService() has received an invalid SERVICE group for strain";
        return false;
    }

    // now we extract values ... so far we dont make many checks ... we just assume the vector<> are of size 1.
    servCanBoard_t thestrain_props = as_service.properties.canboards.at(0);
    servAnalogSensor_t thestrain_sensor = as_service.properties.sensors.at(0);

    strainconfig.acquisitionrate = as_service.settings.acquisitionrate;
    strainconfig.useCalibration = as_strain_settings.useCalibration;


    memset(&strainconfig.ethservice.configuration, 0, sizeof(strainconfig.ethservice.configuration));

    strainconfig.ethservice.configuration.type = eomn_serv_AS_strain;
    memcpy(&strainconfig.ethservice.configuration.data.as.strain.version.protocol, &thestrain_props.protocol, sizeof(eOmn_canprotocolversion_t));
    memcpy(&strainconfig.ethservice.configuration.data.as.strain.version.firmware, &thestrain_props.firmware, sizeof(eOmn_canfirmwareversion_t));

    memcpy(&strainconfig.ethservice.configuration.data.as.strain.canloc, &thestrain_sensor.location.address.oncan, sizeof(eOmn_serv_canlocation_t));


    return true;
}


bool ServiceParser::parseService(Searchable &config, servConfigInertials_t &inertialsconfig)
{
    if(false == check_analog(config, eomn_serv_AS_inertial))
    {
        yError() << "ServiceParser::parseService() has received an invalid SERVICE group for inertials";
        return false;
    }

    // now we extract values ... so far we dont make many checks ... we just assume the vector<> are of size 1.
    servCanBoard_t themtb_props = as_service.properties.canboards.at(0);

    inertialsconfig.acquisitionrate = as_service.settings.acquisitionrate;

    inertialsconfig.mtbconfig.datarate = as_service.settings.acquisitionrate;


    memset(&inertialsconfig.ethservice.configuration, 0, sizeof(inertialsconfig.ethservice.configuration));

    memset(&inertialsconfig.mtbconfig, 0, sizeof(inertialsconfig.mtbconfig));

    inertialsconfig.ethservice.configuration.type = eomn_serv_AS_inertial;
    memcpy(&inertialsconfig.ethservice.configuration.data.as.inertial.version.protocol, &themtb_props.protocol, sizeof(eOmn_canprotocolversion_t));
    memcpy(&inertialsconfig.ethservice.configuration.data.as.inertial.version.firmware, &themtb_props.firmware, sizeof(eOmn_canfirmwareversion_t));

    // now, for all the sensors we must fill:
    // - inertialsconfig.ethservice.configuration.data.as.inertial.canmap[2] with mask of location of mtb boards
    // - inertialsconfig.mtbconfig.internal_accel[2], external_accel[2], external_gyros[2].



    for(int i=0; i<as_service.settings.enabledsensors.size(); i++)
    {
        servAnalogSensor_t sensor = as_service.settings.enabledsensors.at(i);
        eOas_sensor_t type = sensor.type;
        uint8_t port = sensor.location.address.oncan.port;
        uint8_t addr = sensor.location.address.oncan.addr;
        // set bit in position addr of canmap[port]
        eo_common_hlfword_bitset(&inertialsconfig.ethservice.configuration.data.as.inertial.canmap[port], addr);
        // we also set the bit in position addr of a halfword but ... the halfword depends on the type.
        uint16_t *hword = NULL;
        switch(type)
        {
            case eoas_accel_mtb_int:
            {
                hword = &inertialsconfig.mtbconfig.internal_accel[port];
            } break;
            case eoas_accel_mtb_ext:
            {
                hword = &inertialsconfig.mtbconfig.external_accel[port];
            } break;
            case eoas_gyros_mtb_ext:
            {
                hword = &inertialsconfig.mtbconfig.external_gyros[port];
            } break;
            default:
            {
                yError() << "ServiceParser::parseService() has detected a wrong type of inertials: only eoas_accel_mtb_int, eoas_accel_mtb_ext, and eoas_gyros_mtb_ext are supported";
                hword = NULL;
            } break;
        }

        if(NULL == hword)
        {
            return false;
        }

        eo_common_hlfword_bitset(hword, addr);

    }


    return true;
}

// eof
