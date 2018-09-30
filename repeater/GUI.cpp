/*   Bridge Command 5.0 Ship Simulator
     Copyright (C) 2015 James Packer

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

#include "GUI.hpp"
#include "Constants.hpp"
#include "Utilities.hpp"

#include <iostream>
#include <limits>
#include <string>

using namespace irr;

GUIMain::GUIMain(IrrlichtDevice* device, Lang* language, irr::core::stringw message)
{
    this->device = device;
    guienv = device->getGUIEnvironment();

    video::IVideoDriver* driver = device->getVideoDriver();
    u32 su = driver->getScreenSize().Width;
    u32 sh = driver->getScreenSize().Height;

    this->language = language;

    //gui

    messageText = guienv->addStaticText(message.c_str(),core::rect<s32>(su*0.0,sh*0.0,su*0.9,sh*0.1));

    //choice buttons
    headingButton = guienv->addButton(core::rect<s32>(su*0.1,sh*0.1,su*0.9,sh*0.5),0,GUI_ID_HEADING_CHOICE,language->translate("headingIndicator").c_str());
    repeaterButton = guienv->addButton(core::rect<s32>(su*0.1,sh*0.5,su*0.9,sh*0.9),0,GUI_ID_REPEATER_CHOICE,language->translate("repeater").c_str());

    //Heading indicator
    heading = new gui::HeadingIndicator(guienv, guienv->getRootGUIElement(), core::rect<s32>(10,10,su-10,10+(su-20)/4));
    heading->setVisible(false);

    //User hasn't selected what mode to use
    modeChosen=false;

}

GUIMain::~GUIMain()
{
    heading->drop();
}

void GUIMain::updateGuiData(irr::f32 time, irr::f32 ownShipHeading, irr::f32 rudderAngle, irr::f32 wheelAngle)
{

    if(!modeChosen) {
        //Show GUI choice buttons only


    } else if (showHeadingIndicator) {

        irr::f32 angleScale = 2.0; //Magnification factor for indicator

        heading->setVisible(true);

        //hide choice buttons
        headingButton->setVisible(false);
        repeaterButton->setVisible(false);
        messageText->setVisible(false);

        //Set value
        heading->setHeading(ownShipHeading);


        //Draw rudder angle
        video::IVideoDriver* driver = device->getVideoDriver();
        u32 su = driver->getScreenSize().Width;
        u32 sh = driver->getScreenSize().Height;
        core::vector2d<s32> rudderIndicatorCentre = core::vector2d<s32>(0.5*su,0.5*sh);
        core::vector2d<s32> rudderAngleVectorHead = core::vector2d<s32> (        0,  0.240*sh);
        core::vector2d<s32> wheelAngleVectorHead = core::vector2d<s32> (        0,  0.240*sh);
        core::vector2d<s32> rudderAngleVectorBack1 = core::vector2d<s32>(-0.040*sh,  -0.040*sh);
        core::vector2d<s32> rudderAngleVectorBack2 = core::vector2d<s32>( 0.040*sh,  -0.040*sh);
        rudderAngleVectorHead.rotateBy(-1*rudderAngle*angleScale);
        wheelAngleVectorHead.rotateBy(-1*wheelAngle*angleScale);
        rudderAngleVectorBack1.rotateBy(-1*rudderAngle*angleScale);
        rudderAngleVectorBack2.rotateBy(-1*rudderAngle*angleScale);
        driver->draw2DLine(rudderIndicatorCentre+rudderAngleVectorBack1,rudderIndicatorCentre+rudderAngleVectorHead,irr::video::SColor(255,0,0,0));
        driver->draw2DLine(rudderIndicatorCentre+rudderAngleVectorHead,rudderIndicatorCentre+rudderAngleVectorBack2,irr::video::SColor(255,0,0,0));
        driver->draw2DLine(rudderIndicatorCentre+rudderAngleVectorBack2,rudderIndicatorCentre+rudderAngleVectorBack1,irr::video::SColor(255,0,0,0));
        driver->draw2DLine(rudderIndicatorCentre,rudderIndicatorCentre+wheelAngleVectorHead,irr::video::SColor(200,0,0,0));
        driver->draw2DPolygon(rudderIndicatorCentre,0.01*sh,irr::video::SColor(255,0,0,0));

        //Draw scale
        for(int i = -30; i<=30; i+=5) {

            s32 centreX = rudderIndicatorCentre.X;
            s32 centreY = rudderIndicatorCentre.Y;
            s32 radius = 0.25*sh;

            //Draw compass bearings
            f32 xVector = sin(i*core::DEGTORAD*angleScale);
            f32 yVector = cos(i*core::DEGTORAD*angleScale);

            //set scale of line
            f32 lineEnd;
            bool printAngle = false;
            if (i%10==0) {
                lineEnd = 1.1;
                printAngle = true;
            } else if (i%5==0) {
                lineEnd = 1.05;
            }

            s32 startX = centreX + xVector*radius;
            s32 endX = centreX + lineEnd*xVector*radius;
            s32 startY = centreY + yVector*radius;
            s32 endY = centreY + lineEnd*yVector*radius;

            //Set colour
            video::SColor indicatorColour;
            if (i<0) {
                indicatorColour = video::SColor(255,175,0,0);
            } else if (i>0) {
                indicatorColour = video::SColor(255,0,175,0);
            } else {
                indicatorColour = video::SColor(255,0,0,0);
            }

            driver->draw2DLine(core::vector2d<s32>(startX,startY),core::vector2d<s32>(endX,endY),indicatorColour);
            if (printAngle) {

                core::stringw text;
                text = core::stringw(abs(i));

                s32 textWidth = guienv->getSkin()->getFont()->getDimension(text.c_str()).Width;
                s32 textHeight = guienv->getSkin()->getFont()->getDimension(text.c_str()).Height;
                s32 textStartX = centreX + 1.2*xVector*radius-0.5*textWidth;
                s32 textEndX = textStartX+textWidth;
                s32 textStartY = centreY + 1.2*yVector*radius-0.5*textHeight;
                s32 textEndY = textStartY+textHeight;
                guienv->getSkin()->getFont()->draw(text,core::rect<s32>(textStartX,textStartY,textEndX,textEndY),indicatorColour);
            }
        }
        //End of draw scale

    } else {

        //hide choice buttons
        headingButton->setVisible(false);
        repeaterButton->setVisible(false);
        messageText->setVisible(false);

        //draw compass rose
        s32 centreX = device->getVideoDriver()->getScreenSize().Width/2;
        s32 centreY = device->getVideoDriver()->getScreenSize().Height/2;
        s32 radius = device->getVideoDriver()->getScreenSize().Height*0.45;

        video::IVideoDriver* driver = device->getVideoDriver();

        driver->draw2DPolygon(core::vector2d<s32>(centreX,centreY),radius,video::SColor(255,255,255,255),60);

        for(unsigned int i = 0; i<360; i++) {
            //Draw compass bearings
            f32 xVector = sin(core::degToRad(i-ownShipHeading));
            f32 yVector = -1*cos(core::degToRad(i-ownShipHeading));

            //set scale of line
            f32 lineStart;
            bool printAngle = false;
            bool printText = true;
            if (i%45==0) {
                lineStart = 0.7;
                printText=true;
            } else if (i%10==0) {
                lineStart = 0.85;
                printAngle = true;
            } else if (i%5==0) {
                lineStart = 0.88;
            } else {
                lineStart = 0.9;
            }

            s32 startX = centreX + lineStart*xVector*radius;
            s32 endX = centreX + 1.0*xVector*radius;
            s32 startY = centreY + lineStart*yVector*radius;
            s32 endY = centreY + 1.0*yVector*radius;

            driver->draw2DLine(core::vector2d<s32>(startX,startY),core::vector2d<s32>(endX,endY),video::SColor(255,255,255,255));
            if (printAngle || printText) {

                core::stringw text;
                if (printAngle) {
                    text = core::stringw(i);
                } else {
                    if (i==0) {
                        text = L"N";
                    } else if (i==90) {
                        text = L"E";
                    } else if (i==180) {
                        text = L"S";
                    } else if (i==270) {
                        text = L"W";
                    }
                     else {
                        text = L"";
                    }
                }

                s32 textWidth = guienv->getSkin()->getFont()->getDimension(text.c_str()).Width;
                s32 textHeight = guienv->getSkin()->getFont()->getDimension(text.c_str()).Height;
                s32 textStartX = centreX + 0.8*xVector*radius-0.5*textWidth;
                s32 textEndX = textStartX+textWidth;
                s32 textStartY = centreY + 0.8*yVector*radius-0.5*textHeight;
                s32 textEndY = textStartY+textHeight;
                guienv->getSkin()->getFont()->draw(text,core::rect<s32>(textStartX,textStartY,textEndX,textEndY),video::SColor(255,255,255,255));
            }
        }


        //Draw heading indicator
        driver->draw2DLine(core::vector2d<s32>(centreX,centreY),core::vector2d<s32>(centreX,centreY-1.1*radius),video::SColor(255,255,255,255));
    }
    guienv->drawAll();

}

void GUIMain::setMode(bool headingMode)
{
    modeChosen=true;
    showHeadingIndicator=headingMode;
}

std::wstring GUIMain::f32To1dp(irr::f32 value)
{
    //Convert a floating point value to a wstring, with 1dp
    char tempStr[100];
    snprintf(tempStr,100,"%.1f",value);
    return std::wstring(tempStr, tempStr+strlen(tempStr));
}

std::wstring GUIMain::f32To3dp(irr::f32 value)
{
    //Convert a floating point value to a wstring, with 3dp
    char tempStr[100];
    snprintf(tempStr,100,"%.3f",value);
    return std::wstring(tempStr, tempStr+strlen(tempStr));
}
