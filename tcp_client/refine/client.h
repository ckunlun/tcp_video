#pragma once
#include<iostream>
#include<winsock.h>
#include <string>
#pragma comment(lib,"ws2_32.lib")
#include "myopencv.h"
using namespace std;


class Server {
public:
	int m_port; //端口号
	string m_ip; //服务端ip地址
	//定义服务端套接字，接受请求套接字
	SOCKET m_server;
	VideoCapture m_cap;
	vector<Mat> m_file_in;
	vector<Mat> m_file_out;
	//定义发送缓冲区和接受缓冲区
	string m_send_buf;
	string m_recv_buf;

	bool initialization(const int& port, const string& ip, const VideoCapture& cap); // 初始化
	bool initialization(const int& port, const string& ip); // 初始化
	bool build_connect();
	bool send_data();
	bool receive_data();
	bool send_data_frame(Mat input);
	bool receive_data_frame(Mat& output);
	bool free_connect();
};