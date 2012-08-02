// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Jonas Ruesch
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <iCub/CamCalibModule.h>

CamCalibPort::CamCalibPort()
{
    portImgOut=NULL;
    calibTool=NULL;

    verbose=false;
    t0=Time::now();
}

void CamCalibPort::setPointers(yarp::os::Port *_portImgOut, ICalibTool *_calibTool)
{
    portImgOut=_portImgOut;
    calibTool=_calibTool;
}

void CamCalibPort::setSaturation(double satVal)
{
    currSat = satVal;
}

void CamCalibPort::onRead(ImageOf<PixelRgb> &yrpImgIn)
{
    double t=Time::now();
    int temp = 0;
    // execute calibration
    if (portImgOut!=NULL)
    {        
        yarp::sig::ImageOf<PixelRgb> yrpImgOut;

        if (verbose)
            fprintf(stdout,"received input image after %g [s] ... ",t-t0);

        double t1=Time::now();

        if (calibTool!=NULL)
        {
            calibTool->apply(yrpImgIn,yrpImgOut);
            
            //IplImage *cv_img = cvCreateImage( cvSize(yrpImgOut.width(), yrpImgOut.height()), IPL_DEPTH_8U, 3 );
            //cvCvtColor((IplImage*)yrpImgOut.getIplImage(), cv_img, CV_RGB2BGR);
            //cvScale(cv_img, cv_img, currSat);
           
            cv::Mat cv_img (yrpImgOut.height(), yrpImgOut.width(), CV_8UC3);
            cv::Scalar scale(currSat*255.0);
            cv::Mat cv_sMod (yrpImgOut.height(), yrpImgOut.width(), CV_8UC1, scale);
 
            vector<cv::Mat> planes;
            cv::cvtColor( cv::Mat((IplImage*)yrpImgOut.getIplImage()), cv_img, CV_RGB2HSV);
            cv::split(cv_img, planes);
            //planes[1]=cv_sMod; //dont just add the values

            //cv::Mat res(yrpImgOut.height(), yrpImgOut.width(), CV_8UC1);
            //cv::gemm(planes[1], cv_sMod, 1.0, cv::Mat(), 0.0, res);
            
            for (int c =0; c <planes[1].cols; c++)
            {
                for (int r=0; r<planes[1].rows; r++)
                {
                    double to_be_saturated=planes[1].at<uchar>(r,c) * currSat;
                    //fprintf(stdout,"dgb1 %d %lf\n", planes[1].at<uchar>(r,c), to_be_saturated);
                    if(to_be_saturated<0.0)
                        to_be_saturated=0.0;
                    if(to_be_saturated>255.0)
                        to_be_saturated=255.0;
                        
                    planes[1].at<uchar>(r,c)=to_be_saturated;
                }
            }
            //fprintf(stdout,"dgb1\n");
            //planes[1] = res;
            cv::merge(planes,cv_img);
            //fprintf(stdout,"dgb2\n");
            cv::cvtColor( cv_img, cv_img, CV_HSV2RGB);
            //fprintf(stdout,"dgb3\n");
            
            IplImage test = cv_img;
            
            ImageOf<PixelRgb> yarpImg;
            yarpImg.resize(test.width,test.height);
            cvCopyImage(&test, (IplImage*)yarpImg.getIplImage());
            yrpImgOut.zero();
            yrpImgOut = yarpImg;
            
            if (verbose)
                fprintf(stdout,"calibrated in %g [s]\n",Time::now()-t1);
        }
        else
        {
            yrpImgOut=yrpImgIn;

            if (verbose)
                fprintf(stdout,"just copied in %g [s]\n",Time::now()-t1);
        }

        // timestamp propagation
        yarp::os::Stamp stamp;
        BufferedPort<ImageOf<PixelRgb> >::getEnvelope(stamp);
        portImgOut->setEnvelope(stamp);

        portImgOut->write(yrpImgOut);
    }

    t0=t;
}

CamCalibModule::CamCalibModule(){

    _calibTool = NULL;	
}

CamCalibModule::~CamCalibModule(){

}

bool CamCalibModule::configure(yarp::os::ResourceFinder &rf){

	ConstString str = rf.check("name", Value("/camCalib"), "module name (string)").asString();
	setName(str.c_str()); // modulePortName  

    // pass configuration over to bottle
    Bottle botConfig(rf.toString().c_str());
    botConfig.setMonitor(rf.getMonitor());		
    // Load from configuration group ([<group_name>]), if group option present
    Value *valGroup; // check assigns pointer to reference
    if(botConfig.check("group", valGroup, "Configuration group to load module options from (string).")){
        string strGroup = valGroup->asString().c_str();        
        // is group a valid bottle?
        if (botConfig.check(strGroup.c_str())){            
            Bottle &group=botConfig.findGroup(strGroup.c_str(),string("Loading configuration from group " + strGroup).c_str());
            botConfig.fromString(group.toString());
        }
        else{
            cout << endl << "Group " << strGroup << " not found." << endl;
            return false;
        }
    }

    string calibToolName = botConfig.check("projection",
                                         Value("pinhole"),
                                         "Projection/mapping applied to calibrated image [projection|spherical] (string).").asString().c_str();

    _calibTool = CalibToolFactories::getPool().get(calibToolName.c_str());
    if (_calibTool!=NULL) {
        bool ok = _calibTool->open(botConfig);
        if (!ok) {
            delete _calibTool;
            _calibTool = NULL;
            return false;
        }
    }

    if (yarp::os::Network::exists(getName("/in")))
    {
        cout << "====> warning: port " << getName("/in") << " already in use" << endl;
    }
    if (yarp::os::Network::exists(getName("/out")))
    {
        cout << "====> warning: port " << getName("/out") << " already in use" << endl;    
    }
    if (yarp::os::Network::exists(getName("/conf")))
    {
        cout << "====> warning: port " << getName("/conf") << " already in use" << endl;    
    }
    _prtImgIn.setSaturation(rf.check("saturation",Value(1.0)).asDouble());
    _prtImgIn.open(getName("/in"));
    _prtImgIn.setPointers(&_prtImgOut,_calibTool);
    _prtImgIn.setVerbose(rf.check("verbose"));
    _prtImgIn.useCallback();
    _prtImgOut.open(getName("/out"));
    _configPort.open(getName("/conf"));

    attach(_configPort);
	fflush(stdout);

    return true;
}

bool CamCalibModule::close(){
    _prtImgIn.close();
	_prtImgOut.close();
    _configPort.close();
    if (_calibTool != NULL){
        _calibTool->close();
        delete _calibTool;
        _calibTool = NULL;
    }
    return true;
}

bool CamCalibModule::interruptModule(){
    _prtImgIn.interrupt();
	_prtImgOut.interrupt();
    _configPort.interrupt();
    return true;
}

bool CamCalibModule::updateModule(){	
    return true;
}

double CamCalibModule::getPeriod() {
  return 1.0;
}

bool CamCalibModule::respond(const Bottle& command, Bottle& reply) 
{
    reply.clear(); 

    if (command.get(0).asString()=="quit") 
    {
        reply.addString("quitting");
        return false;     
    }
    else if (command.get(0).asString()=="sat" || command.get(0).asString()=="saturation")
    {
        double satVal = command.get(1).asDouble();
        _prtImgIn.setSaturation(satVal);
        reply.addString("ok");
    }
    else
    {
        cout << "command not known - type help for more info" << endl;
    }
    return true;
}


