//
// Example code showing how to blend images using OpenCv multiband blender
// (C) Ulrich Hoffmann, 6 July 2016
//
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/stitching/detail/blenders.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>


class SimpleBlender_C 
{
public:
	enum Result_TP {
		OK = 0,
		SIZE_MISMACTH,
		TYPE_MISMATCH
	};
	
	void SetImage1(cv::Mat image_1) { _image_1 = image_1; };
	void SetImage2(cv::Mat image_2) { _image_2 = image_2; };

	Result_TP BlendImages(cv::Mat &destination) const {
		
		Result_TP result = OK;
		
		// check pre-conditions
		if ( _image_1.size().width  != _image_2.size().width ||
			_image_1.size().height != _image_2.size().height ) {
			result = SIZE_MISMACTH;
		}
		if (_image_1.type() != _image_2.type() ) {
			result = TYPE_MISMATCH;
		}
		
		// blend
		if ( result == OK ) {
			
			// compute position where images are blended and width of final image
			int blend_position = static_cast<int>(_image_2.size().width * .5); 
			int blended_width  = blend_position + _image_2.size().width;
			int blended_height = _image_1.size().height;
			
			// setup blender object
			cv::detail::MultiBandBlender blender;
			cv::Rect bounding_box(0, 0, blended_width, blended_height);
			blender.prepare(bounding_box);
			cv::Mat mask = cv::Mat::ones(_image_1.size().height, _image_1.size().width, CV_8U)*255;
			blender.feed(_image_1, mask, cv::Point(0,0));
			blender.feed(_image_2, mask, cv::Point(blend_position, 0));
			
			// do the blending
			cv::Mat dst_mask;
			blender.blend(destination, dst_mask);
		}
		return result;
	
	};
	
private:
	cv::Mat _image_1;
	cv::Mat _image_2;
};



int main(int argc, char** argv)
{
	if (argc != 3) {
		std::cerr << "Please provide two image filenames as arguments" << std::endl;
		return -1;
	}
	
	cv::Mat image_1;
	image_1 = cvLoadImageM(argv[1], CV_LOAD_IMAGE_UNCHANGED);
	if (image_1.data == NULL ) {
		std::cerr << "Cannot read " << argv[1] << std::endl;
		return -1;
	}
	
	cv::Mat image_2;
	image_2 = cvLoadImageM(argv[2], CV_LOAD_IMAGE_UNCHANGED);
	if (image_2.data == NULL ) {
		std::cerr << "Cannot read " << argv[2] << std::endl;
		return -1;
	}
	
	SimpleBlender_C my_blender;
	my_blender.SetImage1(image_1);
	my_blender.SetImage2(image_2);
	cv::Mat blend_result;
	SimpleBlender_C::Result_TP result = my_blender.BlendImages(blend_result);
	
	if ( result == SimpleBlender_C::OK ) {
		IplImage tmp = blend_result;
		cvSaveImage("result.jpg", &tmp);
		std::cerr << "Ok, blended image written to result.jpg" << std::endl;
	} else if ( result == SimpleBlender_C::SIZE_MISMACTH ) {
		std::cerr << "Size of images does not match" << std::endl;
	} else if ( result == SimpleBlender_C::TYPE_MISMATCH ) {
		std::cerr << "Type of images does not match" << std::endl;
	}
	return result;	
}