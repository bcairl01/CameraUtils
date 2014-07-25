/// @file  	CU_ColorTracking.hpp
/// @brief 	Simple blob tracking library 		
///
///			Dependencies:
///			=============
///			- unistd.h
///			- boost	
///			- OpenCV
///
///	@author Brian Cairl
/// @date 	July, 2014 [rev:1.0]
///
#ifndef CU_ColorTracking
#define CU_ColorTracking

	/// C-STD
	#include <stdio.h>  
	#include <stdlib.h>  

	/// CXX-STD
	#include <iostream>
	#include <cmath>

	/// OpenCV
	#include "opencv/cv.h"  
	#include "opencv2/core/core_c.h"  
	#include "opencv2/highgui/highgui_c.h"  


	namespace CamU
	{
		class Color;
		class ColorTracker;

		typedef uint8_t Pixel[3UL];
		typedef float(*MatchMethod)(Pixel&,Color&,void*&);


		/// IPL Macros
		#define LC_IPL_R 							2UL
		#define LC_IPL_G							1UL
		#define LC_IPL_B							0UL
		#define LC_IPL_ITR(img,i,j,c) 				img->imageData[i*(img->nChannels*img->width) + j*img->nChannels + c]
		#define LC_IPL_PXL(img,i,j) 				(Pixel)(img->imageData + i*(img->nChannels*img->width) + j*img->nChannels)



		namespace MatchMethods
		{
		float linear( Pixel& _pxl, Color& target, void*& params );
		float poly  ( Pixel& _pxl, Color& target, void*& params );
		float fuzzy ( Pixel& _pxl, Color& target, void*& params );
		float thresh( Pixel& _pxl, Color& target, void*& params );
		}


		class Color
		{
		friend std::ostream& operator<<( std::ostream& os, const Color& color );
		public:
			uint8_t			bgr_u08[3UL];
			float 			bgr_f32[3UL];

			Color();
			Color( const uint8_t R08, const uint8_t G08, const uint8_t B08 );
			Color( const float   R32, const float   G32, const float   B32 );
			Color( Pixel& _pxl );
			
			virtual ~Color()
			{}
		};


		class ColorTracker
		{
		friend std::ostream& operator<<( std::ostream& os, const ColorTracker& color_tracker );
		private:
			cv::Point2f		pt;
			float 			saturation;
			float 			gain;
		public:
			void process( IplImage*& _ipl, MatchMethod _mfn, Color& target, void* params = NULL );
			
			ColorTracker( const float _gain ) :
				saturation(0.0f),
				gain(_gain)
			{
				pt.x = pt.y = 0.0f;
			}

			~ColorTracker()
			{}
		};


	}

#endif