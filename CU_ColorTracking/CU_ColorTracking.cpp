#include <CU_ColorTracking.hpp>

namespace CamU
{

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// COLOR
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////


	std::ostream& operator<<( std::ostream& os, const Color& color )
	{
		os  << "<Color>\t"; 

		os 	<< "<F32>\t" 
			<< bgr_f32[LC_IPL_R] << '\t' 
			<< bgr_f32[LC_IPL_G] << '\t' 
			<< bgr_f32[LC_IPL_B] << '\t'
			<< "</F32>\t";

		os 	<< "<U08>\t" 
			<< bgr_u08[LC_IPL_R] << '\t' 
			<< bgr_u08[LC_IPL_G] << '\t' 
			<< bgr_u08[LC_IPL_B] << '\t'
			<< "</U08>\t";

		os  << "</Color>"; 
		return os;
	}



	Color::Color()
	{
		bgr_u08[LC_IPL_R]=bgr_u08[LC_IPL_G]=bgr_u08[LC_IPL_B]=0UL;
		bgr_f32[LC_IPL_R]=bgr_f32[LC_IPL_G]=bgr_f32[LC_IPL_B]=0.0f;
	}



	Color::Color( const uint8_t R08, const uint8_t G08, const uint8_t B08 )
	{
		bgr_u08[LC_IPL_R]=R08;
		bgr_u08[LC_IPL_G]=G08;
		bgr_u08[LC_IPL_B]=B08;

		bgr_f32[LC_IPL_R]=((float)R08)/255.0f;
		bgr_f32[LC_IPL_G]=((float)G08)/255.0f;
		bgr_f32[LC_IPL_B]=((float)B08)/255.0f;
	}



	Color::Color( const float   R32, const float   G32, const float   B32 )
	{
		bgr_f32[LC_IPL_R]=R08;
		bgr_f32[LC_IPL_G]=G08;
		bgr_f32[LC_IPL_B]=B08;

		bgr_u08[LC_IPL_R]=(uint8_t)(R08*255.0f);
		bgr_u08[LC_IPL_G]=(uint8_t)(G08*255.0f);
		bgr_u08[LC_IPL_B]=(uint8_t)(B08*255.0f);
	}



	Color::Color( Pixel* _pxl )
	{
		bgr_u08[LC_IPL_R]=_pxl[LC_IPL_R];
		bgr_u08[LC_IPL_G]=_pxl[LC_IPL_G];
		bgr_u08[LC_IPL_B]=_pxl[LC_IPL_B];

		bgr_f32[LC_IPL_R]=((float)R08)/255.0f;
		bgr_f32[LC_IPL_G]=((float)G08)/255.0f;
		bgr_f32[LC_IPL_B]=((float)B08)/255.0f;
	}


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// MATCH-METHODS
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////


	namespace MatchMethods
	{
		float thresh( Pixel& _pxl, Color& target, void*& params )
		{
			Color cpxl(_pxl);
			float thresh[3UL] = {0.1,0.1,0.1};
			
			if(params!=NULL)
			{
				for(size_t idx = 0; idx < 3UL; idx++)
					thresh[idx] = ((float*)params)[idx];
			}

			for( size_t idx = 0; idx < 3UL; idx++ )
			{
				if( fabs(cpxl.bgr_f32[idx] - target.bgr_f32[idx]) > thresh[idx] )
					return 0.0f;
			}
			return 1.0f;
		} 


		float linear( Pixel& _pxl, Color& target, void*& params )
		{
			Color cpxl(_pxl);
			float diff = 0.0f;
			for( size_t idx = 0; idx < 3UL; idx++ )
				diff += fabs(cpxl.bgr_f32[idx] - target.bgr_f32[idx])/3UL;

			return float;
		}


		float poly  ( Pixel& _pxl, Color& target, void*& params )
		{
			float order = 2.0f;

			if(params!=NULL)
				order = *((float*)params);
			
			return powf(linear(_pxl,target),order);
		}



		float fuzzy ( Pixel& _pxl, Color& target, void*& params )
		{
			float center 		= 0.5f;
			float temperature	= 1.0f;

			if(params!=NULL)
			{
				center 		= *((float*)params+0UL);
				temperature = *((float*)params+1UL);
			}

			float match_b = linear(_pxl,target)-center;
			return 0.5f + 0.5f/(1+expf(match_b*temperature));
		}
	}		


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// COLOR TRACKER
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////


	std::ostream& operator<<( std::ostream& os, const ColorTracker& color_tracker )
	{
		os  << "<ColorTracker>\t"; 

		os 	<< "<Point>\t" 
			<< pt.x <<'\t' 
			<< pt.y <<'\t' 
			<< "</Point>\t";

		os 	<< "<Saturation>\t" 
			<< saturation<<'\t' 
			<< "</Saturation>\t";

		os  << "</ColorTracker>"; 
		return os;
	}


	void ColorTracker::process( IplImage*& _ipl, MatchMethod _mfn, Color& target, void* params = NULL )
	{
		cv::Point2f u_pt; 
		float 	 	u_sat;
		float 		match;
		float 		denom = (_ipl->height * _ipl->width);

		u_pt.x = u_pt.y = 0.0f;

		/// Get Mean Location with provide matching function
		for(size_t i = 0; i < _ipl->height; i++)
		{
			for(size_t j = 0; j < _ipl->width; j++)
			{
				match = _mfn(LC_IPL_PXL(_ipl,i,j),target,params);

				u_pt.x 	+= (float)j;
				u_pt.y 	+= (float)i;
				u_sat	+=	match;
			}
		}
		u_pt.x/=denom;
		u_pt.y/=denom;
		u_sat /=denom;

		/// Update Track-Point
		pt.x 		+= gain*( u_pt.x - pt.x );
		pt.y 		+= gain*( u_pt.y - pt.y );
		saturation 	+= gain*( u_sat  - saturation );
	}

}

