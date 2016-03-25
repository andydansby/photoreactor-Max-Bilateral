// plugin.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "IPlugin.h"

#include <math.h>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>//to use cout


using namespace std;


////////////////////////////////////////////////////////////////////////
// A concrete plugin implementation
////////////////////////////////////////////////////////////////////////

// Photo-Reactor Plugin class

//****************************************************************************
//This code has been generated by the Mediachance photo reactor Code generator.


#define AddParameter(N,S,V,M1,M2,T,D) {strcpy (pParameters[N].m_sLabel,S);pParameters[N].m_dValue = V;pParameters[N].m_dMin = M1;pParameters[N].m_dMax = M2;pParameters[N].m_nType = T;pParameters[N].m_dSpecialValue = D;}

#define GetValue(N) (pParameters[N].m_dValue)
#define GetValueY(N) (pParameters[N].m_dSpecialValue)

#define SetValue(N,V) {pParameters[N].m_dValue = V;}

#define GetBOOLValue(N) ((BOOL)(pParameters[N].m_dValue==pParameters[N].m_dMax))

// if it is not defined, then here it is
//#define RGB(r,g,b) ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))

#define PARAM_RADIUS	0
#define PARAM_SIGMA_C	1

#define NUMBER_OF_USER_PARAMS 2

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))


class Plugin1 : public IPlugin	
{
public:

		//Plugin Icon:
	//you can add your own icon by creating 160x100 png file, naming it the same as plugin dll and then placing it in the plugins folder
	//otherwise a generic icon will be use


	//this is the title of the box in workspace. it should be short
	const char* GetTitle () const
	{
		return "Max Bilateral";
	}
	
	// this will appear in the help pane, you can put your credits and short info
	const char* GetDescription () const
	{
		return "Andy Dansby Max Seperable Bilateral smooth.";
	}

	// BASIC PARAMETERS
	// number of inputs 0,1 or 2
	int GetInputNumber ()
	{
		return 1;
	}

	// number of outputs 0 or 1
	int GetOutputNumber ()
	{
		return 1;
	}

	int GetBoxColor ()
	{
		return RGB(44,78,119);
	}

	int GetTextColor ()
	{
		return RGB(165,236,255);
	}

	// width of the box in the workspace
	// valid are between 50 and 100
	int GetBoxWidth ()
	{
		return 100;
	}

	// set the flags
	// see the interface builder
	// ex: nFlag = FLAG_FAST_PROCESS | FLAG_HELPER;

	//FLAG_NONE same as zero	Default, no other flags set
	//FLAG_UPDATE_IMMEDIATELY	It is very fast process that can update immediately. When user turns the sliders on UI the left display will update
	//							Use Update Immediately only for fast and single loop processes, for example Desaturate, Levels.
	//FLAG_HELPER				It is an helper object. Helper objects will remain visible in Devices and they can react to mouse messages. Example: Knob, Monitor, Bridge Pin
	//FLAG_BINDING				Binding object, attach to other objects and can change its binding value. It never goes to Process_Data functions.  Example: Knob, Switch, Slider
	//FLAG_DUMMY				It is only for interface but never process any data. Never goes to Process_Data functions. Example: Text note
	//FLAG_SKIPFINAL			Process data only during designing, doesn't process during final export. Example: Monitor, Vectorscope 
	//FLAG_LONGPROCESS			Process that takes > 1s to finish. Long Process will display the Progress dialog and will prevent user from changing values during the process.
	//FLAG_NEEDSIZEDATA		    Process need to know size of original image, the zoom and what part of image is visible in the preview. When set the plugin will receive SetSizeData
	//FLAG_NEEDMOUSE			Process will receive Mouse respond data from the workplace. This is only if your object is interactive, for example Knob, Slider

	int GetFlags ()
	{
		// it is fast process
		int nFlag = FLAG_LONGPROCESS;
		//int nFlag = FLAG_NONE;// placed temp
		
		return nFlag;
	}


	// User Interface Build
	// there is maximum 29 Parameters

	int GetUIParameters (UIParameters* pParameters)
	{

		// label, value, min, max, type_of_control, special_value
		// use the UI builder in the software to generate this


		AddParameter( PARAM_RADIUS ,"Radius", 5.0, 1.0, 255.0, TYPE_SLIDER, 0.0);
		AddParameter( PARAM_SIGMA_C ,"Color Sigma", 0.25, 0.1, 5.0, TYPE_SLIDER, 0.0);

		return NUMBER_OF_USER_PARAMS;
	}
	


	virtual void Process_Data (BYTE* pBGRA_out,BYTE* pBGRA_in, int nWidth, int nHeight, UIParameters* pParameters)
	{
		//List of Parameters
		double dRadius = GetValue(PARAM_RADIUS);// used to grab radius from control
		double dSigmaC = GetValue(PARAM_SIGMA_C);// used to grab radius from control

		int radius = (int)dRadius;//convert radius to an interger
		int size = (2 * radius) + 1;// we want an odd sized kernel

		int sigma = 2 * radius;

		int halfsize = size / 2;//int halfsize = radius + 1;//

		BYTE* verticaltemp=new BYTE[nWidth * nHeight * 4]; // this is our tempory buffer to store image blurred vertically
		BYTE* horizonaltemp=new BYTE[nWidth * nHeight * 4];// this is our tempory buffer to store image blurred horizonally
		 
		double red;
		double green;
		double blue;
		//float alpha;

		double colorspace = 255.0f;

		double sum;

		double redsum;
		double greensum;
		double bluesum;
		//double alphasum;


		#pragma region		//blurs vertically
		for(int x = 0; x < nWidth; x++)
		{
			for(int y = 0; y < nHeight; y++)
			{
				//clear your variables to process the pixel
				sum = 0;
				redsum = 0;
				greensum = 0;
				bluesum = 0;

				red = 0;//red
				green = 0;//green
				blue = 0;//blue
				// alpha = 0;//alpha

				//This is our Kernel
				//accumulate colors
				for(int i = max(0, y - size); i <= MIN(nHeight - 1,y + size); i++)
				{
					float factor = expf(-(i - y) * (i - y) / (sigma));					
					sum += factor;

					// this slides / moves along the kernel to collect the neighboring pixels
					double redKernelSlider   = pBGRA_in[(x + i * nWidth) * 4 + CHANNEL_R] / colorspace;
					double greenKernelSlider = pBGRA_in[(x + i * nWidth) * 4 + CHANNEL_G] / colorspace;
					double blueKernelSlider  = pBGRA_in[(x + i * nWidth) * 4 + CHANNEL_B] / colorspace;
					//this slides / moves along the kernel to collect the neighboring pixels

					// this stays in center of kernel
					double redKernelCenter   = pBGRA_in[(x + y * nWidth) * 4 + CHANNEL_R] / colorspace;
					double greenKernelCenter = pBGRA_in[(x + y * nWidth) * 4 + CHANNEL_G] / colorspace;
					double blueKernelCenter  = pBGRA_in[(x + y * nWidth) * 4 + CHANNEL_B] / colorspace;
					// this stays in center of kernel

					double redDiff   =  abs(redKernelSlider - redKernelCenter);
					double greenDiff =  abs(greenKernelSlider - greenKernelCenter);
					double blueDiff  =  abs(blueKernelSlider - blueKernelCenter);

					//double RcolorDist = max (0,  min(1.0f, ((dSigmaC * dSigmaC) * 0.5) / ((redDiff * redDiff) * dRadius) ));
					//double GcolorDist = max (0,  min(1.0f, ((dSigmaC * dSigmaC) * 0.5) / ((greenDiff * greenDiff) * dRadius)));
					//double BcolorDist = max (0,  min(1.0f, ((dSigmaC * dSigmaC) * 0.5) / ((blueDiff * blueDiff) * dRadius)));

					double RcolorDist = max (0,  min(1.0f, ((dSigmaC * dSigmaC) * 0.5) / ((redDiff * redDiff) * halfsize) ));
					double GcolorDist = max (0,  min(1.0f, ((dSigmaC * dSigmaC) * 0.5) / ((greenDiff * greenDiff) * halfsize)));
					double BcolorDist = max (0,  min(1.0f, ((dSigmaC * dSigmaC) * 0.5) / ((blueDiff * blueDiff) * halfsize)));
					
					{
						red   += factor * RcolorDist * pBGRA_in[(x + i * nWidth) * 4 + CHANNEL_R];
						green += factor * GcolorDist * pBGRA_in[(x + i * nWidth) * 4 + CHANNEL_G];
						blue  += factor * BcolorDist * pBGRA_in[(x + i * nWidth) * 4 + CHANNEL_B];
					}
				}//end I

				redsum = red / sum;
				greensum = green / sum;
				bluesum = blue / sum;

				{//output					
						verticaltemp[(x + y * nWidth) * 4 + CHANNEL_R] = redsum;
						verticaltemp[(x + y * nWidth) * 4 + CHANNEL_G] = greensum;
						verticaltemp[(x + y * nWidth) * 4 + CHANNEL_B] = bluesum;
					//	pBGRA_out[(x + y * nWidth) * 4 + CHANNEL_A] = alpha;				
				}//end output


			}//end Y
		}//end X
	#pragma endregion

	#pragma region			//test for proof that the image is in temp and blurred horizonally



		/*
		for (int x = 0; x< nWidth; x++)
		{
			for (int y = 0; y< nHeight; y++)
			{
				int nIdx = x * 4 + y * 4 * nWidth;
				
				int nR = verticaltemp [nIdx + CHANNEL_R];
				int nG = verticaltemp [nIdx + CHANNEL_G];
				int nB = verticaltemp [nIdx + CHANNEL_B];

			//	pBGRA_out[nIdx + CHANNEL_R] = nR;
			//	pBGRA_out[nIdx + CHANNEL_G] = nG;
			//	pBGRA_out[nIdx + CHANNEL_B] = nB;				
			}
		}
		*/
	#pragma endregion

		//blurs horizonally
		for(int y = 0; y < nHeight; y++)
		{
			for(int x = 0; x < nWidth; x++)
			{
				//clear your variables to process the pixel
				sum = 0;
				redsum = 0;
				greensum = 0;
				bluesum = 0;

				red = 0;//red
				green = 0;//green
				blue = 0;//blue
				// alpha = 0;//alpha

				//This is our Kernel
				//accumulate colors
				for(int i = max(0, x - size); i <= MIN(nWidth - 1, x + size); i++)
				{
					float factor = expf(-(i - x) * (i - x) / (sigma));
					sum += factor;

					//this slides / moves along the kernel to collect the neighboring pixels
					double redKernelSlider   = pBGRA_in[(i + y * nWidth) * 4 + CHANNEL_R] / colorspace;
					double greenKernelSlider = pBGRA_in[(i + y * nWidth) * 4 + CHANNEL_G] / colorspace;
					double blueKernelSlider  = pBGRA_in[(i + y * nWidth) * 4 + CHANNEL_B] / colorspace;
					//this slides / moves along the kernel to collect the neighboring pixels

					// this stays in center of kernel
					double redKernelCenter   = pBGRA_in[(x + y * nWidth) * 4 + CHANNEL_R] / colorspace;
					double greenKernelCenter = pBGRA_in[(x + y * nWidth) * 4 + CHANNEL_G] / colorspace;
					double blueKernelCenter  = pBGRA_in[(x + y * nWidth) * 4 + CHANNEL_B] / colorspace;
					// this stays in center of kernel

					double redDiff   =  abs(redKernelSlider - redKernelCenter);
					double greenDiff =  abs(greenKernelSlider - greenKernelCenter);
					double blueDiff  =  abs(blueKernelSlider - blueKernelCenter);

					double RcolorDist = max (0,  min(1.0f, ((dSigmaC * dSigmaC) * 0.5) / ((redDiff * redDiff) * halfsize) ));
					double GcolorDist = max (0,  min(1.0f, ((dSigmaC * dSigmaC) * 0.5) / ((greenDiff * greenDiff) * halfsize)));
					double BcolorDist = max (0,  min(1.0f, ((dSigmaC * dSigmaC) * 0.5) / ((blueDiff * blueDiff) * halfsize)));

					{
						red   += factor * (RcolorDist) * pBGRA_in[(i + y * nWidth) * 4 + CHANNEL_R];
						green += factor * (GcolorDist) * pBGRA_in[(i + y * nWidth) * 4 + CHANNEL_G];
						blue  += factor * (BcolorDist) * pBGRA_in[(i + y * nWidth) * 4 + CHANNEL_B];
					}
				}//end I

				redsum = red / sum;
				greensum = green / sum;
				bluesum = blue / sum;
				//alphasum = alpha / sum;

				{//output
					horizonaltemp[(x + y * nWidth) * 4 + CHANNEL_R] = redsum;
					horizonaltemp[(x + y * nWidth) * 4 + CHANNEL_G] = greensum;
					horizonaltemp[(x + y * nWidth) * 4 + CHANNEL_B] = bluesum;
				}//end output
			}//end X
		}//end Y

	#pragma region			//test for proof that the image is in temp2 and blurred vertically
		/*
		for (int x = 0; x< nWidth; x++)
		{
			for (int y = 0; y< nHeight; y++)
			{
				int nIdx = x * 4 + y * 4 * nWidth;
				
				int nR = horizonaltemp [nIdx + CHANNEL_R];
				int nG = horizonaltemp [nIdx + CHANNEL_G];
				int nB = horizonaltemp [nIdx + CHANNEL_B];

				//pBGRA_out[nIdx + CHANNEL_R] = nR;
				//pBGRA_out[nIdx + CHANNEL_G] = nG;
				//pBGRA_out[nIdx + CHANNEL_B] = nB;				
			}
		}
		*/
	#pragma endregion

		//now we are going to combine the two matrix to produce an output image
		// this will obtain the max of horizontal temp and vertical temp
		for (int x = 0; x< nWidth; x++)
		{
			for (int y = 0; y< nHeight; y++)
			{
				int nIdx = x * 4 + y * 4 * nWidth;
				
				int horizR = horizonaltemp [nIdx + CHANNEL_R];
				int horizG = horizonaltemp [nIdx + CHANNEL_G];
				int horizB = horizonaltemp [nIdx + CHANNEL_B];

				int vertR = verticaltemp [nIdx + CHANNEL_R];
				int vertG = verticaltemp [nIdx + CHANNEL_G];
				int vertB = verticaltemp [nIdx + CHANNEL_B];

				pBGRA_out[nIdx + CHANNEL_R] = max (horizR,vertR);
				pBGRA_out[nIdx + CHANNEL_G] = max (horizG,vertG);
				pBGRA_out[nIdx + CHANNEL_B] = max (horizB,vertB);
			}
		}

		delete [] verticaltemp;//delete the vertical array
		delete [] horizonaltemp;//delete the horizontal array
	}//end routine

						


	// actual processing function for 2 inputs
	//********************************************************************************
	// all buffers are the same size
	// don't change the IN buffers or things will go bad
	// the pBGRA_out comes already with copied data from pBGRA_in1
	virtual void Process_Data2 (BYTE* pBGRA_out, BYTE* pBGRA_in1, BYTE* pBGRA_in2, int nWidth, int nHeight, UIParameters* pParameters)
	{

	}


	//*****************Drawing functions for the BOX *********************************
	//how is the drawing handled
	//DRAW_AUTOMATICALLY	the main program will fully take care of this and draw a box, title, socket and thumbnail
	//DRAW_SIMPLE_A			will draw a box, title and sockets and call CustomDraw
	//DRAW_SIMPLE_B			will draw a box and sockets and call CustomDraw
	//DRAW_SOCKETSONLY      will call CustomDraw and then draw sockets on top of it
	
	// highlighting rectangle around is always drawn except for DRAW_SOCKETSONLY

	virtual int GetDrawingType ()
	{

		int nType = DRAW_AUTOMATICALLY;

		return nType;

	}


	// Custom Drawing
	// custom drawing function called when drawing type is different than DRAW_AUTOMATICALLY
	// it is not always in real pixels but scaled depending on where it is drawn
	// the scale could be from 1.0 to > 1.0
	// so you always multiply the position, sizes, font size, line width with the scale
	
	virtual void CustomDraw (HDC hDC, int nX,int nY, int nWidth, int nHeight, float scale, BOOL bIsHighlighted, UIParameters* pParameters)
	{
	}


	//************ Optional Functions *****************************************************************************************
	// those functions are not necessary for normal effect, they are mostly for special effects and objects


	// Called when FLAG_HELPER set. 
	// When UI data changed (user turned knob) this function will be called as soon as user finish channging the data
	// You will get the latest parameters and also which parameter changed
	// Normally for effects you don't have to do anything here because you will get the same parameters in the process function
	// It is only for helper objects that may not go to Process Data 
	BOOL UIParametersChanged (UIParameters* pParameters, int nParameter)
	{
		
		return FALSE;
	}

	// when button is pressed on UI, this function will be called with the parameter and sub button (for multi button line)
	BOOL UIButtonPushed (int nParam, int nSubButton, UIParameters* pParameters)
	{

		return TRUE;
	}


	// Called when FLAG_NEEDSIZEDATA set
	// Called before each calculation (Process_Data)
	// If your process depends on a position on a frame you may need the data to correctly display it because Process_Data receives only a preview crop
	// Most normal effects don't depend on the position in frame so you don't need the data
	// Example: drawing a circle at a certain position requires to know what is displayed in preview or the circle will be at the same size and position regardless of zoom
	
	// Note: Even if you need position but you don't want to mess with the crop data, just ignore it and pretend the Process_Data are always of full image (they are not). 
	// In worst case this affects only preview when using zoom. The full process image always sends the whole data

	// nOriginalW, nOriginalH - the size of the original - full image. If user sets Resize on input - this will be the resized image
	// nPreviewW, nPreviewH   - this is the currently processed preview width/height - it is the same that Process_Data will receive
	//                        - in full process the nPreviewW, nPreviewH is equal nOriginalW, nOriginalH
	// Crop X1,Y1,X2,Y2       - relative coordinates of preview crop rectangle in <0...1>, for full process they are 0,0,1,1 (full rectangle)	
	// dZoom                  - Zoom of the Preview, for full process the dZoom = 1.0
	void SetSizeData(int nOriginalW, int nOriginalH, int nPreviewW, int nPreviewH, double dCropX1, double dCropY1, double dCropX2, double dCropY2, double dZoom)
	{

		// so if you need the position and zoom, this is the place to get it.
		// Note: because of IBM wisdom the internal bitmaps are on PC always upside down, but the coordinates are not


	}


	// ***** Mouse handling on workplace *************************** 
	// only if FLAG_NEEDMOUSE is set
	//****************************************************************
	//this is for special objects that need to receive mouse, like a knob or slider on workplace
	// normally you use this for FLAG_BINDING objects

	// in coordinates relative to top, left corner of the object (0,0)
	virtual BOOL MouseButtonDown (int nX, int nY, int nWidth, int nHeight, UIParameters* pParameters)
	{
		
		// return FALSE if not handled
		// return TRUE if handled
		return FALSE;
	}

	// in coordinates relative to top, left corner of the object (0,0)
	virtual BOOL MouseMove (int nX, int nY, int nWidth, int nHeight, UIParameters* pParameters)
	{
	

		return FALSE;
	}
	
	// in coordinates relative to top, left corner of the object (0,0)
	virtual BOOL MouseButtonUp (int nX, int nY, int nWidth, int nHeight, UIParameters* pParameters)
	{
		
		// Note: if we changed data and need to recalculate the flow we need to return TRUE

		// return FALSE if not handled
		// return TRUE if handled
		
		return TRUE;
	}


};

extern "C"
{
	// Plugin factory function
	__declspec(dllexport) IPlugin* Create_Plugin ()
	{
		//allocate a new object and return it
		return new Plugin1 ();
	}
	
	// Plugin cleanup function
	__declspec(dllexport) void Release_Plugin (IPlugin* p_plugin)
	{
		//we allocated in the factory with new, delete the passed object
		delete p_plugin;
	}
	
}


// this is the name that will appear in the object library
extern "C" __declspec(dllexport) char* GetPluginName()
{
	return "Andys Bilateral";	
}


// This MUST be unique string for each plugin so we can save the data

extern "C" __declspec(dllexport) char* GetPluginID()
{
	
	
	return "com.lumafilters.maxseperablebilateral";
	
}


// category of plugin, for now the EFFECT go to top library box, everything else goes to the middle library box
extern "C" __declspec(dllexport) int GetCategory()
{
		
	return CATEGORY_EFFECT;
	
}