#pragma once
#include <windows.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "stdafx.h"

class OCVController
{
public:
	struct OCVData {
		glm::vec3 vHeadPositionRaw;
		glm::vec3 vHeadPositionNormalized;
	};
	float m_fZBias = 8.0f;

	BOOL InitOCV();

	void Update();
	void ReleaseOCVController();
	
	OCVData GetData();
	void PrintData();

private:
	const std::string FACE_CASCADE_FILE = "../data/xml/haarcascade_frontalface_alt.xml";
	const std::string EYES_CASCADE_FILE = "../data/xml/haarcascade_eye_tree_eyeglasses.xml";
	const std::string OCVWINDOW_ID = "Capture";

	cv::CascadeClassifier m_cFaceCascadeClassifier;
	cv::CascadeClassifier m_cEyesCascadeClassifier;

	cv::VideoCapture m_cVideoCap;
	cv::Mat m_mFrame;

	OCVData m_ocvData;

	void detectAndDisplay();
};