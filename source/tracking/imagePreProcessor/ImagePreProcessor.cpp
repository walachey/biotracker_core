#include "ImagePreProcessor.h"

#include "source/settings/Settings.h"
#include "source/settings/ParamNames.h"

#include <QReadWriteLock>

QReadWriteLock lock;

ImagePreProcessor::ImagePreProcessor(Settings &settings) :
	_settings(settings)
{
	init();
}

void ImagePreProcessor::init()
{
    _pMOG = std::make_unique<cv::BackgroundSubtractorMOG2>(
        _settings.getValueOfParam<int>(PREPROCESSORPARAM::BG_MOG2_HISTORY),
        _settings.getValueOfParam<int>(PREPROCESSORPARAM::BG_MOG2_VAR_THRESHOLD),
        _settings.getValueOfParam<bool>(PREPROCESSORPARAM::BG_MOG2_SHADOW_DETECTION));
}

cv::Mat ImagePreProcessor::binarize(cv::Mat image)
{	
	//cv::Mat grayImage;
	//cv::cvtColor( image, grayImage, CV_BGR2GRAY );
	cv::Mat binarizedImage;
	if(image.data)
        cv::threshold(image, binarizedImage, _settings.getValueOfParam<int>(PREPROCESSORPARAM::THRESHOLD_BINARIZING), 255, cv::THRESH_BINARY);
	return binarizedImage;
}

cv::Mat ImagePreProcessor::erode(cv::Mat image)
{	
	cv::Mat erodedImage;
	cv::Mat erodeKernel;
    if(_settings.getValueOfParam<int>(PREPROCESSORPARAM::SIZE_ERODE) > 0)
	{
        erodeKernel = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(_settings.getValueOfParam<int>(PREPROCESSORPARAM::SIZE_ERODE),_settings.getValueOfParam<int>(PREPROCESSORPARAM::SIZE_ERODE)));
	} else {
		return image;
	}
	if(image.data)
		cv::erode(image, erodedImage, erodeKernel);
	return erodedImage;
}

cv::Mat ImagePreProcessor::dilate(cv::Mat image)
{	
	cv::Mat dilatedImage;
	cv::Mat dilateKernel;
    if(_settings.getValueOfParam<int>(PREPROCESSORPARAM::SIZE_DILATE) > 0)
	{
		dilateKernel = cv::getStructuringElement(
			cv::MORPH_RECT, 
            cv::Size(_settings.getValueOfParam<int>(PREPROCESSORPARAM::SIZE_DILATE),
            _settings.getValueOfParam<int>(PREPROCESSORPARAM::SIZE_DILATE)));
	} else {
		return image;
	}
	if(image.data)
		cv::dilate(image, dilatedImage, dilateKernel);
	return dilatedImage;
}

cv::Mat ImagePreProcessor::backgroundSubtraction(cv::Mat image)
{
	cv::Mat fgMaskMOG;
	//_pMOG->set("backgroundRatio",_systemProperty.getValueOfParam<double>(TRACKERPARAM::BG_MOG2_BACKGROUND_RATIO));
	//_pMOG->operator()(image,fgMaskMOG);	//fg mask generated by MOG method
	//return fgMaskMOG;

	if(!_backgroundImage.data)
		image.copyTo(_backgroundImage);	

	// calculate the image difference
	fgMaskMOG = (_backgroundImage - image);
		
	// weight the background image
    double alpha = _settings.getValueOfParam<double>(PREPROCESSORPARAM::BG_MOG2_BACKGROUND_RATIO);
	_backgroundImage = (1.0 - alpha) * _backgroundImage + alpha * image;
	
	cv::cvtColor(fgMaskMOG,fgMaskMOG,CV_BGR2GRAY);
	return fgMaskMOG;
}

void ImagePreProcessor::preProcess(cv::Mat image)
{
	lock.lockForWrite();
	image.copyTo(_original);
	lock.unlock();
	image.copyTo(_proceededImage);

    if(_settings.getValueOfParam<bool>(PREPROCESSORPARAM::ENABLE_BG_SUBTRACTION)) // 1. step: do the background subtraction
	{
		_fgMaskMOG = backgroundSubtraction(_proceededImage);
		_fgMaskMOG.copyTo(_proceededImage);
	}

	////TODO:: An exception was thrown here. Maybe one shall use local variable instead of using a call on a data file to get the variable.
	//if(_systemProperty.getValueOfParam<bool>(TRACKERPARAM::ENABLE_BG_SUBTRACTION) && _backgroundEnabled) // 1.1 step: gets the over time background image
	//{
	//	_pMOG->getBackgroundImage(_backgroundImage);
	//}

    if(_settings.getValueOfParam<bool>(PREPROCESSORPARAM::ENABLE_BINARIZING)) // 2. step: binarize the image
	{
		_binarizedImage = binarize(_proceededImage);
		_binarizedImage.copyTo(_proceededImage);
	}
	
    if(_settings.getValueOfParam<bool>(PREPROCESSORPARAM::ENABLE_ERODE)) // 3. step: erode the image
	{
		_erodedImage = erode(_proceededImage);
		_erodedImage.copyTo(_proceededImage);
	}

    if(_settings.getValueOfParam<bool>(PREPROCESSORPARAM::ENABLE_DILATE)) // 4. step: dilate the image
	{
		_dilatedImage = dilate(_proceededImage);
		_dilatedImage.copyTo(_proceededImage);
	}
}

void ImagePreProcessor::setEnable(std::string enableProcess, bool enabled)
{
    if(enableProcess.compare(PREPROCESSORPARAM::ENABLE_BG_SUBTRACTION) == 0)
	{
		_backgroundSubtractionEnabled = enabled;
		return;
	}

    if(enableProcess.compare(PREPROCESSORPARAM::ENABLE_BG_IMAGE) == 0)
	{
		_backgroundEnabled = enabled;
		return;
	}

    if(enableProcess.compare(PREPROCESSORPARAM::ENABLE_BINARIZING) == 0)
	{
		_binarizeEnabled = enabled;
		return;
	}

    if(enableProcess.compare(PREPROCESSORPARAM::ENABLE_ERODE) == 0)
	{
		_erodeEnabled = enabled;
		return;
	}

    if(enableProcess.compare(PREPROCESSORPARAM::ENABLE_DILATE) == 0)
	{
		_dilateEnabled = enabled;
		return;
	}
}

cv::Mat ImagePreProcessor::imageDisplayType(PREPROCESSORPARAM::IMAGE_DISPLAY_TYPE imageDisplayType)
{
	switch(imageDisplayType)
	{
        case PREPROCESSORPARAM::IMAGE_DISPLAY_TYPE::IMAGE_BI:
            if(_settings.getValueOfParam<bool>(PREPROCESSORPARAM::ENABLE_BINARIZING))
				return _binarizedImage;
			break;
        case PREPROCESSORPARAM::IMAGE_DISPLAY_TYPE::IMAGE_ER:
            if(_settings.getValueOfParam<bool>(PREPROCESSORPARAM::ENABLE_ERODE))
				return _erodedImage;
			break;
        case PREPROCESSORPARAM::IMAGE_DISPLAY_TYPE::IMAGE_DI:
            if(_settings.getValueOfParam<bool>(PREPROCESSORPARAM::ENABLE_DILATE))
				return _dilatedImage;
			break;
        case PREPROCESSORPARAM::IMAGE_DISPLAY_TYPE::IMAGE_FG:
            if(_settings.getValueOfParam<bool>(PREPROCESSORPARAM::ENABLE_BG_SUBTRACTION))
				return _fgMaskMOG;
			break;
        case PREPROCESSORPARAM::IMAGE_DISPLAY_TYPE::IMAGE_BG:
            if(_settings.getValueOfParam<bool>(PREPROCESSORPARAM::ENABLE_BG_SUBTRACTION) && _backgroundEnabled)
				return _backgroundImage;
			break;
        case PREPROCESSORPARAM::IMAGE_DISPLAY_TYPE::IMAGE_PP:
			return _proceededImage;
		default:
			return _original;
	}
	if(_original.data)
		return _original;
	return cv::Mat();
}	 

void ImagePreProcessor::resetBackgroundImage()
{
	try 
	{
		/*cv::Mat bgMat;
		_pMOG->getBackgroundImage(bgMat);
		_pMOG->initialize(bgMat.size(), bgMat.type());*/
		lock.lockForRead();
		_original.copyTo(_backgroundImage);
		lock.unlock();
	} catch(cv::Exception)
	{
		
	}
	
}
