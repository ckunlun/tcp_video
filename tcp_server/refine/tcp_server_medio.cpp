#include "server.h"

int main() {
	//string input = "E:\\imageRefine\\image\\test0.avi";
	//VideoCapture cap;
	//cap.open(input);

	string input = "E:\\imageRefine\\image\\300.jpg";
	Mat frame = imread(input);

	Server ser;
	int port = 7777;
	//ser.initialization(port, cap);
	ser.initialization(port);
	ser.build_connect();
	//ser.build_connect();

	//ser.send_data();
	//ser.receive_data();


	//string output = "E:\\imageRefine\\image\\server.avi";
	//int fourcc = cap.get(CAP_PROP_FOURCC);
	//double fps = cap.get(CAP_PROP_FPS);
	//cv::Size size = cv::Size(cap.get(CAP_PROP_FRAME_WIDTH), cap.get(CAP_PROP_FRAME_HEIGHT));


	//VideoWriter vw(output, fourcc, fps, size, true);

	//for (int i = 0; i < ser.m_file_out.size(); ++i) {
	//	vw.write(ser.m_file_out[i]);
	//}

	//vw.release();
	//cap.release();

	Mat output;
	ser.receive_data_frame(output);
	//ser.send_data_frame(frame);
	imshow("s", output);
	waitKey(0);

	ser.free_connect();
	return 0;
}

