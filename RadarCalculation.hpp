/*   Bridge Command 5.0 Ship Simulator
     Copyright (C) 2014 James Packer

     This program is free software; you can redistribute it and/or modify
     it under the terms of the GNU General Public License version 2 as
     published by the Free Software Foundation

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY Or FITNESS For A PARTICULAR PURPOSE.  See the
     GNU General Public License For more details.

     You should have received a copy of the GNU General Public License along
     with this program; if not, write to the Free Software Foundation, Inc.,
     51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

#ifndef __RADARCALCULATION_HPP_INCLUDED__
#define __RADARCALCULATION_HPP_INCLUDED__

#include "irrlicht.h"

#include <vector>
#include <string>
#include <stdint.h> //for uint64_t

#include <ctime> //To check time elapsed between changing EBL when button held down

class Terrain;
class OwnShip;
class Buoys;
class OtherShips;
class RadarData;

enum ARPA_CONTACT_TYPE {
    CONTACT_NORMAL,
    CONTACT_MANUAL
};

struct ARPAScan {
    //ARPA scan information based on the assumption that the own ship position is well known
    irr::f32 x; //Absolute metres
    irr::f32 z; //Absolute metres
    uint64_t timeStamp; //Timestamp in seconds
    irr::f32 estimatedRCS; //Estimated radar cross section
    irr::f32 rangeNm; //Reference only
    irr::f32 bearingDeg; //For reference only
};

struct ARPAEstimatedState {
    bool ignored; // E.g. if detected as static and a small RCS or a buoy. (Should be overridable by user)
    irr::f32 absVectorX; //Estimated X speed (m/s)
    irr::f32 absVectorZ; //Estimated Z speed (m/s)
    irr::f32 absHeading; //Estimated heading (deg)
    //Also to implement
    //Distance of CPA
    //Time to CPA
};

struct ARPAContact {
    std::vector<ARPAScan> scans;
    ARPA_CONTACT_TYPE contactType;
    void* contact;
    irr::u32 displayID;
    ARPAEstimatedState estimate;
};

class RadarCalculation
{
    public:
        RadarCalculation();
        virtual ~RadarCalculation();
        void load(std::string radarConfigFile);
        void increaseRange();
        void decreaseRange();
        irr::f32 getRangeNm() const;
        void setGain(irr::f32 value);
        void setClutter(irr::f32 value);
        void setRainClutter(irr::f32 value);
        irr::f32 getGain() const;
        irr::f32 getClutter() const;
        irr::f32 getRainClutter() const;
        irr::f32 getEBLRangeNm() const;
        irr::f32 getEBLBrg() const;
        void increaseEBLRange();
        void decreaseEBLRange();
        void increaseEBLBrg();
        void decreaseEBLBrg();
        void setNorthUp();
        void setCourseUp();
        void setHeadUp();
        bool getHeadUp() const; //Head or course up
        void update(irr::video::IImage * radarImage, irr::core::vector3d<irr::s64> offsetPosition, const Terrain& terrain, const OwnShip& ownShip, const Buoys& buoys, const OtherShips& otherShips, irr::f32 weather, irr::f32 rain, irr::f32 tideHeight, irr::f32 deltaTime, uint64_t absoluteTime);

    private:
        std::vector<std::vector<irr::f32> > scanArray;
        std::vector<std::vector<irr::f32> > scanArrayAmplified;
        std::vector<std::vector<irr::f32> > scanArrayAmplifiedPrevious;
        std::vector<ARPAContact> arpaContacts;
        irr::f32 radarGain;
        irr::f32 radarRainClutterReduction;
        irr::f32 radarSeaClutterReduction;
        irr::u32 currentScanAngle; //Note that this MUST be an integer, as the angle is used to look up values in radar scan arrays
        irr::u32 scanAngleStep; //Should also be an integer, as the angle being incremented is an integer
        irr::u32 rangeResolution;
        irr::u32 radarRangeIndex;
        irr::f32 radarScannerHeight;
        //parameters for noise behaviour
        irr::f32 radarNoiseLevel;
        irr::f32 radarSeaClutter;
        irr::f32 radarRainClutter;
        //Parameters for EBL
        irr::f32 EBLRangeNm;
        irr::f32 EBLBrg;
        clock_t EBLLastUpdated;
        //Radar config
        bool headUp;
        bool stabilised;
        //colours
        irr::video::SColor radarBackgroundColour;
        irr::video::SColor radarForegroundColour;

        std::vector<irr::f32> radarRangeNm;
        void scan(irr::core::vector3d<irr::s64> offsetPosition, const Terrain& terrain, const OwnShip& ownShip, const Buoys& buoys, const OtherShips& otherShips, irr::f32 weather, irr::f32 rain, irr::f32 tideHeight, irr::f32 deltaTime, uint64_t absoluteTime);
        void updateARPA(irr::core::vector3d<irr::s64> offsetPosition, const OwnShip& ownShip, uint64_t absoluteTime);
        irr::f32 radarNoise(irr::f32 radarNoiseLevel, irr::f32 radarSeaClutter, irr::f32 radarRainClutter, irr::f32 weather, irr::f32 radarRange,irr::f32 radarBrgDeg, irr::f32 windDirectionDeg, irr::f32 radarInclinationAngle, irr::f32 rainIntensity);
        void render(irr::video::IImage * radarImage, irr::f32 ownShipHeading);
        irr::f32 rangeAtAngle(irr::f32 checkAngle,irr::f32 centreX, irr::f32 centreZ, irr::f32 heading);
        void drawSector(irr::video::IImage * radarImage,irr::f32 centreX, irr::f32 centreY, irr::f32 innerRadius, irr::f32 outerRadius, irr::f32 startAngle, irr::f32 endAngle, irr::u32 alpha, irr::u32 red, irr::u32 green, irr::u32 blue, irr::f32 ownShipHeading);

};

#endif
