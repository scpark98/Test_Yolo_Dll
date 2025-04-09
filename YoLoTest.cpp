
#include <iostream>
#include <fstream>

#define OPENCV
#define GPU

#include "yolo_v2_class.hpp"	// imported functions from DLL
#include <opencv2/opencv.hpp>	// C++
#include "opencv2/highgui/highgui.hpp"  

//#pragma comment(lib, "opencv_world330.lib")

void draw_boxes(cv::Mat mat_img, std::vector<bbox_t> result_vec, std::vector<std::string> obj_names,
	int current_det_fps = -1, int current_cap_fps = -1)
{
	int const colors[6][3] = { { 1,0,1 },{ 0,0,1 },{ 0,1,1 },{ 0,1,0 },{ 1,1,0 },{ 1,0,0 } };
	for (auto &i : result_vec) {
		cv::Scalar color(0, 255, 0);// = obj_id_to_color(i.obj_id);
		cv::rectangle(mat_img, cv::Rect(i.x, i.y, i.w, i.h), color, 2);
		if (obj_names.size() > i.obj_id) {
			std::string obj_name = obj_names[i.obj_id];
			if (i.track_id > 0) obj_name += " - " + std::to_string(i.track_id);
			cv::Size const text_size = getTextSize(obj_name, cv::FONT_HERSHEY_COMPLEX_SMALL, 1.2, 2, 0);
			int const max_width = (text_size.width > i.w + 2) ? text_size.width : (i.w + 2);
			cv::rectangle(mat_img, cv::Point2f(std::max((int)i.x - 1, 0), std::max((int)i.y - 30, 0)),
				cv::Point2f(std::min((int)i.x + max_width, mat_img.cols - 1), std::min((int)i.y, mat_img.rows - 1)),
				color, CV_FILLED, 8, 0);
			putText(mat_img, obj_name, cv::Point2f(i.x, i.y - 10), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.2, cv::Scalar(0, 0, 0), 2);
		}
	}
}

std::vector<std::string> objects_names_from_file(std::string const filename) {
	std::ifstream file(filename);
	std::vector<std::string> file_lines;
	if (!file.is_open()) return file_lines;
	for (std::string line; getline(file, line);) file_lines.push_back(line);
	std::cout << "object names loaded \n";
	return file_lines;
}

int main()
{
	//std::string  names_file = "D:\\1.Projects_C++\\1.test\\Test_Yolo_Dll\\bin\\model\\coco.names";
	//std::string  cfg_file = "D:\\1.Projects_C++\\1.test\\Test_Yolo_Dll\\bin\\model\\yolov3-tiny.cfg";
	//std::string  weights_file = "D:\\1.Projects_C++\\1.test\\Test_Yolo_Dll\\bin\\model\\yolov3-tiny.weights";
	std::string  names_file = "model\\coco.names";
	std::string  cfg_file = "model\\yolov3-tiny.cfg";
	std::string  weights_file = "model\\yolov3-tiny.weights";
	Detector detector(cfg_file, weights_file);
	auto obj_names = objects_names_from_file(names_file);

	cv::VideoCapture capture;
	capture.open("d:\\media\\test_video\\도심 거리. 서울 신사동 가로수 길   워킹(2024.6).mp4");
	if (!capture.isOpened())
	{
		printf("Open Error");
	}
 
	long totalFrameNumber = capture.get(CV_CAP_PROP_FRAME_COUNT);
	cv::Mat frame;
	for (size_t i = 0; i < totalFrameNumber; i++)
	{
		capture >> frame;
		std::vector<bbox_t> result_vec = detector.detect(frame);
		draw_boxes(frame, result_vec, obj_names);
		cv::imshow("window name1", frame);
		cv::waitKey(3);
	}
	return 0;
}