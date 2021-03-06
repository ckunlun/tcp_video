#include "client.h"
#define SIZE 100
bool Server::initialization(const int& port, const string& ip, const VideoCapture& cap) {
	m_port = port;
	m_cap = cap;
	m_ip = ip;
	// 初始化库（windows独有）
	//初始化套接字库
	WORD w_req = MAKEWORD(2, 2);//版本号
	WSADATA wsadata;
	int err;
	err = WSAStartup(w_req, &wsadata);
	if (err != 0) {
		cout << "初始化套接字库失败！" << endl;
		return false;
	}
	else {
		cout << "初始化套接字库成功！" << endl;
	}

	//检测版本号
	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2) {
		cout << "套接字库版本号不符！" << endl;
		WSACleanup();
		return false;
	}
	else {
		cout << "套接字库版本正确！" << endl;
	}

	return true;
}

bool Server::initialization(const int& port, const string& ip) {
	m_port = port;
	m_ip = ip;
	// 初始化库（windows独有）
	//初始化套接字库
	WORD w_req = MAKEWORD(2, 2);//版本号
	WSADATA wsadata;
	int err;
	err = WSAStartup(w_req, &wsadata);
	if (err != 0) {
		cout << "初始化套接字库失败！" << endl;
		return false;
	}
	else {
		cout << "初始化套接字库成功！" << endl;
	}

	//检测版本号
	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2) {
		cout << "套接字库版本号不符！" << endl;
		WSACleanup();
		return false;
	}
	else {
		cout << "套接字库版本正确！" << endl;
	}

	return true;
}

bool Server::build_connect() {
	//服务端地址客户端地址
	SOCKADDR_IN server_addr;
	SOCKADDR_IN accept_addr;

	//填充服务端信息
	server_addr.sin_family = AF_INET;  // 用来定义那种地址族，AF_INET：IPV4
	server_addr.sin_addr.S_un.S_addr = inet_addr(m_ip.c_str());  // 保存ip地址，htonl将一个无符号长整型转换为TCP/IP协议网络的大端
	// INADDR_ANY表示一个服务器上的所有网卡
	server_addr.sin_port = htons(m_port);  // 端口号

	//创建套接字
	m_server = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(m_server, (SOCKADDR*)&server_addr, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		cout << "服务器连接失败！" << endl;
		WSACleanup();
		return false;
	}
	else {
		cout << "服务器连接成功！" << endl;
	}

	return true;
}

bool Server::send_data() {
	Mat frame;
	vector<uchar> data_encode;
	std::vector<int> params;  // 压缩参数
	params.resize(3, 0);
	params[0] = IMWRITE_JPEG_QUALITY; // 无损压缩
	params[1] = 100;
	char frames_cnt[10] = { 0, };
	_itoa_s(int(m_cap.get(CAP_PROP_FRAME_COUNT)), frames_cnt, 10);
	send(m_server, frames_cnt, 10, 0);
	cout << "开始发送" << endl;
	int j = 0;
	while (m_cap.read(frame)) {

		m_file_in.push_back(frame.clone());
		imencode(".jpg", frame, data_encode, params);  // 对图像进行压缩
		int len_encoder = data_encode.size();

		_itoa_s(len_encoder, frames_cnt, 10);
		send(m_server, frames_cnt, 10, 0);

		_itoa_s(SIZE, frames_cnt, 10);
		send(m_server, frames_cnt, 10, 0);
		// 发送
		char send_char[SIZE] = { 0, };
		int index = 0;
		bool flag = false;
		for (int i = 0; i < len_encoder / SIZE + 1; ++i) {
			for (int k = 0; k < SIZE; ++k) {
				if (index >= data_encode.size()) {
					flag = true;
					break;
				}
				send_char[k] = data_encode[index++];
			}
			send(m_server, send_char, SIZE, 0);
		}

		//m_send_buf = to_string(j + 1);
		//send(m_server, m_send_buf.c_str(), strlen(frames_cnt.c_str()), 0);
		//waitKey(25);
		//imshow("input", frame);
		data_encode.clear();
		++j;
		cout << j << endl;  // 发送端一直在发送
	}
	cout << "发送完成";
	return true;
}

bool Server::receive_data() {
	Mat frame;
	vector<uchar> data_decode;
	std::vector<int> params;  // 压缩参数
	char recv_char[100];

	params.resize(3, 0);
	params[0] = IMWRITE_JPEG_QUALITY; // 无损压缩
	params[1] = 100;

	char frams_cnt[10] = { 0, };
	recv(m_server, frams_cnt, 10, 0);
	// 解析总帧数
	int count = atoi(frams_cnt);
	for (unsigned int i = 0; i < count; ++i) {
		recv(m_server, frams_cnt, 10, 0);
		// 解析总帧数
		int cnt = atoi(frams_cnt);
		recv(m_server, frams_cnt, 10, 0);
		int size = atoi(frams_cnt);

		data_decode.resize(cnt);
		int index = 0;
		bool flag = true;
		for (int i = 0; i < cnt / size + 1; ++i) {
			recv(m_server, recv_char, size, 0);
			for (int k = 0; k < size; ++k) {
				if (index >= cnt) {
					flag = true;
					break;
				}
				data_decode[index++] = recv_char[k];
			}
		}

		//data_decode.pop_back();
		frame = imdecode(data_decode, IMREAD_COLOR);
		m_file_out.push_back(frame.clone());
		//waitKey(25);
		//imshow("s", frame);
		//char recv_buf[50];
		//recv(m_accept, recv_char, 50, 0);
		//cout << "客户端信息:" << recv_buf << endl;
		data_decode.clear();
	}
	cout << "接受完成";
	return true;
}

bool Server::send_data_frame(Mat input) {
	Mat frame = input;
	vector<uchar> data_encode;
	std::vector<int> params;  // 压缩参数
	params.resize(3, 0);
	params[0] = IMWRITE_JPEG_QUALITY; // 无损压缩
	params[1] = 100;
	char frames_cnt[10] = { 0, };
	cout << "开始发送" << endl;

	m_file_in.push_back(frame.clone());
	imencode(".jpg", frame, data_encode, params);  // 对图像进行压缩
	int len_encoder = data_encode.size();

	_itoa_s(len_encoder, frames_cnt, 10);
	send(m_server, frames_cnt, 10, 0);

	_itoa_s(SIZE, frames_cnt, 10);
	send(m_server, frames_cnt, 10, 0);
	// 发送
	char send_char[SIZE] = { 0, };
	int index = 0;
	bool flag = false;
	for (int i = 0; i < len_encoder / SIZE + 1; ++i) {
		for (int k = 0; k < SIZE; ++k) {
			if (index >= data_encode.size()) {
				flag = true;
				break;
			}
			send_char[k] = data_encode[index++];
		}
		send(m_server, send_char, SIZE, 0);
	}

	data_encode.clear();

	cout << "发送完成";
	return true;
}

bool Server::receive_data_frame(Mat& output) {
	Mat frame;
	vector<uchar> data_decode;
	std::vector<int> params;  // 压缩参数
	char recv_char[100];

	params.resize(3, 0);
	params[0] = IMWRITE_JPEG_QUALITY; // 无损压缩
	params[1] = 100;

	char frams_cnt[10] = { 0, };

	recv(m_server, frams_cnt, 10, 0);
	// 解析总帧数
	int cnt = atoi(frams_cnt);
	recv(m_server, frams_cnt, 10, 0);
	int size = atoi(frams_cnt);

	data_decode.resize(cnt);
	int index = 0;
	bool flag = true;
	for (int i = 0; i < cnt / size + 1; ++i) {
		recv(m_server, recv_char, size, 0);
		for (int k = 0; k < size; ++k) {
			if (index >= cnt) {
				flag = true;
				break;
			}
			data_decode[index++] = recv_char[k];
		}
	}

	output = imdecode(data_decode, IMREAD_COLOR);

	data_decode.clear();

	cout << "接受完成";
	return true;
}

bool Server::free_connect() {
	m_cap.release();
	//关闭套接字
	closesocket(m_server);
	//释放DLL资源
	WSACleanup();
	return true;
}