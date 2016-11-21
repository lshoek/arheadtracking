#include "ocvcontroller.h"

BOOL OCVController::InitOCV()
{
	/* STRUCT */
	m_ocvData.vHeadPositionRaw = glm::vec3(0.5f, 0.5f, 0);
	m_ocvData.vHeadPositionNormalized = glm::vec3(0.5f, 0.5f, 0);

	/* CAPTURING */
	m_cVideoCap = cv::VideoCapture(0);
	if (!m_cVideoCap.isOpened()) {
		std::cout << "Capture Device ID " << 0 << "cannot be opened." << std::endl;
		return -1;
	}
	m_cVideoCap.read(m_mFrame);
	cv::imshow(OCVWINDOW_ID, m_mFrame);
	cv::moveWindow(OCVWINDOW_ID, WND_WIDTH + 2 * WND_MARGIN, 0);

	/* DETECTION */
	if (!m_cFaceCascadeClassifier.load(FACE_CASCADE_FILE)) { 
		printf("--(!)Error loading\n"); 
		return -1; 
	};
	if (!m_cEyesCascadeClassifier.load(EYES_CASCADE_FILE)) {
		printf("--(!)Error loading\n"); 
		return -1; 
	};
	return 1;
}

void OCVController::Update()
{
	m_cVideoCap.read(m_mFrame);

	if (!m_mFrame.empty()) detectAndDisplay();
	else printf(" --(!) No captured frame -- Break!\n");
}

void OCVController::detectAndDisplay()
{
	std::vector<cv::Rect> faces;
	cv::Mat frame_gray;
	cv::Point center;

	cv::cvtColor(m_mFrame, frame_gray, CV_BGR2GRAY);
	cv::equalizeHist(frame_gray, frame_gray);

	//-- Detect faces
	m_cFaceCascadeClassifier.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));

	if (!faces.empty()) {
		center = cv::Point(faces[0].x + faces[0].width*0.5, faces[0].y + faces[0].height*0.5);

		int w = faces[0].width;
		int h = faces[0].height;
		cv::rectangle(m_mFrame, cv::Rect(center.x - w/2, center.y -h/2, w, h), cv::Scalar(0, 0, 255), 0, 0, 0);

		//for (size_t i = 0; i < faces.size(); i++)
		//{
		//cv::Point center(faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5);
		//cv::ellipse(m_mFrame, center, cv::Size(faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, cv::Scalar(255, 0, 255), 4, 8, 0);

		//cv::Mat faceROI = frame_gray(faces[i]);
		//std::vector<cv::Rect> eyes;

		//-- In each face, detect eyes
		//eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));

		//for (size_t j = 0; j < eyes.size(); j++)
		//{
		//	cv::Point center(faces[i].x + eyes[j].x + eyes[j].width*0.5, faces[i].y + eyes[j].y + eyes[j].height*0.5);
		//	int radius = cvRound((eyes[j].width + eyes[j].height)*0.25);
		//	cv::circle(m_mFrame, center, radius, cv::Scalar(255, 0, 0), 4, 8, 0);
		//}
	//}
		m_ocvData.vHeadPositionNormalized = glm::vec3(
			0.5f - float(center.x) / WND_WIDTH,
			0.5f - float(center.y) / WND_HEIGHT,
			m_fZBias*float(w*h) / (WND_WIDTH*WND_HEIGHT)
		);
		m_ocvData.vHeadPositionRaw = glm::vec3(center.x, center.y, m_fZBias*float(w*h));
	}
	cv::imshow(OCVWINDOW_ID, m_mFrame);
}

void OCVController::ReleaseOCVController()
{
	//
}

OCVController::OCVData OCVController::GetData()
{
	return m_ocvData;
}

void OCVController::PrintData()
{
	printf("OCVData(%f, %f, %f) -> (%f, %f, %f)\n",
		m_ocvData.vHeadPositionRaw.x,
		m_ocvData.vHeadPositionRaw.y,
		m_ocvData.vHeadPositionRaw.z,
		m_ocvData.vHeadPositionNormalized.x,
		m_ocvData.vHeadPositionNormalized.y,
		m_ocvData.vHeadPositionNormalized.z
	);
}