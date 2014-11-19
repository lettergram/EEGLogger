/**
 * Origionally an example from Emotiv for EEGLogger using Qt
 * Modified by Austin Walters
 *
 * Written in C++, Nov. 2014
 *
 * Using Emotiv Premium SDK
 *
 * To Compile: g++ -L/usr/local/lib/EmotivSDKLib-PREMIUM -ledk collectEEG.cpp -o collectEEG
 * To Run: ./collectEEG <Name of Filename> <location to save file>
 *
 * Used to collect data on various faces/movements for my 
 * CS598 (machine learning and signal processing) course, for my final project.
 *
 * The idea is to use this to classify various signals, and in turn fix errors.
 *
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctime>

#include <EmoStateDLL.h>
#include <edk.h>
#include <edkErrorCode.h>

EE_DataChannel_t targetChannelList[] = {
        ED_COUNTER,
        ED_AF3, ED_F7, ED_F3, ED_FC5, ED_T7,
        ED_P7, ED_O1, ED_O2, ED_P8, ED_T8,
        ED_FC6, ED_F4, ED_F8, ED_AF4, ED_GYROX, ED_GYROY, ED_TIMESTAMP,
        ED_FUNC_ID, ED_FUNC_VALUE, ED_MARKER, ED_SYNC_SIGNAL
    };

const char header[] = "COUNTER,AF3,F7,F3, FC5, T7, P7, O1, O2,P8"
                      ", T8, FC6, F4,F8, AF4,GYROX, GYROY, TIMESTAMP, "
                      "FUNC_ID, FUNC_VALUE, MARKER, SYNC_SIGNAL,";

int kbhit (void)
{
    struct timeval tv;
    fd_set rdfs;

    tv.tv_sec = 0;
    tv.tv_usec = 0;

    FD_ZERO(&rdfs);
    FD_SET (STDIN_FILENO, &rdfs);

    select(STDIN_FILENO+1, &rdfs, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &rdfs);
}

/**
 * Has users make a face every 20 seconds,
 * or 20 * 128 (sampling rate) 
 *
 * Returns true if keep test going,
 * Returns false if it test should stop
 *
 */
bool printFace(long long count){
    if(count == 0){
        std::cout << "Blank Face" << std::endl;
        count++;
    }else if(count == 20 * 128){
        std::cout << "Blink" << std::endl;
    }else if(count == 40 * 128){
        std::cout << "Right Wink" << std::endl;
    }else if(count == 60 * 128){
        std::cout << "Left Wink" << std::endl;
    }else if(count == 80 * 128){
        std::cout << "Fake Talk" << std::endl;
    }else if(count == 100 * 128){
        std::cout << "Furrow Brow" << std::endl;
    }else if(count == 120 * 128){
        std::cout << "Smile" << std::endl;
    }else if(count == 140 * 128){
        std::cout << "Squint" << std::endl;
    }else if(count == 160 * 128){
        std::cout << "Scratch Right Ear" << std::endl;
    }else if(count == 180 * 128){
        std::cout << "Scratch Left Ear" << std::endl;
    }else if(count == 200 * 128){
        std::cout << "Scratch Right Face" << std::endl;
    }else if(count == 220 * 128){
        std::cout << "Scratch Left Face" << std::endl;
    }else if(count == 240 * 128){
        std::cout << "Close Eyes & Count to 15" << std::endl;
    }else if(count == 260 * 128){
        std::cout << "Act Like Chewing Gum" << std::endl;
    }else if(count == 280 * 128){
        std::cout << "View Flashing Box" << std::endl;
    }else if(count == 300 * 128){
        std::cout << "Complete, Thank you!" << std::endl;
        return false;
    }
    return true;
}

int main(int argc, char** argv) {

    EmoEngineEventHandle eEvent			= EE_EmoEngineEventCreate();
    EmoStateHandle eState				= EE_EmoStateCreate();
    unsigned int userID					= 0;    
    float secs							= 1;
    unsigned int datarate				= 0;
    bool readytocollect					= false;
    int state							= 0;
    bool connected                      = true;
    long long count                     = 0;

    // Connect to the EE_Engine.
    if (EE_EngineConnect() != EDK_OK) {
        std::cout << "Could not connect to EEG" << std::endl;
        return 0;
    }

    std::string directory = argv[2];
    std::string filename = argv[1];
    filename = directory + filename + "-EEGLogger.csv";

    std::cout << filename << std::endl; // were the file will be saved

    std::cout << "Start receiving EEG Data! Press any key to stop logging...\n" << std::endl;
    std::ofstream ofs(filename.c_str(),std::ios::trunc);
    ofs << header << std::endl;

    if(connected){
        DataHandle hData = EE_DataCreate();
        EE_DataSetBufferSizeInSec(secs);

        std::cout << "Buffer size in secs:" << secs << std::endl;

        while (!kbhit()){
            state = EE_EngineGetNextEvent(eEvent);
          
          if(!printFace(count)){ break; } // print new face to make every 20 seconds
            
            if (state == EDK_OK){

                EE_Event_t eventType = EE_EmoEngineEventGetType(eEvent);
                EE_EmoEngineEventGetUserId(eEvent, &userID);

                // Log the EmoState if it has been updated
                if (eventType == EE_UserAdded){
                    std::cout << "User added" << std::endl;
                    EE_DataAcquisitionEnable(userID,true);
                    readytocollect = true;
                }
            }
            if (readytocollect){
                EE_DataUpdateHandle(0, hData);

                unsigned int nSamplesTaken=0;
                EE_DataGetNumberOfSample(hData,&nSamplesTaken);

                if (nSamplesTaken != 0) {
                      double* data = new double[nSamplesTaken];
                      for (int sampleIdx=0 ; sampleIdx<(int)nSamplesTaken ; ++ sampleIdx) {
                           for (int i = 0 ; i<sizeof(targetChannelList)/sizeof(EE_DataChannel_t) ; i++) {
                               EE_DataGet(hData, targetChannelList[i], data, nSamplesTaken);
                               ofs << data[sampleIdx] << ",";
                            }
                            ofs << std::endl;
                            count++;
                       }
                       delete[] data;
                }
            }
        }

        ofs.close();
        EE_DataFree(hData);
    }

    std::cout << "Disconnected" << std::endl;

    EE_EngineDisconnect();
    EE_EmoStateFree(eState);
    EE_EmoEngineEventFree(eEvent);

    return 0;
}

