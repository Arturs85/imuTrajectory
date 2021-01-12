//! @file
//! @brief	main executable



#include <unistd.h>
#include <inttypes.h>

#include <iostream>
#include <string>
#include <sstream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <vector>
#include <cmath>
#include <fstream>

using std::cout;
using std::endl;
using std::string;

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();

    }
    return result;
}

double g =  9.80665;
double pi = 3.14159;
std::ifstream imuAccFile;
std::ifstream imuGyroFile;
double accx, accy, accz;
double accxp, accyp, acczp;

double gx, gy, gz;
unsigned long currentGyroTimeNs=0;
unsigned long prevGyroTimeNs=0;

unsigned long currentAccTimeNs=0;

int readNextImuData(){//both gyro and acc

    std::string line;
    std::getline(imuGyroFile, line);
    std::istringstream iss(line);
    double unused1;
    unsigned long unused4;
    char c;
    if (!(iss >> unused4>>c>>currentAccTimeNs>>c>>currentGyroTimeNs>>c>>accx>>c>>accy>>c>>accz>>c>>gx>>c>>gy>>c>>gz)) { return 0; } // error
    return 1;
}

int readFirstImuData(){//to initialize previous time

    std::string line;
    std::getline(imuGyroFile, line);
    std::istringstream iss(line);
    double unused1;
    unsigned long unused4;
    char c;
    if (!(iss >> unused4>>c>>currentAccTimeNs>>c>>prevGyroTimeNs>>c>>accx>>c>>accy>>c>>accz>>c>>gx>>c>>gy>>c>>gz)) { return 0; } // error
    return 1;
}

int main(int argc, char *argv[])
{


    std::string inputPathLIDAR;
        if (argc == 1) // check the command line arguments
        {
            cout << "Please, provide imu file path (wo filename) in the argument !" << endl;

            return(1); // return: failed

        }
        // valid filename

        inputPathLIDAR= argv[1];
        std::cout << "Starting reading file" << std::endl;


    std::string imuDataFilePath = inputPathLIDAR+"imu_data.dat";
    imuAccFile = std::ifstream(imuDataFilePath.c_str(), std::ios::in);//open file
    std::string line;
    std::getline(imuAccFile, line);//discard first line- header
    imuGyroFile = std::ifstream(imuDataFilePath.c_str(), std::ios::in);//open file
    //std::string line;
    std::getline(imuGyroFile, line);//discard first line- header


int count =0;
unsigned long firstGyroTimeNs=0;

double vx=0;
double vy=0;

readFirstImuData();
firstGyroTimeNs = prevGyroTimeNs;// store first time value for dt
readNextImuData();

double accxSum = 0;
double accySum = 0;


while(1){// just count acc, to get correction value


                   double linear_acceleration_x= accx*g;//convert to m/s^2
                   double linear_acceleration_y= accy*g;
                   double linear_acceleration_z= accz*g;

                   // imuMsg.angular_velocity.x= gx/(180/pi);//convert to rad/s
                   // imuMsg.angular_velocity.y= gy/(180/pi);
                   // imuMsg.angular_velocity.z= gz/(180/pi);



accxSum+= linear_acceleration_x;
accySum+= linear_acceleration_y;

     //   if(count%100 == 0)
     //   std::cout<<count<<". "<<dt<<" "<<linear_acceleration_y<<" "<<vy<<"\n";

        prevGyroTimeNs = currentGyroTimeNs;
                    int ok = readNextImuData();
                    count ++;
                    if(ok != 1)break;

                }
imuGyroFile.close();
unsigned long dtGyroFile = currentGyroTimeNs-firstGyroTimeNs;



imuGyroFile = std::ifstream(imuDataFilePath.c_str(), std::ios::in);//reopen file
std::getline(imuGyroFile, line);//discard first line- header
readFirstImuData();
firstGyroTimeNs = prevGyroTimeNs;// store first time value for dt
readNextImuData();

count =0;

double x = 0;
double y = 0;
double anglez = 0;
while(1){


                   double linear_acceleration_x= accx*g;//convert to m/s^2
                   double linear_acceleration_y= accy*g;
                   double linear_acceleration_z= accz*g;

//                    imuMsg.angular_velocity.x= gx/(180/pi);//convert to rad/s
//                    imuMsg.angular_velocity.y= gy/(180/pi);
//                    imuMsg.angular_velocity.z= gz/(180/pi);


        double dt = currentGyroTimeNs-prevGyroTimeNs;
double corAccx = accxSum*dt/dtGyroFile;
double corAccy = accySum*dt/dtGyroFile;

        if(std::abs(linear_acceleration_x) > 0){
        vx += ((linear_acceleration_x-corAccx)*dt/1000000000 );
}
        if(std::abs(linear_acceleration_y) > 0){
        vy += ((linear_acceleration_y-corAccy)*dt/1000000000 );
}

        anglez += dt*gz/1000000000;

          if(count%100 == 0){
x+=std::cos(anglez/(180/pi));
y+=std::sin(anglez/(180/pi));
          }
        if(count%100 == 0){
//        std::cout<<count<<". "<<dt<<" "<<linear_acceleration_x<<" "<<vx<<" "<<vy<<"\n";
            std::cout<<count<<" "<<dt<<" "<<gz<<" "<<" "<<anglez<<" "<<x<<" "<<y<< "\n";
}
        prevGyroTimeNs = currentGyroTimeNs;
                    int ok = readNextImuData();
                    count ++;
                    if(ok != 1)break;

                }



std::cout<<"accyxsum "<<accxSum<<" accysum "<<accySum<<" "<<vy<<"\n";

imuGyroFile.close();


}
