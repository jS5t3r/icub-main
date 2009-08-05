// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef _BMLEngine_H_
#define _BMLEngine_H_

#include <ace/config.h>

//YARP include
#include <yarp/os/all.h>
#include <yarp/sig/all.h>

//openCV include
#include <cv.h>
#include <cvaux.h>
#include <highgui.h>

//IPP include
#include <ipp.h>

//within Project Include
//#include <iCub/BMLEngine.h>
#include <iCub/MachineBoltzmann.h>
#include <iCub/YarpImgRecv.h>
#include <string>


using namespace std;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::sig::draw;

/**
* This class implements a process able of getting command from a controller 
* interpreting them in term of callings to function of the 
* library BM(BOLZMANN MACHINE LIBRARY)
*
* \author Rea Francesco
*/
class BMLEngine : public Module {
private:
    /** 
	 * input port for possible coming images
	 */
	BufferedPort<ImageOf<PixelRgb> > port; // input port for possible coming images
	/** 
	* output port n�0 for writing image out
	*/
	BufferedPort<ImageOf<PixelRgb> > port0; //port for writing the image out
	/** 
	* output port n�1 for writing image out
	*/
	BufferedPort<ImageOf<PixelRgb> > port1; //port for writing the image out
	/** 
	* output port n�2 for writing image out
	*/
	BufferedPort<ImageOf<PixelRgb> > port2; //port for writing the image out
	/** 
	* output port n�3 for writing image out
	*/
	BufferedPort<ImageOf<PixelRgb> > port3; //port for writing the image out
	/** 
	* port where the commands are vehiculated from controller to engine
	*/
	BufferedPort<yarp::os::Bottle> portCmd;

	BufferedPort<ImageOf<PixelMono>> port_plane; 
	/** 
	* counter for the update step
	*/
    int ct;
	/** 
	* options for the connection
	*/
	Property options;	//options of the connection
	/** 
	* Object that recalls the Boltzmann Machine Library
	*/
	MachineBoltzmann *mb;
	/** 
	* scale factor for the output image representing a layer (X axis)
	*/
	int scaleFactorX;
	/** 
	* scale factor for the output image representing a layer (Y axis)
	*/
	int scaleFactorY;
	/** 
	* sinchronized with the number of layer active
	*/
	int currentLayer;

	int count;  /**counter incremented inside the updateModule**/
	map<std::string,Layer>::iterator iterE;
	map<std::string,Unit>::iterator iterU;
	map<std::string,Connection>::iterator iterC;
	ImageOf<PixelRgb> img_tmp; //=new ImageOf<PixelRgb>;
	ImageOf<PixelRgb> img; //=new ImageOf<PixelRgb>;
	ImageOf<PixelRgb> *img0; //=new ImageOf<PixelRgb>;
	ImageOf<PixelRgb> *img2; //=new ImageOf<PixelRgb>;
	/** 
	* flag that enable the drawing of the layer present in the simulation
	*/
	bool enableDraw;
	/** 
	* flag that regulates the execution of the freely mode
	*/
	bool runFreely;
	/** 
	* flag that regulates the execution of the clamped mode
	*/
	bool runClamped;

	Ipp8u *red_tmp;
	Ipp8u *blue_tmp;
	Ipp8u *green_tmp;
	Ipp8u *im_out;
	Ipp8u* im_tmp[3];
	Ipp8u* im_tmp_tmp;
	int psb;
	/** 
	* value that indicates whether an area can be visually clamped 
	*/
	int clampingThreshold;
public:
	/** 
	* open the ports
	*/
	bool open(Searchable& config); //open the port
	/** 
	* try to interrupt any communication or resource usage
	*/
    bool interruptModule(); // try to interrupt any communications or resource usage
	/** 
	* close all the ports
	*/
	bool close(); //closes all the ports
	/** 
	* active control of the module
	*/
 	bool updateModule(); //active control of the Module
	/**
	* uses a bottle to comunicate a command to the module linked on 
	*/
	bool outCommandPort();
	/**
	* open the port necessary to send commands
	*/
	void openCommandPort();
	/** 
	* set the attribute options of class Property
	*/
	void setOptions(Property options); //set the attribute options of class Property
	/** 
	* function that sets the scaleFactorX
	* @param value new value of the scaleFactorX
	*/
	void setScaleFactorX(int value);
	/** 
	* function that sets the scaleFactorY
	* @param value new value of the scaleFactorY
	*/
	void setScaleFactorY(int value);
	/** 
	* function that set the number of the layer active 
	* @param value number of the layer actually active
	*/
	void setCurrentLayer(int value);
	/** 
	* function that loads the configuration file with a specific filename 
	* param filename name of the file where the configuration is loaded
	* 
	*/
	void loadConfiguration(string filename);
	/** 
	* function that adds a layer to boltzmann machine
	* @param the number of the already istantiated layers
	* @param the number of the columns in the layer
	* @param the number of rows in the layer
	*/
	void BMLEngine::addLayer(int number,int colDimension, int rowDimension);
	/** 
	* function that clamp a Layer mapping an image onto it
	* param layerNumber reference to the layer name
	*/
	void clampLayer(int LayerNumber);
	/** 
	* input Image which is mapped onto the selected layer
	*/
	ImageOf<PixelRgb> *ptr_inputImage;
	/** 
	* input Image which is mapped onto the selected layer
	*/
	ImageOf<PixelRgb> *ptr_inputImage2;
	/** 
	* number of layers already istantiated
	*/
	int countLayer;
	/**
	* Output Port for commands
	*/
	yarp::os::BufferedPort<yarp::os::Bottle> *_pOutPort;
	/**
	* command that is send throgh the command port
	*/
	string *outCommand;
	/**
	*bottle containing the option of the command
	*/
	Bottle bOptions;
	/**
	* flag that tells if the probability of the freely mode has to be calculated
	*/
	bool probFreely_flag;
	/**
	* flag that tells if the probability of the clamped mode has to be calculated
	*/
	bool probClamped_flag;

};

#endif //_BMLEngine_H_