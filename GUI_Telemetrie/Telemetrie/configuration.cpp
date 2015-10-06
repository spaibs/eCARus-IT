#include "configuration.h"
#include "mainwindow.h"
#include <fstream>
#include <Windows.h>
#include <stdlib.h>
#include <cstdlib>
#include <sstream>
#include <QString>

configdata config_handler::defaultconfig;
configdata config_handler::currentconfig;

void config_handler::init_config ()
{
    /*
        sets values for defaultconfig
    */

    // all default values where the corect values afaik, whe I wrote this application

    // network information
    defaultconfig.networkADR = "192.168.1.255";
    defaultconfig.networkPORT = "32787";

    // car informations
    defaultconfig.tirecircmm = 1753;  // the tire circumference of ecarus

    // accelerator fallback data
    defaultconfig.acceleratorfallback=false; // no fallback by default
    defaultconfig.acceleratorrange=0x8C;
    defaultconfig.acceleratoroffset=0x0C;
    // this values are the (former) fallback values from the car code

    // converter data factors
    defaultconfig.converterbatterycurrentfactor = 0.1;	// ??? i am not sure about this data
    defaultconfig.convertercapacitorvoltagefactor = 0.015625;
    defaultconfig.convertercontrollertempfactor = 0.1;
    defaultconfig.convertercurrentrequestfactor = 0.1;
    defaultconfig.convertercurrentrmsfactor = 0.1;
    defaultconfig.convertermotorrpmfactor = 1;
    defaultconfig.convertermotortempfactor = 0.1;
    defaultconfig.convertervclthrottlefactor = 0.00305185094759971922971282082583;
    defaultconfig.DCDCcurrentfactor = 0.01;
    defaultconfig.HVVfactor = 0.01;

    // converter data offsets
    defaultconfig.converterbatterycurrentoffset = 0;
    defaultconfig.convertercapacitorvoltageoffset = 0;
    defaultconfig.convertercontrollertempoffset = 0;
    defaultconfig.convertercurrentrmsoffset = 0;
    defaultconfig.convertercurrentrequestoffset = 0;
    defaultconfig.convertermotorrpmoffset = 0;
    defaultconfig.convertermotortempoffset = 0;
    defaultconfig.convertervclthrottleoffset = 0;
    defaultconfig.DCDCcurrentoffset = 0;
    defaultconfig.HVVoffset = 0;

    return;
}

int config_handler::interprete(configdata *config,const QString buf1,const QString buf2, int line)
{
    /*
    used by the read function to interprete the data.
    this function is just to make the readfuntion smaller and more readable

    Inputs:
    configdata*		config	The configuration data that should be changed
    const char*		buf1	Null terminated string that contains the keyword
    const char*		buf2	Null terminated string that contains the value
    int				line	Current line (used to print error messages)
    Return Values:
    0	SUCESS
    1	ERROR	There was an error. The error has been logged. The Read function should
                continue but end with errorcode 2.
*/

    // define format settings (to use . as decimal seperator)
    //TFormatSettings format;
    //format.DecimalSeparator='.';

    try
    {
        if (!strncmp(buf1.toLocal8Bit().data(),"tire_circ_mm",500))
        {
            config->tirecircmm=buf2.toFloat();
        }
        else if (!strncmp (buf1.toLocal8Bit().data(),"converterbatterycurrentfactor",500))
        {
            config->converterbatterycurrentfactor=buf2.toFloat();
        }
        else if (!strncmp (buf1.toLocal8Bit().data(),"converterbatterycurrentoffset",500))
        {
            config->converterbatterycurrentoffset=buf2.toFloat();
        }
        else if (!strncmp (buf1.toLocal8Bit().data(),"convertercapacitorvoltagefactor",500))
        {
            config->convertercapacitorvoltagefactor=buf2.toFloat();
        }
        else if (!strncmp (buf1.toLocal8Bit().data(),"convertercapacitorvoltageoffset",500))
        {
            config->convertercapacitorvoltageoffset=buf2.toFloat();
        }
        else if (!strncmp (buf1.toLocal8Bit().data(),"convertercontrollertempfactor",500))
        {
            config->convertercontrollertempfactor=buf2.toFloat();
        }
        else if (!strncmp (buf1.toLocal8Bit().data(),"convertercontrollertempoffset",500))
        {
            config->convertercontrollertempoffset=buf2.toFloat();
        }
        else if (!strncmp (buf1.toLocal8Bit().data(),"convertercurrentrequestfactor",500))
        {
            config->convertercurrentrequestfactor=buf2.toFloat();
        }
        else if (!strncmp (buf1.toLocal8Bit().data(),"convertercurrentrequestoffset",500))
        {
            config->convertercurrentrequestoffset=buf2.toFloat();
        }
        else if (!strncmp (buf1.toLocal8Bit().data(),"convertercurrentrmsfactor",500))
        {
            config->convertercurrentrmsfactor=buf2.toFloat();
        }
        else if (!strncmp (buf1.toLocal8Bit().data(),"convertercurrentrmsoffset",500))
        {
            config->convertercurrentrmsoffset=buf2.toFloat();
        }
        else if (!strncmp (buf1.toLocal8Bit().data(),"convertermotorrpmfactor",500))
        {
            config->convertermotorrpmfactor=buf2.toInt();
        }
        else if (!strncmp (buf1.toLocal8Bit().data(),"convertermotorrpmoffset",500))
        {
            config->convertermotorrpmoffset=buf2.toInt();
        }
        else if (!strncmp (buf1.toLocal8Bit().data(),"convertermotortempfactor",500))
        {
            config->convertermotortempfactor=buf2.toFloat();
        }
        else if (!strncmp (buf1.toLocal8Bit().data(),"convertermotortempoffset",500))
        {
            config->convertermotortempoffset=buf2.toFloat();
        }
        else if (!strncmp (buf1.toLocal8Bit().data(),"convertervclthrottlefactor",500))
        {
            config->convertervclthrottlefactor=buf2.toFloat();
        }
        else if (!strncmp (buf1.toLocal8Bit().data(),"convertervclthrottleoffset",500))
        {
            config->convertervclthrottleoffset=buf2.toFloat();
        }
        else if (!strncmp (buf1.toLocal8Bit().data(),"networkADR",500))
        {
            config->networkADR = buf2;
        }
        else if (!strncmp (buf1.toLocal8Bit().data(),"networkPORT",500))
        {
            config->networkPORT=buf2;
        }
        else if (!strncmp (buf1.toLocal8Bit().data(),"acceleratoroffset",500))
        {
            config->acceleratoroffset=buf2.toInt();
        }
        else if (!strncmp (buf1.toLocal8Bit().data(),"acceleratorrange",500))
        {
            config->acceleratorrange=buf2.toInt();
            if ( config->acceleratorrange <= 0 )
            {
                //TODO				Form1->app_log->Lines->Add("config: acceleratorrange is smaller or equal zero. The percantage will not be calculated." );
            }
        }
        else if (!strncmp (buf1.toLocal8Bit().data(),"acceleratorfallback",500))
        {
            config->acceleratorfallback=buf2.toInt();
        }
        else if (!strncmp (buf1.toLocal8Bit().data(),"DCDCcurrentfactor",500))
        {
            config->DCDCcurrentfactor=buf2.toFloat();
        }
        else if (!strncmp (buf1.toLocal8Bit().data(),"DCDCcurrentoffset",500))
        {
            config->DCDCcurrentoffset=buf2.toFloat();
        }
        else if (!strncmp (buf1.toLocal8Bit().data(),"HVVfactor",500))
        {
            config->HVVfactor=buf2.toFloat();
        }
        else if (!strncmp (buf1.toLocal8Bit().data(),"HVVoffset",500))
        {
            config->HVVoffset=buf2.toFloat();
        }
        // ... please add more parameters here
        else
        {

            //TODO			Form1->app_log->Lines->Add("Config: Error parsing line " + IntToStr(line) + ": " + String(buf1) + " is not a valid keyword.");
            return 1;
        }

    }
    catch (...)
    {
        // Please note that despite the use of try catch the compiler will
        // print a warning if an exception occures during debuging.
        //TODO		Form1->app_log->Lines->Add("Config: Error parsing line " + IntToStr(line) +": The Value for " +String(buf1) + " is not parsable.");
        return 1;
    }
    return 0;
}

int config_handler::load_config(configdata *config, std::wstring filename)
{
    /*
        opens a config file and reads the config to *config
        It will use the default config on values that are not specified in the config file.
        It will ignore (but log) all errors and proceed with the next line of the config file.
        It will load the default config if the filename is "!default". In this case the return value
        will always be 0. If the filename is not "!default" and incorrect it will also load the default
        values but return code 1. Also if the filename is set to "!config" it will load the configuration
        from the default path "%appdata%\eCarus\eCarus Telemetry\config.txt". If the enviroment variable
        %appdata% does not exist it will return 3.

        Return Values:
        0		Sucess
        1		File Read error		The config datastructure is set to the default values
        2		Parsing error		There was at least one parsing error. All data that could
                                    be read have been read. Everything else has been set to
                                    the default Values.
        3		missing enviroment	The enviroment variable appdata does not exist. This variable
                variable			should always exist.

*/

    HANDLE hFile;            // The file that will be read
    char buf1[500];    		// stores first half of line (before =)
    char buf2[500];    		// stores second half of line

    bool warn=false;   		// when set to true the methode will return code 2

    int line=0;				// stores the current line (for logging)



    // copy the default config
    *config = config_handler::defaultconfig;

    // check if the user wants to load the default config
    if ( filename == L"!default" )
    {
        // the programm is sucessfull, because the config already is set to the default config
        return 0;
    }

    // open the config file
    if ( filename == L"!config" )
    {
        // open the file from appdata/eCarus/eCarus telemetry/config.txt

        wchar_t wappdataloc[3500];

        if ( !GetEnvironmentVariable(L"appdata", wappdataloc, 3500) )
        {
            return 3;	// appdata does not exist
        }
        // build the complete path
        std::wstring stdconfigsaveloc(wappdataloc);
        stdconfigsaveloc.append(L"\\eCarus\\eCarus Telemetry\\config.txt");
        hFile = CreateFile(stdconfigsaveloc.c_str(),GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    }
    else
    {
        hFile = CreateFile(filename.c_str(),GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    }

    if ( hFile == INVALID_HANDLE_VALUE )
    {
        //TODO		Form1->app_log->Lines->Add("config: Unable to open config file (\""+ String(filename.c_str(),filename.length()) + "\").");
        return 1;    // could not open file
    }

    // read the configuration using the windows api
    int filesize;	// size of the file
    char *FileBuf;  // array where the file will be saved to
    filesize = GetFileSize(hFile,NULL);
    FileBuf = (char*)malloc((filesize+1)*sizeof(char));
    unsigned long byteread; // stores the bytes that have been read
    ReadFile(hFile,FileBuf,filesize,&byteread,NULL);
    FileBuf[filesize] = '\0';    // terminate the string
    unsigned int filepos;	// the current position (characternumber) in the file
    int writepos=0;    // the positon in the aim variable
    char* bufpnt = buf1;  // pointer to the array to which the function writes

    for ( filepos = 0; filepos < byteread; filepos++ )
    {
        if ( FileBuf[filepos] == '=' )
        {
            if (bufpnt == buf2)
            {
                //TODO				Form1->app_log->Lines->Add("Config: Error parsing line " + IntToStr(line) +": Unexpected =");
                bufpnt = buf1;
                while ( FileBuf[filepos] != '\n' )
                {
                    filepos++;
                }
                bufpnt[writepos] = '\0';
                bufpnt = buf1;
                writepos = 0;
                line++;
                continue;
            }
            bufpnt[writepos] = '\0';
            bufpnt = buf2; // write to the second buffer
            writepos = 0;
            continue;
        }
        if ( FileBuf[filepos] == '\r' )
        {
            if ( (bufpnt == buf1) && (writepos != 0) )
            {
                // warn about line end that is on a wrong position
                // this will also warn when the file says "   \r\n"
                // this could be fixed but is not critical
                //TODO				Form1->app_log->Lines->Add("Config: Error parsing line " + IntToStr(line) +": Unexpected end of line");
                warn = true;
            }
            bufpnt[writepos] = '\0';
            if (bufpnt == buf2)
            {
                if (config_handler::interprete(config,buf1,buf2,line))
                {
                    // set warn to true if there is a problem
                    warn = true;
                }
            }
            bufpnt = buf1; // reset the buf pointer to the first buffer
            writepos = 0;
            filepos++; // take care of the windows endline
            line++;
            continue;
        }

        if ( FileBuf[filepos] == '#' )
        {
            if ( (bufpnt == buf1) && (writepos != 0) )
            {
                // warn about comment that is on a wrong position
                // this will also warn when the file says "   #comment"
                // this could be fixed but is not critical
                //TODO				Form1->app_log->Lines->Add("Config: Error parsing line " + IntToStr(line) +": Unexpected comment");
                warn = true;

            }
            // Search for the end of the line //FIXME: when filepos reaches end of file, it is still being incremented leading to segmentation fault

            while ( FileBuf[filepos] != '\n' )
            {
                filepos++;

            }

            bufpnt[writepos] = '\0';
            bufpnt = buf1;
            writepos = 0;
            line++;
            continue;
        }
        //write the data
        bufpnt[writepos] = FileBuf[filepos];
        writepos++;
    }


    // close the config file
    CloseHandle(hFile);
    // free the memory
    delete[] FileBuf;
    if (warn)
    {
        // warn the user if there was a problem reading the file by sending error code 2
        // the config data structure will contain all read data
        return 2;
    }
    return 0;
}

int config_handler::save_config(configdata *config, std::wstring file)
{
    /*

    Writes a config to a file.
    If the file already exists this methode will overwrite it.
    If the filename is "!config" the config will be writte to the default configuration file
    located in the %appdata%\eCarus\eCarus Telemetry folder.

    Return Values:
    0		SUCESS
    1		COULD NOT OPEN FILE
    3		missing enviroment	The enviroment variable appdata does not exist. This variable
            variable should always exist.

    */
    std::stringstream config_file;
    HANDLE hFile; // handle to the config file
    if ( file == L"!config" )
    {
        // open the file from appdata/eCarus/eCarus telemetry/config.txt


        // create the directory structure if it is not already existent
        wchar_t* wappdataloc = new wchar_t[3500]; // this should be enough in most cases
        // get the enviroment variabel
        if ( !GetEnvironmentVariable(L"appdata", wappdataloc, 3500) )
        {
            return 3;	// appdata does not exist
        }
        SetCurrentDirectory (wappdataloc);
        CreateDirectory (L"eCarus", NULL);

        SetCurrentDirectory (L"eCarus");
        CreateDirectory (L"eCarus Telemetry", NULL);


        // build the path
        std::wstring stdconfigsaveloc(wappdataloc);
        stdconfigsaveloc.append(L"\\eCarus\\eCarus Telemetry\\config.txt");

        // open the file
        hFile = CreateFile(stdconfigsaveloc.c_str(),GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    }
    else
    {
        hFile = CreateFile(file.c_str(),GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    }

    if (hFile == INVALID_HANDLE_VALUE )
    {
        return 1;
    }

    // Write the config file

    // convert network adress
    char temphost[500];
    wcstombs(temphost,config->networkADR.toStdWString().c_str(),500);
    std::string stdstringhst(temphost);
    config_file << "#Ecarus config file\r\n";
    config_file << "#This file has been automaticaly generated\r\n";
    config_file << "\r\n";

    config_file << "#Network Settings\r\n";
    config_file << "networkADR=" << stdstringhst << "\r\n"; // this string has been converted (see above)
    config_file << "networkPORT=" << config->networkPORT.toInt() << "\r\n";
    config_file << "\r\n";

    config_file << "#car data" << "\r\n";
    config_file << "tire_circ_mm=" << config->tirecircmm << "\r\n";
    config_file << "\r\n";

    config_file << "#information on how to interprete the raw data" << "\r\n";
    config_file << "\r\n";

    config_file << "#factors" << "\r\n";
    config_file << "converterbatterycurrentfactor=" << config->converterbatterycurrentfactor << "\r\n";
    config_file << "convertercapacitorvoltagefactor=" << config->convertercapacitorvoltagefactor << "\r\n";
    config_file << "convertercontrollertempfactor=" << config->convertercontrollertempfactor << "\r\n";
    config_file << "convertercurrentrequestfactor=" << config->convertercurrentrequestfactor << "\r\n";
    config_file << "convertercurrentrmsfactor=" << config->convertercurrentrmsfactor << "\r\n";
    config_file << "convertermotorrpmfactor=" << config->convertermotorrpmfactor << "\r\n";
    config_file << "convertermotortempfactor=" << config->convertermotortempfactor << "\r\n";
    config_file << "convertervclthrottlefactor=" << config->convertervclthrottlefactor << "\r\n";
    config_file << "DCDCcurrentfactor=" << config->DCDCcurrentfactor << "\r\n";
    config_file << "HVVfactor=" << config->HVVfactor << "\r\n";
    config_file << "\r\n";

    config_file << "#offsets" << "\r\n";
    config_file << "converterbatterycurrentoffset=" << config->converterbatterycurrentoffset << "\r\n";
    config_file << "convertercapacitorvoltageoffset=" << config->convertercapacitorvoltageoffset << "\r\n";
    config_file << "convertercontrollertempoffset=" << config->convertercontrollertempoffset << "\r\n";
    config_file << "convertercurrentrequestoffset=" << config->convertercurrentrequestoffset << "\r\n";
    config_file << "convertercurrentrmsoffset=" << config->convertercurrentrmsoffset << "\r\n";
    config_file << "convertermotorrpmoffset=" << config->convertermotorrpmoffset << "\r\n";
    config_file << "convertermotortempoffset=" << config->convertermotortempoffset << "\r\n";
    config_file << "convertervclthrottleoffset=" << config->convertervclthrottleoffset << "\r\n";
    config_file << "DCDCcurrentoffset=" << config->DCDCcurrentoffset << "\r\n";
    config_file << "HVVoffset=" << config->HVVoffset << "\r\n";
    config_file << "\r\n";

    config_file << "#Accelerator Fallback" << "\r\n";
    config_file << "acceleratorfallback=" << config->acceleratorfallback << "\r\n";
    config_file << "acceleratoroffset=" << config->acceleratoroffset << "\r\n";
    config_file << "acceleratorrange=" << config->acceleratorrange << "\r\n";

    config_file << "\r\n";
    config_file << "##################" << std::endl << "#End of file" << std::endl;

    // Write the real file
    unsigned long byteswritten;
    WriteFile(hFile,config_file.str().c_str(),config_file.str().length(), &byteswritten, NULL);
    CloseHandle(hFile);
    return 0;
}
